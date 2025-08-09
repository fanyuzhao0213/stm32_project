#include "motor.h"


int16_t g_Motor_Speed = 0;			//电机速度
int16_t g_Motor_Location= 0;		//电机位置

/**
  * 函    数：直流电机设置PWM
  * 参    数：PWM 要设置的PWM值，范围：-100~100（负数为反转）
  * 返 回 值：无
  */
void Motor_SetPWM(int16_t PWM)
{
	if (PWM >= 0)							//如果设置正转的PWM
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, PWM);	//设置 PWM TIM_CHANNEL_1 PWM信号输出
	}
	else									//否则，即设置反转的速度值
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, -PWM);	//设置 PWM TIM_CHANNEL_1 PWM信号输出
	}
}

//返回有符号位的16位计数器的值
int16_t Encoder_GetSpeed(void)
{
	int16_t temp;
	temp = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);  // 读取编码器值
//	__HAL_TIM_SET_COUNTER(&htim3, 0);		//因为速度计算外部已经相减了  所以这里不清零
	return -temp;
}

/**
  * 函    数：设置编码器的位置值（CNT值）
  * 参    数：Location 编码器的位置值（CNT值）
  * 返 回 值：无
  */
void Encoder_SetLocation(int16_t Location)
{
	__HAL_TIM_SET_COUNTER(&htim3, Location);
}

/**
  * 函    数：获取编码器的位置值（CNT值）
  * 返 回 值：编码器的位置值（CNT值）
  */
int16_t Encoder_GetLocation(void)
{
	int16_t temp;
	temp = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);  // 读取编码器值
	return -temp;
}
