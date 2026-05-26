#include "ti_msp_dl_config.h"
#include "motor.h"
#include "delay.h"
#include "stdio.h"
#include "line_tracking.h"
#include "car.h"
#include "key.h"
#include "encoder.h"
#include "soft_i2c.h"
#include "icm20948.h"
#include "task_imu.h"
#include "control.h"

volatile float MA_RPM = 0.0f, MB_RPM = 0.0f;

float RotationalA_target = 0;
float RotationalB_target = 0;
static icm20948_device_t icm;
static icm20948_config_i2c_t icm_config = {
	.i2c_addr = ICM_20948_I2C_ADDR_AD0
};

static icm20948_status_e result;
static icm_20948_DMP_data_t data;
static icm20948_status_e status;


int main(void)
{
    SYSCFG_DL_init();
	SYSCFG_DL_GPIO_init();
    Encoder_Init();
    DL_GPIO_setPins(BUZZER_PORT,  BUZZER_EN_PIN);
	delay_ms(1000);

    while (1) {
	}
}





