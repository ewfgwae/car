#include "hardware_iic.h"
#include "delay.h"
#include "string.h"

void GW_I2C_Init(void)
{
    static const DL_I2C_ClockConfig i2c_clk_cfg = {
        .clockSel = DL_I2C_CLOCK_BUSCLK,
        .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
    };
    DL_I2C_setClockConfig(I2C_0_INST, (DL_I2C_ClockConfig *) &i2c_clk_cfg);
    DL_I2C_setAnalogGlitchFilterPulseWidth(I2C_0_INST,
        DL_I2C_ANALOG_GLITCH_FILTER_WIDTH_50NS);
    DL_I2C_enableAnalogGlitchFilter(I2C_0_INST);
    DL_I2C_resetControllerTransfer(I2C_0_INST);
    DL_I2C_setTimerPeriod(I2C_0_INST, 31);
    DL_I2C_setControllerTXFIFOThreshold(I2C_0_INST, DL_I2C_TX_FIFO_LEVEL_BYTES_7);
    DL_I2C_setControllerRXFIFOThreshold(I2C_0_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_8);
    DL_I2C_enableControllerClockStretching(I2C_0_INST);
    DL_I2C_enableController(I2C_0_INST);

    p_s("GW_I2C_Init done, MCR=0x%08X\r\n",
        (unsigned int)I2C_0_INST->MASTER.MCR);
}

