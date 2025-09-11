/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_SPI1_Init(void);
void MX_SPI3_Init(void);

/* USER CODE BEGIN Prototypes */
HAL_StatusTypeDef	SPI_TransmitOneByte(uint8_t	byteData);
//SPI接口发送多个字节, pBuffer是发送数据缓存区指针，byteCount是发送数据字节数，byteCount最大256
HAL_StatusTypeDef	SPI_TransmitBytes(uint8_t* pBuffer, uint16_t byteCount);
//SPI接口接收一个字节， 返回接收的一个字节数据
uint8_t	SPI_ReceiveOneByte(void);
//SPI接口接收多个字节， pBuffer是接收数据缓存区指针，byteCount是需要接收数据的字节数
HAL_StatusTypeDef	SPI_ReceiveBytes(uint8_t* pBuffer, uint16_t byteCount);

/* ADX */
//SPI接口发送一个字节,byteData是需要发送的数据
HAL_StatusTypeDef	ADX_TransmitOneByte(uint8_t	byteData);
//SPI接口接收一个字节， 返回接收的一个字节数据
uint8_t	ADX_ReceiveOneByte(void);
uint32_t SPI_ADXL345_ReadID(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

