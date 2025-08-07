/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
#include "usart.h"
#include "my_key.h"
#include "motor.h"
#include "OLED.h"
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM1 update interrupt.
  */
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */

  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */

  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

int16_t encoder_value = 0;
float speed = 0;				//这个代表每分钟多少转

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim_baseHandle)
{
	static uint8_t query_encoder_speed_count = 0;
	if(tim_baseHandle->Instance == htim1.Instance)
	{
		Key_Tick();						//按键扫描函数
		query_encoder_speed_count++;
		if(query_encoder_speed_count >= 40)	//40ms
		{
			query_encoder_speed_count = 0;
			
			#if 1
			
			Motor_PID.PID_Actual = Encoder_Get();									//获取当前实际的速度（代表40ms检测到的脉宽数）
			
			Motor_PID.PID_Error1 = Motor_PID.PID_Error0;							//获取上次误差
			Motor_PID.PID_Error0 = Motor_PID.PID_Target - Motor_PID.PID_Actual;		//获取本次误差，目标值减实际值，即为误差值
			
			#if 0
			/*误差积分（累加）*/
			/*如果Ki不为0，才进行误差积分，这样做的目的是便于调试*/
			/*因为在调试时，我们可能先把Ki设置为0，这时积分项无作用，误差消除不了，误差积分会积累到很大的值*/
			/*后续一旦Ki不为0，那么因为误差积分已经积累到很大的值了，这就导致积分项疯狂输出，不利于调试*/
			if (Motor_PID.PID_Ki != 0)				//如果Ki不为0
			{
				Motor_PID.PID_ErrorInt += Motor_PID.PID_Error0;		//进行误差积分
			}
			else									//否则
			{
				Motor_PID.PID_ErrorInt = 0;							//误差积分直接归0
			}
			#endif
			
			/*误差积分（累加）*/
			Motor_PID.PID_ErrorInt += Motor_PID.PID_Error0;		//进行误差积分
			/*
				积分限幅
				最大值可以计算或者实际测试得出，
				比如现在最大输出PWM为100，KI为0.2  则这个参数最大为500
			*/
			if (Motor_PID.PID_ErrorInt > 500) {Motor_PID.PID_ErrorInt = 500;}		//限制误差积分最大为500
			if (Motor_PID.PID_ErrorInt < -500) {Motor_PID.PID_ErrorInt = -500;}		//限制误差积分最小为-500
			
			
			/*PID计算*/
			/*使用位置式PID公式，计算得到输出值*/
			Motor_PID.PWM_Out = Motor_PID.PID_Kp * Motor_PID.PID_Error0 + \
								Motor_PID.PID_Ki * Motor_PID.PID_ErrorInt + \
								Motor_PID.PID_Kd * (Motor_PID.PID_Error0 - Motor_PID.PID_Error1);
			
			/*输出限幅 根据PWM最大输出参数来决定·*/
			if (Motor_PID.PWM_Out > 100) {Motor_PID.PWM_Out = 100;}		//限制输出值最大为100
			if (Motor_PID.PWM_Out < -100) {Motor_PID.PWM_Out = -100;}	//限制输出值最小为100
			
			/*执行控制*/
			/*输出值给到电机PWM*/
			/*因为此函数的输入范围是-100~100，所以上面输出限幅，需要给Out值限定在-100~100*/
			if(MotorErrorFlag == 1)
			{
				Motor_SetPWM(Motor_PID.PWM_Out);
			}else
			{
				Motor_SetPWM(0);	
			}
			

			#else
			encoder_value = Encoder_Get();
			//一圈是11个脉冲  4倍频  就是44个脉冲   减速比是9.3左右,因此一圈是  408个脉冲，这个是40ms检测到的脉冲，因此还要除以0.04
			speed = encoder_value/408.0/0.04;
			// 你可以在这里做其他处理，比如计算速度、位置变化等
			// 这个只是获取到的脉宽个数  还需要进一步处理来计算速度
			/*获取实际速度值*/
			/*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
			/*此值正比于速度，所以可以表示速度，但它的单位并不是速度的标准单位*/
			/*此处每隔40ms获取一次计次值增量，电机旋转一周的计次值增量约为408*/
			/*因此如果想转换为标准单位，比如转/秒*/
			/*则可将此句代码改成Actual = Encoder_Get() / 408.0 / 0.04;*/
			printf("Encoder: %d\r\n", encoder_value);
			printf("speed: %.2f\r\n", speed);
//			OLED_Printf(0, 16, OLED_8X16, "Encoder: %d",encoder_value);
//			OLED_Printf(0, 32, OLED_8X16, "speed: %.2f",speed);
//			OLED_Update();
			#endif
		}
//		printf("htim1 CALLBACK!\r\n");
	}
}
/* USER CODE END 1 */

