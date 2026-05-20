#include "task_imu.h"
#include "icm20948.h"

static icm20948_device_t icm;
static icm20948_config_i2c_t icm_config = { .i2c_addr = ICM_20948_I2C_ADDR_AD0 };


void init_dmp(icm20948_device_t *icm)
{
	p_s("start\r\n");
  	bool success = true; // Use success to show if the DMP configuration was successful

  	// Initialize the DMP with defaults.
  	success &= (icm20948_init_dmp_sensor_with_defaults(icm) == ICM_20948_STAT_OK);
	// DMP sensor options are defined in ICM_20948_DMP.h
	//    INV_ICM20948_SENSOR_ACCELEROMETER               (16-bit accel)
	//    INV_ICM20948_SENSOR_GYROSCOPE                   (16-bit gyro + 32-bit calibrated gyro)
	//    INV_ICM20948_SENSOR_RAW_ACCELEROMETER           (16-bit accel)
	//    INV_ICM20948_SENSOR_RAW_GYROSCOPE               (16-bit gyro + 32-bit calibrated gyro)
	//    INV_ICM20948_SENSOR_MAGNETIC_FIELD_UNCALIBRATED (16-bit compass)
	//    INV_ICM20948_SENSOR_GYROSCOPE_UNCALIBRATED      (16-bit gyro)
	//    INV_ICM20948_SENSOR_STEP_DETECTOR               (Pedometer Step Detector)
	//    INV_ICM20948_SENSOR_STEP_COUNTER                (Pedometer Step Detector)
	//    INV_ICM20948_SENSOR_GAME_ROTATION_VECTOR        (32-bit 6-axis quaternion)
	//    INV_ICM20948_SENSOR_ROTATION_VECTOR             (32-bit 9-axis quaternion + heading accuracy)
	//    INV_ICM20948_SENSOR_GEOMAGNETIC_ROTATION_VECTOR (32-bit Geomag RV + heading accuracy)
	//    INV_ICM20948_SENSOR_GEOMAGNETIC_FIELD           (32-bit calibrated compass)
	//    INV_ICM20948_SENSOR_GRAVITY                     (32-bit 6-axis quaternion)
	//    INV_ICM20948_SENSOR_LINEAR_ACCELERATION         (16-bit accel + 32-bit 6-axis quaternion)
	//    INV_ICM20948_SENSOR_ORIENTATION                 (32-bit 9-axis quaternion + heading accuracy)

	// Enable the DMP orientation sensor
	success &= (inv_icm20948_enable_dmp_sensor(icm, INV_ICM20948_SENSOR_ORIENTATION, 1) == ICM_20948_STAT_OK);

	// Enable any additional sensors / features
	//success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_RAW_GYROSCOPE) == ICM_20948_STAT_OK);
	//success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_RAW_ACCELEROMETER) == ICM_20948_STAT_OK);
	//success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_MAGNETIC_FIELD_UNCALIBRATED) == ICM_20948_STAT_OK);

	// Configuring DMP to output data at multiple ODRs:
	// DMP is capable of outputting multiple sensor data at different rates to FIFO.
	// Setting value can be calculated as follows:
	// Value = (DMP running rate / ODR ) - 1
	// E.g. For a 5Hz ODR rate when DMP is running at 55Hz, value = (55/5) - 1 = 10.
	success &= (inv_icm20948_set_dmp_sensor_period(icm, DMP_ODR_Reg_Quat9, 2) == ICM_20948_STAT_OK); // Set to the maximum
	//success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Accel, 0) == ICM_20948_STAT_OK); // Set to the maximum
	//success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Gyro, 0) == ICM_20948_STAT_OK); // Set to the maximum
	//success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Gyro_Calibr, 0) == ICM_20948_STAT_OK); // Set to the maximum
	//success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Cpass, 0) == ICM_20948_STAT_OK); // Set to the maximum
	//success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Cpass_Calibr, 0) == ICM_20948_STAT_OK); // Set to the maximum
	// Enable the FIFO
	success &= (icm20948_enable_fifo(icm, true) == ICM_20948_STAT_OK);
	// Enable the DMP
	success &= (icm20948_enable_dmp(icm, 1) == ICM_20948_STAT_OK);
	// Reset DMP
	success &= (icm20948_reset_dmp(icm) == ICM_20948_STAT_OK);
	// Reset FIFO
	success &= (icm20948_reset_fifo(icm) == ICM_20948_STAT_OK);

	// Check success
	if (success)
	{
		p_s("DMP enabled!\r\n");
	} else {
		p_s("Enable DMP failed!\r\n");

	}	
}

