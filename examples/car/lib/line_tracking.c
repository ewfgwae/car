#include "line_tracking.h"
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "car.h"
#include "PID.h"
#include "key.h"
#include "control.h"
#include "hardware_iic.h"
#include <math.h>

Motor_parameter MotorA, MotorB;

/*=============================================================================
 * 可调参数区域
 *=============================================================================*/
float Turn90Angle  = 125.0f;
float ForwardLimit = 200.0f;
float TurnMinAngle = 15.0f;
float BaseSpeed    = 370.0f;

PID_t line_pid;
float Line_Kp = 110.0f;
float Line_Ki = 1.0f;
float Line_Kd = 25.0f;

float RAMP_LIMIT = 20.0f;

/*=============================================================================
 * 权重循迹参数
 *=============================================================================*/
#define SENSOR_COUNT       8
#define SENSOR_CENTER      3.5f
#define ERROR_SCALE        1.0f
#define STOP_DEBOUNCE      4

float base_speed_mm = 0.0f;
float turn_diff = 0.0f;
float line_error = 0.0f;

/*=============================================================================
 * 巡线状态机状态定义
 *=============================================================================*/
typedef enum {
    STATE_FORWARD = 0,
    STATE_TURN,
    STATE_STOP
} LineState_t;

    static uint8_t corner_count = 0;
    static int8_t  turn_direction = 0;
    static uint8_t total_corners = 0;
    static uint16_t corner_period_cnt = 0;
    static uint16_t stop_debounce_cnt = 0;
    static uint16_t stop_straight_cnt = 0;
#define CORNER_DEBOUNCE_CNT  700 

/*=============================================================================
 * 权重法：计算8路灰度值的质心（0.0 ~ 7.0）
 * 返回值 < 0 表示丢线
 *=============================================================================*/
static float calc_weighted_centroid(uint8_t *sen)
{
    float sum_num = 0.0f;
    float sum_den = 0.0f;

    for (int i = 0; i < SENSOR_COUNT; i++) {
        float blackness = 255.0f - (float)sen[i];
        sum_num += blackness * (float)i;
        sum_den += blackness;
    }

    if (sum_den < 1.0f) {
        return -1.0f;
    }
    return sum_num / sum_den;
}

/*=============================================================================
 * 巡线主函数（权重质心法 + PD控制）
 *=============================================================================*/
void IRDM_line_inspection(void)
{
    static LineState_t state = STATE_FORWARD;
    static int   last_direction = 0;
    static uint8_t pid_inited = 0;
    static float turn_diff_last = 0.0f;
    static float d_filtered = 0.0f;
    float left_motor_speed, right_motor_speed;
    int8_t num = key_get_turn_num();
    uint8_t sensor_values[8];
    float centroid;

    if (num >= 1) {
        total_corners = num * 4;
    }

    if (!pid_inited) {
        line_pid.error = line_pid.error_i = line_pid.error_last1 = line_pid.error_last2 = 0.0f;
        line_pid.p = line_pid.i = line_pid.d = line_pid.out = 0.0f;
        line_pid.kp = Line_Kp;
        line_pid.ki = Line_Ki;
        line_pid.kd = Line_Kd;
        pid_inited = 1;
    }

    if (corner_period_cnt < CORNER_DEBOUNCE_CNT) {
        corner_period_cnt++;
    }

    IIC_Get_Anolog(sensor_values, 8);
    centroid = calc_weighted_centroid(sensor_values);

    {
        uint8_t digital = IIC_Get_Digtal();
        uint8_t i, hit = 0;
        for (i = 0; i <= 5; i++) {
            if ((digital & (0x07 << i)) == 0) {
                hit = 1;
                break;
            }
        }
        if (hit) {
            stop_debounce_cnt++;
        } else {
            stop_debounce_cnt = 0;
        }
    }

    if (stop_debounce_cnt >= STOP_DEBOUNCE && state != STATE_STOP) {
        state = STATE_STOP;
        stop_straight_cnt = 5;
    }

    switch (state)
    {
        case STATE_FORWARD:
        {
            if (centroid >= 0.0f) {
                float error = (SENSOR_CENTER - centroid) * ERROR_SCALE;

                float p_out = error * line_pid.kp;
                float i_out = 0.0f;

                float d_raw = (error - line_pid.error_last1) * line_pid.kd;
                d_filtered = 0.5f * d_filtered + 0.5f * d_raw;
                float d_out = d_filtered;

                line_pid.error_last1 = error;
                turn_diff = -(p_out + i_out + d_out);
                line_error = error;
            } else {
                line_pid.error_last1 = 0.0f;
                d_filtered = 0.0f;
                if (last_direction == 1)
                    turn_diff = -TurnMinAngle;
                else if (last_direction == -1)
                    turn_diff = TurnMinAngle;
                else
                    turn_diff = 0.0f;
            }
            break;
        }

        case STATE_STOP:
        {
            if (stop_straight_cnt > 0) {
                stop_straight_cnt--;
                turn_diff = 0.0f;
                base_speed_mm = BaseSpeed;
                break;
            }
            turn_diff = 0.0f;
            base_speed_mm = 0.0f;
            MotorA.Target_Encoder = 0;
            MotorB.Target_Encoder = 0;
            Flag_Stop = 1;
            return;
        }

        default:
            break;
    }

    if (centroid >= 0.0f && state == STATE_FORWARD) {
        if (centroid < SENSOR_CENTER - 0.3f)
            last_direction = 1;
        else if (centroid > SENSOR_CENTER + 0.3f)
            last_direction = -1;
    }

    float turn_diff_filtered = turn_diff;
    float delta = turn_diff - turn_diff_last;

    if (delta > RAMP_LIMIT) {
        delta = RAMP_LIMIT;
    } else if (delta < -RAMP_LIMIT) {
        delta = -RAMP_LIMIT;
    }
    turn_diff_filtered = turn_diff_last + delta;
    turn_diff_last = turn_diff_filtered;

    if (fabsf(turn_diff_filtered) < ForwardLimit) {
        base_speed_mm = BaseSpeed - (BaseSpeed * (fabsf(turn_diff_filtered) / ForwardLimit));
    } else {
        base_speed_mm = 0.0f;
    }

    left_motor_speed  = 0.001f * (base_speed_mm - turn_diff_filtered);
    right_motor_speed = 0.001f * (base_speed_mm + turn_diff_filtered);

    MotorA.Target_Encoder = left_motor_speed;
    MotorB.Target_Encoder = right_motor_speed;

    if (total_corners > 0 && corner_count >= total_corners && state != STATE_STOP) {
        state = STATE_STOP;
        Flag_Stop = 1;
    }
}
