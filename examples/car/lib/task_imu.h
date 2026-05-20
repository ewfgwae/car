#ifndef __TASK_IMU_H__
#define __TASK_IMU_H__

#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include "soft_i2c.h"
#include "icm20948.h"
#include "icm20948_i2c.h"
#include "delay.h"

void init_dmp(icm20948_device_t *icm);

void task_imu_init(void);
void task_imu_run(void);

#endif