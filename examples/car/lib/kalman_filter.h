#ifndef __KALMAN_FILTER_H
#define __KALMAN_FILTER_H

#include <stdint.h>

/*=============================================================================
 * 一维卡尔曼滤波器（最小实现）
 * 用于平滑巡线误差 line_error，抑制传感器跳变导致的抖动
 *=============================================================================*/
typedef struct {
    float q;          // 过程噪声协方差（模型信任度，越小越信任模型）
    float r;          // 测量噪声协方差（传感器噪声，越大越不信任测量）
    float x;          // 当前估计值
    float p;          // 估计误差协方差
    float k;          // 卡尔曼增益
} Kalman1D_t;

void Kalman1D_Init(Kalman1D_t *kf, float q, float r, float init_x);
float Kalman1D_Update(Kalman1D_t *kf, float measurement);

#endif /* __KALMAN_FILTER_H */