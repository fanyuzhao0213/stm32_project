#ifndef _SOFTWAREIIC_H_
#define _SOFTWAREIIC_H_

#include "main.h"

typedef enum{
    NACK = 0,
    ACK  = 1
}ACK_STATUS;

#define IIC_DELAY_TIME 1

#define IIC_SCL_PIN GPIO_PIN_7
#define IIC_SCL_PORT GPIOC

#define IIC_SDA_PIN GPIO_PIN_6
#define IIC_SDA_PORT GPIOC

// #define IIC_SDA_H() GPIOB->BSRR = 0x010
// #define IIC_SDA_L() GPIOB->BRR = 0x010
// #define IIC_SCL_H() GPIOB->BSRR = 0x100
// #define IIC_SCL_L() GPIOB->BRR = 0x100

#define IIC_SCL_H()     HAL_GPIO_WritePin(IIC_SCL_PORT,IIC_SCL_PIN,GPIO_PIN_SET)
#define IIC_SCL_L()     HAL_GPIO_WritePin(IIC_SCL_PORT,IIC_SCL_PIN,GPIO_PIN_RESET)
#define IIC_SDA_H()     HAL_GPIO_WritePin(IIC_SDA_PORT,IIC_SDA_PIN,GPIO_PIN_SET)
#define IIC_SDA_L()     HAL_GPIO_WritePin(IIC_SDA_PORT,IIC_SDA_PIN,GPIO_PIN_RESET)

//-----------------OLED IIC¶Ë¿Ú¶¨Òå----------------  					   
 
void Soft_IIC_Init(void);
void Soft_IIC_Start(void);
void Soft_IIC_Stop(void);
void Soft_IIC_ACK(void);
void Soft_IIC_NACK(void);
uint8_t Soft_IIC_Wait_ACK(void);
uint8_t Soft_IIC_Write_Byte(uint8_t Byte);
uint8_t Soft_IIC_Recv_Byte(ACK_STATUS ack_sta);

void Write_IIC_Command(uint8_t IIC_Command);
void Write_IIC_Data(uint8_t IIC_Data);

#endif

