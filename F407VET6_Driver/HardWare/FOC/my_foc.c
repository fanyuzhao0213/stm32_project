/**
 * @file my_foc.c
 * @brief STM32F4 基于HAL库的FOC控制模块
 *
 * 主要功能：
 * - PWM初始化与输出
 * - AS5600编码器角度、速度读取
 * - 电机零点对齐
 * - PID控制：速度闭环、角度+速度双闭环
 * - 低通滤波器进行速度信号平滑
 * - 串口/外部接口接收目标角度
 */

#include "my_foc.h"
#include "as5600.h"
#include "pid.h"
#include "lowpass_filter.h"
#include "tim.h"
#include "stdio.h"
#include "math.h"
#include "i2c.h"

/* ======================= 静态变量 ======================= */

/* 电源电压，用于计算PWM占空比 */
static float voltage_power_supply = 0.0f;

/* FOC计算中间量 */
static float Ualpha = 0.0f, Ubeta = 0.0f;     // αβ坐标系电压
static float Ua = 0.0f, Ub = 0.0f, Uc = 0.0f; // 三相实际电压

/* 电机参数 */
static float zero_electric_angle = 0.0f; // 电机零点电角度
static int PP = 1;                        // 极对数
static int DIR = 1;                       // 电机方向：1=正转，-1=反转

/* PWM配置结构体 */
MotorPWM_TypeDef motor_pwm;

/* PID与滤波 */
static LowPassFilter M0_Vel_Flt;      // 速度低通滤波器
static PIDController vel_loop_M0;     // 速度PID控制器
static PIDController angle_loop_M0;   // 角度PID控制器

/* 编码器ID（支持多个AS5600） */
static uint8_t as5600_id = 0; // 当前电机使用的AS5600编号

/* 电机目标角度 */
float motor_target_angle = 0.0f;

/* ======================= 内部工具函数 ======================= */

/**
 * @brief 将角度限制到 [0, 2π]
 * @param angle 输入角度（弧度）
 * @return 归一化后的角度
 */
static float _normalize_angle(float angle) {
    float a = fmodf(angle, 2.0f * M_PI);
    return (a >= 0.0f) ? a : (a + 2.0f * M_PI);
}

/**
 * @brief 设置三相PWM占空比
 * @param Ua_val U相电压
 * @param Ub_val V相电压
 * @param Uc_val W相电压
 */
static void _set_pwm(float Ua_val, float Ub_val, float Uc_val) {
    /* 限制电压到 [0, 电源电压] */
    float dc_a = CONSTRAIN(Ua_val / voltage_power_supply, 0.0f, 1.0f);
    float dc_b = CONSTRAIN(Ub_val / voltage_power_supply, 0.0f, 1.0f);
    float dc_c = CONSTRAIN(Uc_val / voltage_power_supply, 0.0f, 1.0f);

    /* 输出到定时器CCR寄存器 */
    __HAL_TIM_SET_COMPARE(motor_pwm.htim, motor_pwm.channelA,
                          (uint32_t)(dc_a * (__HAL_TIM_GET_AUTORELOAD(motor_pwm.htim))));
    __HAL_TIM_SET_COMPARE(motor_pwm.htim, motor_pwm.channelB,
                          (uint32_t)(dc_b * (__HAL_TIM_GET_AUTORELOAD(motor_pwm.htim))));
    __HAL_TIM_SET_COMPARE(motor_pwm.htim, motor_pwm.channelC,
                          (uint32_t)(dc_c * (__HAL_TIM_GET_AUTORELOAD(motor_pwm.htim))));
}

/* ======================= FOC初始化相关 ======================= */

/**
 * @brief 初始化FOC模块
 * @param pwm_cfg PWM配置
 * @param supply_voltage 电机供电电压
 * @param encoder_index 对应的AS5600编号
 */
void my_foc_init(MotorPWM_TypeDef *pwm_cfg, float supply_voltage, uint8_t encoder_index) {
    voltage_power_supply = supply_voltage;
    motor_pwm = *pwm_cfg;
    as5600_id = encoder_index;

    rtt_printf("PWM init finish! Motor supply voltage: %.2f V\r\n", voltage_power_supply);
}

/**
 * @brief 电机零点对齐
 * @param pole_pairs 电机极对数
 * @param dir 电机方向（1或-1）
 */
