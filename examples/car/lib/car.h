#ifndef __CAR_H_
#define __CAR_H_

#include "stdint.h"

void car_run(float Rotational1, float Rotational2);
void car_deflection(float val);
void car_left();
void car_turn_left_diff(float speed_fast, float speed_slow);
void car_stop();

#endif
