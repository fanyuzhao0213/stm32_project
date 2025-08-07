#include "motor.h"



/**
  * 函    数：直流电机设置PWM
  * 参    数：PWM 要设置的PWM值，范围：-100~100（负数为反转）
  * 返 回 值：无
  */
void Motor_SetPWM(int8_t PWM)
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
int16_t Encoder_Get(void)
{
	int16_t temp;
	temp = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);  // 读取编码器值
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	return temp;
}