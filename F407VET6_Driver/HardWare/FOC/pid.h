#ifndef PID_H
#define PID_H

#include "stm32f4xx_hal.h"

/**
 * @brief PID控制器结构体
 * 
 * 用于闭环控制，如电机角度控制。
 */
typedef struct {
    float P;              // 比例系数
    float I;              // 积分系数
    float D;              // 微分系数
    float output_ramp;    // 输出变化速率限制（单位：输出/秒）
    float limit;          // 输出限幅
    float integral_prev;  // 上一次积分值
    float output_prev;    // 上一次输出值
    float error_prev;     // 上一次误差
    uint32_t timestamp_prev; // 上一次更新时间（毫秒）
} PIDController;

/**
 * @brief 初始化PID控制器
 * @param pid PID结构体
 * @param P 比例系数
 * @param I 积分系数
 * @param D 微分系数
 * @param ramp 输出变化速率限制（0表示不限制）
 * @param limit 输出限幅
 */
void PID_Init(PIDController *pid, float P, float I, float D, float ramp, float limit);

/**
 * @brief 更新PID输出
 * @param pid PID结构体
 * @param error 当前误差
 * @return PID输出
 */
float PID_Update(PIDController *pid, float error);

#endif


