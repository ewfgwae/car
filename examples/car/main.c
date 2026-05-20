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

volatile float MA_RPM = 0.0f, MB_RPM = 0.0f;
bool flag_start = 0;
float RotationalA_target = 0;
float RotationalB_target = 0;
static icm20948_device_t icm;
static icm20948_config_i2c_t icm_config = {
	.i2c_addr = ICM_20948_I2C_ADDR_AD0
};

static icm20948_device_t icm;
static icm20948_status_e result;
static icm_20948_DMP_data_t data;
static icm20948_status_e status;


int main(void)
{
    SYSCFG_DL_init();
    Encoder_Init();
    motor_init();
    line_tracking_init();
	icm20948_init_i2c(&icm, &icm_config);
	task_imu_init();
	
    while (1) {

	}
}

void TIMER_ENCODER_READ_INST_IRQHandler(void) {
    if (flag_start) {
        if (DL_TimerA_getPendingInterrupt(TIMER_ENCODER_READ_INST) & DL_TIMER_INTERRUPT_ZERO_EVENT) {
            MA_RPM = Calculate_Motor_RPM(Get_Encoder_countA, 10);
            MB_RPM = Calculate_Motor_RPM(Get_Encoder_countB, 10);
            motorA_Rotational_speed_Handle(RotationalA_target, MA_RPM);
            motorB_Rotational_speed_Handle(RotationalB_target, MB_RPM);
            Get_Encoder_countA = 0;
            Get_Encoder_countB = 0;
            DL_TimerA_clearInterruptStatus(TIMER_ENCODER_READ_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
        }
    }
}
void SysTick_Handler() {			
 key_read(); line_tracking_run(); }

