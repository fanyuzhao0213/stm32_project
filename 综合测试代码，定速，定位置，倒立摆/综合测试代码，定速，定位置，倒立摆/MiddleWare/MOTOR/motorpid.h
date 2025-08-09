#ifndef __MOTOR_PID_H
#define __MOTOR_PID_H

#include "pid.h"
#include <math.h>
#include "motor.h"
#include "FlashStore.h"
#include "OLED.h"
#include "my_key.h"

/**
 * @brief 速度控制 PID 参数
 */
extern PID_t Speed_pid;

/**
 * @brief 位置控制 PID 参数
 */
extern PID_t Location_pid;


/**
 * @brief 初始化定速控制模式
 */
void PID_SpeedControl_Init(void);

/**
 * @brief 定速控制模式循环
 */
void PID_SpeedControl_Process(void);

/**
 * @brief 定速控制周期任务（40ms 调用一次）
 */
void PID_SpeedControl_Update(void);

/**
 * @brief 初始化定位置控制模式
 */
void PID_PositionControl_Init(void);

/**
 * @brief 定位置控制模式循环
 */
void PID_PositionControl_Process(void);

/**
 * @brief 定位置控制周期任务（40ms 调用一次）
 */
void PID_PositionControl_Update(void);

/**
 * @brief 保存定速 PID 参数到 Flash
 */
void PID_SpeedControl_SaveParams(void);

/**
 * @brief 从 Flash 加载定速 PID 参数
 */
void PID_SpeedControl_LoadParams(void);

/**
 * @brief 保存定位置 PID 参数到 Flash
 */
void PID_PositionControl_SaveParams(void);

/**
从 Flash 加载定位置 PID 参数
 */
void PID_PositionControl_LoadParams(void);


#endif

