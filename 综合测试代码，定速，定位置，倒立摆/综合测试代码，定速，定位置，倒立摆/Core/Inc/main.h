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
#include "OLED.h"
#include "my_key.h"
#include "motor.h"
#include "FlashStore.h"
#include "motorpid.h"
#include "pid.h"

#include "systerm_hardwaretest.h"
#include "systerm_speedcontrol.h"
#include "systerm_locationcontrol.h"
#include "systerm_menu.h"
#include "systerm_daolibai.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum {
    SYSTEM_STAGE_IDLE = 0,                          // ϵͳ��ʼ״̬
    SYSTEM_STAGE_SELECTION,                 // ϵͳѡ��
    SYSTEM_STAGE_KEY_TEST  = 0x11,                // Ӳ����������
	SYSTEM_STAGE_RP_TEST,                       // Ӳ��RP����
    SYSTEM_STAGE_PWM_TEST,                      // Ӳ��PWM����
	SYSTEM_STAGE_ENCODER_TEST,                      // Ӳ������������
    SYSTEM_STAGE_ANGLE_TEST,                    // Ӳ���Ƕȴ���������
	SYSTEM_STAGE_MOTOR_CONTROL = 0x20,            // �������
    SYSTEM_STAGE_SPEED_CONTROL,            // ���ٿ���
    SYSTEM_STAGE_LOCATION_CONTROL,              // ��λ�ÿ���
	SYSTEM_STAGE_DAOLIBAI_IDLE = 0x30,              // �����ڿ���
    SYSTEM_STAGE_DAOLIBAI_CONTROL,              // �����ڿ���
    SYSTEM_STAGE_MENU                           // ϵͳ�˵�ѡ��
} SystemStage_t;

extern uint8_t currentSystemStage;   // ��ǰϵͳ�׶Σ�ģʽ��
extern uint8_t nextSystemStage;      // ��һϵͳ�׶Σ�ģʽ��

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

#define LED_ON()     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)
#define LED_OFF()     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
