#ifndef __KEY_H_
#define __KEY_H_

#include "stdint.h"

typedef struct
{
    uint32_t key_turn_state[5];
    uint32_t key_start_state;
    uint8_t key_turn_press_num;
}key_t;

key_t key_read();
int8_t key_get_turn_num();
int8_t key_start_is_press();

#endif