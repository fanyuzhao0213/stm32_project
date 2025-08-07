/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
// PID控制器结构体定义
typedef struct {
    float PID_Target;      // 目标值
    float PID_Actual;      // 实际值
    float PWM_Out;         // 输出值
    float PID_Kp;          // 比例项的权重
    float PID_Ki;          // 积分项的权重
    float PID_Kd;          // 微分项的权重
    float PID_Error0;      // 本次误差
    float PID_Error1;      // 上次误差
	float PID_Error2;      // 上上次误差
    float PID_ErrorInt;    // 误差积分
} PID_Controller;

// 创建PID控制器实例
extern PID_Controller Motor_PID;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SENSOR1_Pin GPIO_PIN_0
#define SENSOR1_GPIO_Port GPIOB
#define SENSOR2_Pin GPIO_PIN_1
#define SENSOR2_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_10
#define KEY1_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_11
#define KEY2_GPIO_Port GPIOB
#define MOTOR_CTR1_Pin GPIO_PIN_12
#define MOTOR_CTR1_GPIO_Port GPIOB
#define MOTOR_CTR12_Pin GPIO_PIN_13
#define MOTOR_CTR12_GPIO_Port GPIOB
#define KEY3_Pin GPIO_PIN_11
#define KEY3_GPIO_Port GPIOA
#define KEY4_Pin GPIO_PIN_12
#define KEY4_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