void my_foc_align_sensor(int pole_pairs, int dir) {
    PP = pole_pairs;
    DIR = dir;
	/*
	物理含义
	FOC 通过 定向电流控制 定子磁场。
	这里给电机注入一个固定大小的 定向电压（3.0f），对应一个固定方向的电流，产生一个恒定的磁场。
	这个磁场会 强制拉住转子，让转子指向某个已知的机械角度。
	通过延时 3 秒，确保电机完全停止并稳定。
	目的：
	把电机“锁死”在某个已知位置，这样我们就能读取此时传感器的反馈，作为 零电角度基准。
	为什么用 M_3PI_2？
	M_3PI_2 = =270°
	在 FOC 里，d轴 和 q轴 是正交的。
	如果电机坐标系固定在 d轴 上，
	代表一个标准参考方向。
	选择 M_3PI_2 作为参考，使后续 FOC 算法计算更方便。
	*/
    /* 给定一个固定电压使电机定位 */
    my_foc_set_torque(3.0f, M_3PI_2);
    HAL_Delay(3000);

    /* 读取当前电角度作为零点 */
    zero_electric_angle = my_foc_get_electrical_angle();

    /* 电机断电 */
    my_foc_set_torque(0.0f, M_3PI_2);

    rtt_printf("[FOC] Motor zero electric angle: %.3f rad\r\n", zero_electric_angle);
}

/* ======================= 编码器接口封装 ======================= */
/**
 * @brief 获取电机电角度
 * @return 电角度 (0 ~ 2π)
 */
float my_foc_get_electrical_angle(void) {
    float mech_angle = AS5600_GetAngleWithoutOffset(as5600_id);
    return _normalize_angle((float)(DIR * PP) * mech_angle - zero_electric_angle);
}

/**
 * @brief 获取电机机械角度
 * @return 机械角度 (0 ~ 2π)
 */
float my_foc_get_motor_angle(void) {
    return AS5600_GetAngle(as5600_id);
}

/* ======================= FOC核心计算 ======================= */

/**
 * @brief 设置电机q轴电压（FOC输出）
 * @param Uq q轴电压
 * @param electrical_angle 电角度
 */
void my_foc_set_torque(float Uq, float electrical_angle) {
    /* 电压限制在电源电压的一半 */
    Uq = CONSTRAIN(Uq, -voltage_power_supply / 2.0f, voltage_power_supply / 2.0f);
    electrical_angle = _normalize_angle(electrical_angle);

    /* Park逆变换 dq -> αβ */
    Ualpha = -Uq * sinf(electrical_angle);
    Ubeta  =  Uq * cosf(electrical_angle);

    /* Clarke逆变换 αβ -> ABC */
    Ua = Ualpha + voltage_power_supply / 2.0f;
    Ub = (sqrtf(3.0f) * Ubeta - Ualpha) / 2.0f + voltage_power_supply / 2.0f;
    Uc = (-Ualpha - sqrtf(3.0f) * Ubeta) / 2.0f + voltage_power_supply / 2.0f;

    /* 输出PWM */
    _set_pwm(Ua, Ub, Uc);
}

/* ======================= PID接口 ======================= */

/* 设置速度PID参数 */
void DFOC_M0_SetVelPID(float P, float I, float D, float ramp) {
    vel_loop_M0.P = P;
    vel_loop_M0.I = I;
    vel_loop_M0.D = D;
    vel_loop_M0.output_ramp = ramp;
}

/* 设置角度PID参数 */
void DFOC_M0_SetAnglePID(float P, float I, float D, float ramp) {
    angle_loop_M0.P = P;
    angle_loop_M0.I = I;
    angle_loop_M0.D = D;
    angle_loop_M0.output_ramp = ramp;
}

/* 调用PID计算 */
float DFOC_M0_VelPID(float error) {
    return PID_Update(&vel_loop_M0, error);
}

float DFOC_M0_AnglePID(float error) {
    return PID_Update(&angle_loop_M0, error);
}

/* ======================= 电机闭环控制接口 ======================= */
#define MIN_DT 0.001f   // 最小时间间隔 1 ms
#define MAX_DT 0.05f    // 最大时间间隔 50 ms
/**
 * @brief 角度+速度双闭环控制
 * @param target 目标角度 (rad)
 *
 * 原理：
 * 1. 角度PID计算出目标速度（vel_target）
 * 2. 根据总角度与上一次采样时间计算实际速度（vel_measured）
 * 3. 速度PID计算输出扭矩
 * 4. 根据电角度输出FOC电压
 */
