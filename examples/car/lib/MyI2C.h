#ifndef __MYI2C_H
#define __MYI2C_H
#include "ti_msp_dl_config.h"                  // Device header
#define I2C_SDA_PORT  I2C_PORT
#define I2C_SCL_PORT  I2C_PORT


void SDA_OUT(void);   

void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);

#endif