unsigned char hardware_IIC_WirteByte(unsigned char Salve_Adress,unsigned char Reg_Address, unsigned char dat)
{
    uint8_t temp[2];
    temp[0] = Reg_Address;
    temp[1] = dat;
    DL_I2C_fillControllerTXFIFO(I2C_0_INST, temp, 2);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_0_INST, Salve_Adress, DL_I2C_CONTROLLER_DIRECTION_TX, 2);
    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_flushControllerTXFIFO(I2C_0_INST);
    return 1;
}
unsigned char hardware_IIC_WirteBytes(unsigned char Salve_Adress,unsigned char Reg_Address, unsigned char * dat, unsigned char len)
{
    unsigned char temp[len+1];
    temp[0] = Reg_Address;
    memcpy(temp+1,dat,len);
    DL_I2C_fillControllerTXFIFO(I2C_0_INST, temp, len+1);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_0_INST, Salve_Adress, DL_I2C_CONTROLLER_DIRECTION_TX, len+1);
    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_flushControllerTXFIFO(I2C_0_INST);
    return 1;
}
unsigned char hardware_IIC_ReadByte(unsigned char Salve_Adress,unsigned char Reg_Address)
{
    unsigned char data;
    uint32_t status_tx, status_rx;
    DL_I2C_fillControllerTXFIFO(I2C_0_INST, &Reg_Address, 1);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_0_INST, Salve_Adress, DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    status_tx = DL_I2C_getControllerStatus(I2C_0_INST);
    DL_I2C_flushControllerTXFIFO(I2C_0_INST);

    DL_I2C_startControllerTransfer(I2C_0_INST, Salve_Adress, DL_I2C_CONTROLLER_DIRECTION_RX, 1);
    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    status_rx = DL_I2C_getControllerStatus(I2C_0_INST);
    data = DL_I2C_receiveControllerData(I2C_0_INST);

    p_s("I2C RD ADDR=0x%02X REG=0x%02X TX_S=0x%04X RX_S=0x%04X D=0x%02X\r\n",
        (unsigned int)Salve_Adress, (unsigned int)Reg_Address,
        (unsigned int)status_tx, (unsigned int)status_rx, (unsigned int)data);
    return data;
}
unsigned char hardware_IIC_ReadBytes(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char *data,unsigned char len)
{
    DL_I2C_fillControllerTXFIFO(I2C_0_INST, &Reg_Address, 1);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_0_INST, Salve_Adress, DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_flushControllerTXFIFO(I2C_0_INST);

    DL_I2C_startControllerTransfer(I2C_0_INST, Salve_Adress, DL_I2C_CONTROLLER_DIRECTION_RX, len);
    while (DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    for (uint8_t i = 0; i < len; i++) {
        data[i] = DL_I2C_receiveControllerData(I2C_0_INST);
    }
    return 1;
}

/* 上层封装函数 */
unsigned char IIC_ReadByte(unsigned char Salve_Adress,unsigned char Reg_Address)
{
    return hardware_IIC_ReadByte(Salve_Adress,Reg_Address);
}
unsigned char IIC_ReadBytes(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char *Result,unsigned char len)
{
    return hardware_IIC_ReadBytes(Salve_Adress,Reg_Address,Result,len);
}
unsigned char IIC_WriteByte(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char data)
{
    return hardware_IIC_WirteByte(Salve_Adress,Reg_Address,data);
}
unsigned char IIC_WriteBytes(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char *data,unsigned char len)
{
    return hardware_IIC_WirteBytes(Salve_Adress,Reg_Address,data,len);
}
unsigned char Ping(void)
{
    unsigned char dat;
    dat=IIC_ReadByte(GW_GRAY_ADDR_DEF,GW_GRAY_PING);
    if(dat==GW_GRAY_PING_OK)
    {
        return 0;
    }
    else return 1;
}
unsigned char IIC_Get_Digtal(void)
{
    unsigned char dat;
    dat=IIC_ReadByte(GW_GRAY_ADDR_DEF,GW_GRAY_DIGITAL_MODE);
    p_s("Digital %d-%d-%d-%d-%d-%d-%d-%d\r\n",
        (dat>>0)&0x01,(dat>>1)&0x01,(dat>>2)&0x01,(dat>>3)&0x01,
        (dat>>4)&0x01,(dat>>5)&0x01,(dat>>6)&0x01,(dat>>7)&0x01);
    return dat;
}
unsigned char IIC_Get_Anolog(unsigned char * Result,unsigned char len)
{
    if(IIC_ReadBytes(GW_GRAY_ADDR_DEF,GW_GRAY_ANALOG_BASE_,Result,len))
    {
        p_s("Anolog %d-%d-%d-%d-%d-%d-%d-%d\r\n",
            Result[0],Result[1],Result[2],Result[3],
            Result[4],Result[5],Result[6],Result[7]);
        return 1;
    }
    else return 0;
}
unsigned char IIC_Get_Single_Anolog(unsigned char Channel)
{
    unsigned char dat;
    IIC_ReadBytes(GW_GRAY_ADDR_DEF,GW_GRAY_ANALOG(Channel),&dat,1);
    return dat;
}
uint8_t IIC_write_buff[10]={0};
unsigned char IIC_Get_Normalize(unsigned char * Result,unsigned char len)
{
    IIC_write_buff[0]=GW_GRAY_ANALOG_NORMALIZE;
    IIC_write_buff[1]=0xff;
    IIC_WriteBytes(GW_GRAY_ADDR_DEF,GW_GRAY_ANALOG_NORMALIZE ,&IIC_write_buff[1], 2 );
    delay_ms(10);
    IIC_ReadBytes(GW_GRAY_ADDR_DEF, GW_GRAY_ANALOG_MODE, Result , 8 );
    p_s("Normalize %d-%d-%d-%d-%d-%d-%d-%d\r\n",
        Result[0],Result[1],Result[2],Result[3],
        Result[4],Result[5],Result[6],Result[7]);
    IIC_write_buff[0]=GW_GRAY_ANALOG_NORMALIZE;
    IIC_write_buff[1]=0x00;
    IIC_WriteBytes(GW_GRAY_ADDR_DEF,GW_GRAY_ANALOG_NORMALIZE,&IIC_write_buff[1], 2 );
    return 1;
}

uint8_t reset_magic_number[8] = {
    0xB8, 0xD0, 0xCE, 0xAA,
    0xBF, 0xC6, 0xBC, 0xBC
};

uint8_t i2c_write(uint8_t *data, uint8_t length)
{
    if(hardware_IIC_WirteBytes(0x00, 0x00, data, length)) {
        return length;
    }
    return 0;
}

void i2c_reset(void)
{
    i2c_write((uint8_t*)reset_magic_number, 8);
}