void DFOC_M0_SetVelocityAngle(float target) 
{
    // 静态变量记录上一次总角度和时间
    static float last_total_angle = 0;
    static uint32_t last_timestamp = 0; // 单位：微秒

    // 获取当前累计角度
    float total_angle = AS5600_GetTotalAngle(as5600_id);
    // 获取当前微秒时间
    uint32_t timestamp_now = Get_Systerm_Us();
    // 计算时间间隔 dt，处理32位计数器溢出
    uint32_t dt_us = (timestamp_now >= last_timestamp) ? 
                     (timestamp_now - last_timestamp) : 
                     (0xFFFF - last_timestamp + timestamp_now + 1);
    float dt = dt_us * 1e-6f; // 转换为秒
	if(dt < MIN_DT) dt = MIN_DT;		// 防止除零或时间过小
	if(dt > MAX_DT) dt = MAX_DT;		// 防止时间过大
	
    // 计算瞬时速度（rad/s）
    float vel_measured = (total_angle - last_total_angle) / dt;
    // 速度低通滤波，滤掉高频噪声
    vel_measured = LowPassFilter_Update(&M0_Vel_Flt, vel_measured);

    // 角度PID计算目标速度
	float current_angle = AS5600_GetTotalAngle(as5600_id); 		// rad, 累计旋转
    float angle_error = target - current_angle;					//目标角度 减去 现在的角度		
    float vel_target = DFOC_M0_AnglePID(angle_error);

    // 速度PID计算输出扭矩
    float vel_error = vel_target - vel_measured;
    float torque = DFOC_M0_VelPID(vel_error);

    // 输出FOC电压，控制电机
    my_foc_set_torque(torque, my_foc_get_electrical_angle());

    // 更新上一次角度和时间
    last_total_angle = total_angle;
    last_timestamp = timestamp_now;
}


/**
 * @brief 速度闭环控制
 * @param target 目标速度 (rad/s)
 */
void DFOC_M0_SetVelocity(float target) 
{
    static float last_total_angle = 0;
    static uint32_t last_timestamp = 0; // 上一次采样时间（微秒）
    static uint8_t first_flag = 1;      // 第一次调用标志

    // 获取当前总角度
    float total_angle = AS5600_GetTotalAngle(as5600_id);
    // 获取当前微秒时间
    uint32_t timestamp_now = Get_Systerm_Us();

    // 第一次调用，初始化 last 值并返回，不计算速度
    if(first_flag) {
        last_total_angle = total_angle;
        last_timestamp = timestamp_now;
        first_flag = 0;
        return;
    }

    // 计算时间间隔 dt
    uint32_t dt_us;
    if(timestamp_now >= last_timestamp) {
        dt_us = timestamp_now - last_timestamp;
    } else {
        dt_us = (0xFFFF - last_timestamp) + timestamp_now + 1; // 16位计数器溢出
    }

    float dt = dt_us * 1e-6f; // 转换为秒

    // 设置最小和最大 dt 限制
    const float dt_min = 0.0001f; // 100us
    const float dt_max = 0.05f;   // 50ms

    if(dt < dt_min) {
        // dt 太小，保持上次速度
        dt = dt_min;
        // 可以直接使用 last_vel
    } else if(dt > dt_max) {
        // dt 太大，限制为最大值，防止速度突变
        dt = dt_max;
    }

    // 计算速度（rad/s）
    float vel_measured = (total_angle - last_total_angle) / dt;

    // 速度低通滤波
    vel_measured = LowPassFilter_Update(&M0_Vel_Flt, vel_measured);

    // PID计算
    float vel_error = target - vel_measured;
    float torque = DFOC_M0_VelPID(vel_error);

    // 输出FOC电压
    my_foc_set_torque(torque, my_foc_get_electrical_angle());

    // 更新上次值
    last_total_angle = total_angle;
    last_timestamp = timestamp_now;
}

/**
 * @brief 直接设置转矩输出
 * @param target 目标电压
 */
void DFOC_M0_SetTorque(float target) 
{
    my_foc_set_torque(target, my_foc_get_electrical_angle());
}

/* ======================= 初始化函数 ======================= */

/**
 * @brief FOC完整初始化流程
 * @param vbus 电源电压
 */
void DFOC_Init(float vbus) {
    voltage_power_supply = vbus;

    /* 启动PWM输出 */
    HAL_TIM_PWM_Start(PWM_TIM, PWM_CHANNEL_A);
    HAL_TIM_PWM_Start(PWM_TIM, PWM_CHANNEL_B);
    HAL_TIM_PWM_Start(PWM_TIM, PWM_CHANNEL_C);

    /* 初始化速度低通滤波器 */
    LowPassFilter_Init(&M0_Vel_Flt, VELOCITY_LPF_COEF);

    /* 初始化PID */
    PID_Init(&vel_loop_M0, 2.0f, 0.0f, 0.0f, 100000.0f, voltage_power_supply / 2.0f);
    PID_Init(&angle_loop_M0, 2.0f, 0.0f, 0.0f, 100000.0f, 100.0f);

    rtt_printf("[FOC] Init complete, vbus = %.2f V\r\n", vbus);
}

// 初始化PWM配置
void foc_pwm_config(void)
{
    motor_pwm.htim = PWM_TIM;
    motor_pwm.channelA = PWM_CHANNEL_A;
    motor_pwm.channelB = PWM_CHANNEL_B;
    motor_pwm.channelC = PWM_CHANNEL_C;
}

/* ======================= 电机目标角度接口 ======================= */

