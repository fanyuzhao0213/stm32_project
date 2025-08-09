#include "FlashStore.h"
#include "flash.h"  // 你自己的底层 Flash 操作接口

uint16_t FlashStore_Data[FLASH_STORE_COUNT];

/**
 * @brief 初始化 Flash 存储
 */
uint8_t FlashStore_Init(void)
{
    uint8_t isFirstInit = 0;

    // 检查标志位
    if (MyFLASH_ReadHalfWord(FLASH_STORE_START_ADDR) != FLASH_STORE_KEY)
    {
        MyFLASH_ErasePage(FLASH_STORE_START_ADDR);
        MyFLASH_ProgramHalfWord(FLASH_STORE_START_ADDR, FLASH_STORE_KEY);

        // 清空数据区
        for (uint16_t i = 1; i < FLASH_STORE_COUNT; i++)
        {
            MyFLASH_ProgramHalfWord(FLASH_STORE_START_ADDR + i * 2, 0x0000);
        }
        isFirstInit = 1;
    }

    // 读取所有数据到 RAM
    for (uint16_t i = 0; i < FLASH_STORE_COUNT; i++)
    {
        FlashStore_Data[i] = MyFLASH_ReadHalfWord(FLASH_STORE_START_ADDR + i * 2);
    }

    return isFirstInit;
}

/**
 * @brief 保存当前数据到 Flash
 */
void FlashStore_Save(void)
{
    MyFLASH_ErasePage(FLASH_STORE_START_ADDR);

    for (uint16_t i = 0; i < FLASH_STORE_COUNT; i++)
    {
        MyFLASH_ProgramHalfWord(FLASH_STORE_START_ADDR + i * 2, FlashStore_Data[i]);
    }
}

/**
 * @brief 清除存储数据（保留标志位）
 */
void FlashStore_Clear(void)
{
    for (uint16_t i = 1; i < FLASH_STORE_COUNT; i++)
    {
        FlashStore_Data[i] = 0x0000;
    }
    FlashStore_Save();
}

