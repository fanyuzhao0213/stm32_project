#include "pid.h"


/**
 * @brief  初始化 PID 控制器
 * @param  pid: PID 控制器结构体指针
 * @param  kp, ki, kd: PID 系数
 * @param  outMax: 输出上限
 * @param  outMin: 输出下限
 * @retval None
 */
void PID_Init(PID_t *pid, float kp, float ki, float kd, float outMax, float outMin)
{
    pid->PID_Kp = kp;
    pid->PID_Ki = ki;
    pid->PID_Kd = kd;
    pid->PID_OutMax = outMax;
    pid->PID_OutMin = outMin;

    // 初始化运行状态变量
    pid->PID_Target = 0;
    pid->PID_Actual = 0;
    pid->PID_PwmOut = 0;
    pid->PID_Error = 0;
    pid->PID_LastError = 0;
    pid->PID_ErrorSum = 0;
}


/**
 * @brief  重置 PID 状态（积分、误差清零）
 * @param  pid: PID 控制器结构体指针
 * @retval None
 */
void PID_Reset(PID_t *pid)
{
    pid->PID_PwmOut = 0;
    pid->PID_Error = 0;
    pid->PID_LastError = 0;
    pid->PID_ErrorSum = 0;
}

/**
 * @brief  运行时修改 PID 参数
 * @param  pid: PID 控制器结构体指针
 * @param  kp, ki, kd: 新的 PID 系数
 * @retval None
 */
void PID_SetTunings(PID_t *pid, float kp, float ki, float kd)
{
    pid->PID_Kp = kp;
    pid->PID_Ki = ki;
    pid->PID_Kd = kd;
}


/**
 * @brief  单环 PID 计算（位置式 PID + 积分防饱和 + 微分低通滤波）
 * @param  pid: PID 控制器结构体指针
 * @retval None
 */
void PID_Update(PID_t *pid)
{
    // 1. 保存上一次的误差（用于微分项计算）
    pid->PID_LastError = pid->PID_Error;

    // 2. 计算本次误差 = 目标值 - 实际值
    pid->PID_Error = pid->PID_Target - pid->PID_Actual;

    // 3. 积分计算（Anti-windup 积分防饱和）
    //    - 当输出未达到饱和区时才允许积分累积
    //    - 这样避免积分项在饱和时继续累加，导致控制器“卡死”
  // 3. 积分计算（先判断是否允许积分）
    if (pid->PID_Ki != 0)
    {
        // 用“预测输出”判断是否允许积分
        float predict_output = pid->PID_Kp * pid->PID_Error
                             + pid->PID_Ki * pid->PID_ErrorSum
                             + pid->PID_Kd * (pid->PID_Error - pid->PID_LastError);

        if (!((predict_output >= pid->PID_OutMax && pid->PID_Error > 0) ||
              (predict_output <= pid->PID_OutMin && pid->PID_Error < 0)))
        {
            pid->PID_ErrorSum += pid->PID_Error;

            // 积分限幅
            if (pid->PID_ErrorSum > ITERM_MAX) pid->PID_ErrorSum = ITERM_MAX;
            if (pid->PID_ErrorSum < ITERM_MIN) pid->PID_ErrorSum = ITERM_MIN;
        }
    }
    else
    {
        pid->PID_ErrorSum = 0;
    }

    // 4. 微分项计算（带低通滤波器）
    //    - 传统 D 项：e(k) - e(k-1)
    //    - 低通滤波公式：D_filt = α * D_last + (1 - α) * D_raw
    //    - α 取值 0.0 ~ 1.0，越接近 1.0 滤波越平滑，延迟也越大
    static float d_last = 0;   // 上一次的微分值
    float d_raw = pid->PID_Error - pid->PID_LastError;  // 原始微分
    float alpha = 0.8f;        // 低通滤波系数（可调）
    float d_filt = alpha * d_last + (1.0f - alpha) * d_raw;
    d_last = d_filt;           // 保存滤波后的微分值

    // 5. PID 位置式计算公式
    pid->PID_PwmOut = pid->PID_Kp * pid->PID_Error
                    + pid->PID_Ki * pid->PID_ErrorSum
                    + pid->PID_Kd * d_filt;

    // 6. 输出限幅（限制 PWM 输出范围）
    if (pid->PID_PwmOut > pid->PID_OutMax) pid->PID_PwmOut = pid->PID_OutMax;
    if (pid->PID_PwmOut < pid->PID_OutMin) pid->PID_PwmOut = pid->PID_OutMin;
}


/**
 * @brief  串级 PID 计算（位置环 + 速度环）
 * @param  cpid: 串级 PID 结构体指针
 * @retval None
 */
void CascadePID_Update(CascadePID_t *cpid)
{
    // --- 外环（位置环） ---
    PID_Update(&cpid->PositionPID);
	
	// 输入：目标位置、实际位置
    // 输出：速度目标值（作为内环输入）
    cpid->SpeedPID.PID_Target = cpid->PositionPID.PID_PwmOut;

    // --- 内环（速度环） ---
    // 输入：速度目标值、实际速度
    // 输出：PWM 占空比
    PID_Update(&cpid->SpeedPID);
}
