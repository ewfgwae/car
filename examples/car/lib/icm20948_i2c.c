
#include "icm20948.h"
#include "icm20948_i2c.h"


icm20948_status_e icm20948_internal_write_i2c(uint8_t reg, uint8_t *data, uint32_t len, void *user)
{
	icm20948_status_e status = ICM_20948_STAT_OK;
	icm20948_config_i2c_t *args = (icm20948_config_i2c_t*)user;
	uint8_t addr8 = args->i2c_addr << 1;   // 7-bit addr -> 8-bit addr (R/W bit will be set by WriteAddress)

	I2C_Start();
	I2C_WriteAddress(addr8, I2C_RW_WRITE);
	if(I2C_Wait_ACK() != ACK) { I2C_Stop(); return ICM_20948_STAT_ERR; }

	I2C_WriteByte(reg);
	if(I2C_Wait_ACK() != ACK) { I2C_Stop(); return ICM_20948_STAT_ERR; }

	for(uint32_t i = 0; i < len; i++)
	{
		I2C_WriteByte(data[i]);
		if(I2C_Wait_ACK() != ACK) { I2C_Stop(); return ICM_20948_STAT_ERR; }
	}

	I2C_Stop();
	return status;
}

icm20948_status_e icm20948_internal_read_i2c(uint8_t reg, uint8_t *buff, uint32_t len, void *user)
{
	icm20948_status_e status = ICM_20948_STAT_OK;
	icm20948_config_i2c_t *args = (icm20948_config_i2c_t*)user;
	uint8_t addr8 = args->i2c_addr << 1;

	// Phase 1: Send register address
	I2C_Start();
	I2C_WriteAddress(addr8, I2C_RW_WRITE);
	if(I2C_Wait_ACK() != ACK) { I2C_Stop(); return ICM_20948_STAT_ERR; }

	I2C_WriteByte(reg);
	if(I2C_Wait_ACK() != ACK) { I2C_Stop(); return ICM_20948_STAT_ERR; }

	// Phase 2: Repeated start + read data
	I2C_Start();
	I2C_WriteAddress(addr8, I2C_RW_READ);
	if(I2C_Wait_ACK() != ACK) { I2C_Stop(); return ICM_20948_STAT_ERR; }

	for(uint32_t i = 0; i < len; i++)
	{
		buff[i] = I2C_ReadByte();
		if(i < len - 1)
		{
			I2C_ACK();      // ACK for all bytes except last
		}
		else
		{
			I2C_NACK();     // NACK on last byte to signal end of read
		}
	}

	I2C_Stop();
	return status;
}

/* default serif */
icm20948_serif_t default_serif = {
    icm20948_internal_write_i2c,
    icm20948_internal_read_i2c,
    NULL,
};

void icm20948_init_i2c(icm20948_device_t *icm_device, icm20948_config_i2c_t *args)
{
	Soft_I2C_Init();
	icm20948_init_struct(icm_device);
	default_serif.user = (void *)args;
    icm20948_link_serif(icm_device, &default_serif);

#ifdef CONFIG_ICM_20948_USE_DMP
  icm_device->_dmp_firmware_available = true;
#else
  icm_device->_dmp_firmware_available = false;
#endif

    icm_device->_firmware_loaded = false;
    icm_device->_last_bank = 255;
    icm_device->_last_mems_bank = 255;
    icm_device->_gyroSF = 0;
    icm_device->_gyroSFpll = 0;
    icm_device->_enabled_Android_0 = 0;
    icm_device->_enabled_Android_1 = 0;
    icm_device->_enabled_Android_intr_0 = 0;
    icm_device->_enabled_Android_intr_1 = 0;

}
