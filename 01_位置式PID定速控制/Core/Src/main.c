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
#include "OLED.h"
#include "my_key.h"
#include "motor.h"
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
// ����PID������ʵ��
PID_Controller Motor_PID;

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
void my_oled_test(void)
{
	/*��(0, 0)λ����ʾ�ַ�'A'�������СΪ8*16����*/
	OLED_ShowChar(0, 0, 'A', OLED_8X16);
	/*��(16, 0)λ����ʾ�ַ���"Hello World!"�������СΪ8*16����*/
	OLED_ShowString(16, 0, "Hello World!", OLED_8X16);
	/*��(0, 18)λ����ʾ�ַ�'A'�������СΪ6*8����*/
	OLED_ShowChar(0, 18, 'A', OLED_6X8);
	/*��(16, 18)λ����ʾ�ַ���"Hello World!"�������СΪ6*8����*/
	OLED_ShowString(16, 18, "Hello World!", OLED_6X8);
	/*��(0, 28)λ����ʾ����12345������Ϊ5�������СΪ6*8����*/
	OLED_ShowNum(0, 28, 12345, 5, OLED_6X8);
	/*��(40, 28)λ����ʾ�з�������-66������Ϊ2�������СΪ6*8����*/
	OLED_ShowSignedNum(40, 28, -66, 2, OLED_6X8);
	/*��(70, 28)λ����ʾʮ����������0xA5A5������Ϊ4�������СΪ6*8����*/
	OLED_ShowHexNum(70, 28, 0xA5A5, 4, OLED_6X8);
	/*��(0, 38)λ����ʾ����������0xA5������Ϊ8�������СΪ6*8����*/
	OLED_ShowBinNum(0, 38, 0xA5, 8, OLED_6X8);
	/*��(60, 38)λ����ʾ��������123.45���������ֳ���Ϊ3��С�����ֳ���Ϊ2�������СΪ6*8����*/
	OLED_ShowFloatNum(60, 38, 123.45, 3, 2, OLED_6X8);
	/*��(0, 48)λ����ʾӢ�ĺͺ��ִ�"Hello,���硣"��֧����Ӣ�Ļ�д*/
	OLED_ShowString(0, 48, "Hello,���硣", OLED_8X16);
	/*��(96, 48)λ����ʾͼ�񣬿�16���أ���16���أ�ͼ������ΪDiode����*/
	OLED_ShowImage(96, 48, 16, 16, Diode);
	/*��(96, 18)λ�ô�ӡ��ʽ���ַ����������СΪ6*8���󣬸�ʽ���ַ���Ϊ"[%02d]"*/
	OLED_Printf(96, 18, OLED_6X8, "[%02d]", 6);
	/*����OLED_Update��������OLED�Դ���������ݸ��µ�OLEDӲ��������ʾ*/
	OLED_Update();
	/*��ʱ3000ms���۲�����*/
	HAL_Delay(3000);
	/*���OLED�Դ�����*/
	OLED_Clear();
	
	/*��(5, 8)λ�û���*/
	OLED_DrawPoint(5, 8);
	/*��ȡ(5, 8)λ�õĵ�*/
	if (OLED_GetPoint(5, 8))
	{
		/*���ָ�������������(10, 4)λ����ʾ�ַ���"YES"�������СΪ6*8����*/
		OLED_ShowString(10, 4, "YES", OLED_6X8);
	}
	else
	{
		/*���ָ����δ����������(10, 4)λ����ʾ�ַ���"NO "�������СΪ6*8����*/
		OLED_ShowString(10, 4, "NO ", OLED_6X8);
	}
	
	/*��(40, 0)��(127, 15)λ��֮�仭ֱ��*/
	OLED_DrawLine(40, 0, 127, 15);
	/*��(40, 15)��(127, 0)λ��֮�仭ֱ��*/
	OLED_DrawLine(40, 15, 127, 0);
	/*��(0, 20)λ�û����Σ���12���أ���15���أ�δ���*/
	OLED_DrawRectangle(0, 20, 12, 15, OLED_UNFILLED);
	/*��(0, 40)λ�û����Σ���12���أ���15���أ����*/
	OLED_DrawRectangle(0, 40, 12, 15, OLED_FILLED);
	/*��(20, 20)��(40, 25)��(30, 35)λ��֮�仭�����Σ�δ���*/
	OLED_DrawTriangle(20, 20, 40, 25, 30, 35, OLED_UNFILLED);
	/*��(20, 40)��(40, 45)��(30, 55)λ��֮�仭�����Σ����*/
	OLED_DrawTriangle(20, 40, 40, 45, 30, 55, OLED_FILLED);
	/*��(55, 27)λ�û�Բ���뾶8���أ�δ���*/
	OLED_DrawCircle(55, 27, 8, OLED_UNFILLED);
	/*��(55, 47)λ�û�Բ���뾶8���أ����*/
	OLED_DrawCircle(55, 47, 8, OLED_FILLED);
	/*��(82, 27)λ�û���Բ���������12���أ��������8���أ�δ���*/
	OLED_DrawEllipse(82, 27, 12, 8, OLED_UNFILLED);
	/*��(82, 47)λ�û���Բ���������12���أ��������8���أ����*/
	OLED_DrawEllipse(82, 47, 12, 8, OLED_FILLED);
	/*��(110, 18)λ�û�Բ�����뾶15���أ���ʼ�Ƕ�25�ȣ���ֹ�Ƕ�125�ȣ�δ���*/
	OLED_DrawArc(110, 18, 15, 25, 125, OLED_UNFILLED);
	/*��(110, 38)λ�û�Բ�����뾶15���أ���ʼ�Ƕ�25�ȣ���ֹ�Ƕ�125�ȣ����*/
	OLED_DrawArc(110, 38, 15, 25, 125, OLED_FILLED);
	/*����OLED_Update��������OLED�Դ���������ݸ��µ�OLEDӲ��������ʾ*/
	OLED_Update();
	/*��ʱ3000ms���۲�����*/
	HAL_Delay(1000);
	
	/*���OLED�Դ�����*/
	OLED_Clear();
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
	uint8_t KeyNum = 0;
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
  /* USER CODE BEGIN 2 */
	OLED_Init();
	motor_pwm_encoder_start();
//	my_oled_test();
	HAL_TIM_Base_Start_IT(&htim1); 								// htim1 ��ʱ�������ж�
	/*OLED��ӡһ������*/
	OLED_Printf(0, 0, OLED_8X16, "Speed Control");
	OLED_Update();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		/*�����޸�Ŀ��ֵ*/
	  
		# if 1
		/*�������ע�ͺ󣬼ǵ����ε�λ����ť�޸�Ŀ��ֵ�Ĵ���*/
		KeyNum = my_get_key_num();
		switch(KeyNum)
		{
			case 0:									//���K1����
				Motor_PID.PID_Target +=	10;			//Ŀ��ֵ��10
				break;
			case 1:									//���K2����
				Motor_PID.PID_Target -=	10;			//Ŀ��ֵ��10
				break;
			case 2:									//���K3����
				break;
			case 3:
				Motor_PID.PID_Target = 0;			//Ŀ��ֵ��0
				break;
			default:
				break;
		}
		#endif 
		Read_All_ADC_Channels();							//��λ����ȡֵ
		/*
			OLED��ʾ
			%4.2f  	
			%	��ʽ������ʼ��
			4	��С������Ϊ 4 ���ַ�
			.2	С������� 2 λ
			f	��ʾΪ��������fixed-point��
			%+04.0f	
			%	��ʼ��ʽ��
			+	������ʾ���ţ�+ �� -��
			0	�� 0 ������λ�������ǿո�
			4	��С������Ϊ 4
			.0	С������� 0 λ����ֻ��ʾ����
			f	��ʾΪ������
		*/
		OLED_Printf(0, 16, OLED_8X16, "Kp:%4.2f", Motor_PID.PID_Kd);			//��ʾKp
		OLED_Printf(0, 32, OLED_8X16, "Ki:%4.2f", Motor_PID.PID_Ki);			//��ʾKi
		OLED_Printf(0, 48, OLED_8X16, "Kd:%4.2f", Motor_PID.PID_Kd);			//��ʾKd
		
		OLED_Printf(64, 16, OLED_8X16, "Tar:%+04.0f", Motor_PID.PID_Target);	//��ʾĿ��ֵ
		OLED_Printf(64, 32, OLED_8X16, "Act:%+04.0f", Motor_PID.PID_Actual);	//��ʾʵ��ֵ
		OLED_Printf(64, 48, OLED_8X16, "Out:%+04.0f", Motor_PID.PWM_Out);		//��ʾ���ֵ
		
		OLED_Update();	//OLED���£�������ʾ�����������ô˺������£�������ʾ�����ݲ�����µ�OLED��
		

		/*
			//���ڴ�ӡĿ��ֵ��ʵ��ֵ�����ֵ
			//���SerialPlot��ͼ�����������ʾ���ݵĲ���
		*/
		printf("%f,%f,%f\r\n", Motor_PID.PID_Target, Motor_PID.PID_Actual, Motor_PID.PWM_Out);		//���ڴ�ӡĿ��ֵ��ʵ��ֵ�����ֵ
																								
																
																
//		my_key_test_func();								//����OLED��ʾ
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

