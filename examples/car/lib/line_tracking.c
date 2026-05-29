#include "line_tracking.h"
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "car.h"
#include "PID.h"
#include "key.h"
#include "control.h"


Motor_parameter MotorA, MotorB;
uint32_t ir_dh0_state, ir_dh1_state, ir_dh2_state, ir_dh3_state, ir_dh4_state;

/*=============================================================================
 * 可调参数区域
 *=============================================================================*/
float Turn90Angle  = 100;
float TurnMinAngle = 15;
float TurnBigAngle = 15;
float BaseSpeed = 180;
float ForwardLimit = 140;

// PID参数
PID_t line_pid;
float Line_Kp = 30.0f;
float Line_Ki = 0.0f;
float Line_Kd = 35.0f;

// 输出变化率限制（Ramp Limit）——只在直道生效
float RAMP_LIMIT = 15.0f;

/*=============================================================================
 * 传感器状态定义（识别到黑线为1）
 *=============================================================================*/
typedef enum {
    STATE_LEFT_90_A     = 30,	//11110 直角左转
    STATE_LEFT_90_B     = 28,	//11100 直角左转
    STATE_LEFT_90_C     = 24,	//11000 直角左转
    STATE_LEFT_90_D		= 16,	//10000 直角左转
	
    STATE_LEFT_SMALL    = 8 ,	//01000 小调左转
    STATE_STRAIGHT      = 4 ,	//00100 直线
    STATE_RIGHT_SMALL   = 2 ,	//00010 小调右转
	
    STATE_RIGHT_90_D	= 1,	//10000 直角右转
    STATE_RIGHT_90_C    = 3 ,	//00011 直角右转
    STATE_RIGHT_90_B    = 7 ,	//00111 直角右转
    STATE_RIGHT_90_A    = 15,	//01111 直角右转

    STATE_LOST          = 0	    //00000 丢失
} SensorState_t;

/*=============================================================================
 * 巡线状态机状态定义
 *=============================================================================*/
typedef enum {
    STATE_FORWARD = 0,
    STATE_TURN,
    STATE_STOP
} LineState_t;

float base_speed_mm = 0;
float turn_diff = 0;

static uint8_t corner_count = 0;
static int8_t  turn_direction = 0;
static uint8_t total_corners;
static uint16_t corner_period_cnt = 0;   // 【新增】消抖周期计数器
#define CORNER_DEBOUNCE_CNT  1400 

/*=============================================================================
 * 巡线主函数（v4 配重修复版：Ramp Limit 只限直道）
 *=============================================================================*/
