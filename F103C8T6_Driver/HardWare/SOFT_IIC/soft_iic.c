#include "soft_iic.h"

/*----------------------------------------
  软件 I2C 底层接口函数
  - 通过 GPIO 模拟 I2C 时序
  - SDA 和 SCL 使用开漏输出 + 上拉
----------------------------------------*/

/**
 * @brief 设置 SCL 引脚电平
 * @param BitValue: 0=低电平, 1=高电平
 */
void IIC_W_SCL(uint8_t BitValue)
{
    HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, (GPIO_PinState)BitValue);
    // 如果单片机太快，可以在这里加延时，避免通信超速
}

/**
 * @brief 设置 SDA 引脚电平
 * @param BitValue: 0=低电平, 1=高电平
 */
void IIC_W_SDA(uint8_t BitValue)
{
    HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, (GPIO_PinState)BitValue);
    // 如果单片机太快，可以在这里加延时
}

/**
 * @brief 读取 SDA 引脚电平
 * @retval 0=低电平, 1=高电平
 */
uint8_t IIC_R_SDA(void)
{
    return HAL_GPIO_ReadPin(IIC_SDA_PORT, IIC_SDA_PIN);
}

/**
 * @brief 初始化 I2C GPIO 引脚
 * - SDA / SCL 配置为开漏输出，上拉
 */
void IIC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 根据实际端口修改

    // 初始化 SCL
    GPIO_InitStruct.Pin = IIC_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // 开漏输出
    GPIO_InitStruct.Pull = GPIO_PULLUP;         // 上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(IIC_SCL_PORT, &GPIO_InitStruct);

    // 初始化 SDA
    GPIO_InitStruct.Pin = IIC_SDA_PIN;
    HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);

    // 释放总线
    IIC_W_SCL(1);
    IIC_W_SDA(1);
}

/*----------------------------------------
  软件 I2C 协议函数
----------------------------------------*/

/**
 * @brief 产生 I2C 起始信号
 */
void IIC_START(void)
{
    IIC_W_SDA(1);
    IIC_W_SCL(1);
    IIC_W_SDA(0);   // SCL 高电平时 SDA 拉低，产生 START
    IIC_W_SCL(0);
}

/**
 * @brief 产生 I2C 停止信号
 */
void IIC_STOP(void)
{
    IIC_W_SDA(0);
    IIC_W_SCL(1);
    IIC_W_SDA(1);   // SCL 高电平时 SDA 释放，产生 STOP
}

/**
 * @brief I2C 发送 1 字节
 * @param Byte 待发送数据
 */
void IIC_Send_Byte(uint8_t Byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        IIC_W_SDA(!!(Byte & (0x80 >> i)));  // 依次写入最高位 → 最低位
        IIC_W_SCL(1); // 上升沿有效，从机采样
        IIC_W_SCL(0);
    }
	
	/*目前OLED需要*/
	IIC_W_SCL(1);	//额外的一个时钟，不处理应答信号
	IIC_W_SCL(0);
}

/**
 * @brief I2C 接收 1 字节
 * @retval 接收到的数据
 */
uint8_t MyI2C_ReceiveByte(void)
{
    uint8_t Byte = 0x00;
    IIC_W_SDA(1);   // 释放 SDA，避免干扰从机

    for (uint8_t i = 0; i < 8; i++)
    {
        IIC_W_SCL(1);
        if (IIC_R_SDA())
            Byte |= (0x80 >> i); // 采样 SDA
        IIC_W_SCL(0);
    }
    return Byte;
}

/**
 * @brief 主机发送 ACK/NACK
 * @param AckBit: 0=ACK, 1=NACK
 */
void MyI2C_SendAck(uint8_t AckBit)
{
    IIC_W_SDA(AckBit);
    IIC_W_SCL(1);
    IIC_W_SCL(0);
}

/**
 * @brief 主机接收从机的 ACK
 * @retval 0=ACK, 1=NACK
 */
uint8_t MyI2C_ReceiveAck(void)
{
    uint8_t AckBit;
    IIC_W_SDA(1);   // 释放 SDA
    IIC_W_SCL(1);
    AckBit = IIC_R_SDA();
    IIC_W_SCL(0);
    return AckBit;
}

/*----------------------------------------
  OLED 驱动函数 (基于 I2C)
----------------------------------------*/

/**
 * @brief 写命令到 OLED
 * @param Command OLED 命令字
 */
void OLED_WriteCommand(uint8_t Command)
{
    IIC_START();
    IIC_Send_Byte(0x78);   // OLED I2C 地址（SSD1306 默认 0x78）
    IIC_Send_Byte(0x00);   // 控制字节，表示写命令
    IIC_Send_Byte(Command);
    IIC_STOP();
}


/*
	OLED每次发送1个字节,需要多给1个时钟
	原理是什么 不知道 待查找解决
*/

/**
 * @brief 写数据到 OLED
 * @param Data 数据缓冲区
 * @param Count 数据字节数
 */
void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
    IIC_START();
    IIC_Send_Byte(0x78);   // OLED I2C 地址
    IIC_Send_Byte(0x40);   // 控制字节，表示写数据
    for (uint8_t i = 0; i < Count; i++)
    {
        IIC_Send_Byte(Data[i]);
    }
    IIC_STOP();
}


