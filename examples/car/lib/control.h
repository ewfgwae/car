#ifndef __CONTROL_H
#define __CONTROL_H

#include "stdint.h"

extern int Sensor_Left,Sensor_Middle,Sensor_Right,Sensor;
#define EncoderMultiples  2				//编码器倍频数，取决于编码器初始化设置
#define CONTROL_FREQUENCY 200			//编码器数据读取频率
#define	Black_WheelDiameter   0.065f	//轮胎直径
#define Perimeter	0.204203519			//轮子周长(单位:m)
#define MOTOR_GEAR_RATIO       28.0f    // 电机减速比
#define ENCODER_RESOLUTION     13.0f    // 编码器线数
#define Wheelspacing 0.1610f		//主动轮轴距(单位:m)
#define PI 3.1415926

extern int volatile Get_Encoder_countA,Get_Encoder_countB;

typedef struct  
{
	float Current_Encoder;     	//编码器数值，读取电机实时速度
	float Motor_Pwm;     		//电机PWM数值，控制电机实时速度
	float Target_Encoder;  		//电机目标编码器速度值，控制电机目标速度
	float Velocity; 	 		//电机速度值
}Motor_parameter;

//编码器结构体
typedef struct  
{
  int A;      
  int B;  
}Encoder;
extern float Move_X,Move_Z;						//目标速度和目标转向速度
extern Encoder OriginalEncoder; 					//编码器原始数据   
extern Motor_parameter MotorA,MotorB;				//左右电机相关变量
extern uint8_t Flag_Stop;


void Get_Velocity_From_Encoder(int Encoder1,int Encoder2);
void Get_Target_Encoder(float Vx,float Vz);
int Incremental_PI_Right (float Encoder,float Target);
int Incremental_PI_Left (float Encoder,float Target);



#endif


