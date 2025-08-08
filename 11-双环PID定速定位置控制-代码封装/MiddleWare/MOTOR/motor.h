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
#define POS_OUT_MAX  				100.0f   		// �������,������ٶȵĲ������������100����λ
#define POS_OUT_MIN 				-50.0f   		// �������,������ٶȵĲ����Ƿ������20����λ

// �ڻ����ٶȻ�������
#define SPD_KP      				0.3f    		// ����ϵ��
#define SPD_KI       				0.3f    		// ����ϵ��
#define SPD_KD       				0.0f    		// ΢��ϵ��
#define SPD_OUT_MAX  				100.0f  		// �������
#define SPD_OUT_MIN 				-100.0f  		// �������


// ===== �������ڣ���λ ms��=====
#define OUTER_LOOP_PERIOD_MS  		40
#define INNER_LOOP_PERIOD_MS  		40

#define USE_CascadePID_UPDATE  		1				//�궨������Ƿ�ʹ�ô���PID���� ������ݴ���PID�ĸ���ʱ���Ƿ������ȷ���Ƿ�ʹ��

extern CascadePID_t motorPID;


extern int16_t g_Motor_Speed;			//����ٶ�
extern int16_t g_Motor_Location;		//���λ��

extern void Motor_SetPWM(int16_t PWM);
extern int16_t Encoder_Get(void);
extern void Motor_PID_Config(void);
#endif

