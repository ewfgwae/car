#include "jy931.h"
#include "wit_c_sdk.h"
#include "ti_msp_dl_config.h"
#include "delay.h"

extern uint8_t ucRegIndex;
extern uint16_t usRegDataBuff[4];
extern uint32_t uiRegDataLen;

static volatile uint32_t s_cA, s_cG, s_cM, s_cR;

static void JY931_UartSend(uint8_t *p_data, uint32_t uiSize)
{
	uint32_t i;
	for (i = 0; i < uiSize; i++)
		DL_UART_Main_transmitDataBlocking(UART_MPU_INST, p_data[i]);
}
static void JY931_Delayms(uint16_t ucMs) { delay_ms(ucMs); }
static void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum) {}

static void ParsePacket(uint8_t type)
{
	int16_t *p = (int16_t *)usRegDataBuff;
	switch (type) {
		case 0x51: sReg[AX]=p[0];sReg[AY]=p[1];sReg[AZ]=p[2]; s_cA++; break;
		case 0x52: sReg[GX]=p[0];sReg[GY]=p[1];sReg[GZ]=p[2]; s_cG++; break;
		case 0x53: sReg[Roll]=p[0];sReg[Pitch]=p[1];sReg[Yaw]=p[2]; s_cR++; break;
		case 0x54: sReg[HX]=p[0];sReg[HY]=p[1];sReg[HZ]=p[2]; s_cM++; break;
	}
}

static void ParseByte(uint8_t ucData)
{
	static uint8_t buf[11];
	static uint8_t cnt;
	uint8_t i, chk;

	if (cnt == 0) {
		if (ucData == 0x55) { buf[0] = 0x55; cnt = 1; }
		return;
	}
	buf[cnt++] = ucData;
	if (cnt == 11) {
		cnt = 0;
		chk = 0;
		for (i = 0; i < 10; i++) chk += buf[i];
		if (chk == buf[10]) {
			usRegDataBuff[0] = ((uint16_t)buf[3] << 8) | buf[2];
			usRegDataBuff[1] = ((uint16_t)buf[5] << 8) | buf[4];
			usRegDataBuff[2] = ((uint16_t)buf[7] << 8) | buf[6];
			usRegDataBuff[3] = ((uint16_t)buf[9] << 8) | buf[8];
			ucRegIndex = buf[1];
			uiRegDataLen = 4;
			ParsePacket(buf[1]);
		}
	}
}

void UART3_IRQHandler(void)
{
	ParseByte(DL_UART_Main_receiveData(UART_MPU_INST));
}

void JY931_Init(void)
{
	WitInit(WIT_PROTOCOL_NORMAL, 0x50);
	WitSerialWriteRegister(JY931_UartSend);
	WitRegisterCallBack(SensorDataUpdata);
	WitDelayMsRegister(JY931_Delayms);

	DL_UART_Main_enableInterrupt(UART_MPU_INST, DL_UART_MAIN_INTERRUPT_RX);
	NVIC_EnableIRQ(UART_MPU_INST_INT_IRQN);

	delay_ms(200);
	WitSetContent(RSW_ACC | RSW_GYRO | RSW_ANGLE | RSW_MAG);
	delay_ms(100);
	WitSetOutputRate(RRATE_100HZ);
}

void JY931_Update(void)
{
	uint8_t ucData;
	while (DL_UART_Main_receiveDataCheck(UART_MPU_INST, &ucData))
		ParseByte(ucData);
}

void JY931_Print(void)
{
	static uint32_t t = 0;
	t++;
	if (t >= 5)
	{
		p_s("A:%.2f,%.2f,%.2f G:%.1f,%.1f,%.1f M:%d,%d,%d R:%.1f,%.1f,%.1f\r\n",
			sReg[AX]/32768.0f*16.0f, sReg[AY]/32768.0f*16.0f, sReg[AZ]/32768.0f*16.0f,
			sReg[GX]/32768.0f*2000.0f, sReg[GY]/32768.0f*2000.0f, sReg[GZ]/32768.0f*2000.0f,
			sReg[HX],sReg[HY],sReg[HZ],
			sReg[Roll]/32768.0f*180.0f, sReg[Pitch]/32768.0f*180.0f, sReg[Yaw]/32768.0f*180.0f);
		t = 0;
	}
}

void JY931_GetGyro(float *gx, float *gy, float *gz)
{
	*gx = sReg[GX] / 32768.0f * 2000.0f;
	*gy = sReg[GY] / 32768.0f * 2000.0f;
	*gz = sReg[GZ] / 32768.0f * 2000.0f;
}
void JY931_GetAcc(float *ax, float *ay, float *az)
{
	*ax = sReg[AX] / 32768.0f * 16.0f;
	*ay = sReg[AY] / 32768.0f * 16.0f;
	*az = sReg[AZ] / 32768.0f * 16.0f;
}
void JY931_GetAngle(float *roll, float *pitch, float *yaw)
{
	*roll  = sReg[Roll]  / 32768.0f * 180.0f;
	*pitch = sReg[Pitch] / 32768.0f * 180.0f;
	*yaw   = sReg[Yaw]   / 32768.0f * 180.0f;
}
