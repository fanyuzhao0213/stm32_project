/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define UART_DMA_RX_BUF_SIZE   256   // DMA��������С


// DMA���ջ�����
extern uint8_t uart_dma_rx_buf[UART_DMA_RX_BUF_SIZE];
// ʵ�ʽ������ݻ��棨��ֹ���ݸ��ǣ�
extern uint8_t uart_frame_buf[UART_DMA_RX_BUF_SIZE];
extern uint16_t uart_rx_len;    // ʵ�ʽ��յ������ݳ���
extern uint8_t uart_frame_flag; // һ֡������ɱ�־

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void my_data_analysis(void);
HAL_StatusTypeDef UART1_Send_DMA(uint8_t *data, uint16_t length);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

