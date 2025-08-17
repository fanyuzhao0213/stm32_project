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
	CAN通信波特率计算
	CAN波特率 = APB1_PCLK1/分频/(tq1 + tq2 + ss)，其中SS是SS段，在STM32中已经固定为1个tq
	本代码波特率 = 42MHZ/2分频/(14 + 6 + 1) = 1MHZ = 1000KHZ
	
	Instance = CAN1
		选择使用的 CAN 外设，这里是 CAN1。
	Prescaler = 2
		CAN 外设时钟分频系数，决定波特率。
	Mode = CAN_MODE_NORMAL
		工作模式选择为正常通信模式。	常用模式包括：
								正常模式（Normal）
								回环测试模式（Loopback）
								静默监听模式（Silent）
	SyncJumpWidth = CAN_SJW_4TQ
		同步跳转宽度为 4 个时间量子（TQ），用于相位误差补偿，范围 1~4 TQ。
	TimeSeg1 = CAN_BS1_14TQ
		位段 1 长度为 14 个时间量子，包括传播段和相位段 1。
	TimeSeg2 = CAN_BS2_6TQ
		位段 2 长度为 6 个时间量子，即相位段 2。
		
	TimeTriggeredMode = DISABLE
		禁用时间触发模式，不在帧中附加时间戳。
		作用：开启后，每一帧 CAN 报文会在数据段中附带时间戳（由 CAN 外设硬件生成）。
		用途：主要用于 时间触发型 CAN（TTCAN），在一些实时性要求非常高的工业或汽车应用中，需要精确知道每个数据的发送时间点。
		区别：
			DISABLE（常用）：正常 CAN 帧，无时间戳。
			ENABLE：帧中会包含时间戳，需要配合协议解析。
		注意：开启后帧格式会变化，不是所有 CAN 节点都支持解析。

	AutoBusOff = DISABLE
		禁用自动总线关闭恢复功能，如果进入总线关闭状态，需要软件干预恢复。
		作用：CAN 控制器在检测到严重错误（进入 Bus-Off 状态）时，能否自动尝试恢复。
		原理：
			CAN 协议规定，当错误计数器达到一定值时，节点会进入 Bus-Off 状态，停止发送。
			若开启 AutoBusOff，硬件会自动等待 128 次 11 位的空闲帧时间，然后恢复通信。
			区别：
			DISABLE（常用）：进入 Bus-Off 后必须软件调用 HAL_CAN_Start() 才能恢复。
			ENABLE：硬件会自动等待并恢复，不需要软件干预。
			应用：自动恢复可减少通信中断时间，但可能掩盖错误原因。
	AutoWakeUp = DISABLE
		禁用自动唤醒功能，不会在总线有活动时自动唤醒。
		作用：当 CAN 控制器处于睡眠模式时，如果检测到总线上有活动（显性电平），能否自动唤醒。
		区别：
		DISABLE（常用）：需要软件调用 HAL_CAN_WakeUp() 唤醒。
		ENABLE：有总线活动会自动唤醒，适合低功耗模式下的自动恢复通信。
		应用：电动车、车载 MCU 等低功耗场景中，CAN 总线有消息时自动唤醒 MCU。
	AutoRetransmission = DISABLE
		禁用自动重发功能，发送失败不会自动重试，需要软件处理。
		作用：发送失败（仲裁丢失或错误）时，硬件是否自动重新尝试发送。
		区别：
		DISABLE：发送失败后立即丢弃，需要软件重新发送。
		ENABLE（常用）：硬件自动重试，直到发送成功或进入错误状态。
		应用：
		自动重发适合普通数据传输，保证数据可靠送达。
		禁用重发适合实时控制（超时就丢弃，不延迟新消息）。
	ReceiveFifoLocked = DISABLE
		接收 FIFO 未锁定，FIFO 满时会覆盖旧数据。
		作用：接收 FIFO 满时，如何处理新来的数据。
		区别：
		DISABLE（常用）：FIFO 满时，新数据会覆盖旧数据（保证接收最新的）。
		ENABLE：FIFO 满时丢弃新数据，保留旧数据直到被读取。
		应用：
		锁定模式适合关键数据（必须完整读取），防止覆盖丢失。
		覆盖模式适合实时性高的场景（只关心最新数据）。
		
	TransmitFifoPriority = DISABLE
		发送 FIFO 使用请求顺序优先，而不是按 CAN ID 优先级。
		作用：发送缓冲区有多个报文时，决定发送顺序。
		区别：
		ENABLE：按 CAN ID 优先级 发送（ID 越小优先级越高）。
		DISABLE（常用）：按 发送请求顺序 发送。
		应用：
		优先级模式适合多任务共享一个 CAN 总线，关键报文可用低 ID 提高优先级。
		顺序模式适合任务内的时序一致性，不打乱消息顺序。
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

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
