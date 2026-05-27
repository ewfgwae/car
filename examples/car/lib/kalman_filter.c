#include "kalman_filter.h"

/*=============================================================================
 * 一维卡尔曼滤波器实现
 * 状态方程: x(k) = x(k-1) + w(k)   (w: 过程噪声)
 * 观测方程: z(k) = x(k) + v(k)     (v: 测量噪声)
 *=============================================================================*/

void Kalman1D_Init(Kalman1D_t *kf, float q, float r, float init_x)
{
    kf->q = q;
    kf->r = r;
    kf->x = init_x;
    kf->p = 1.0f;      // 初始不确定性设为较大值
    kf->k = 0.0f;
}

float Kalman1D_Update(Kalman1D_t *kf, float measurement)
{
    /*--- 预测步骤 ---*/
    // x(k|k-1) = x(k-1)  (状态预测，假设匀速)
    // p(k|k-1) = p(k-1) + q
    kf->p = kf->p + kf->q;

    /*--- 更新步骤 ---*/
    // 卡尔曼增益: K = p(k|k-1) / (p(k|k-1) + r)
    kf->k = kf->p / (kf->p + kf->r);

    // 状态更新: x(k) = x(k|k-1) + K * (z(k) - x(k|k-1))
    kf->x = kf->x + kf->k * (measurement - kf->x);

    // 协方差更新: p(k) = (1 - K) * p(k|k-1)
    kf->p = (1.0f - kf->k) * kf->p;

    return kf->x;
}