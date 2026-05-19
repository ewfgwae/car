#ifndef _ICM_20948_I2C_H_
#define _ICM_20948_I2C_H_

#include "soft_i2c.h"
#include "icm20948.h"

typedef struct
{
	uint8_t i2c_addr;   // 7-bit I2C slave address (e.g. 0x68 or 0x69)
} icm20948_config_i2c_t;


void icm20948_init_i2c(icm20948_device_t *device, icm20948_config_i2c_t *config);

/* these functions are exposed in order to make a custom setup of a serif_t possible */
icm20948_status_e icm20948_internal_write_i2c(uint8_t reg, uint8_t *data, uint32_t len, void *user);
icm20948_status_e icm20948_internal_read_i2c(uint8_t reg, uint8_t *buff, uint32_t len, void *user);

#endif