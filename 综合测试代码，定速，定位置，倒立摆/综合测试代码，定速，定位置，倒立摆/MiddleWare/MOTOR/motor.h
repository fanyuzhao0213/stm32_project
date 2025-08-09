#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"
#include "tim.h"
#include "pid.h"

/*---------------- PID �����궨�� ----------------*/

// �⻷��λ�û�������
#define POS_KP       				0.3f    		// ����ϵ��
#define POS_KI      				0.0f    		// ����ϵ��
#define POS_KD       				0.4f    		// ΢��ϵ��
#define POS_OUT_MAX  				20.0f   		// �������
#define POS_OUT_MIN 				-20.0f   		// �������

// �ڻ����ٶȻ�������
#define SPD_KP      				0.3f    		// ����ϵ��
#define SPD_KI       				0.3f    		// ����ϵ��
#define SPD_KD       				0.0f    		// ΢��ϵ��
#define SPD_OUT_MAX  				100.0f  		// �������
#define SPD_OUT_MIN 				-100.0f  		// �������


// ===== �������ڣ���λ ms��=====
#define OUTER_LOOP_PERIOD_MS  		40
#define INNER_LOOP_PERIOD_MS  		40


extern void Motor_SetPWM(int16_t PWM);
extern int16_t Encoder_GetSpeed(void);
extern void Encoder_SetLocation(int16_t Location);
extern int16_t Encoder_GetLocation(void);
#endif

