#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"
#include "tim.h"






extern void Motor_SetPWM(int16_t PWM);
extern int16_t Encoder_Get(void);
#endif

