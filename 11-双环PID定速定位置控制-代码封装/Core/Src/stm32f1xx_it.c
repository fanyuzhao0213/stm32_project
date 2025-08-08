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
#include "pid.h"
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim_baseHandle)
{
	static uint16_t innerCount = 0; // 内环计数，速度环
    static uint16_t outerCount = 0; // 外环计数，位置环
	
	if(tim_baseHandle->Instance == htim1.Instance)
	{
		Key_Tick();						//按键扫描函数
		innerCount++;
		outerCount++;
		if(innerCount >= INNER_LOOP_PERIOD_MS)	//40ms	内环速度环处理
		{
			innerCount = 0;
			#if !USE_CascadePID_UPDATE
			/*获取实际位置值*/
			/*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
			/*计次值增量进行累加，即可得到计次值本身（即实际位置）*/
			/*这里先获取增量，再进行累加，实际上是绕了个弯子*/
			/*如果只需要得到编码器的位置，而不需要得到速度*/
			/*则Encode_Get函数内部的代码可以修改为return TIM_GetCounter(TIM3);直接返回CNT计数器的值*/
			/*修改后，此处代码改为Actual = Encoder_Get();直接得到位置，就不再需要累加了，这样更直接*/
			g_Motor_Speed = Encoder_Get();								//获取速度值
			g_Motor_Location += g_Motor_Speed;							//获取速度值以后 encode会清零 因此需要位置累加
			
			motorPID.SpeedPID.PID_Actual = g_Motor_Speed;				// 内环实际值（速度）
			PID_Update(&motorPID.SpeedPID);								// 内环 PID 计算								
			Motor_SetPWM(motorPID.SpeedPID.PID_PwmOut);					// 控制 电机输出PWM

			#else
			/*
				1、可以使用串级PID一次性更新
				2、前提条件是串级PID更新的时间是一致的，否则还是要分开
			*/
			
			// 1. 获取速度和位置
			g_Motor_Speed = Encoder_Get();								//获取速度值
			g_Motor_Location += g_Motor_Speed;							//获取速度值以后 encode会清零 因此需要位置累加
			
			// 2. 更新 PID 的输入量
			motorPID.SpeedPID.PID_Actual = g_Motor_Speed;				// 内环实际速度
			motorPID.PositionPID.PID_Actual = g_Motor_Location;			// 外环实际位置
			
			// 3. 运行串级 PID 更新（外环→内环）
			CascadePID_Update(&motorPID);
			
			 // 4. 内环的 PWM 输出作用到电机
			Motor_SetPWM(motorPID.SpeedPID.PID_PwmOut);
	
			#if 0
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
			#endif
			#endif
		}
		#if !USE_CascadePID_UPDATE
		if(outerCount >= OUTER_LOOP_PERIOD_MS)	//40ms	外环位置环处理
		{
			outerCount = 0;

			motorPID.PositionPID.PID_Actual = g_Motor_Location;			// 外环实际值（位置）
			PID_Update(&motorPID.PositionPID);							// 外环 PID 计算		
			motorPID.SpeedPID.PID_Target = motorPID.PositionPID.PID_PwmOut; // 外环输出作为内环目标速度,而不是直接作用于电机
			Motor_SetPWM(motorPID.SpeedPID.PID_PwmOut);					// 控制 电机输出PWM
		}
		#endif
	}
}
/* USER CODE END 1 */

