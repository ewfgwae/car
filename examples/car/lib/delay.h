#ifndef __DELAY_H_
#define __DELAY_H_


#include "stdint.h"
#include "stdarg.h"


void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

void led_off();
void led_on();
void led_toggle();

void p_s(char *string,  ...);

void buzzer_on();
void buzzer_off();
void buzzer_toggle();

#endif