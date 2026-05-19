#include "car.h"
#include "motor.h"

extern float RotationalA_target,RotationalB_target;
extern volatile float MA_RPM, MB_RPM;

typedef struct
{
    float speed;
}car_t;


car_t car;

void car_run(float Rotational)
{
	car.speed=Rotational;
	RotationalA_target=car.speed;
	RotationalB_target=car.speed;	
}

/**
 * @brief 
 * 
 */
void car_deflection(float val)
{
	RotationalA_target=car.speed+val;
	RotationalB_target=car.speed-val;	
	
}


void car_left()
{
	RotationalB_target=(car.speed)*0.8;
	RotationalA_target=-(car.speed)*0.6;	
}

void car_stop()
{
	RotationalA_target=0;
	RotationalB_target=0;	
}