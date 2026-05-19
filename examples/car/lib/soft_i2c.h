#ifndef __SOFT_I2C_H_
#define __SOFT_I2C_H_

#include "ti_msp_dl_config.h"
#include "delay.h"


#define I2C_SDA_GPIO_PORT    I2C_SDA_Port
#define I2C_SCL_GPIO_PORT    I2C_SCL_Port

typedef enum {
    LOW = 0,
    HIGH
} I2C_Bus_Status_t;

typedef enum {
    ACK = 0,
    NACK
}  I2C_Responed_t;

typedef enum {
    I2C_RW_WRITE = 0,
    I2C_RW_READ
}  I2C_RW_t; 

#define I2C_Delay() for(uint16_t  i=0; i<16; i++)


static inline void Soft_SDA_OUT(void) {
    DL_GPIO_initDigitalOutput(I2C_SDA_IOMUX);
    DL_GPIO_setPins(I2C_PORT, I2C_SDA_PIN);	   
    DL_GPIO_enableOutput(I2C_PORT, I2C_SDA_PIN); 
}

static inline void Soft_SDA_IN(void) {
    DL_GPIO_initDigitalInputFeatures(I2C_SDA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}

static inline void I2C_SCL_OUTPUT_HIGH(void) {
    DL_GPIO_setPins(I2C_PORT, I2C_SCL_PIN);
    delay_us(10);
}

static inline void I2C_SCL_OUTPUT_LOW(void) {
    DL_GPIO_clearPins(I2C_PORT, I2C_SCL_PIN);
    delay_us(10);
}

static inline void I2C_SDA_OUTPUT_HIGH(void) {
    Soft_SDA_OUT();
    DL_GPIO_setPins(I2C_PORT, I2C_SDA_PIN);
    delay_us(10);
}

static inline void I2C_SDA_OUTPUT_LOW(void) {
    Soft_SDA_OUT();
    DL_GPIO_clearPins(I2C_PORT, I2C_SDA_PIN);
    delay_us(10);
}

static inline uint8_t I2C_SDA_Read(void) {
    uint32_t BitValue;
    Soft_SDA_IN();
    BitValue = DL_GPIO_readPins(I2C_PORT, I2C_SDA_PIN);
    delay_us(10);
    return BitValue ? 1 : 0;
}


void Soft_I2C_Init();

void I2C_Start();
void I2C_WriteByte(uint8_t byte);
void I2C_WriteAddress(uint8_t address,  I2C_RW_t rw);
uint8_t I2C_ReadByte();
void I2C_ACK();
void I2C_NACK();
I2C_Responed_t I2C_Wait_ACK();
void I2C_Stop();

#endif