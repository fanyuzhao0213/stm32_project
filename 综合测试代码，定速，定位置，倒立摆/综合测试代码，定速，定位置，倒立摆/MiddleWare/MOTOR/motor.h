#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"
#include "tim.h"
#include "pid.h"

/*---------------- PID 参数宏定义 ----------------*/

// 外环（位置环）参数
#define POS_KP       				0.3f    		// 比例系数
#define POS_KI      				0.0f    		// 积分系数
#define POS_KD       				0.4f    		// 微分系数
#define POS_OUT_MAX  				20.0f   		// 输出上限
#define POS_OUT_MIN 				-20.0f   		// 输出下限

// 内环（速度环）参数
#define SPD_KP      				0.3f    		// 比例系数
#define SPD_KI       				0.3f    		// 积分系数
#define SPD_KD       				0.0f    		// 微分系数
#define SPD_OUT_MAX  				100.0f  		// 输出上限
#define SPD_OUT_MIN 				-100.0f  		// 输出下限


// ===== 控制周期（单位 ms）=====
#define OUTER_LOOP_PERIOD_MS  		40
#define INNER_LOOP_PERIOD_MS  		40


extern void Motor_SetPWM(int16_t PWM);
extern int16_t Encoder_GetSpeed(void);
extern void Encoder_SetLocation(int16_t Location);
extern int16_t Encoder_GetLocation(void);
#endif

