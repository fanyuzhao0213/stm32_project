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
float speed = 0;				//�������ÿ���Ӷ���ת

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim_baseHandle)
{
	static uint8_t query_encoder_speed_count = 0;
	if(tim_baseHandle->Instance == htim1.Instance)
	{
		Key_Tick();						//����ɨ�躯��
		query_encoder_speed_count++;
		if(query_encoder_speed_count >= 40)	//40ms
		{
			query_encoder_speed_count = 0;
			
			#if 1
			
			Motor_PID.PID_Actual = Encoder_Get();									//��ȡ��ǰʵ�ʵ��ٶȣ�����40ms��⵽����������
			
			Motor_PID.PID_Error1 = Motor_PID.PID_Error0;							//��ȡ�ϴ����
			Motor_PID.PID_Error0 = Motor_PID.PID_Target - Motor_PID.PID_Actual;		//��ȡ������Ŀ��ֵ��ʵ��ֵ����Ϊ���ֵ
			
			#if 0
			/*�����֣��ۼӣ�*/
			/*���Ki��Ϊ0���Ž��������֣���������Ŀ���Ǳ��ڵ���*/
			/*��Ϊ�ڵ���ʱ�����ǿ����Ȱ�Ki����Ϊ0����ʱ�����������ã�����������ˣ������ֻ���۵��ܴ��ֵ*/
			/*����һ��Ki��Ϊ0����ô��Ϊ�������Ѿ����۵��ܴ��ֵ�ˣ���͵��»�����������������ڵ���*/
			if (Motor_PID.PID_Ki != 0)				//���Ki��Ϊ0
			{
				Motor_PID.PID_ErrorInt += Motor_PID.PID_Error0;		//����������
			}
			else									//����
			{
				Motor_PID.PID_ErrorInt = 0;							//������ֱ�ӹ�0
			}
			#endif
			
			/*�����֣��ۼӣ�*/
			Motor_PID.PID_ErrorInt += Motor_PID.PID_Error0;		//����������
			/*
				�����޷�
				���ֵ���Լ������ʵ�ʲ��Եó���
				��������������PWMΪ100��KIΪ0.2  ������������Ϊ500
			*/
			if (Motor_PID.PID_ErrorInt > 500) {Motor_PID.PID_ErrorInt = 500;}		//�������������Ϊ500
			if (Motor_PID.PID_ErrorInt < -500) {Motor_PID.PID_ErrorInt = -500;}		//������������СΪ-500
			
			
			/*PID����*/
			/*ʹ��λ��ʽPID��ʽ������õ����ֵ*/
			Motor_PID.PWM_Out = Motor_PID.PID_Kp * Motor_PID.PID_Error0 + \
								Motor_PID.PID_Ki * Motor_PID.PID_ErrorInt + \
								Motor_PID.PID_Kd * (Motor_PID.PID_Error0 - Motor_PID.PID_Error1);
			
			/*����޷� ����PWM������������������*/
			if (Motor_PID.PWM_Out > 100) {Motor_PID.PWM_Out = 100;}		//�������ֵ���Ϊ100
			if (Motor_PID.PWM_Out < -100) {Motor_PID.PWM_Out = -100;}	//�������ֵ��СΪ100
			
			/*ִ�п���*/
			/*���ֵ�������PWM*/
			/*��Ϊ�˺��������뷶Χ��-100~100��������������޷�����Ҫ��Outֵ�޶���-100~100*/
			if(MotorErrorFlag == 1)
			{
				Motor_SetPWM(Motor_PID.PWM_Out);
			}else
			{
				Motor_SetPWM(0);	
			}
			

			#else
			encoder_value = Encoder_Get();
			//һȦ��11������  4��Ƶ  ����44������   ���ٱ���9.3����,���һȦ��  408�����壬�����40ms��⵽�����壬��˻�Ҫ����0.04
			speed = encoder_value/408.0/0.04;
			// �����������������������������ٶȡ�λ�ñ仯��
			// ���ֻ�ǻ�ȡ�����������  ����Ҫ��һ�������������ٶ�
			/*��ȡʵ���ٶ�ֵ*/
			/*Encoder_Get���������Ի�ȡ���ζ�ȡ�������ļƴ�ֵ����*/
			/*��ֵ�������ٶȣ����Կ��Ա�ʾ�ٶȣ������ĵ�λ�������ٶȵı�׼��λ*/
			/*�˴�ÿ��40ms��ȡһ�μƴ�ֵ�����������תһ�ܵļƴ�ֵ����ԼΪ408*/
			/*��������ת��Ϊ��׼��λ������ת/��*/
			/*��ɽ��˾����ĳ�Actual = Encoder_Get() / 408.0 / 0.04;*/
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

