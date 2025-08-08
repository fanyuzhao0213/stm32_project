#include "motor.h"


/*定义串级PID结构体*/
CascadePID_t motorPID;


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

/**
 * @brief  初始化电机 PID 参数
 * @note   采用串级 PID 控制，外环为位置环，内环为速度环
 *         外环输出作为内环的目标速度，内环最终控制 PWM 输出
 * 
 *         参数通过宏定义配置，方便调试与统一管理
 */
void Motor_PID_Config(void)
{
    // 初始化外环（位置环）PID
    PID_Init(&motorPID.PositionPID, 
             POS_KP, POS_KI, POS_KD, 
             POS_OUT_MAX, POS_OUT_MIN);

    // 初始化内环（速度环）PID
    PID_Init(&motorPID.SpeedPID,    
             SPD_KP, SPD_KI, SPD_KD, 
             SPD_OUT_MAX, SPD_OUT_MIN);
}



//返回有符号位的16位计数器的值
int16_t Encoder_Get(void)
{
	int16_t temp;
	temp = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);  // 读取编码器值
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	return -temp;
}
