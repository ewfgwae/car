#include "motor.h"
#include "ti_msp_dl_config.h"
#include "math.h"
#include "PID.h"
#include "delay.h"


static PID_t pidA = {
	.kd = 0,
	.ki = 0.4,
	.kp = 25,
};

static PID_t pidB = {
	.kd = 0,
	.ki = 0.4,
	.kp = 25,
};

void motor_init()
{
    DL_TimerA_startCounter(TIMA0);
    motor_set_a_speed(0);
    motor_set_b_speed(0);
}



void motor_set_a_speed(int32_t speed)
{
    if(speed > 0){
        DL_GPIO_setPins(MOTOR_PORT,  MOTOR_A_1_PIN);
        DL_GPIO_clearPins(MOTOR_PORT,  MOTOR_A_2_PIN);
    }else{
        DL_GPIO_clearPins(MOTOR_PORT,  MOTOR_A_1_PIN);
        DL_GPIO_setPins(MOTOR_PORT,  MOTOR_A_2_PIN);
        speed = -speed;
    }
    
    if(speed > 6000){
			speed = 6000;
		}

    DL_Timer_setCaptureCompareValue(TIMA0,  speed,  GPIO_PWM_0_C0_IDX);
}

void motor_set_b_speed(int32_t speed)
{
    if(speed > 0){
        DL_GPIO_clearPins(MOTOR_PORT,  MOTOR_B_1_PIN);
        DL_GPIO_setPins(MOTOR_PORT,  MOTOR_B_2_PIN);
    }else{
        
        DL_GPIO_setPins(MOTOR_PORT,  MOTOR_B_1_PIN);
        DL_GPIO_clearPins(MOTOR_PORT,  MOTOR_B_2_PIN);
        speed = -speed;
    }
    
		if(speed > 6000){
			speed = 6000;
		}
		
    DL_Timer_setCaptureCompareValue(TIMA0,  speed,  GPIO_PWM_0_C1_IDX);
}


void motor_set_a_Rotational_speed(uint8_t Rotational)
{
	uint8_t motor_a_Rotational_speed = Rotational;
	
}


void motor_set_b_Rotational_speed(uint8_t Rotational)
{
	uint8_t motor_b_Rotational_speed = Rotational;
}

void motorA_Rotational_speed_Handle(float target,float actual)
{
	PID_Incremental(&pidA, target, actual);
	motor_set_a_speed(-(int32_t)pidA.out);
//	p_s("pidA.out=%.2f\r\n",pidA.out);
}

void motorB_Rotational_speed_Handle(float target,float actual)
{
	PID_Incremental(&pidB, target, actual);
	motor_set_b_speed(-(int32_t)pidB.out);
//	p_s("MB_RPM:%.2lf\r\n",actual);
//	p_s("pidB.out:%.2f\r\n\r\n",pidB.out);
	p_s("I:%.2f,%.2f\r\n",actual, pidB.out);
}
