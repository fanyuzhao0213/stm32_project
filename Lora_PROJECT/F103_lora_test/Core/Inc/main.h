/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define USER_KEY_Pin GPIO_PIN_13
#define USER_KEY_GPIO_Port GPIOC
#define USER_KEY_EXTI_IRQn EXTI15_10_IRQn
#define LED_RGB_Pin GPIO_PIN_1
#define LED_RGB_GPIO_Port GPIOA
#define Lora_IT_Pin GPIO_PIN_8
#define Lora_IT_GPIO_Port GPIOA
#define Lora_IT_EXTI_IRQn EXTI9_5_IRQn
#define Lora_Reset_Pin GPIO_PIN_15
#define Lora_Reset_GPIO_Port GPIOA
#define Lora_IT2_Pin GPIO_PIN_4
#define Lora_IT2_GPIO_Port GPIOB
#define Lora_IT2_EXTI_IRQn EXTI4_IRQn
#define Lora_Reset2_Pin GPIO_PIN_5
#define Lora_Reset2_GPIO_Port GPIOB
#define SPI2_NSS_Pin GPIO_PIN_6
#define SPI2_NSS_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_7
#define LED_RED_GPIO_Port GPIOB
#define LED_BLUE_Pin GPIO_PIN_8
#define LED_BLUE_GPIO_Port GPIOB
#define SPI1_NSS_Pin GPIO_PIN_9
#define SPI1_NSS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