void task_imu_init(void)
{
    static icm20948_device_t icm;
    static icm20948_status_e result;
    static icm_20948_DMP_data_t data;
	static icm20948_status_e status;
	
	/* 设置 ICM20948 设备 */
	icm20948_init_i2c(&icm, &icm_config);
		
	/* 检查 ID */
    while ((result = icm20948_check_id(&icm)) != ICM_20948_STAT_OK)
	{
		p_s("check id failed, %d\r\n", result);
		delay_ms( 1000);
    }
	p_s("check id passed\r\n");

	/* 检查 whoami */
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
	delay_ms( 250);

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
    while(1) {
        // 读取 FIFO 中等待的任何 DMP 数据
        // 注意：
        //    readDMPdataFromFIFO 若无数据可用，将返回 ICM_20948_STAT_FIFO_NO_DATA_AVAIL
        //    若有数据可用，readDMPdataFromFIFO 将尝试读取 _一帧_ DMP 数据
        //    若帧存在但不完整，readDMPdataFromFIFO 将返回 ICM_20948_STAT_FIFO_INCOMPLETE_DATA
        //    若成功读取有效帧，readDMPdataFromFIFO 将返回 ICM_20948_STAT_OK
        //    若成功读取有效帧 _且_ FIFO 中还有更多（未读）数据，readDMPdataFromFIFO 将返回 ICM_20948_STAT_FIFO_MORE_DATA_AVAIL
        icm_20948_DMP_data_t data;
        icm20948_status_e status = inv_icm20948_read_dmp_data(&icm, &data);
        /* 是否有有效数据可用？ */
        if ((status == ICM_20948_STAT_OK) || (status == ICM_20948_STAT_FIFO_MORE_DATA_AVAIL)) 
        {
            /* 我们请求的是方向数据，因此应该收到 Quat9 */
            if ((data.header & DMP_header_bitmap_Quat9) > 0) 
            {
                // Q0 值由以下方程计算得出：Q0^2 + Q1^2 + Q2^2 + Q3^2 = 1。
                // 如果存在漂移，总和将不等于 1，因此四元数数据需要用正确的偏置值进行校正。
                // 四元数数据按 2^30 缩放。
                // 缩放至 +/- 1 范围
                double q1 = ((double)data.Quat9.Data.Q1) / 1073741824.0; // 转换为双精度浮点数，除以 2^30
                double q2 = ((double)data.Quat9.Data.Q2) / 1073741824.0; // 转换为双精度浮点数，除以 2^30
                double q3 = ((double)data.Quat9.Data.Q3) / 1073741824.0; // 转换为双精度浮点数，除以 2^30
                double q0 = sqrt(1.0 - ((q1 * q1) + (q2 * q2) + (q3 * q3)));

                double pitch = atan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2));
                double roll  = asin(2*(q0*q2 - q3*q1));
                double yaw   = atan2(2*(q0*q3 + q1*q2), 1 - 2*(q2*q2 + q3*q3));

                // 转为度数
                pitch *= 180.0 / 3.14;
                roll  *= 180.0 / 3.14;
                yaw   *= 180.0 / 3.14;
//                p_s("Q0: %f Q1: %f Q2: %f Q3: %f Accuracy: %d\r\n", q0, q1, q2, q3, data.Quat9.Data.Accuracy);
//                p_s("pitch:%.2f roll:%.2f yaw:%.2f \r\n", pitch, roll, yaw);
                p_s("I:%.2f,%.2f,%.2f\r\n", pitch, roll, yaw);
				
            }
        }
        if(status != ICM_20948_STAT_FIFO_MORE_DATA_AVAIL) {
            delay_ms(10);
        }
    }
}

