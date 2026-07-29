#ifndef __LINE_TRACKING_H_
#define __LINE_TRACKING_H_

#include "stdint.h"

void IRDM_line_inspection(void);

extern float Turn90Angle ;
extern float TurnMaxAngle;
extern float TurnMidAngle;
extern float TurnMinAngle;
extern float base_speed_mm ,BaseSpeed,ForwardLimit;
extern float turn_diff ;

#endif