void IRDM_line_inspection(void)
{
    static LineState_t state = STATE_FORWARD;
    static int last_state = 0;
    static uint8_t pid_inited = 0;
    static float turn_diff_last = 0;
    static float d_filtered = 0;       // 唯一保留：D项不完全微分状态
    float left_motor_speed, right_motor_speed;
	int8_t num = key_get_turn_num();
	if (num >= 1) {
		total_corners = num * 4;   // 1→4, 2→8, 3→12, 4→16, 5→20
	}

    if (!pid_inited) {
        line_pid.error = line_pid.error_i = line_pid.error_last1 = line_pid.error_last2 = 0;
        line_pid.p = line_pid.i = line_pid.d = line_pid.out = 0;
        line_pid.kp = Line_Kp;
        line_pid.ki = Line_Ki;
        line_pid.kd = Line_Kd;
        pid_inited = 1;
    }

    /* 【新增】消抖周期计数器：每 5ms 累加，直到上限 */
    if (corner_period_cnt < CORNER_DEBOUNCE_CNT) {
        corner_period_cnt++;
    }

    ir_dh4_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_4_PIN) ? 1 : 0;
    ir_dh3_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_3_PIN) ? 1 : 0;
    ir_dh2_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_2_PIN) ? 1 : 0;
    ir_dh1_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_1_PIN) ? 1 : 0;
    ir_dh0_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_0_PIN) ? 1 : 0;

    int sensor_state = (ir_dh0_state << 4) | (ir_dh1_state << 3) |
                       (ir_dh2_state << 2) | (ir_dh3_state << 1) | ir_dh4_state;

    switch (state)
    {
        case STATE_FORWARD:
        {
            if (sensor_state == STATE_LEFT_90_A || 
                sensor_state == STATE_LEFT_90_B ||
                sensor_state == STATE_LEFT_90_D ||
                sensor_state == STATE_LEFT_90_C)
            {
                /* 【新增】消抖：2秒内只计数一次，但转弯照常执行 */
                if (corner_period_cnt >= CORNER_DEBOUNCE_CNT) {
                    corner_count++;
                    corner_period_cnt = 0;
                }
                turn_direction = 1;
                state = STATE_TURN;
                break;
            }
            else if (sensor_state == STATE_RIGHT_90_A || 
                     sensor_state == STATE_RIGHT_90_B ||
                     sensor_state == STATE_RIGHT_90_D ||
                     sensor_state == STATE_RIGHT_90_C)
            {
                /* 【新增】消抖：2秒内只计数一次，但转弯照常执行 */
                if (corner_period_cnt >= CORNER_DEBOUNCE_CNT) {
                    corner_count++;
                    corner_period_cnt = 0;
                }
                turn_direction = -1;
                state = STATE_TURN;
                break;
            }

            float line_error = 0.0f;
            uint8_t valid_line = 1;

            switch (sensor_state)
            {
                case STATE_STRAIGHT:    line_error = 0.0f;  break;
                case STATE_LEFT_SMALL:  line_error = 1.0f;  break;
                case STATE_RIGHT_SMALL: line_error = -1.0f; break;
                case STATE_LOST:        valid_line = 0;     break;
                default:                line_error = 0.0f;  break;
            }

            if (valid_line) {
                /*========== 直道PID：只留D项防抖，其余低通全砍 ==========*/
                float error = -line_error;   // target=0
                
                // P：直接响应，不拖泥带水
                float p_out = error * line_pid.kp;
                
                // I：Ki=0，彻底关掉，杜绝积分漂移
                float i_out = 0;
                
                // D：不完全微分 0.5/0.5，只防D项冲击，不引入滞后
                float d_raw = (error - line_pid.error_last1) * line_pid.kd;
                d_filtered = 0.5f * d_filtered + 0.5f * d_raw;
                float d_out = d_filtered;
                
                line_pid.error_last1 = error;
                turn_diff = -(p_out + i_out + d_out);
            } else {
                // 丢线：清状态，按记忆盲走
                line_pid.error_last1 = 0;
                d_filtered = 0;
                if (last_state == STATE_LEFT_SMALL)
                    turn_diff = TurnMinAngle;
                else if (last_state == STATE_RIGHT_SMALL)
                    turn_diff = -TurnMinAngle;
                else
                    turn_diff = 0;
            }
            break;
        }

        case STATE_TURN:
        {
            turn_diff = turn_direction * Turn90Angle;
            d_filtered = 0;  // 进弯道清D项

            if (sensor_state == STATE_STRAIGHT||sensor_state == STATE_LEFT_SMALL)
            {
                state = STATE_FORWARD;
                turn_diff = 0;
                line_pid.error_last1 = 0;
                d_filtered = 0;
                turn_diff_last = 0;
            }
            break;
        }

        case STATE_STOP:
        {
            turn_diff = 0;
            base_speed_mm = 0;
            MotorA.Target_Encoder = 0;
            MotorB.Target_Encoder = 0;
            return;
        }

        default:
            break;
    }

    if (sensor_state != STATE_LOST && state == STATE_FORWARD)
    {
        last_state = sensor_state;
    }

    /*=====================================================================
     * Ramp Limit：唯一保留的输出限速，防止电机突变
     *=====================================================================*/
    float turn_diff_filtered = turn_diff;

    if (state == STATE_FORWARD) {
        float delta = turn_diff - turn_diff_last;
        if (delta > RAMP_LIMIT) {
            delta = RAMP_LIMIT;
        } else if (delta < -RAMP_LIMIT) {
            delta = -RAMP_LIMIT;
        }
        turn_diff_filtered = turn_diff_last + delta;
        turn_diff_last = turn_diff_filtered;
    } else {
        turn_diff_last = turn_diff;
    }

    /*------- 速度计算 -------*/
    if (fabs(turn_diff_filtered) < ForwardLimit)
    {
        base_speed_mm = BaseSpeed - (BaseSpeed * (fabs(turn_diff_filtered) / ForwardLimit));
    }
    else
    {
        base_speed_mm = 0;
    }

    left_motor_speed  = 0.001f * (base_speed_mm - turn_diff_filtered);
    right_motor_speed = 0.001f * (base_speed_mm + turn_diff_filtered);

    MotorA.Target_Encoder = left_motor_speed;
    MotorB.Target_Encoder = right_motor_speed;

    if (corner_count >= total_corners && state != STATE_STOP)
    {
        state = STATE_STOP;
        Flag_Stop = 1;
    }
}