/**
 * @brief 设置目标角度
 * @param target 目标角度，单位：弧度
 */
void my_foc_set_target_angle(float target) {
    motor_target_angle = target;
}

/**
 * @brief 获取目标角度
 * @return 当前目标角度
 */
float my_foc_get_target_angle(void) {
    return motor_target_angle;
}


/**
 * @brief FOC 测试主流程
 */

#define MOTOR_PP        7      // 电机极对数，根据实际电机填写
#define MOTOR_DIR       1      // 电机方向 1=正转 -1=反转

void MyFOC_Test(void)
{
    /* ========== 1. 初始化硬件 ========== */
    rtt_printf("==== FOC Test Start ====\r\n");


    AS5600_InitDevice(&hi2c2, (0x36 << 1), 0);	    // 初始化AS5600编码器
	foc_pwm_config();    							// 初始化PWM配置
    my_foc_init(&motor_pwm, MOTOR_VOLTAGE, 0);    	// 初始化FOC模块
    DFOC_Init(MOTOR_VOLTAGE);    					// 初始化FOC控制器

    /* ========== 2. 电机零点对齐 ========== */
    rtt_printf("[FOC Test] Aligning motor zero...\r\n");
    my_foc_align_sensor(MOTOR_PP, MOTOR_DIR);
    rtt_printf("[FOC Test] Align done!\r\n");

    HAL_Delay(1000);

    /* ========== 3. 开环测试 ========== */
    rtt_printf("[FOC Test] Open-loop torque test!\r\n");
    for(float tq = 0.0f; tq < 3.0f; tq += 0.1f)
    {
        DFOC_M0_SetTorque(tq);
        HAL_Delay(100);
    }
    DFOC_M0_SetTorque(0.0f);
    rtt_printf("[FOC Test] Open-loop done!\r\n");

    HAL_Delay(1000);

    /* ========== 4. 速度闭环测试 ========== */
	/*
		总结流程
		测量电机速度 → vel_measured
		计算速度误差 → vel_error = target - vel_measured
		PID控制 → torque = PID(vel_error) → q轴电压
		FOC逆变换 → q轴电压 + 电角度 → αβ → ABC
		PWM输出 → 电机加减速
		核心逻辑：速度误差 → PID → q轴电压 → 三相PWM电压 → 电机转速调整
	*/
    rtt_printf("[FOC Test] Velocity loop test!\r\n");
    DFOC_M0_SetVelPID(2.0f, 0.2f, 0.0f, 10000.0f);  // 设置速度PID参数
    float vel_target = 5.0f; // 目标速度 rad/s
    uint32_t start_tick = HAL_GetTick();
    while(HAL_GetTick() - start_tick < 5000)  // 运行5秒
    {
        DFOC_M0_SetVelocity(vel_target);
        HAL_Delay(1);
    }
    DFOC_M0_SetTorque(0.0f);
    rtt_printf("[FOC Test] Velocity loop done!\r\n");
    HAL_Delay(1000);

	
    /* ========== 5. 角度+速度双闭环测试 ========== */
    rtt_printf("[FOC Test] Position+Velocity loop test!\r\n");
	/* 
		采样周期 T = 5ms = 0.005s

		角度PID：
			output_ramp = 2000
			每次采样最大输出增量：
				Δvel = 2000 * 0.005 = 10 rad/s
			这个输出速度给速度环，保证角度环不会瞬间给出过大速度。

		速度PID：
			output_ramp = 1200
			每次采样最大输出增量：
				ΔU = 1200 * 0.005 = 6 V
			这个输出电压给FOC，保证电机扭矩变化平稳，不会瞬间冲击PWM。
	*/
    DFOC_M0_SetAnglePID(3.0f, 0.1f, 0.0f, 2000.0f);  // 角度PID
    DFOC_M0_SetVelPID(2.0f, 0.1f, 0.0f, 1200.0f);   // 速度PID

    float target_angles[] = {M_PI/1, M_PI/2, M_PI, 3*M_PI/2, 2*M_PI};
    for(int i = 0; i < 5; i++)
    {
        rtt_printf("[FOC Test] Move to %.2f rad\r\n", target_angles[i]);
        uint32_t move_start = HAL_GetTick();
        while(HAL_GetTick() - move_start < 3000)  // 每个位置保持3秒
        {
            DFOC_M0_SetVelocityAngle(target_angles[i]);
            // 打印当前角度
            float now_angle = my_foc_get_motor_angle();
            rtt_printf("Current Angle: %.2f rad\r\n", now_angle);
            HAL_Delay(5);
        }
    }
    DFOC_M0_SetTorque(0.0f);
    rtt_printf("[FOC Test] Position+Velocity loop done!\r\n");

    rtt_printf("==== FOC Test Complete ====\r\n");
}


