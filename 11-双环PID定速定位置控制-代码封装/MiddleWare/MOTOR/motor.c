#include "motor.h"


/*���崮��PID�ṹ��*/
CascadePID_t motorPID;


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

/**
 * @brief  ��ʼ����� PID ����
 * @note   ���ô��� PID ���ƣ��⻷Ϊλ�û����ڻ�Ϊ�ٶȻ�
 *         �⻷�����Ϊ�ڻ���Ŀ���ٶȣ��ڻ����տ��� PWM ���
 * 
 *         ����ͨ���궨�����ã����������ͳһ����
 */
void Motor_PID_Config(void)
{
    // ��ʼ���⻷��λ�û���PID
    PID_Init(&motorPID.PositionPID, 
             POS_KP, POS_KI, POS_KD, 
             POS_OUT_MAX, POS_OUT_MIN);

    // ��ʼ���ڻ����ٶȻ���PID
    PID_Init(&motorPID.SpeedPID,    
             SPD_KP, SPD_KI, SPD_KD, 
             SPD_OUT_MAX, SPD_OUT_MIN);
}



//�����з���λ��16λ��������ֵ
int16_t Encoder_Get(void)
{
	int16_t temp;
	temp = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);  // ��ȡ������ֵ
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	return -temp;
}
