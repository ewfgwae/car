#include "PID.h"
#include "math.h"

/**
  * @brief  计算PID
  * @param  target 目标值
  * @param  actual 测量的实际值
  * @retval 输出的控制量
  */
float PID(PID_t* pid, float target, float actual){
    float out;
    pid->error = target - actual;
    
    if (fabs(pid->error) < 0.5f) {
        pid->error_i += pid->error;
    } else {
        pid->error_i = 0;
    }
    
    float I_MAX = 50.0f;
    if(pid->error_i > I_MAX) pid->error_i = I_MAX;
    if(pid->error_i < -I_MAX) pid->error_i = -I_MAX;
    // =========================================================
    
    pid->p = pid->error * pid->kp;
    pid->i = pid->error_i * pid->ki;
    
    /*----- 最小改动：D项低通滤波，抑制直道高频抖动 -----*/
    float d_raw = (pid->error - pid->error_last1) * pid->kd;
    pid->d = pid->d * 0.5f + d_raw * 0.2f;   // 历史值与新值各半，直道更稳
    
    pid->out = pid->p + pid->i + pid->d;
    pid->error_last1 = pid->error;
    return pid->out;
}
/**
  * @brief  计算增量式PID
  * @param  target 目标值
  * @param  actual 测量的实际值
  * @retval 输出的控制量
  */
float PID_Incremental(PID_t* pid,  float target,  float actual){
  float out;
  float out_d; //pid增量
  pid->error = target - actual;

  pid->p = (pid->error-pid->error_last1)*pid->kp;
  pid->i = pid->error*pid->ki;
  pid->d = (pid->error - 2*pid->error_last1 + pid->error_last2)*pid->kd;

  out_d = pid->p + pid->i + pid->d;
  pid->out = pid->out + out_d;

  pid->error_last1 = pid->error;
  pid->error_last2 = pid->error_last1;
  return pid->out;
}