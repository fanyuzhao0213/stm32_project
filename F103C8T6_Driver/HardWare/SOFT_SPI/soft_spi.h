#ifndef __SOFT_SPI_H
#define __SOFT_SPI_H

#include "main.h"

/*================== 配置宏 ==================*/
/* 
	SPI模式选择：
	0 → Mode0 (CPOL=0, CPHA=0)
	1 → Mode1 (CPOL=0, CPHA=1)
	2 → Mode2 (CPOL=1, CPHA=0)
	3 → Mode3 (CPOL=1, CPHA=1)
   	Mode 0	0	0	SCK 空闲低	第 1 个边沿  上升沿采样
	Mode 1	0	1	SCK 空闲低	第 2 个边沿  下降沿采样
	Mode 2	1	0	SCK 空闲高	第 1 个边沿  下降沿采样
	Mode 3	1	1	SCK 空闲高	第 2 个边沿  上升沿采样
*/
#define SPI_MODE 		0    

/*-----------------------------
   用户配置部分
   修改SPI端口 所在端口和引脚
   如果有多个片选，则仿写CS函数就行
------------------------------*/

#define SPI_CS_PORT					GPIOB
#define SPI_CS_PIN					GPIO_PIN_12
#define SPI_MOSI_PORT				GPIOA
#define SPI_MOSI_PIN				GPIO_PIN_7
#define SPI_MISO_PORT				GPIOA
#define SPI_MISO_PIN				GPIO_PIN_6
#define SPI_SCK_PORT				GPIOA
#define SPI_SCK_PIN					GPIO_PIN_5

/*==================== 外部函数声明 ====================*/

/**
  * @brief  初始化软件SPI所需的引脚
  * @note   会初始化 CS、SCK、MOSI 为推挽输出，MISO 为上拉输入
  */
void SPI_GPIO_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);

/**
  * @brief  通过软件SPI发送并接收一个字节
  * @param  ByteSend 要发送的字节
  * @retval 接收到的字节
  */
uint8_t MySPI_SwapByte(uint8_t ByteSend);
#endif
