/**
 * @author penC_QuQ
 */


#include "soft_i2c.h"

/**
  * @brief  初始化软I2C
  * @retval None
  */
void Soft_I2C_Init(void) {
    /* 设置默认电平 */
    DL_GPIO_setPins(I2C_PORT, I2C_SCL_PIN);
    DL_GPIO_setPins(I2C_PORT, I2C_SDA_PIN);
}

/**
  * @brief  释放或拉低数据线
  * @param  status 释放或拉低数据线
  * @retval None
  */
/* void I2C_SCL_OUTPUT(I2C_Bus_Status_t status){
    if(status == HIGH){
      HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_SET);
    }else if(status == LOW){
      HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_RESET);
    }
} */

/**
  * @brief  释放或拉低时钟线
  * @param  status 释放或拉低时钟线
  * @retval None
  */
 /* void I2C_SDA_OUTPUT(I2C_Bus_Status_t status){
    if(status == HIGH){
      HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_SET);
    }else if(status == LOW){
      HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_RESET);
    }
 } */

/**
  * @brief  读取SDA电平
  * @retval I2C_SDA的电平状态
  */
/* GPIO_PinState I2C_SDA_Read(){
  return HAL_GPIO_ReadPin(I2C_SDA_GPIO_PORT,I2C_SDA_PIN);
} */

/**
  * @brief  I2C延迟（3us）
  * @param  
  * @retval None
  */
/* void I2C_Delay(){
  //Delay_us(1);
} */

/**
  * @brief  I2C开始
  * @param  
  * @retval None
  */
void I2C_Start(){
  I2C_SCL_OUTPUT_LOW();
  I2C_Delay();
  I2C_SDA_OUTPUT_HIGH();
  I2C_Delay();
  I2C_SCL_OUTPUT_HIGH() ;
  I2C_Delay();
  I2C_SDA_OUTPUT_LOW();
}

/**
  * @brief  I2C写字节
  * @param  byte 要写入的字节
  * @retval None
  */
void I2C_WriteByte(uint8_t byte){
  for(uint8_t i = 0x80; i != 0x00; i = i>>1){
    I2C_SCL_OUTPUT_LOW() ;
    I2C_Delay();
    if(byte & i){
      I2C_SDA_OUTPUT_HIGH();
    }else{
      I2C_SDA_OUTPUT_LOW();
    }
    I2C_Delay();
    I2C_SCL_OUTPUT_HIGH();
    I2C_Delay();
   }
}

/**
  * @brief  I2C写地址
  * @param  address 设备地址
  * @param  rw      读或写寄存器
  * @retval None
  */
void I2C_WriteAddress(uint8_t address,  I2C_RW_t rw){
  uint8_t byte;

  //修改address的最后一位
  if(rw == I2C_RW_WRITE){
    byte = address & (~0x01);
  }else if(rw == I2C_RW_READ){
    byte = address | 0x01;
  }

  I2C_WriteByte(byte);
}


/**
  * @brief  I2C读字节
  * @retval 读取到的一个字节
  */
uint8_t I2C_ReadByte(){
  uint8_t i;
  uint8_t byte = 0x00;

  I2C_SCL_OUTPUT_LOW() ;
  I2C_Delay();
  I2C_SDA_OUTPUT_HIGH();  //释放总线
  I2C_Delay();

  for(i = 0x80;   i != 0x00;  i = i>>1){
    I2C_SCL_OUTPUT_LOW() ;
    I2C_Delay();
    I2C_SCL_OUTPUT_HIGH() ;
    I2C_Delay();
    if(I2C_SDA_Read() != 0){          //当SDA为高电平
      //将byte的当前位设为1
      byte |= i;
    }
  }
  return byte;
}

/**
  * @brief  I2C应答ACK
  * @param  
  * @retval None
  */
void I2C_ACK(){
  I2C_SCL_OUTPUT_LOW() ;
  I2C_Delay();
  I2C_SDA_OUTPUT_LOW();
  I2C_Delay();

  I2C_SCL_OUTPUT_HIGH() ;
  I2C_Delay();
}

/**
  * @brief  I2C应答NACK
  * @retval None
  */
void I2C_NACK(){
  I2C_SCL_OUTPUT_LOW() ;
  I2C_Delay();

  I2C_SDA_OUTPUT_HIGH();
  I2C_Delay();

  I2C_SCL_OUTPUT_HIGH() ;
  I2C_Delay();
}

/**
  * @brief  I2C等待从机应答
  * @retval 从机的应答信号
  */
I2C_Responed_t I2C_Wait_ACK(){
  I2C_SCL_OUTPUT_LOW() ;
  I2C_Delay();

  I2C_SDA_OUTPUT_HIGH(); //释放总线
  I2C_Delay();

  I2C_SCL_OUTPUT_HIGH() ;
  I2C_Delay();
  if(I2C_SDA_Read() == 0){
    return ACK;
  }else{
    return NACK;
  }


}

/**
  * @brief  I2C结束
  * @retval None
  */
void I2C_Stop(){
  I2C_SCL_OUTPUT_LOW() ;
  I2C_Delay();
  I2C_SDA_OUTPUT_LOW();
  I2C_Delay();
  I2C_SCL_OUTPUT_HIGH() ;
  I2C_Delay();
  I2C_SDA_OUTPUT_HIGH();
  I2C_Delay();
}



