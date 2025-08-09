/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "systerm_select.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/*定义PID相关变量*/
uint8_t currentSystemStage = SYSTEM_STAGE_IDLE;   // 当前系统阶段（模式）
uint8_t nextSystemStage = SYSTEM_STAGE_IDLE;      // 下一系统阶段（模式）

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void read_pid_param_from_flash(void)
{
	//STM32的FLASH存储器里有一些掉电不丢失的参数
	//此处判断FLASH是第一次使用，还是已经存储过数据了
	if (FlashStore_Init())
	{
		/*进入if，表示FLAH是第一次使用*/
		/*调用保存参数的函数，初始化FLASH内存储的参数，即重置参数*/
		
		PID_SpeedControl_SaveParams();				
		PID_PositionControl_SaveParams();
		PID_daolibai_SaveParam();

		/*OLED显示已重置参数*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "     [提示]     ", OLED_8X16);
		OLED_ShowString(0, 16, "   已重置参数   ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "             K4>", OLED_8X16);
		OLED_Update();

		HAL_Delay(1000);
	}
}

void oled_display_idle_page(void)
{
	/*显示初始界面*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "    [FYZ-HHH]   ", OLED_8X16);
	OLED_ShowString(4, 16, "PID综合测试程序  ",   OLED_8X16);
	OLED_ShowString(0, 32, "      V1.1      ", OLED_8X16);
	OLED_ShowString(0, 48, "             K4>", OLED_8X16);
	OLED_Update();
}

void motor_pwm_encoder_start(void)
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);					//启动 PWM TIM_CHANNEL_1 PWM信号输出
//	  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);					//启动 PWM TIM_CHANNEL_2 PWM信号输出
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);  			// 启动htim3编码器模式
//	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);  			// 启动htim4编码器模式
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADCEx_Calibration_Start(&hadc1); // 校准
	HAL_ADCEx_Calibration_Start(&hadc2); // 校准
	OLED_Init();
	motor_pwm_encoder_start();									//电机PWM和编码器检测初始化
	HAL_TIM_Base_Start_IT(&htim1); 								// htim1 定时器更新中断

	read_pid_param_from_flash();
	oled_display_idle_page();
	/*等待K4按键按下*/
	while (Key_Check(4, KEY_SINGLE) == 0);
	Key_Clear();
	
	/*K4按键已按下，切换下一个模式为0x01*/
	nextSystemStage = SYSTEM_STAGE_SELECTION;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	if(nextSystemStage == currentSystemStage)		//currentSystemStage和nextSystemStage相同，表示模式没有切换
	{
		switch (currentSystemStage)			//currentSystemStage的值，执行对应模式的Loop函数
		{
			case SYSTEM_STAGE_SELECTION: 	//系统选择状态
				systerm_select_Loop(); break;
			case SYSTEM_STAGE_KEY_TEST: 	// 硬件按键测试
				KeyTest_Loop(); break;
			case SYSTEM_STAGE_RP_TEST: 		// 硬件RP测试
				RPTest_Loop(); break;
			case SYSTEM_STAGE_PWM_TEST: 	// 硬件PWM测试
				PWMTest_Loop(); break;
			case SYSTEM_STAGE_ENCODER_TEST: // 硬件编码器测试
				EncoderTest_Loop(); break;
			case SYSTEM_STAGE_ANGLE_TEST: 	// 硬件角度传感器测试
				AngleTest_Loop(); break;
			case SYSTEM_STAGE_SPEED_CONTROL: 		//定速控制
				motor_speed_control_Loop(); break;
			case SYSTEM_STAGE_LOCATION_CONTROL: 	//定位置控制
				motor_location_control_Loop(); break;
			case SYSTEM_STAGE_DAOLIBAI_CONTROL: 	//双环倒立摆控制
				systerm_daolibai_Loop(); break;
			case SYSTEM_STAGE_MENU: 
				Menu_Loop(); break;
		}
	}
	else						//CurrMode和NextMode不相同，表示当前处于模式切换的瞬间
	{
		switch (currentSystemStage)		//首先调用当前模式的Exit函数，处理掉当前模式的收尾工作
		{
			case SYSTEM_STAGE_PWM_TEST: 
				PWMTest_Exit(); break;
			case SYSTEM_STAGE_DAOLIBAI_CONTROL: 
				systerm_daolibai_Exit(); break;
			case SYSTEM_STAGE_MENU: 
				Menu_Exit(); break;
		}
		currentSystemStage = nextSystemStage;			//CurrMode更新为NextMode，完成模式切换
		Key_Clear();									//模式切换时，清除一下按键标志位，避免干扰下一个模式对按键的判断
		switch (nextSystemStage)						//模式切换后，执行一遍切换后模式的Init函数，用于初始化
		{
			/*界面显示和初始化*/
			case SYSTEM_STAGE_SELECTION: 
				systerm_select_Init(); break;
			case SYSTEM_STAGE_KEY_TEST: 
				KeyTest_Init(); break;
			case SYSTEM_STAGE_RP_TEST: 
				RPTest_Init(); break;
			case SYSTEM_STAGE_PWM_TEST: 
				PWMTest_Init(); break;
			case SYSTEM_STAGE_ENCODER_TEST: 
				EncoderTest_Init(); break;
			case SYSTEM_STAGE_ANGLE_TEST: 
				AngleTest_Init(); break;
			case SYSTEM_STAGE_MOTOR_CONTROL: 
				Motor_Control_Ready_Init(); break;
			case SYSTEM_STAGE_SPEED_CONTROL: 
				motor_speed_control_Init(); break;
			case SYSTEM_STAGE_LOCATION_CONTROL: 
				motor_location_control_Init(); break;
			case SYSTEM_STAGE_DAOLIBAI_IDLE: 
				Location_Ready_Init(); break;
			case SYSTEM_STAGE_DAOLIBAI_CONTROL: 
				systerm_daolibai_Init(); break;
			case SYSTEM_STAGE_MENU: 
				Menu_Init(); break;
		}
	}
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
