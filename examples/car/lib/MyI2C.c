#include "delay.h"
#include "ti_msp_dl_config.h"
#include "ti/driverlib/dl_gpio.h"
#include "MyI2C.h"



void SDA_OUT(void)   
{
    DL_GPIO_initDigitalOutput(I2C_SDA_IOMUX);
	DL_GPIO_setPins(I2C_SDA_PORT, I2C_SDA_PIN);	   
    DL_GPIO_enableOutput(I2C_SDA_PORT, I2C_SDA_PIN); 
}
void SDA_IN(void)
{
 
    DL_GPIO_initDigitalInputFeatures(I2C_SDA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
 
 
}

void MyI2C_W_SCL(uint8_t BitValue)
{
    if (BitValue) {
        DL_GPIO_setPins(I2C_SCL_PORT, I2C_SCL_PIN);
    } else {
        DL_GPIO_clearPins(I2C_SCL_PORT, I2C_SCL_PIN);
    }
    delay_us(10);
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	
    SDA_OUT();

    if (BitValue) {
        DL_GPIO_setPins(I2C_SDA_PORT, I2C_SDA_PIN);
    } else {
        DL_GPIO_clearPins(I2C_SDA_PORT, I2C_SDA_PIN);
    }
    delay_us(10);
}


uint8_t MyI2C_R_SDA(void)
{
	uint8_t b;
    uint32_t BitValue;
    SDA_IN();
	BitValue = DL_GPIO_readPins(I2C_SDA_PORT, I2C_SDA_PIN);		//读取SDA电平
    {
        if(BitValue)   b=1;
        else           b=0;
    }
	delay_us(10);		//延时8us，防止时序频率超过要求
	return b;	        //返回SDA电平
}



void MyI2C_Init(void)
{
    SYSCFG_DL_GPIO_init();
	/*设置默认电平*/
	DL_GPIO_setPins(GPIOA,I2C_SCL_PIN);
	DL_GPIO_setPins(GPIOB,I2C_SDA_PIN);
}


void MyI2C_Start(void)
{
	
    SDA_OUT();

	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
    SDA_OUT();
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
	
    SDA_OUT();
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(!!(Byte & (0x80 >> i)));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)
{

    SDA_OUT();
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{

        SDA_IN();
		MyI2C_W_SCL(1);		
		if (MyI2C_R_SDA()){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)
{

    SDA_OUT();
	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{

    SDA_OUT();
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	SDA_IN();
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}
