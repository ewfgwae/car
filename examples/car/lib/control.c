#include "control.h"
#include "ti_msp_dl_config.h"
#include "line_tracking.h"
#include "motor.h"
#include "key.h"
#include "delay.h"
#include "jy931.h"
#include "hardware_iic.h"
#include "OLED.h"
#include "MyI2C.h"
#include "PID.h"

extern float Turn90Angle;
extern float ForwardLimit;
extern float TurnMinAngle;
extern float BaseSpeed;

extern PID_t line_pid;
extern float Line_Kp;
extern float Line_Ki;
extern float Line_Kd;

extern float RAMP_LIMIT;


uint8_t CCD_count,ELE_count;
int Sensor_Left,Sensor_Middle,Sensor_Right,Sensor;
Encoder OriginalEncoder;
bool flag_start=0;
uint32_t uiTick = 0;
uint8_t run_seconds = 0;
extern uint8_t flag_oled;
uint8_t i=0;
uint8_t key_get_num;
float Velocity_KP=6000,Velocity_KI=2000;	
int Run_Mode=1;
uint8_t Flag_Stop=1;

void TIMER_ENCODER_READ_INST_IRQHandler(void)
{
	i++;
	if (!Flag_Stop) {
		if (i >= 100) {
			run_seconds++;
			flag_oled = 1;
			i = 0;
		}
	} else {
		i = 0;
	}
	if(run_seconds>=10&&key_get_num==1)
	{
		BaseSpeed = 100.0f;

	}
	
	
	
	 if(DL_TimerA_getPendingInterrupt(TIMER_ENCODER_READ_INST))
		{
			key_read();
			if(Flag_Stop==0)
			{
				
			}
			else if (key_get_turn_num() == 1) {
				key_get_num=1;
				Turn90Angle  = 125.0f;
				ForwardLimit = 200.0f;
				TurnMinAngle = 15.0f;
				BaseSpeed    = 460.0f;
				Line_Kp = 85.0f;
				Line_Ki = 1.0f;
				Line_Kd = 20.0f;
				RAMP_LIMIT = 15.0f;
				Velocity_KP=5000;
				Velocity_KI=1200;
				
				OLED_Clear();
				OLED_ShowString(0, 0, "mod1", OLED_32X64);
				OLED_Update();
			} 
			else if (key_get_turn_num() == 2) {
				key_get_num=2;
				Turn90Angle  = 125.0f;
				ForwardLimit = 150.0f;
				TurnMinAngle = 15.0f;
				BaseSpeed    = 220.0f;
				Line_Kp = 60.0f;
				Line_Ki = 1.0f;
				Line_Kd = 25.0f;
				RAMP_LIMIT = 12.0f;
				Velocity_KP=3500;
				Velocity_KI=800;
				OLED_Clear();
				OLED_ShowString(0, 0, "mod2", OLED_32X64);
				OLED_Update();
			}
			if(key_start_is_press() && Flag_Stop)
			{
				Flag_Stop=0;
				run_seconds = 0;
				i = 0;
				g_start_ramp = 0.0f;	//mod2缓启动系数归零，重新缓启动
				g_stop_ramp = 1.0f;		//mod2缓停车系数复位
				g_restart = 1;			//通知巡线状态机复位到FORWARD
				Reset_Velocity_PID();	//速度环PID清零，避免启动冲击
			}

			
			
			Get_Velocity_From_Encoder(-Get_Encoder_countA,-Get_Encoder_countB);
			Get_Encoder_countA=Get_Encoder_countB=0;
			if(!Flag_Stop)
			{
				IRDM_line_inspection();
			}
			//计算左右电机对应的PWM
			MotorA.Motor_Pwm = Incremental_PI_Left(MotorA.Current_Encoder,MotorA.Target_Encoder);	
			MotorB.Motor_Pwm = Incremental_PI_Right(MotorB.Current_Encoder,-MotorB.Target_Encoder);
			if(!Flag_Stop)
			{
				Set_PWM(MotorA.Motor_Pwm,MotorB.Motor_Pwm);
			}
			else Set_PWM(0,0);
		}	
}

