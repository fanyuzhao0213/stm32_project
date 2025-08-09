#ifndef SYSTERM_DAOLIBAI_H
#define SYSTERM_DAOLIBAI_H

#include "main.h"


void PID_daolibai_SaveParam(void);	//保存参数（倒立摆的Kp、Ki、Kd、中心角度、启动力度、启动时间、PWM偏移）
void PID_daolibai_LoadParam(void);	//加载参数（倒立摆的Kp、Ki、Kd、中心角度、启动力度、启动时间、PWM偏移）


void systerm_daolibai_Init(void);
void systerm_daolibai_Exit(void);
void systerm_daolibai_Tick(void);
void systerm_daolibai_Loop(void);
#endif

