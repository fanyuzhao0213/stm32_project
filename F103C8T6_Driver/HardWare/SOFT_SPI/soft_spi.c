#include "soft_spi.h"


/*==================== 底层引脚操作函数 ====================*/

/**
  * @brief 写CS引脚电平
  * @param BitValue 0=低电平，1=高电平
  */
void SPI_W_CS(uint8_t BitValue)
{
    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, (GPIO_PinState)BitValue);
}

/**
  * @brief 写SCK引脚电平
  * @param BitValue 0=低电平，1=高电平
  */
void SPI_W_SCK(uint8_t BitValue)
{
    HAL_GPIO_WritePin(SPI_SCK_PORT, SPI_SCK_PIN, (GPIO_PinState)BitValue);
}

/**
  * @brief 写MOSI引脚电平
  * @param BitValue 0=低电平，非0=高电平
  */
void SPI_W_MOSI(uint8_t BitValue)
{
    HAL_GPIO_WritePin(SPI_MOSI_PORT, SPI_MOSI_PIN, (GPIO_PinState)(!!BitValue));
}

/**
  * @brief 读MISO引脚电平
  * @retval 0=低电平，1=高电平
  */
uint8_t SPI_R_MISO(void)
{
    return HAL_GPIO_ReadPin(SPI_MISO_PORT, SPI_MISO_PIN);
}

/*==================== 初始化函数 ====================*/

/**
  * @brief 初始化SPI相关的GPIO引脚
  */
void SPI_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 开启GPIO时钟（根据实际端口修改） */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* CS → 推挽输出 */
    GPIO_InitStruct.Pin = SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(SPI_CS_PORT, &GPIO_InitStruct);

    /* MOSI → 推挽输出 */
    GPIO_InitStruct.Pin = SPI_MOSI_PIN;
    HAL_GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStruct);

    /* SCK → 推挽输出 */
    GPIO_InitStruct.Pin = SPI_SCK_PIN;
    HAL_GPIO_Init(SPI_SCK_PORT, &GPIO_InitStruct);

    /* MISO → 上拉输入 */
    GPIO_InitStruct.Pin = SPI_MISO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI_MISO_PORT, &GPIO_InitStruct);

    /* 设置默认电平 */
    SPI_W_CS(1);   // CS默认高电平
#if (SPI_MODE == 0 || SPI_MODE == 1)
    SPI_W_SCK(0);  // Mode0/1 → 空闲低电平
#else
    SPI_W_SCK(1);  // Mode2/3 → 空闲高电平
#endif
}


/*==================== 协议层函数 ====================*/

/**
  * @brief 开始SPI通信（片选拉低）
  */
void MySPI_Start(void)
{
    SPI_W_CS(0);
}

/**
  * @brief 结束SPI通信（片选拉高）
  */
void MySPI_Stop(void)
{
    SPI_W_CS(1);
}


/*
	四种 SPI 模式
	模式	CPOL	CPHA	时钟空闲电平	采样点
	Mode 0	0	0	SCK 空闲低	第 1 个边沿  上升沿采样
	Mode 1	0	1	SCK 空闲低	第 2 个边沿  下降沿采样
	Mode 2	1	0	SCK 空闲高	第 1 个边沿  下降沿采样
	Mode 3	1	1	SCK 空闲高	第 2 个边沿  上升沿采样
*/


/*================== 模拟SPI函数 ==================*/

/**
  * @brief 模拟SPI交换传输一个字节
  * @param ByteSend 要发送的一个字节
  * @retval 接收到的一个字节
  */
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
    uint8_t i, ByteReceive = 0x00;

    /*================== Mode0 ==================*/
#if (SPI_MODE == 0)   /* CPOL=0, CPHA=0 */
    for (i = 0; i < 8; i++)
    {
        SPI_W_MOSI(!!(ByteSend & (0x80 >> i)));  // MOSI先稳定
        SPI_W_SCK(1);                            // 上升沿采样
        if (SPI_R_MISO()) {ByteReceive |= (0x80 >> i);}
        SPI_W_SCK(0);                            // 下降沿准备下一个bit
    }

    /*================== Mode1 ==================*/
#elif (SPI_MODE == 1) /* CPOL=0, CPHA=1 */
    for (i = 0; i < 8; i++)
    {
        SPI_W_SCK(1);                            // 先拉高SCK（空闲=低，第一个边沿=上升沿）
        SPI_W_MOSI(!!(ByteSend & (0x80 >> i)));  // MOSI在高电平时准备数据
        SPI_W_SCK(0);                            // 下降沿采样
        if (SPI_R_MISO()) {ByteReceive |= (0x80 >> i);}
    }

    /*================== Mode2 ==================*/
#elif (SPI_MODE == 2) /* CPOL=1, CPHA=0 */
    SPI_W_SCK(1);                                // 空闲为高
    for (i = 0; i < 8; i++)
    {
        SPI_W_MOSI(!!(ByteSend & (0x80 >> i)));  // MOSI先稳定
        SPI_W_SCK(0);                            // 第一个边沿=下降沿 → 采样
        if (SPI_R_MISO()) {ByteReceive |= (0x80 >> i);}
        SPI_W_SCK(1);                            // 上升沿准备下一个bit
    }

    /*================== Mode3 ==================*/
#elif (SPI_MODE == 3) /* CPOL=1, CPHA=1 */
    SPI_W_SCK(1);                                // 空闲为高
    for (i = 0; i < 8; i++)
    {
        SPI_W_SCK(0);                            // 第一个边沿=下降沿 → 准备数据
        SPI_W_MOSI(!!(ByteSend & (0x80 >> i)));
        SPI_W_SCK(1);                            // 上升沿采样
        if (SPI_R_MISO()) {ByteReceive |= (0x80 >> i);}
    }
#endif

    return ByteReceive;
}



