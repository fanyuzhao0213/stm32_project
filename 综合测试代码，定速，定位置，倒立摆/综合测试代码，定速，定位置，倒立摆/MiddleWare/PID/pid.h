#ifndef __PID_H
#define __PID_H
#include "stm32f1xx_hal.h"

/**
 * @brief PID 控制器结构体
 */
typedef struct {
    float pid_target;           // 目标值
    float pid_actual;           // 实际值
    float pid_output;           // 输出值

    float pid_err_now;          // 当前误差 e(k)
    float pid_err_last;         // 上一次误差 e(k-1)
    float pid_err_sum;          // 误差积分 ∑e(k)

    float pid_err_int_limit;    // 误差积分限制阈值（防积分饱和，0 表示不限制）

    float pid_kp;               // 比例系数
    float pid_ki;               // 积分系数
    float pid_kd;               // 微分系数

    float pid_output_max;       // 输出最大值
    float pid_output_min;       // 输出最小值
} PID_t;


/**
 * @brief 清除 PID 控制器内部状态
 * @param pid PID 控制器对象指针
 */
void PID_Reset(PID_t *pid);

/**
 * @brief 更新 PID 控制器输出
 * @param pid PID 控制器对象指针
 */
void PID_Compute(PID_t *pid, uint8_t OutInLimitationFlag);

#endif
