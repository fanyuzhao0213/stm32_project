#include "motor.h"


int16_t g_Motor_Speed = 0;			//����ٶ�
int16_t g_Motor_Location= 0;		//���λ��

/**
  * ��    ����ֱ���������PWM
  * ��    ����PWM Ҫ���õ�PWMֵ����Χ��-100~100������Ϊ��ת��
  * �� �� ֵ����
  */
void Motor_SetPWM(int16_t PWM)
{
	if (PWM >= 0)							//���������ת��PWM
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, PWM);	//���� PWM TIM_CHANNEL_1 PWM�ź����
	}
	else									//���򣬼����÷�ת���ٶ�ֵ
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, -PWM);	//���� PWM TIM_CHANNEL_1 PWM�ź����
	}
}

//�����з���λ��16λ��������ֵ
int16_t Encoder_GetSpeed(void)
{
	int16_t temp;
	temp = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);  // ��ȡ������ֵ
//	__HAL_TIM_SET_COUNTER(&htim3, 0);		//��Ϊ�ٶȼ����ⲿ�Ѿ������  �������ﲻ����
	return -temp;
}

/**
  * ��    �������ñ�������λ��ֵ��CNTֵ��
  * ��    ����Location ��������λ��ֵ��CNTֵ��
  * �� �� ֵ����
  */
void Encoder_SetLocation(int16_t Location)
{
	__HAL_TIM_SET_COUNTER(&htim3, Location);
}

/**
  * ��    ������ȡ��������λ��ֵ��CNTֵ��
  * �� �� ֵ����������λ��ֵ��CNTֵ��
  */
int16_t Encoder_GetLocation(void)
{
	int16_t temp;
	temp = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);  // ��ȡ������ֵ
	return -temp;
}
