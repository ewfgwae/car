#ifndef __PID_H_
#define __PID_H_



typedef struct {
    float kp;
    float ki;
    float kd;
    float error;  //误差
    float error_last1; //上一次的误差
    float error_last2; //上上一次的误差
    float error_i; //积累的误差
    float dt;  //PID的计算周期
    float p;
    float i;
    float d;
    float out;
} PID_t;


float PID(PID_t* pid,  float target,  float actual);
float PID_Incremental(PID_t* pid,  float target,  float actual);


#endif