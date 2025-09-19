/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
/*
	CANͨ�Ų����ʼ���
	CAN������ = APB1_PCLK1/��Ƶ/(tq1 + tq2 + ss)������SS��SS�Σ���STM32���Ѿ��̶�Ϊ1��tq
	�����벨���� = 42MHZ/2��Ƶ/(14 + 6 + 1) = 1MHZ = 1000KHZ
	
	Instance = CAN1
		ѡ��ʹ�õ� CAN ���裬������ CAN1��
	Prescaler = 2
		CAN ����ʱ�ӷ�Ƶϵ�������������ʡ�
	Mode = CAN_MODE_NORMAL
		����ģʽѡ��Ϊ����ͨ��ģʽ��	����ģʽ������
								����ģʽ��Normal��
								�ػ�����ģʽ��Loopback��
								��Ĭ����ģʽ��Silent��
	SyncJumpWidth = CAN_SJW_4TQ
		ͬ����ת���Ϊ 4 ��ʱ�����ӣ�TQ����������λ��������Χ 1~4 TQ��
	TimeSeg1 = CAN_BS1_14TQ
		λ�� 1 ����Ϊ 14 ��ʱ�����ӣ����������κ���λ�� 1��
	TimeSeg2 = CAN_BS2_6TQ
		λ�� 2 ����Ϊ 6 ��ʱ�����ӣ�����λ�� 2��
		
	TimeTriggeredMode = DISABLE
		����ʱ�䴥��ģʽ������֡�и���ʱ�����
		���ã�������ÿһ֡ CAN ���Ļ������ݶ��и���ʱ������� CAN ����Ӳ�����ɣ���
		��;����Ҫ���� ʱ�䴥���� CAN��TTCAN������һЩʵʱ��Ҫ��ǳ��ߵĹ�ҵ������Ӧ���У���Ҫ��ȷ֪��ÿ�����ݵķ���ʱ��㡣
		����
			DISABLE�����ã������� CAN ֡����ʱ�����
			ENABLE��֡�л����ʱ�������Ҫ���Э�������
		ע�⣺������֡��ʽ��仯���������� CAN �ڵ㶼֧�ֽ�����

	AutoBusOff = DISABLE
		�����Զ����߹رջָ����ܣ�����������߹ر�״̬����Ҫ�����Ԥ�ָ���
		���ã�CAN �������ڼ�⵽���ش��󣨽��� Bus-Off ״̬��ʱ���ܷ��Զ����Իָ���
		ԭ��
			CAN Э��涨��������������ﵽһ��ֵʱ���ڵ����� Bus-Off ״̬��ֹͣ���͡�
			������ AutoBusOff��Ӳ�����Զ��ȴ� 128 �� 11 λ�Ŀ���֡ʱ�䣬Ȼ��ָ�ͨ�š�
			����
			DISABLE�����ã������� Bus-Off ������������ HAL_CAN_Start() ���ָܻ���
			ENABLE��Ӳ�����Զ��ȴ����ָ�������Ҫ�����Ԥ��
			Ӧ�ã��Զ��ָ��ɼ���ͨ���ж�ʱ�䣬�������ڸǴ���ԭ��
	AutoWakeUp = DISABLE
		�����Զ����ѹ��ܣ������������лʱ�Զ����ѡ�
		���ã��� CAN ����������˯��ģʽʱ�������⵽�������л�����Ե�ƽ�����ܷ��Զ����ѡ�
		����
		DISABLE�����ã�����Ҫ������� HAL_CAN_WakeUp() ���ѡ�
		ENABLE�������߻���Զ����ѣ��ʺϵ͹���ģʽ�µ��Զ��ָ�ͨ�š�
		Ӧ�ã��綯�������� MCU �ȵ͹��ĳ����У�CAN ��������Ϣʱ�Զ����� MCU��
	AutoRetransmission = DISABLE
		�����Զ��ط����ܣ�����ʧ�ܲ����Զ����ԣ���Ҫ�������
		���ã�����ʧ�ܣ��ٲö�ʧ�����ʱ��Ӳ���Ƿ��Զ����³��Է��͡�
		����
		DISABLE������ʧ�ܺ�������������Ҫ������·��͡�
		ENABLE�����ã���Ӳ���Զ����ԣ�ֱ�����ͳɹ���������״̬��
		Ӧ�ã�
		�Զ��ط��ʺ���ͨ���ݴ��䣬��֤���ݿɿ��ʹ
		�����ط��ʺ�ʵʱ���ƣ���ʱ�Ͷ��������ӳ�����Ϣ����
	ReceiveFifoLocked = DISABLE
		���� FIFO δ������FIFO ��ʱ�Ḳ�Ǿ����ݡ�
		���ã����� FIFO ��ʱ����δ������������ݡ�
		����
		DISABLE�����ã���FIFO ��ʱ�������ݻḲ�Ǿ����ݣ���֤�������µģ���
		ENABLE��FIFO ��ʱ���������ݣ�����������ֱ������ȡ��
		Ӧ�ã�
		����ģʽ�ʺϹؼ����ݣ�����������ȡ������ֹ���Ƕ�ʧ��
		����ģʽ�ʺ�ʵʱ�Ըߵĳ�����ֻ�����������ݣ���
		
	TransmitFifoPriority = DISABLE
		���� FIFO ʹ������˳�����ȣ������ǰ� CAN ID ���ȼ���
		���ã����ͻ������ж������ʱ����������˳��
		����
		ENABLE���� CAN ID ���ȼ� ���ͣ�ID ԽС���ȼ�Խ�ߣ���
		DISABLE�����ã����� ��������˳�� ���͡�
		Ӧ�ã�
		���ȼ�ģʽ�ʺ϶�������һ�� CAN ���ߣ��ؼ����Ŀ��õ� ID ������ȼ���
		˳��ģʽ�ʺ������ڵ�ʱ��һ���ԣ���������Ϣ˳��
*/

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 2;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_4TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_14TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_6TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
