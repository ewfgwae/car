#include "key.h"
#include "ti_msp_dl_config.h"
#include "delay.h"


key_t key;

key_t key_read()
{
    
    key.key_turn_press_num = 0;

    key.key_start_state = DL_GPIO_readPins(TURN_NUM_PORT,  TURN_NUM__START_PIN);

    key.key_turn_state[0] = DL_GPIO_readPins(TURN_NUM_PORT,  TURN_NUM__1_PIN);
    key.key_turn_state[1] = DL_GPIO_readPins(TURN_NUM_PORT,  TURN_NUM__2_PIN);
    key.key_turn_state[2] = DL_GPIO_readPins(TURN_NUM_PORT,  TURN_NUM__3_PIN);
    key.key_turn_state[3] = DL_GPIO_readPins(TURN_NUM_PORT,  TURN_NUM__4_PIN);
    key.key_turn_state[4] = DL_GPIO_readPins(TURN_NUM_PORT,  TURN_NUM__5_PIN);

    for(uint8_t i=0;  i<5;  i++){
        if(key.key_turn_state[i]  ==  0){
            key.key_turn_press_num++;
        }
    }
    
}


int8_t key_get_turn_num()
{
    if(key.key_turn_press_num != 1){
        return -1;
    }

    for(uint8_t i=0;  i<5;  i++){
        if(key.key_turn_state[i]  ==  0){
            return i+1;
        }
    }

    return -2;
}

int8_t key_start_is_press()
{
    if(key.key_start_state != 0){
        return 0;
    }else{
        return 1;
    }
}
