#include "delay.h"
#include "ti_msp_dl_config.h"
#include "MyI2C.h"

#define I2C_DELAY   delay_us(1)

static void SDA_OUT(void)
{
    DL_GPIO_enableOutput(I2C_SDA_PORT, I2C_SDA_PIN);
}

static void SDA_IN(void)
{
    DL_GPIO_setPins(I2C_SDA_PORT, I2C_SDA_PIN);
    DL_GPIO_disableOutput(I2C_SDA_PORT, I2C_SDA_PIN);
}

static void I2C_W_SCL(uint8_t val)
{
    if (val)
        DL_GPIO_setPins(I2C_SCL_PORT, I2C_SCL_PIN);
    else
        DL_GPIO_clearPins(I2C_SCL_PORT, I2C_SCL_PIN);
    I2C_DELAY;
}

static void I2C_W_SDA(uint8_t val)
{
    if (val)
        DL_GPIO_setPins(I2C_SDA_PORT, I2C_SDA_PIN);
    else
        DL_GPIO_clearPins(I2C_SDA_PORT, I2C_SDA_PIN);
    I2C_DELAY;
}

static uint8_t I2C_R_SDA(void)
{
    uint32_t v = DL_GPIO_readPins(I2C_SDA_PORT, I2C_SDA_PIN);
    I2C_DELAY;
    return v ? 1 : 0;
}

void MyI2C_Init(void)
{
    DL_GPIO_initDigitalOutput(I2C_SDA_IOMUX);
    DL_GPIO_initDigitalOutput(I2C_SCL_IOMUX);
    DL_GPIO_setPins(I2C_SDA_PORT, I2C_SDA_PIN);
    DL_GPIO_setPins(I2C_SCL_PORT, I2C_SCL_PIN);
    DL_GPIO_enableOutput(I2C_SDA_PORT, I2C_SDA_PIN);
    DL_GPIO_enableOutput(I2C_SCL_PORT, I2C_SCL_PIN);
}

void MyI2C_Start(void)
{
    SDA_OUT();
    I2C_W_SDA(1);
    I2C_W_SCL(1);
    I2C_W_SDA(0);
    I2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
    SDA_OUT();
    I2C_W_SDA(0);
    I2C_W_SCL(1);
    I2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    SDA_OUT();
    for (i = 0; i < 8; i++)
    {
        I2C_W_SDA(!!(Byte & (0x80 >> i)));
        I2C_W_SCL(1);
        I2C_W_SCL(0);
    }
    SDA_IN();
    I2C_W_SCL(1);
    I2C_W_SCL(0);
    SDA_OUT();
}

uint8_t MyI2C_SendByteGetAck(uint8_t Byte)
{
    uint8_t i, ack;
    SDA_OUT();
    for (i = 0; i < 8; i++)
    {
        I2C_W_SDA(!!(Byte & (0x80 >> i)));
        I2C_W_SCL(1);
        I2C_W_SCL(0);
    }
    SDA_IN();
    I2C_W_SCL(1);
    ack = I2C_R_SDA();
    I2C_W_SCL(0);
    SDA_OUT();
    return ack;
}

uint8_t MyI2C_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;
    SDA_OUT();
    I2C_W_SDA(1);
    for (i = 0; i < 8; i++)
    {
        SDA_IN();
        I2C_W_SCL(1);
        if (I2C_R_SDA()) { Byte |= (0x80 >> i); }
        I2C_W_SCL(0);
    }
    return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)
{
    SDA_OUT();
    I2C_W_SDA(AckBit);
    I2C_W_SCL(1);
    I2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
    uint8_t AckBit;
    SDA_OUT();
    I2C_W_SDA(1);
    I2C_W_SCL(1);
    SDA_IN();
    AckBit = I2C_R_SDA();
    I2C_W_SCL(0);
    SDA_OUT();
    return AckBit;
}

uint8_t MyI2C_CheckAddr(uint8_t addr)
{
    uint8_t ack;
    MyI2C_Start();
    ack = MyI2C_SendByteGetAck(addr);
    MyI2C_Stop();
    return ack;
}
