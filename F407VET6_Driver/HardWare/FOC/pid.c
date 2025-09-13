#include "pid.h"


// HAL_GetTick_us() 微秒计时，参考 TIMx + CNT
extern uint32_t Get_Systerm_Us(void);

/**
 * @brief 内部限幅函数
 * @param value 输入值
 * @param min 最小值
 * @param max 最大值
 * @return 限幅后的值
 * 
 * 说明：保证积分、输出等不会超过安全范围
 */
static float _pid_constrain(float value, float min, float max)
{
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

/**
 * @brief 初始化PID控制器
 * @param pid PID结构体指针
 * @param P 比例系数
 * @param I 积分系数
 * @param D 微分系数
 * @param ramp 输出变化速率限制
 * @param limit 输出限幅
 * 
 * 说明：
 * 初始化PID的各项状态变量，保证第一次更新不会异常。
 */
void PID_Init(PIDController *pid, float P, float I, float D, float ramp, float limit)
{
    pid->P = P;
    pid->I = I;
    pid->D = D;
    pid->output_ramp = ramp;
    pid->limit = limit;

    pid->integral_prev = 0.0f;   // 初始积分值
    pid->output_prev = 0.0f;     // 初始输出值
    pid->error_prev = 0.0f;      // 初始误差
    pid->timestamp_prev = Get_Systerm_Us(); // 记录初始化时间
}

/**
 * @brief 计算PID输出
 * @param pid PID结构体指针
 * @param error 当前误差
 * @return PID控制器输出
 * 
 * 说明：
 * 1. P项：与当前误差成正比
 * 2. I项：使用梯形积分方法，防止积分误差累积过快
 * 3. D项：微分环节计算误差变化率
 * 4. 输出限幅：防止电机或执行器过载
 * 5. 输出斜率限制：防止输出突变导致冲击
 */
float PID_Update(PIDController *pid, float error)
{
    // 获取当前微秒时间
    uint32_t timestamp_now = Get_Systerm_Us();

    // 时间间隔，溢出安全
    float Ts = (timestamp_now - pid->timestamp_prev) * 1e-6f;

    // 防止采样异常
	/*
		1e-3f 秒 = 1 ms，是一个比较安全的小采样时间，保证：
		D 项不会除零
		I 项不会瞬间累积过多
		输出变化平滑
	*/
    if (Ts <= 0.0f || Ts > 0.05f) Ts = 1e-3f;

    // P项
    float proportional = pid->P * error;

    // I项（梯形积分）
    float integral = pid->integral_prev + pid->I * Ts * 0.5f * (error + pid->error_prev);
	//防止积分值过大（积分饱和）导致 PID 输出过冲或控制器失稳
    integral = _pid_constrain(integral, -pid->limit, pid->limit);

    // D项
    float derivative = pid->D * (error - pid->error_prev) / Ts;

    // PID输出
    float output = proportional + integral + derivative;
    output = _pid_constrain(output, -pid->limit, pid->limit);

    // 输出斜率限制
	//功能：限制输出变化速率，防止冲击  逻辑：计算允许的最大变化量 → 超过就截断
    if (pid->output_ramp > 0.0f) {
        float max_change = pid->output_ramp * Ts;
        if (output - pid->output_prev > max_change)
            output = pid->output_prev + max_change;
        else if (output - pid->output_prev < -max_change)
            output = pid->output_prev - max_change;
    }

    // 更新状态
    pid->integral_prev = integral;
    pid->output_prev = output;
    pid->error_prev = error;
    pid->timestamp_prev = timestamp_now;

    return output;
}

