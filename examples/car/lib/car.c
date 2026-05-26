#include "car.h"
#include "motor.h"

extern float RotationalA_target, RotationalB_target;
extern volatile float MA_RPM, MB_RPM;

typedef struct {
    float speed_left;
    float speed_right;
} car_t;

car_t car;

void car_run(float Rotational1, float Rotational2)
{
    car.speed_left = -Rotational1;
    car.speed_right = -Rotational2;
    RotationalA_target = car.speed_left;
    RotationalB_target = car.speed_right;
}

void car_deflection(float val)
{
    RotationalA_target = car.speed_left + val;
    RotationalB_target = car.speed_right - val;
}

void car_left()
{
    RotationalB_target = car.speed_left;
    RotationalA_target = (car.speed_right) * 0.4f;
}

void car_turn_left_diff(float speed_fast, float speed_slow)
{
    RotationalA_target = -speed_slow;
    RotationalB_target = -speed_fast;
}

void car_stop()
{
    RotationalA_target = 0;
    RotationalB_target = 0;
}
