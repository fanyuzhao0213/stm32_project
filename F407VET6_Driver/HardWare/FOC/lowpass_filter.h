#ifndef _LOWPASS_FILTER_H_
#define _LOWPASS_FILTER_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

/**
 * @brief 低通滤波器结构体
 * Tf           : 滤波器时间常数 (秒)
 * y_prev       : 上一次滤波输出
 * timestamp_prev : 上一次采样时间 (微秒)
 */
typedef struct {
    float Tf;
    float y_prev;
    uint32_t timestamp_prev;
} LowPassFilter;

/**
 * @brief 初始化低通滤波器
 * @param filter 低通滤波器结构体指针
 * @param time_constant 时间常数 (秒)
 * 
 * 说明：
 * 初始化滤波器状态，y_prev 置零，记录初始时间。
 */
void LowPassFilter_Init(LowPassFilter *filter, float time_constant);

/**
 * @brief 更新低通滤波器输出
 * @param filter 低通滤波器结构体指针
 * @param x 当前输入值
 * @return 滤波后的输出值
 * 
 * 说明：
 * 1. 计算采样间隔 dt
 * 2. 若 dt 异常（<=0 或过大），直接输出当前值
 * 3. 按滤波公式计算 alpha 并更新输出
 */
float LowPassFilter_Update(LowPassFilter *filter, float x);

#endif
