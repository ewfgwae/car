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
	success &= (inv_icm20948_set_dmp_sensor_period(icm, DMP_ODR_Reg_Quat9, 0) == ICM_20948_STAT_OK); // Set to the maximum
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

bool task_imu_init(void)
{
    uint8_t whoami = 0;

    icm20948_init_i2c(&icm, &icm_config);

    while (icm20948_get_who_am_i(&icm, &whoami) != ICM_20948_STAT_OK || whoami != ICM_20948_WHOAMI) {
        delay_ms(100);
    }

    icm20948_sw_reset(&icm);
    delay_ms(250);

    icm20948_sleep(&icm, false);
    icm20948_low_power(&icm, false);

    icm20948_init_dmp_sensor_with_defaults(&icm);
    inv_icm20948_enable_dmp_sensor(&icm, INV_ICM20948_SENSOR_ORIENTATION, 1);
    inv_icm20948_set_dmp_sensor_period(&icm, DMP_ODR_Reg_Quat9, 0);
    icm20948_enable_fifo(&icm, true);
    icm20948_enable_dmp(&icm, 1);
    icm20948_reset_dmp(&icm);
    icm20948_reset_fifo(&icm);

    return true;
}

void task_imu_run(void)
{
    icm_20948_DMP_data_t data;

    while (1) {
        if (inv_icm20948_read_dmp_data(&icm, &data) == ICM_20948_STAT_OK) {
            if (data.header & DMP_header_bitmap_Quat9) {
                double q1 = (double)data.Quat9.Data.Q1 / 1073741824.0;
                double q2 = (double)data.Quat9.Data.Q2 / 1073741824.0;
                double q3 = (double)data.Quat9.Data.Q3 / 1073741824.0;
                double q0 = sqrt(1.0 - (q1 * q1 + q2 * q2 + q3 * q3));

                double pitch = atan2(2 * (q0 * q1 + q2 * q3), 1 - 2 * (q1 * q1 + q2 * q2)) * 180.0 / 3.14;
                double roll  = asin(2 * (q0 * q2 - q3 * q1)) * 180.0 / 3.14;
                double yaw   = atan2(2 * (q0 * q3 + q1 * q2), 1 - 2 * (q2 * q2 + q3 * q3)) * 180.0 / 3.14;

                p_s("I:%.2f,%.2f,%.2f\r\n", pitch, roll, yaw);
            }
        }
        delay_ms(10);
    }
}