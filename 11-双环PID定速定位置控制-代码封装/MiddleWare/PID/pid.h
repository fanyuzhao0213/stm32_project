#ifndef __PID_H
#define __PID_H



#define ITERM_MAX  500.0f   // 积分项最大值，可调
#define ITERM_MIN -500.0f


typedef struct {
    float PID_Target;     // 目标值
    float PID_Actual;     // 实际值
    float PID_PwmOut;        // PID输出

    float PID_Kp;         // 比例系数
    float PID_Ki;         // 积分系数
    float PID_Kd;         // 微分系数

    float PID_Error;      // 本次误差
    float PID_LastError;  // 上次误差
    float PID_ErrorSum;   // 误差积分

    float PID_OutMax;     // 输出上限
    float PID_OutMin;     // 输出下限
} PID_t;


// 串级 PID 结构体（位置环+速度环）
typedef struct {
    PID_t PositionPID; // 外环
    PID_t SpeedPID;    // 内环
} CascadePID_t;



// === 基础功能函数 ===
void PID_Init(PID_t *pid, float kp, float ki, float kd, float outMax, float outMin);
void PID_Reset(PID_t *pid);
void PID_SetTunings(PID_t *pid, float kp, float ki, float kd);

// === 计算函数 ===
void PID_Update(PID_t *pid);					// 单个 PID 更新
void CascadePID_Update(CascadePID_t *cpid);		// 串级 PID 更新
#endif


