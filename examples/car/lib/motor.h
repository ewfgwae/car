#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "stdint.h"

void motor_init();
void motor_set_a_speed(int32_t speed);
void motor_set_b_speed(int32_t speed);

void motor_set_a_Rotational_speed(uint8_t Rotational);
void motorA_Rotational_speed_Handle(float target,float actual);


void motor_set_b_Rotational_speed(uint8_t Rotational);
void motorB_Rotational_speed_Handle(float target,float actual);

#endif