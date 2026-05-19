#ifndef __LINE_TRACKING_H_
#define __LINE_TRACKING_H_

#include "stdint.h"

typedef struct {
    float pos;
    uint8_t size;
    uint8_t point[7];
}line_t;

void line_tracking_init();
line_t line_tracking_get_line_state();
void line_tracking_run();

#endif