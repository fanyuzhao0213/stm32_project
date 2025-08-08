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
#define POS_OUT_MAX  				100.0f   		// 输出上限,代表给速度的参数是正向最大100个单位
#define POS_OUT_MIN 				-50.0f   		// 输出下限,代表给速度的参数是反向最大20个单位

// 内环（速度环）参数
#define SPD_KP      				0.3f    		// 比例系数
#define SPD_KI       				0.3f    		// 积分系数
#define SPD_KD       				0.0f    		// 微分系数
#define SPD_OUT_MAX  				100.0f  		// 输出上限
#define SPD_OUT_MIN 				-100.0f  		// 输出下限


// ===== 控制周期（单位 ms）=====
#define OUTER_LOOP_PERIOD_MS  		40
#define INNER_LOOP_PERIOD_MS  		40

#define USE_CascadePID_UPDATE  		1				//宏定义控制是否使用串级PID更新 这个根据串级PID的更新时间是否相等来确定是否使用

extern CascadePID_t motorPID;


extern int16_t g_Motor_Speed;			//电机速度
extern int16_t g_Motor_Location;		//电机位置

extern void Motor_SetPWM(int16_t PWM);
extern int16_t Encoder_Get(void);
extern void Motor_PID_Config(void);
#endif

