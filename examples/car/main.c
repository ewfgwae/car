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

volatile float MA_RPM = 0.0f, MB_RPM = 0.0f;

float RotationalA_target = 0;
float RotationalB_target = 0;

int main(void)
{

    SYSCFG_DL_init();
	SYSCFG_DL_GPIO_init();
    Encoder_Init();
    DL_GPIO_setPins(BUZZER_PORT,  BUZZER_EN_PIN);

	JY931_Init();

    while (1) {
	}
}
