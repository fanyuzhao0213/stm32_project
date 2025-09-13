#include "lowpass_filter.h"

// HAL_GetTick_us() 微秒计时，参考 TIMx + CNT
extern uint32_t Get_Systerm_Us(void);

void LowPassFilter_Init(LowPassFilter *filter, float time_constant)
{
    filter->Tf = time_constant;
    filter->y_prev = 0.0f;
    filter->timestamp_prev = Get_Systerm_Us();
}

/**
 * @brief 更新低通滤波器输出
 * @param filter 低通滤波器结构体指针
 * @param x 当前输入值
 * @return 滤波后的输出值
 * 
 * 说明：
 * 使用基于微秒计时的增量计算 dt，保证计时器溢出时仍然安全。
 * 根据时间常数 Tf 和 dt 计算滤波系数 alpha，执行一阶低通滤波。
 */
float LowPassFilter_Update(LowPassFilter *filter, float x)
{
    // 获取当前微秒时间
    uint32_t timestamp_now = Get_Systerm_Us();

    // 计算时间间隔 dt_us（微秒），考虑 32 位计数器溢出情况
    uint32_t dt_us = 0;
    if (timestamp_now >= filter->timestamp_prev) {
        // 正常情况，当前时间大于上次时间
        dt_us = timestamp_now - filter->timestamp_prev;
    } else {
        // 计数器溢出情况 (16 位最大值 0xFFFF)
        dt_us = (0xFFFF - filter->timestamp_prev) + timestamp_now + 1;
    }

    // 将微秒转换为秒，用于滤波公式
    float dt = dt_us * 1e-6f;
    // 防止 dt 异常（0 或过小）
    if (dt <= 0.0f) dt = 1e-6f;

    // 如果 dt 过大，说明数据可能丢失或暂停
    if (dt > 0.3f) {
        // 直接输出当前输入值，并重置滤波器状态
        filter->y_prev = x;
        filter->timestamp_prev = timestamp_now;
        return x;
    }

    // 计算低通滤波系数 alpha
    // alpha 越接近 1，滤波越慢；越接近 0，滤波越快
    float alpha = filter->Tf / (filter->Tf + dt);

    // 一阶低通滤波公式
    float y = alpha * filter->y_prev + (1.0f - alpha) * x;

    // 更新滤波器状态，保存上一次输出和时间
    filter->y_prev = y;
    filter->timestamp_prev = timestamp_now;

    return y;
}

