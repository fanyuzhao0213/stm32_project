#ifndef SYSTERM_DAOLIBAI_H
#define SYSTERM_DAOLIBAI_H

#include "main.h"


void PID_daolibai_SaveParam(void);	//��������������ڵ�Kp��Ki��Kd�����ĽǶȡ��������ȡ�����ʱ�䡢PWMƫ�ƣ�
void PID_daolibai_LoadParam(void);	//���ز����������ڵ�Kp��Ki��Kd�����ĽǶȡ��������ȡ�����ʱ�䡢PWMƫ�ƣ�


void systerm_daolibai_Init(void);
void systerm_daolibai_Exit(void);
void systerm_daolibai_Tick(void);
void systerm_daolibai_Loop(void);
#endif

