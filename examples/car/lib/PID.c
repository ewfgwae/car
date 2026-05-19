#include "PID.h"

/**
  * @brief  计算PID
  * @param  target 目标值
  * @param  actual 测量的实际值
  * @retval 输出的控制量
  */
float PID(PID_t* pid,  float target,  float actual){
  float out;
  pid->error = target - actual;
  pid->error_i += pid->error;

  pid->p = pid->error*pid->kp;
  pid->i = pid->error_i*pid->ki;
  pid->d = (pid->error-pid->error_last1)*pid->kd;
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