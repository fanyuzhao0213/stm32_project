#ifndef MY_FOC_H
#define MY_FOC_H

#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdbool.h>
#include <main.h>
#include <string.h>

/* ======================= 宏定义 ======================= */
/* ================== 配置参数 ================== */
#define M_PI 3.14159265359f
#define M_3PI_2 4.71238898038f

/* ------------------ FOC参数 ------------------ */
#define VELOCITY_LPF_COEF  		0.01f   			// 速度低通滤波系数

#define	CONSTRAIN(val, low, high)  ((val) < (low) ? (low) : ((val) > (high) ? (high) : (val)))
#define	MOTOR_VOLTAGE			12.0f
#define PWM_TIM            		&htim2     				// PWM使用的定时器
#define PWM_CHANNEL_A      		TIM_CHANNEL_2
#define PWM_CHANNEL_B      		TIM_CHANNEL_3
#define PWM_CHANNEL_C      		TIM_CHANNEL_4

typedef struct {
    float last_total_angle;
    uint32_t last_timestamp;
    float total_angle;
    uint32_t timestamp_now;
    uint32_t dt_us;
    float dt;
	float vel_measured;
} My_TestTydef;

extern My_TestTydef M0_Motor_Param;

/* ======================= PWM 配置结构体 ======================= */
typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channelA;
    uint32_t channelB;
    uint32_t channelC;
} MotorPWM_TypeDef;

/*外部调用变量*/
extern MotorPWM_TypeDef motor_pwm;
extern float motor_target_angle;

/**
 * @brief 初始化 my_foc 模块
 * @param pwm_cfg     PWM 硬件配置（定时器句柄 + 通道）
 * @param supply_volt 电源电压 (V)
 * @param as5600_index AS5600 编码器索引（对应 as5600 driver 的设备索引：0 或 1）
 *
 * 说明：
 * 此函数不会初始化 I2C / AS5600 设备本身（调用 AS5600_InitDevice()），
 * 只将 my_foc 模块与指定的 AS5600 索引关联并初始化内部滤波/PID。
 */
void my_foc_init_module(MotorPWM_TypeDef *pwm_cfg, float supply_volt, uint8_t as5600_index);

/* 设置 q 轴电压 (FOC 输出) */
void my_foc_set_torque(float Uq, float electrical_angle);

/* 获取当前电角度（弧度，0..2π） */
float my_foc_get_electrical_angle(void);

/* 获取机械角度（弧度，来自 AS5600_GetAngle(index)） */
float my_foc_get_motor_angle(void);

/* 获取/设置目标角度（外部可以写入目标角度供轮询使用） */
float my_foc_get_target_angle(void);
void  my_foc_set_target_angle(float target_angle);

/* 角度环 -> 速度环 -> 转矩 的串级控制函数
   - 外环：角度 PID 输出 target_velocity (rad/s)
   - 内环：速度 PID 以 target_velocity 与实际速度计算 torque
*/
void my_foc_set_position_control(float target_angle_rad);

/* 仅速度环控制（目标为角速度 rad/s） */
void my_foc_set_velocity_control(float target_velocity_rad_s);

/* 直接应用转矩（归一化电压 Uq） */
void my_foc_set_torque_control(float Uq);

/* 设置/调整 PID 参数（便捷函数） */
void my_foc_set_velocity_pid(float P, float I, float D, float ramp, float limit);
void my_foc_set_position_pid(float P, float I, float D, float ramp, float limit);

/* 对 AS5600 的零点偏移进行设置（转成电角度零点） */
void my_foc_set_encoder_offset(float offset_rad);

/* 进行编码器-电角度的零点对齐 (会短时施加电压使电机停在位置) */
void my_foc_align_sensor(int pole_pairs, int dir);

/* 模块循环相关：速度估计器（若外部在定时器中周期调用，可降低噪声） */
void my_foc_update_velocity_estimator(void); // 更新内部速度估计（按需调用）

void MyFOC_Test(void);
void my_foc_init_test(void);
void DFOC_M0_OpenLoop(float torque);
#endif /* MY_FOC_H */
