#include "ti_msp_dl_config.h"
#include "motor.h"
#include "delay.h"
#include "stdio.h"
#include "line_tracking.h"
#include "car.h"
#include "key.h"
#include "encoder.h"
#include "control.h"
#include "jy931.h"
#include "hardware_iic.h"
#include "OLED.h"

volatile float MA_RPM = 0.0f, MB_RPM = 0.0f;

float RotationalA_target = 0;
float RotationalB_target = 0;
uint8_t flag_oled = 0;
int main(void)
{

    SYSCFG_DL_init(); 
	GW_I2C_Init();
	Encoder_Init();
    OLED_Init();
	JY931_Init();
	OLED_Init();
	OLED_Clear();
	OLED_Update();
	while (Ping()) {
		delay_ms(10);
	}
                                                                                                                                                                                                  
    while (1) {
	
		
	
	if(flag_oled == 1) {
			flag_oled = 0;
			OLED_Clear();
			OLED_ShowNum(0, 0, run_seconds, 3, OLED_32X64);
			OLED_ShowString(96, 0, "S", OLED_32X64);
			OLED_Update();
		}
    }
}


void SysTick_Handler()
{
}

