#ifndef __JY931_H
#define __JY931_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void JY931_Init(void);
void JY931_Update(void);
void JY931_Print(void);
void JY931_GetGyro(float *gx, float *gy, float *gz);
void JY931_GetAcc(float *ax, float *ay, float *az);
void JY931_GetAngle(float *roll, float *pitch, float *yaw);

#ifdef __cplusplus
}
#endif

#endif /* __JY931_H */