/**************************************************************************
Function: Get_Velocity_From_Encoder
Input   : none
Output  : none
函数功能：读取编码器和转换成速度
入口参数: 无 
返回  值：无
**************************************************************************/	 	
void Get_Velocity_From_Encoder(int Encoder1,int Encoder2)
{
	
	//Retrieves the original data of the encoder
	//获取编码器的原始数据
	float Encoder_A_pr,Encoder_B_pr; 
	OriginalEncoder.A=-Encoder1;	
	OriginalEncoder.B=-Encoder2;	
	Encoder_A_pr=-OriginalEncoder.A; Encoder_B_pr=-OriginalEncoder.B;
	//编码器原始数据转换为车轮速度，单位m/s
	MotorA.Current_Encoder= Encoder_A_pr*CONTROL_FREQUENCY*Perimeter/(EncoderMultiples*ENCODER_RESOLUTION*MOTOR_GEAR_RATIO);  
	MotorB.Current_Encoder= Encoder_B_pr*CONTROL_FREQUENCY*Perimeter/(EncoderMultiples*ENCODER_RESOLUTION*MOTOR_GEAR_RATIO);  
}
//运动学逆解，由x和y的速度得到编码器的速度,Vx是m/s,Vz单位是度/s(角度制)
void Get_Target_Encoder(float Vx,float Vz)
{
	float amplitude=3.5f; //Wheel target speed limit //车轮目标速度限幅
	if(Vx<0) Vz=-Vz;
	else     Vz=Vz;
	//Inverse kinematics //运动学逆解
	 MotorA.Target_Encoder = Vx - Vz * Wheelspacing / 2.0f; //计算出左轮的目标速度
	 MotorB.Target_Encoder = Vx + Vz * Wheelspacing / 2.0f; //计算出右轮的目标速度
}


/**************************************************************************
Function: PWM_Limit
Input   : IN;max;min
Output  : OUT
函数功能：限制PWM赋值
入口参数: IN：输入参数  max：限幅最大值  min：限幅最小值 
返回  值：限幅后的值
**************************************************************************/	 	
float PWM_Limit(float IN,float max,float min)
{
	float OUT = IN;
	if(OUT>max) OUT = max;
	if(OUT<min) OUT = min;
	return OUT;
}

/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
static float Bias_L, Pwm_L, Last_bias_L;
static float Bias_R, Pwm_R, Last_bias_R;

/* 重置增量式速度环PID状态，避免启动瞬间PID输出冲击 */
void Reset_Velocity_PID(void)
{
	Bias_L = Pwm_L = Last_bias_L = 0.0f;
	Bias_R = Pwm_R = Last_bias_R = 0.0f;
}

int Incremental_PI_Left (float Encoder,float Target)
{ 	
	 Bias_L=Target-Encoder;                					//计算偏差
	 Pwm_L+=Velocity_KP*(Bias_L-Last_bias_L)+Velocity_KI*Bias_L;   	//增量式PI控制器
	 if(Flag_Stop) Pwm_L=0;
	 if(Pwm_L>4000)Pwm_L=4000;
	 if(Pwm_L<-4000)Pwm_L=-4000;
	 Last_bias_L=Bias_L;	                   					//保存上一次偏差 
	 return Pwm_L;                         					//增量输出
}


int Incremental_PI_Right (float Encoder,float Target)
{ 	
	 Bias_R=Target-Encoder;                					//计算偏差
	 Pwm_R+=Velocity_KP*(Bias_R-Last_bias_R)+Velocity_KI*Bias_R;   	//增量式PI控制器
	if(Flag_Stop) Pwm_R=0;
	 if(Pwm_R>4000)Pwm_R=4000;
	 if(Pwm_R<-4000)Pwm_R=-4000;
	 Last_bias_R=Bias_R;	                   					//保存上一次偏差 
	 return Pwm_R;                         					//增量输出
}
