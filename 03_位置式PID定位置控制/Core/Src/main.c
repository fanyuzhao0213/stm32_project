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

/*定义PID相关变量*/
// 创建PID控制器实例
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
	/*在(0, 0)位置显示字符'A'，字体大小为8*16点阵*/
	OLED_ShowChar(0, 0, 'A', OLED_8X16);
	/*在(16, 0)位置显示字符串"Hello World!"，字体大小为8*16点阵*/
	OLED_ShowString(16, 0, "Hello World!", OLED_8X16);
	/*在(0, 18)位置显示字符'A'，字体大小为6*8点阵*/
	OLED_ShowChar(0, 18, 'A', OLED_6X8);
	/*在(16, 18)位置显示字符串"Hello World!"，字体大小为6*8点阵*/
	OLED_ShowString(16, 18, "Hello World!", OLED_6X8);
	/*在(0, 28)位置显示数字12345，长度为5，字体大小为6*8点阵*/
	OLED_ShowNum(0, 28, 12345, 5, OLED_6X8);
	/*在(40, 28)位置显示有符号数字-66，长度为2，字体大小为6*8点阵*/
	OLED_ShowSignedNum(40, 28, -66, 2, OLED_6X8);
	/*在(70, 28)位置显示十六进制数字0xA5A5，长度为4，字体大小为6*8点阵*/
	OLED_ShowHexNum(70, 28, 0xA5A5, 4, OLED_6X8);
	/*在(0, 38)位置显示二进制数字0xA5，长度为8，字体大小为6*8点阵*/
	OLED_ShowBinNum(0, 38, 0xA5, 8, OLED_6X8);
	/*在(60, 38)位置显示浮点数字123.45，整数部分长度为3，小数部分长度为2，字体大小为6*8点阵*/
	OLED_ShowFloatNum(60, 38, 123.45, 3, 2, OLED_6X8);
	/*在(0, 48)位置显示英文和汉字串"Hello,世界。"，支持中英文混写*/
	OLED_ShowString(0, 48, "Hello,世界。", OLED_8X16);
	/*在(96, 48)位置显示图像，宽16像素，高16像素，图像数据为Diode数组*/
	OLED_ShowImage(96, 48, 16, 16, Diode);
	/*在(96, 18)位置打印格式化字符串，字体大小为6*8点阵，格式化字符串为"[%02d]"*/
	OLED_Printf(96, 18, OLED_6X8, "[%02d]", 6);
	/*调用OLED_Update函数，将OLED显存数组的内容更新到OLED硬件进行显示*/
	OLED_Update();
	/*延时3000ms，观察现象*/
	HAL_Delay(3000);
	/*清空OLED显存数组*/
	OLED_Clear();
	
	/*在(5, 8)位置画点*/
	OLED_DrawPoint(5, 8);
	/*获取(5, 8)位置的点*/
	if (OLED_GetPoint(5, 8))
	{
		/*如果指定点点亮，则在(10, 4)位置显示字符串"YES"，字体大小为6*8点阵*/
		OLED_ShowString(10, 4, "YES", OLED_6X8);
	}
	else
	{
		/*如果指定点未点亮，则在(10, 4)位置显示字符串"NO "，字体大小为6*8点阵*/
		OLED_ShowString(10, 4, "NO ", OLED_6X8);
	}
	
	/*在(40, 0)和(127, 15)位置之间画直线*/
	OLED_DrawLine(40, 0, 127, 15);
	/*在(40, 15)和(127, 0)位置之间画直线*/
	OLED_DrawLine(40, 15, 127, 0);
	/*在(0, 20)位置画矩形，宽12像素，高15像素，未填充*/
	OLED_DrawRectangle(0, 20, 12, 15, OLED_UNFILLED);
	/*在(0, 40)位置画矩形，宽12像素，高15像素，填充*/
	OLED_DrawRectangle(0, 40, 12, 15, OLED_FILLED);
	/*在(20, 20)、(40, 25)和(30, 35)位置之间画三角形，未填充*/
	OLED_DrawTriangle(20, 20, 40, 25, 30, 35, OLED_UNFILLED);
	/*在(20, 40)、(40, 45)和(30, 55)位置之间画三角形，填充*/
	OLED_DrawTriangle(20, 40, 40, 45, 30, 55, OLED_FILLED);
	/*在(55, 27)位置画圆，半径8像素，未填充*/
	OLED_DrawCircle(55, 27, 8, OLED_UNFILLED);
	/*在(55, 47)位置画圆，半径8像素，填充*/
	OLED_DrawCircle(55, 47, 8, OLED_FILLED);
	/*在(82, 27)位置画椭圆，横向半轴12像素，纵向半轴8像素，未填充*/
	OLED_DrawEllipse(82, 27, 12, 8, OLED_UNFILLED);
	/*在(82, 47)位置画椭圆，横向半轴12像素，纵向半轴8像素，填充*/
	OLED_DrawEllipse(82, 47, 12, 8, OLED_FILLED);
	/*在(110, 18)位置画圆弧，半径15像素，起始角度25度，终止角度125度，未填充*/
	OLED_DrawArc(110, 18, 15, 25, 125, OLED_UNFILLED);
	/*在(110, 38)位置画圆弧，半径15像素，起始角度25度，终止角度125度，填充*/
	OLED_DrawArc(110, 38, 15, 25, 125, OLED_FILLED);
	/*调用OLED_Update函数，将OLED显存数组的内容更新到OLED硬件进行显示*/
	OLED_Update();
	/*延时3000ms，观察现象*/
	HAL_Delay(1000);
	
	/*清空OLED显存数组*/
	OLED_Clear();
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
	HAL_TIM_Base_Start_IT(&htim1); 								// htim1 定时器更新中断
	/*OLED打印一个标题*/
	OLED_Printf(0, 0, OLED_8X16, "Speed Control");
	OLED_Update();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		/*按键修改目标值*/
	  
		# if 1
		/*解除以下注释后，记得屏蔽电位器旋钮修改目标值的代码*/
		KeyNum = my_get_key_num();
		switch(KeyNum)
		{
			case 0:									//如果K1按下
				Motor_PID.PID_Target +=	10;			//目标值加10
				break;
			case 1:									//如果K2按下
				Motor_PID.PID_Target -=	10;			//目标值加10
				break;
			case 2:									//如果K3按下
				break;
			case 3:
				Motor_PID.PID_Target = 0;			//目标值归0
				break;
			default:
				break;
		}
		#endif 
		Read_All_ADC_Channels();							//电位器读取值
		/*
			OLED显示
			%4.2f  	
			%	格式化的起始符
			4	最小输出宽度为 4 个字符
			.2	小数点后保留 2 位
			f	显示为浮点数（fixed-point）
			%+04.0f	
			%	开始格式化
			+	总是显示符号（+ 或 -）
			0	用 0 填充多余位（而不是空格）
			4	最小输出宽度为 4
			.0	小数点后保留 0 位，即只显示整数
			f	显示为浮点数
		*/
		OLED_Printf(0, 16, OLED_8X16, "Kp:%4.2f", Motor_PID.PID_Kd);			//显示Kp
		OLED_Printf(0, 32, OLED_8X16, "Ki:%4.2f", Motor_PID.PID_Ki);			//显示Ki
		OLED_Printf(0, 48, OLED_8X16, "Kd:%4.2f", Motor_PID.PID_Kd);			//显示Kd
		
		OLED_Printf(64, 16, OLED_8X16, "Tar:%+04.0f", Motor_PID.PID_Target);	//显示目标值
		OLED_Printf(64, 32, OLED_8X16, "Act:%+04.0f", Motor_PID.PID_Actual);	//显示实际值
		OLED_Printf(64, 48, OLED_8X16, "Out:%+04.0f", Motor_PID.PWM_Out);		//显示输出值
		
		OLED_Update();	//OLED更新，调用显示函数后必须调用此函数更新，否则显示的内容不会更新到OLED上
		

		/*
			//串口打印目标值、实际值和输出值
			//配合SerialPlot绘图软件，可以显示数据的波形
		*/
		printf("%f,%f,%f\r\n", Motor_PID.PID_Target, Motor_PID.PID_Actual, Motor_PID.PWM_Out);		//串口打印目标值、实际值和输出值
																								
																
																
//		my_key_test_func();								//按键OLED显示
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

