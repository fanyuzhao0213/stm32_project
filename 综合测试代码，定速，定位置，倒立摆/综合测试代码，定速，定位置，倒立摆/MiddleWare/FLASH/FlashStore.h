#ifndef __FLASH_STORE_H
#define __FLASH_STORE_H

#include "stm32f1xx_hal.h" // 根据你的芯片型号修改
#include <stdint.h>

/**
 * @brief Flash 存储配置
 */
// 存储起始地址（最后一页，STM32F103C8T6 = 64KB Flash, 最后一页地址 0x0800FC00）
#define FLASH_STORE_START_ADDR          0x0800FC00
#define FLASH_STORE_COUNT               512             // 存储字节数
#define FLASH_STORE_KEY                 0xA5A5          // 初始化标志

extern uint16_t FlashStore_Data[FLASH_STORE_COUNT];

/**
 * @brief  初始化 Flash 存储
 * @note   会检查标志位，如果没有初始化则清空数据并写入标志
 * @retval 0 = 已存在数据，1 = 首次初始化
 */
uint8_t FlashStore_Init(void);

/**
 * @brief  保存当前数据到 Flash
 */
void FlashStore_Save(void);

/**
 * @brief  清除存储数据（保留标志位）
 */
void FlashStore_Clear(void);

#endif // __FLASH_STORE_H


