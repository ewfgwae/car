#ifndef __LINE_TRACKING_H_
#define __LINE_TRACKING_H_

#include "stdint.h"

void IR_Module_Read(void);
void IRDM_line_inspection(void);

extern float Turn90Angle;
extern float TurnMinAngle;
extern float base_speed_mm, BaseSpeed, ForwardLimit;
extern float turn_diff;
extern float line_error;

#endif