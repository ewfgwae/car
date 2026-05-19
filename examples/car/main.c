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
	
	/* 检查 whoami */
    while ((result = icm20948_check_id(&icm)) != ICM_20948_STAT_OK)
	{
		p_s("check id failed, %d\r\n", result);
		delay_ms(1000);
    }
	p_s("check id passed\r\n");
	
	/* 这里执行软件复位，确保设备从一个已知状态启动 */
	icm20948_status_e stat = ICM_20948_STAT_ERR;
	uint8_t whoami = 0x00;
	while ((stat != ICM_20948_STAT_OK) || (whoami != ICM_20948_WHOAMI))
	{
		whoami = 0x00;
		stat = icm20948_get_who_am_i(&icm, &whoami);
		if ((stat != ICM_20948_STAT_OK) || (whoami != ICM_20948_WHOAMI))
		{
			p_s("whoami does not match (0x%02X, expected 0x%02X, stat=%d). Retrying...\r\n", whoami, ICM_20948_WHOAMI, stat);
			delay_ms( 1000);
		}
	}
	p_s("whoami matched: 0x%02X\r\n", whoami);
	/* 这里执行软件复位，确保设备从一个已知状态启动 */
	icm20948_sw_reset(&icm);
	delay_ms(250);

	icm20948_internal_sensor_id_bm sensors = (icm20948_internal_sensor_id_bm)(ICM_20948_INTERNAL_ACC | ICM_20948_INTERNAL_GYR);

	// 将陀螺仪和加速度计设置为特定的采样模式
	// 可选: SAMPLE_MODE_CONTINUOUS（连续模式）、SAMPLE_MODE_CYCLED（周期模式）
	icm20948_set_sample_mode(&icm, sensors, SAMPLE_MODE_CONTINUOUS); 

	// 设置加速度计和陀螺仪的满量程范围
	icm20948_fss_t myfss;
	myfss.a = GPM_2;   // (icm20948_accel_config_fs_sel_e)
	myfss.g = DPS_250; // (icm20948_gyro_config_1_fs_sel_e)
	icm20948_set_full_scale(&icm, sensors, myfss);

	// 设置 DLPF 配置
	icm20948_dlpcfg_t myDLPcfg;
	myDLPcfg.a = ACC_D473BW_N499BW;
	myDLPcfg.g = GYR_D361BW4_N376BW5;
	icm20948_set_dlpf_cfg(&icm, sensors, myDLPcfg);

	// 选择是否使用 DLPF
	icm20948_enable_dlpf(&icm, ICM_20948_INTERNAL_ACC, false);
	icm20948_enable_dlpf(&icm, ICM_20948_INTERNAL_GYR, false);

	// 现在唤醒传感器
	icm20948_sleep(&icm, false);
	icm20948_low_power(&icm, false);

	/* 现在开始 DMP 相关操作 */
	init_dmp(&icm);
	
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

