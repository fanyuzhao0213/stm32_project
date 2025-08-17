#ifndef __SOFT_IIC_H
#define __SOFT_IIC_H

#include "main.h"

/*-----------------------------
   用户配置部分
   修改 SDA / SCL 所在端口和引脚
------------------------------*/
#define IIC_SCL_PORT GPIOA
#define IIC_SCL_PIN  GPIO_PIN_3

#define IIC_SDA_PORT GPIOA
#define IIC_SDA_PIN  GPIO_PIN_4

/*-----------------------------
   函数声明
------------------------------*/

// GPIO 基础操作
void IIC_W_SCL(uint8_t BitValue);
void IIC_W_SDA(uint8_t BitValue);
uint8_t IIC_R_SDA(void);
void IIC_GPIO_Init(void);

// I2C 协议操作
void IIC_START(void);
void IIC_STOP(void);
void IIC_Send_Byte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);

// OLED I2C 操作
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t *Data, uint8_t Count);

#endif
