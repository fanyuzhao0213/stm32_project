#include "flash.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "FlashStore.h"
/**
 * @brief  从指定地址读取一个 32 位数据
 * @param  Address 读取的 Flash 地址
 * @retval 读取到的 32 位数据
 */
uint32_t MyFLASH_ReadWord(uint32_t Address)
{
    return *((__IO uint32_t *)(Address));
}

/**
 * @brief  从指定地址读取一个 16 位数据
 * @param  Address 读取的 Flash 地址
 * @retval 读取到的 16 位数据
 */
uint16_t MyFLASH_ReadHalfWord(uint32_t Address)
{
    return *((__IO uint16_t *)(Address));
}

/**
 * @brief  从指定地址读取一个 8 位数据
 * @param  Address 读取的 Flash 地址
 * @retval 读取到的 8 位数据
 */
uint8_t MyFLASH_ReadByte(uint32_t Address)
{
    return *((__IO uint8_t *)(Address));
}

/**
 * @brief  擦除整个 Flash (HAL 没有直接提供此功能，需循环擦除每页)
 * @note   注意擦除会清空整个用户 Flash，请谨慎使用
 */
void MyFLASH_EraseAllPages(void)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    HAL_FLASH_Unlock();

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_BASE;
    EraseInitStruct.NbPages = (FLASH_BANK1_END - FLASH_BASE + 1) / FLASH_PAGE_SIZE;

    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

    HAL_FLASH_Lock();
}

/**
 * @brief  擦除指定地址所在的 Flash 页面
 * @param  PageAddress 要擦除的页面首地址
 */
void MyFLASH_ErasePage(uint32_t PageAddress)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    HAL_FLASH_Unlock();

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = PageAddress;
    EraseInitStruct.NbPages = 1;

    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

    HAL_FLASH_Lock();
}

/**
 * @brief  向 Flash 写入一个 32 位数据
 * @param  Address 写入地址（必须是 4 字节对齐）
 * @param  Data    要写入的 32 位数据
 */
void MyFLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
    HAL_FLASH_Unlock();
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Data);
    HAL_FLASH_Lock();
}

/**
 * @brief  向 Flash 写入一个 16 位数据
 * @param  Address 写入地址（必须是 2 字节对齐）
 * @param  Data    要写入的 16 位数据
 */
void MyFLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
    HAL_FLASH_Unlock();
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address, Data);
    HAL_FLASH_Lock();
}

#include "usart.h"
void Flash_Test(void)
{
    uint32_t test_data_w = 0x12345678;
    uint32_t test_data_r = 0;

    printf("Flash Test Start...\r\n");

    // 1. 擦除最后一页
    printf("Erasing last flash page...\r\n");
    MyFLASH_ErasePage(FLASH_STORE_START_ADDR);

    // 2. 写入一个 32 位数据
    printf("Writing 0x%08d to flash...\r\n", test_data_w);
    MyFLASH_ProgramWord(FLASH_STORE_START_ADDR, test_data_w);

    // 3. 读取刚写入的数据
    test_data_r = MyFLASH_ReadWord(FLASH_STORE_START_ADDR);
    printf("Read data: 0x%08d\r\n", test_data_r);

    // 4. 判断读写是否一致
    if (test_data_r == test_data_w)
    {
        printf("Flash test PASSED!\r\n");
    }
    else
    {
        printf("Flash test FAILED!\r\n");
    }
}

