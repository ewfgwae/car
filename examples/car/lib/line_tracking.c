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
// 转向角度参数（直角弯仍用固定差速，不变）
float Turn90Angle  = 90;
float TurnMinAngle = 15;
float TurnBigAngle = 15;
// 速度参数
float BaseSpeed = 220;
float ForwardLimit = 90;

// >>> 改动1：增加巡线PID变量与参数（位置式PID） <<<
PID_t line_pid;
float Line_Kp = 15.0f;   // 比例，根据实际调试调整
float Line_Ki = 0.0f;    // 积分，建议先给0，防止积分饱和冲出赛道
float Line_Kd = 25.0f;    // 微分，抑制振荡

/*=============================================================================
 * 传感器状态定义（识别到黑线为1）
 *=============================================================================*/
typedef enum {
    STATE_LEFT_90_A     = 30,	//直角左转
    STATE_LEFT_90_B     = 28,	//直角左转
    STATE_LEFT_BIG      = 16,	//大调左转
    STATE_LEFT_SMALL    = 8 ,	//小调左转
    STATE_STRAIGHT      = 4 ,	//直线
    STATE_RIGHT_SMALL   = 2 ,	//直角右转
    STATE_RIGHT_BIG     = 1 ,	//直角右转
    STATE_RIGHT_90_B    = 7 ,	//大调右转
    STATE_RIGHT_90_A    = 15,	//小调右转
    STATE_LOST          = 0	    //丢失
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
static uint8_t total_corners = 4;

/*=============================================================================
 * 巡线主函数（状态机实现，仅直角+直行+微调）
 *=============================================================================*/
void IRDM_line_inspection(void)
{
    static LineState_t state = STATE_FORWARD;
    static int last_state = 0;
    static uint8_t pid_inited = 0;   // >>> 改动2：首次运行标志 <<<
    float left_motor_speed, right_motor_speed;

    /*------- PID首次初始化（清零，防止上电时历史值随机）-------*/
    if (!pid_inited) {
        line_pid.error = line_pid.error_i = line_pid.error_last1 = line_pid.error_last2 = 0;
        line_pid.p = line_pid.i = line_pid.d = line_pid.out = 0;
        line_pid.kp = Line_Kp;
        line_pid.ki = Line_Ki;
        line_pid.kd = Line_Kd;
        pid_inited = 1;
    }

    /*------- 1. 读取5个传感器（高电平=黑线）-------*/
    ir_dh4_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_4_PIN) ? 1 : 0;
    ir_dh3_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_3_PIN) ? 1 : 0;
    ir_dh2_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_2_PIN) ? 1 : 0;
    ir_dh1_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_1_PIN) ? 1 : 0;
    ir_dh0_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_0_PIN) ? 1 : 0;

    int sensor_state = (ir_dh0_state << 4) | (ir_dh1_state << 3) |
                       (ir_dh2_state << 2) | (ir_dh3_state << 1) | ir_dh4_state;

    /*------- 2. 状态机处理 -------*/
    switch (state)
    {
        case STATE_FORWARD:
        {
            // 直角弯检测完全不变
            if (sensor_state == STATE_LEFT_90_A || sensor_state == STATE_LEFT_90_B)
            {
                corner_count++;
                turn_direction = 1;
                state = STATE_TURN;
                break;
            }
            else if (sensor_state == STATE_RIGHT_90_A || sensor_state == STATE_RIGHT_90_B)
            {
                corner_count++;
                turn_direction = -1;
                state = STATE_TURN;
                break;
            }

            // >>> 改动3：正常巡线改为PID计算差速 <<<
            float line_error = 0.0f;
            uint8_t valid_line = 1;

            switch (sensor_state)
            {
                case STATE_STRAIGHT:
                    line_error = 0.0f;
                    break;
                case STATE_LEFT_SMALL:
                    line_error = 1.0f;   // 偏左，误差为正
                    break;
                case STATE_RIGHT_SMALL:
                    line_error = -1.0f;  // 偏右，误差为负
                    break;
                case STATE_LEFT_BIG:
                    line_error = 2.0f;
                    break;
                case STATE_RIGHT_BIG:
                    line_error = -2.0f;
                    break;
                case STATE_LOST:
                    valid_line = 0;
                    break;
                default:
                    line_error = 0.0f;
                    break;
            }

            if (valid_line) {
                // target=0（目标走中线），actual=line_error（当前偏差）
                // 加负号是因为：偏左时 error>0，需要 turn_diff>0（左轮慢右轮快，向右转）
                turn_diff = -PID(&line_pid, 0.0f, line_error);
            } else {
                // 丢线时清零积分，防止累积冲出赛道；再用原逻辑保持方向记忆
                line_pid.error_i = 0;
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
            // 直角弯仍用固定大差速，不变
            turn_diff = turn_direction * Turn90Angle;

            if (sensor_state == STATE_STRAIGHT)
            {
                state = STATE_FORWARD;
                turn_diff = 0;
                // 切回直行时清零PID历史，防止转弯历史导致切回瞬间突变
                line_pid.error = line_pid.error_i = line_pid.error_last1 = line_pid.error_last2 = 0;
                line_pid.p = line_pid.i = line_pid.d = line_pid.out = 0;
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

    /*------- 3. 保存非丢线状态 -------*/
    if (sensor_state != STATE_LOST && state == STATE_FORWARD)
    {
        last_state = sensor_state;
    }

    /*------- 4. 根据转向差速计算基础速度 -------*/
    if (fabs(turn_diff) < ForwardLimit)
    {
        base_speed_mm = BaseSpeed - (BaseSpeed * (fabs(turn_diff) / ForwardLimit));
    }
    else
    {
        base_speed_mm = 0;
    }

    /*------- 5. 计算左右电机目标速度 -------*/
    left_motor_speed  = 0.001f * (base_speed_mm - turn_diff);
    right_motor_speed = 0.001f * (base_speed_mm + turn_diff);

    MotorA.Target_Encoder = left_motor_speed;
    MotorB.Target_Encoder = right_motor_speed;

    /*------- 6. 检查是否完成一圈 -------*/
    if (corner_count >= total_corners && state != STATE_STOP)
    {
        state = STATE_STOP;
        Flag_Stop = 1;
    }
}