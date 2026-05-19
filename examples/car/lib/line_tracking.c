#include "line_tracking.h"
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "car.h"
#include "PID.h"
#include "key.h"

uint8_t corner;  
int8_t turn;
uint32_t turn_delay_time;
extern bool flag_start;
extern float RotationalA_target;
extern float RotationalB_target;




void line_tracking_init()
{

}

/*
 * 函数功能：读取7路灰度巡线传感器状态，计算并返回黑线中心位置
 * 返回值：line_t 结构体，包含传感器原始状态数组和计算出的平均位置
 */
line_t line_tracking_get_line_state()
{
    line_t temp = {0};  // 初始化临时结构体，所有成员置零

    /* 读取第6路传感器（最左侧或最右侧，具体取决于硬件定义） */
    if(DL_GPIO_readPins(SENSOR_PORT,  SENSOR_PIN_6_PIN) == 0){
        temp.point[0] = 1;   // 检测到黑线，置1
    }else{
        temp.point[0] = 0;   // 未检测到，置0
    }

    /* 读取第5路传感器 */
    if(DL_GPIO_readPins(SENSOR_PORT,  SENSOR_PIN_5_PIN) == 0){
        temp.point[1] = 1;
    }else{
        temp.point[1] = 0;
    }

    /* 读取第4路传感器 */
    if(DL_GPIO_readPins(SENSOR_PORT,  SENSOR_PIN_4_PIN) == 0){
        temp.point[2] = 1;
    }else{
        temp.point[2] = 0;
    }

    /* 读取第3路传感器 */
    if(DL_GPIO_readPins(SENSOR_PORT,  SENSOR_PIN_3_PIN) == 0){
        temp.point[3] = 1;
    }else{
        temp.point[3] = 0;
    }

    /* 读取第2路传感器 */
    if(DL_GPIO_readPins(SENSOR_PORT,  SENSOR_PIN_2_PIN) == 0){
        temp.point[4] = 1;
    }else{
        temp.point[4] = 0;
    }

    /* 读取第1路传感器 */
    if(DL_GPIO_readPins(SENSOR_PORT,  SENSOR_PIN_1_PIN) == 0){
        temp.point[5] = 1;
    }else{
        temp.point[5] = 0;
    }

    /* 读取第0路传感器（最右侧或最左侧） */
    if(DL_GPIO_readPins(SENSOR_PORT,  SENSOR_PIN_0_PIN) == 0){
        temp.point[6] = 1;
    }else{
        temp.point[6] = 0;
    }

    /* 遍历7个传感器，计算黑线的加权平均位置 */
    for(uint8_t i=0;  i<7;  i++){
        if(temp.point[i] == 0){       // 当前传感器未检测到黑线（0表示无效/白线）
            temp.pos += i-3;          // 累加相对于中间传感器的偏移量（传感器编号0~6，中心为3）
            temp.size++;              // 统计参与计算的传感器数量
        }
    }

    temp.pos = temp.pos/temp.size;    // 计算平均偏移位置，得到黑线中心相对于中间传感器的坐标

    return temp;                      // 返回状态及位置结果
}

/*
 * 函数功能：根据当前传感器状态调整小车运动状态，实现简单的线跟踪控制逻辑
 */

void line_tracking_run()
{
    //状态
    static enum {
		state_start,
        state_forward,
        state_turn,
        state_stop,
    }state = state_stop; 
	//读取按键状态，计算转弯圈数
	key_t circle_num = key_read();
    uint8_t turn_num = 4*circle_num.key_turn_press_num;

    //判断按键状态
	if(key_start_is_press())
	{
		state=state_start;
	}
	line_t line;

    //状态机实现线跟踪控制逻辑
	switch (state)
	{

        //开始状态，蜂鸣器响起，延时500ms后进入前进状态
		case state_start:
			buzzer_on();
			delay_ms(500);
			buzzer_off();
			flag_start=1;
			state=state_forward;
			
			break;
		
		//前进状态，当3个以上传感器检测到黑线时进入转弯状态，否则根据黑线位置调整小车偏转
		case state_forward:
			car_run(-80);
			line=line_tracking_get_line_state();
			car_deflection(-line.pos);
            if(line.size>=3)
            {
                state=state_turn;
            }
            else
            {
                car_deflection(-line.pos);
            }

			break;
		
		//转弯状态，当黑线位置回到中心附近时进入前进状态，否则继续转弯
		case state_turn:
            car_left();
            if(line.pos<=1||line.pos>=-1)
            {
                state=state_forward;
            }
            else
            {
                state=state_turn;
            }
			break;
		
		//停止状态，速度调节为0，等待下一次按键触发
		case state_stop:
            car_stop();
			break;
		
		//默认状态
		default :
			state=state_stop;
	}
	
	
}