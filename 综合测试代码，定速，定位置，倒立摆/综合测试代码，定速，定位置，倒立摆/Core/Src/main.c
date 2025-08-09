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

/*����PID��ر���*/
uint8_t currentSystemStage = SYSTEM_STAGE_IDLE;   // ��ǰϵͳ�׶Σ�ģʽ��
uint8_t nextSystemStage = SYSTEM_STAGE_IDLE;      // ��һϵͳ�׶Σ�ģʽ��

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
	//STM32��FLASH�洢������һЩ���粻��ʧ�Ĳ���
	//�˴��ж�FLASH�ǵ�һ��ʹ�ã������Ѿ��洢��������
	if (FlashStore_Init())
	{
		/*����if����ʾFLAH�ǵ�һ��ʹ��*/
		/*���ñ�������ĺ�������ʼ��FLASH�ڴ洢�Ĳ����������ò���*/
		
		PID_SpeedControl_SaveParams();				
		PID_PositionControl_SaveParams();
		PID_daolibai_SaveParam();

		/*OLED��ʾ�����ò���*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "     [��ʾ]     ", OLED_8X16);
		OLED_ShowString(0, 16, "   �����ò���   ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "             K4>", OLED_8X16);
		OLED_Update();

		HAL_Delay(1000);
	}
}

void oled_display_idle_page(void)
{
	/*��ʾ��ʼ����*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "    [FYZ-HHH]   ", OLED_8X16);
	OLED_ShowString(4, 16, "PID�ۺϲ��Գ���  ",   OLED_8X16);
	OLED_ShowString(0, 32, "      V1.1      ", OLED_8X16);
	OLED_ShowString(0, 48, "             K4>", OLED_8X16);
	OLED_Update();
}

void motor_pwm_encoder_start(void)
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);					//���� PWM TIM_CHANNEL_1 PWM�ź����
//	  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);					//���� PWM TIM_CHANNEL_2 PWM�ź����
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);  			// ����htim3������ģʽ
//	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);  			// ����htim4������ģʽ
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
	HAL_ADCEx_Calibration_Start(&hadc1); // У׼
	HAL_ADCEx_Calibration_Start(&hadc2); // У׼
	OLED_Init();
	motor_pwm_encoder_start();									//���PWM�ͱ���������ʼ��
	HAL_TIM_Base_Start_IT(&htim1); 								// htim1 ��ʱ�������ж�

	read_pid_param_from_flash();
	oled_display_idle_page();
	/*�ȴ�K4��������*/
	while (Key_Check(4, KEY_SINGLE) == 0);
	Key_Clear();
	
	/*K4�����Ѱ��£��л���һ��ģʽΪ0x01*/
	nextSystemStage = SYSTEM_STAGE_SELECTION;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	if(nextSystemStage == currentSystemStage)		//currentSystemStage��nextSystemStage��ͬ����ʾģʽû���л�
	{
		switch (currentSystemStage)			//currentSystemStage��ֵ��ִ�ж�Ӧģʽ��Loop����
		{
			case SYSTEM_STAGE_SELECTION: 	//ϵͳѡ��״̬
				systerm_select_Loop(); break;
			case SYSTEM_STAGE_KEY_TEST: 	// Ӳ����������
				KeyTest_Loop(); break;
			case SYSTEM_STAGE_RP_TEST: 		// Ӳ��RP����
				RPTest_Loop(); break;
			case SYSTEM_STAGE_PWM_TEST: 	// Ӳ��PWM����
				PWMTest_Loop(); break;
			case SYSTEM_STAGE_ENCODER_TEST: // Ӳ������������
				EncoderTest_Loop(); break;
			case SYSTEM_STAGE_ANGLE_TEST: 	// Ӳ���Ƕȴ���������
				AngleTest_Loop(); break;
			case SYSTEM_STAGE_SPEED_CONTROL: 		//���ٿ���
				motor_speed_control_Loop(); break;
			case SYSTEM_STAGE_LOCATION_CONTROL: 	//��λ�ÿ���
				motor_location_control_Loop(); break;
			case SYSTEM_STAGE_DAOLIBAI_CONTROL: 	//˫�������ڿ���
				systerm_daolibai_Loop(); break;
			case SYSTEM_STAGE_MENU: 
				Menu_Loop(); break;
		}
	}
	else						//CurrMode��NextMode����ͬ����ʾ��ǰ����ģʽ�л���˲��
	{
		switch (currentSystemStage)		//���ȵ��õ�ǰģʽ��Exit�������������ǰģʽ����β����
		{
			case SYSTEM_STAGE_PWM_TEST: 
				PWMTest_Exit(); break;
			case SYSTEM_STAGE_DAOLIBAI_CONTROL: 
				systerm_daolibai_Exit(); break;
			case SYSTEM_STAGE_MENU: 
				Menu_Exit(); break;
		}
		currentSystemStage = nextSystemStage;			//CurrMode����ΪNextMode�����ģʽ�л�
		Key_Clear();									//ģʽ�л�ʱ�����һ�°�����־λ�����������һ��ģʽ�԰������ж�
		switch (nextSystemStage)						//ģʽ�л���ִ��һ���л���ģʽ��Init���������ڳ�ʼ��
		{
			/*������ʾ�ͳ�ʼ��*/
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
