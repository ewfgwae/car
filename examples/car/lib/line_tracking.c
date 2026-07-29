#include "line_tracking.h"
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "car.h"
#include "PID.h"
#include "key.h"
#include "control.h"
#include "hardware_iic.h"


Motor_parameter MotorA, MotorB;

/*=============================================================================
 * 可调参数区域
 *=============================================================================*/
float Turn90Angle  = 125;
float ForwardLimit = 128;
float TurnMinAngle = 15;
float TurnBigAngle = 15;
float BaseSpeed = 340;

// PID参数
PID_t line_pid;
float Line_Kp = 20.0f;
float Line_Ki = 0.0f;
float Line_Kd = 35.0f;

float RAMP_LIMIT = 20.0f;

/*=============================================================================
 * 8路传感器状态定义（识别到黑线为1）
 * bit7=最左探头, bit0=最右探头
 *=============================================================================*/
typedef enum {
	// 左转直角弯
	STATE_LEFT_90_A     = 0xFE,	//11111110
	STATE_LEFT_90_B     = 0xFC,	//11111100
	STATE_LEFT_90_C     = 0xF8,	//11111000
	STATE_LEFT_90_D     = 0xF0,	//11110000
	STATE_LEFT_90_E     = 0xE0,	//11100000
	STATE_LEFT_90_F     = 0xC0,	//11000000
	STATE_LEFT_90_G     = 0x80,	//10000000

	// 小调左转
	STATE_LEFT_SMALL_B  = 0x60,	//01100000
	STATE_LEFT_SMALL_A  = 0x50,	//01010000
	STATE_LEFT_SMALL    = 0x20,	//00100000

	// 直线
	STATE_STRAIGHT      = 0x18,	//00011000
	STATE_STRAIGHT_4    = 0x10,	//00010000
	STATE_STRAIGHT_3    = 0x08,	//00001000

	// 小调右转
	STATE_RIGHT_SMALL   = 0x04,	//00000100
	STATE_RIGHT_SMALL_B = 0x0A,	//00001010
	STATE_RIGHT_SMALL_C = 0x06,	//00000110

	// 右转直角弯
	STATE_RIGHT_90_G    = 0x01,	//00000001
	STATE_RIGHT_90_F    = 0x03,	//00000011
	STATE_RIGHT_90_E    = 0x07,	//00000111
	STATE_RIGHT_90_D    = 0x0F,	//00001111
	STATE_RIGHT_90_C    = 0x1F,	//00011111
	STATE_RIGHT_90_B    = 0x3F,	//00111111
	STATE_RIGHT_90_A    = 0x7F,	//01111111

	STATE_LOST          = 0x00	//00000000
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
static uint16_t corner_period_cnt = 0;
#define CORNER_DEBOUNCE_CNT  700 

/*=============================================================================
 * 巡线主函数（8路I2C灰度传感器版）
 *=============================================================================*/
void IRDM_line_inspection(void)
{
	static LineState_t state = STATE_FORWARD;
	static int last_state = 0;
	static uint8_t pid_inited = 0;
	static float turn_diff_last = 0;
	static float d_filtered = 0;
	float left_motor_speed, right_motor_speed;
	int8_t num = key_get_turn_num();
	if (num >= 1) {
		total_corners = num * 4;
	}

	if (!pid_inited) {
		line_pid.error = line_pid.error_i = line_pid.error_last1 = line_pid.error_last2 = 0;
		line_pid.p = line_pid.i = line_pid.d = line_pid.out = 0;
		line_pid.kp = Line_Kp;
		line_pid.ki = Line_Ki;
		line_pid.kd = Line_Kd;
		pid_inited = 1;
	}

	if (corner_period_cnt < CORNER_DEBOUNCE_CNT) {
		corner_period_cnt++;
	}

	int sensor_state = IIC_Get_Digtal();

	switch (state)
	{
		case STATE_FORWARD:
		{
			if (sensor_state == STATE_LEFT_90_A || 
				sensor_state == STATE_LEFT_90_B ||
				sensor_state == STATE_LEFT_90_C ||
				sensor_state == STATE_LEFT_90_D ||
				sensor_state == STATE_LEFT_90_E ||
				sensor_state == STATE_LEFT_90_F ||
				sensor_state == STATE_LEFT_90_G)
			{
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
					 sensor_state == STATE_RIGHT_90_C ||
					 sensor_state == STATE_RIGHT_90_D ||
					 sensor_state == STATE_RIGHT_90_E ||
					 sensor_state == STATE_RIGHT_90_F ||
					 sensor_state == STATE_RIGHT_90_G)
			{
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
				case STATE_STRAIGHT_4:  line_error = 0.0f;  break;
				case STATE_STRAIGHT_3:  line_error = 0.0f;  break;
				case STATE_LEFT_SMALL:  line_error = 1.0f;  break;
				case STATE_LEFT_SMALL_A:line_error = 0.75f; break;
				case STATE_LEFT_SMALL_B:line_error = 0.5f;  break;
				case STATE_RIGHT_SMALL: line_error = -1.0f; break;
				case STATE_RIGHT_SMALL_B:line_error = -0.75f;break;
				case STATE_RIGHT_SMALL_C:line_error = -0.5f; break;
				case STATE_LOST:        valid_line = 0;     break;
				default:                line_error = 0.0f;  break;
			}

			if (valid_line) {
				float error = -line_error;
				float p_out = error * line_pid.kp;
				float i_out = 0;
				float d_raw = (error - line_pid.error_last1) * line_pid.kd;
				d_filtered = 0.5f * d_filtered + 0.5f * d_raw;
				float d_out = d_filtered;
				line_pid.error_last1 = error;
				turn_diff = -(p_out + i_out + d_out);
			} else {
				line_pid.error_last1 = 0;
				d_filtered = 0;
				if (last_state == STATE_LEFT_SMALL ||
					last_state == STATE_LEFT_SMALL_A ||
					last_state == STATE_LEFT_SMALL_B)
					turn_diff = TurnMinAngle;
				else if (last_state == STATE_RIGHT_SMALL ||
						 last_state == STATE_RIGHT_SMALL_B ||
						 last_state == STATE_RIGHT_SMALL_C)
					turn_diff = -TurnMinAngle;
				else
					turn_diff = 0;
			}
			break;
		}

		case STATE_TURN:
		{
			turn_diff = turn_direction * Turn90Angle;
			d_filtered = 0;

			if (sensor_state == STATE_STRAIGHT ||
				sensor_state == STATE_STRAIGHT_4 ||
				sensor_state == STATE_STRAIGHT_3 ||
				sensor_state == STATE_LEFT_SMALL)
			{
				state = STATE_FORWARD;
				line_pid.error_last1 = 0;
				d_filtered = 0;
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

	float turn_diff_filtered = turn_diff;
	float delta = turn_diff - turn_diff_last;

	if (delta > RAMP_LIMIT) {
		delta = RAMP_LIMIT;
	} else if (delta < -RAMP_LIMIT) {
		delta = -RAMP_LIMIT;
	}
	turn_diff_filtered = turn_diff_last + delta;
	turn_diff_last = turn_diff_filtered;
	
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
