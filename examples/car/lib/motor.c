#include "motor.h"
#include "ti_msp_dl_config.h"

#define ABS(a)      (a>0 ? a:(-a))




void Set_PWM(int pwmL,int pwmR)
{
	 if(pwmL>0)
    {
        DL_GPIO_setPins(MOTOR_PORT,MOTOR_A_2_PIN);
        DL_GPIO_clearPins(MOTOR_PORT,MOTOR_A_1_PIN);
		DL_Timer_setCaptureCompareValue(TIMA0,ABS(pwmL),GPIO_PWM_0_C0_IDX);
    }
    else
    {
        DL_GPIO_setPins(MOTOR_PORT,MOTOR_A_1_PIN);
        DL_GPIO_clearPins(MOTOR_PORT,MOTOR_A_2_PIN);
		DL_Timer_setCaptureCompareValue(TIMA0,ABS(pwmL),GPIO_PWM_0_C0_IDX);
    }
    if(pwmR>0)
    {
		DL_GPIO_setPins(MOTOR_PORT,MOTOR_B_2_PIN);
        DL_GPIO_clearPins(MOTOR_PORT,MOTOR_B_1_PIN);
        DL_Timer_setCaptureCompareValue(TIMA0,ABS(pwmR),GPIO_PWM_0_C1_IDX);
    }
    else
    {
		DL_GPIO_setPins(MOTOR_PORT,MOTOR_B_1_PIN);
        DL_GPIO_clearPins(MOTOR_PORT,MOTOR_B_2_PIN);
		DL_Timer_setCaptureCompareValue(TIMA0,ABS(pwmR),GPIO_PWM_0_C1_IDX);
    }
}

