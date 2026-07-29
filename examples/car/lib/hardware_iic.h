#include "ti_msp_dl_config.h"
#include "gw_grayscale_sensor.h"

unsigned char hardware_IIC_WirteByte(unsigned char Salve_Adress,unsigned char Reg_Address, unsigned char dat);
unsigned char hardware_IIC_WirteBytes(unsigned char Salve_Adress,unsigned char Reg_Address, unsigned char * dat, unsigned char len);
unsigned char hardware_IIC_ReadByte(unsigned char Salve_Adress,unsigned char Reg_Address);
unsigned char hardware_IIC_ReadBytes(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char *data,unsigned char len);

void GW_I2C_Init(void);
unsigned char Ping(void);
unsigned char IIC_Get_Digtal(void);
unsigned char IIC_Get_Anolog(unsigned char * Result,unsigned char len);
unsigned char IIC_Get_Single_Anolog(unsigned char Channel);
unsigned char IIC_Get_Normalize(unsigned char * Result,unsigned char len);
unsigned short IIC_Get_Offset(void );
unsigned char IIC_ReadByte(unsigned char Salve_Adress,unsigned char Reg_Address);
unsigned char IIC_ReadBytes(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char *Result,unsigned char len);
unsigned char IIC_WriteByte(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char data);
unsigned char IIC_WriteBytes(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char *data,unsigned char len);
void i2c_reset(void);
