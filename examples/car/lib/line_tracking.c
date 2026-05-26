#include "line_tracking.h"
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "car.h"
#include "PID.h"
#include "key.h"
#include "control.h"

Motor_parameter MotorA,MotorB;				//左右电机相关变量
uint32_t ir_dh0_state, ir_dh1_state, ir_dh2_state, ir_dh3_state, ir_dh4_state;
/*=============================================================================
 * 可调参数区域
 *=============================================================================*/
// 转向角度参数
float Turn90Angle  = 90;   // 直角弯转向参数
float TurnMaxAngle = 45;   // 大弯道转向参数
float TurnMidAngle = 20;   // 中等转向参数（丢线时使用）
float TurnMinAngle = 15;   // 微调转向参数
// 速度参数
float BaseSpeed = 200;      // 基础巡线速度（直行时的速度）
float ForwardLimit = 90;		//前行限制(转向大于该值限制其前进)
/*=============================================================================
 * 传感器状态定义--识别到黑线时为1
 *=============================================================================*/
typedef enum {
    STATE_CROSS         = 31,   // 11111 - 十字路口
    STATE_LEFT_90_A     = 30,   // 11110 - 左直角弯
    STATE_LEFT_90_B     = 28,   // 11100 - 
    STATE_LEFT_BIG      = 16,   // 10000 - 左大弯
    STATE_LEFT_SMALL    = 8 ,   // 01000 - 左微调
    STATE_RIGHT_90_A    = 15,   // 01111 - 右直角弯
    STATE_RIGHT_90_B    = 7 ,   // 00111 - 
    STATE_RIGHT_BIG     = 1 ,   // 00001 - 右大弯
    STATE_RIGHT_SMALL   = 2 	 ,   // 00010 - 右微调
    STATE_STRAIGHT      = 4 ,   // 00100 - 直行
    STATE_LOST          = 0     // 00000 - 丢线
} SensorState_t;
float base_speed_mm = 0;        // 基础速度（mm/s）
float turn_diff = 0;            // 转向差速


/*=============================================================================
 * 巡线功能函数（输出两电机目标速度）
 *=============================================================================*/
void IRDM_line_inspection(void)
{
    static int last_state = 0;      // 记录上一次的状态
	float left_motor_speed = 0;     // 左电机临时速度（m/s）
    float right_motor_speed = 0;    // 右电机临时速度（m/s）
	static int turn_cnt=0;
	static int saved_state = 0;  // 保存转向状态
	static int stop_turn_cnt = 0; 
    // 读取传感器状态：4个传感器组合值
	    // 读取四个引脚的状态并强制转换为0或1
    ir_dh4_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_4_PIN) ? 1 : 0;
    ir_dh3_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_3_PIN) ? 1 : 0;
    ir_dh2_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_2_PIN) ? 1 : 0;
    ir_dh1_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_1_PIN) ? 1 : 0;
    ir_dh0_state = DL_GPIO_readPins(SENSOR_PORT, SENSOR_PIN_0_PIN) ? 1 : 0;
	
    int sensor_state =(ir_dh0_state << 4) | (ir_dh1_state << 3) | (ir_dh2_state << 2) | (ir_dh3_state << 1) | ir_dh4_state; // 将传感器状态组合成一个整数
	p_s("sensor_state=%d\r\n",sensor_state);
    // 直角弯两段式处理：前200次直行，后转向
    if((sensor_state == STATE_LEFT_90_A || sensor_state == STATE_RIGHT_90_A||sensor_state == STATE_LEFT_90_B || sensor_state == STATE_RIGHT_90_B) && turn_cnt == 0)
    {
        saved_state = sensor_state;  // 记住转向状态
        turn_cnt = 1;
		stop_turn_cnt++;
		if(stop_turn_cnt >= 4) {
			Flag_Stop = 1;
			stop_turn_cnt=0;
		}
    }
    if(turn_cnt > 0)
    {
        if(turn_cnt < 50) sensor_state = STATE_STRAIGHT;  
		else if(turn_cnt >= 50&&turn_cnt < 150) sensor_state = STATE_LEFT_90_A                          ;	
        else if(turn_cnt < 4000&&sensor_state!=STATE_LEFT_BIG&&sensor_state!=STATE_RIGHT_BIG) sensor_state = saved_state; 
        else { turn_cnt = 0; saved_state = 0; }  
        if(turn_cnt > 0) turn_cnt++;
    }
  /*=========================================================================*
     * 状态判断：设置转向差速												   *
     *=========================================================================*/
    switch (sensor_state)
    {
        case STATE_CROSS:// 交叉路口处理
			turn_diff = 0;
            break;
        case STATE_LEFT_90_A: // 左直角弯
		case STATE_LEFT_90_B: // 左直角弯
            turn_diff = Turn90Angle;
            break;
        case STATE_RIGHT_90_A: // 右直角弯
		case STATE_RIGHT_90_B: // 右直角弯
            turn_diff = -Turn90Angle;
            break;
        case STATE_LEFT_SMALL://左微调
            turn_diff = TurnMinAngle;
            break;
        case STATE_RIGHT_SMALL://右微调
            turn_diff = -TurnMinAngle;
            break;
        case STATE_STRAIGHT://直行
            turn_diff = 0;
            break;
        case STATE_LOST://丢线处理
            if (last_state == STATE_LEFT_SMALL) turn_diff = TurnMidAngle;		//继续左转
			else if (last_state == STATE_RIGHT_SMALL) turn_diff = -TurnMidAngle;//继续右转
			else if(last_state == STATE_LEFT_BIG ) turn_diff = TurnMaxAngle;	//继续左转
			else if(last_state == STATE_RIGHT_BIG ) turn_diff = -TurnMaxAngle;  //继续右转
            break;
        default: // 未定义状态，直行
            turn_diff = 0;
            break;
    }
	//保存传感器状态
	if(sensor_state!=STATE_LOST)
	{
		last_state=sensor_state;
	}
    // 转向速度越大，基础速度越低
	if(fabs(turn_diff)<ForwardLimit)
	{
		base_speed_mm = BaseSpeed - (BaseSpeed * (fabs(turn_diff) / ForwardLimit));
	}
	else base_speed_mm=0;
    /*========================================================================*
     * 设置电机目标速度（左-转向差速，右+转向差速，单位：mm/s）                   *
     *=========================================================================*/
	left_motor_speed = 0.001f * (base_speed_mm - turn_diff); 
    right_motor_speed = 0.001f * (base_speed_mm + turn_diff);
    // 赋值给电机目标速度
    MotorA.Target_Encoder = left_motor_speed;//左电机
    MotorB.Target_Encoder = right_motor_speed;//右电机
}






