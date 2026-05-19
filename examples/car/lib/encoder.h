#ifndef _ENCODER_H
#define _ENCODER_H
#include "ti_msp_dl_config.h"

extern volatile int Get_Encoder_countA,Get_Encoder_countB;
void Encoder_Init();

float Calculate_Motor_RPM(int encoder_count, int sample_time_ms);
#endif
