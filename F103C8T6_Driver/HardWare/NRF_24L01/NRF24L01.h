#ifndef __NRF_24L01_H
#define __NRF_24L01_H

#include "main.h"

#define NRF24L01_TX_PACKET_WIDTH		32							//发送数据包宽度，范围：1~32字节
#define NRF24L01_RX_PACKET_WIDTH		32							//接收通道0数据包宽度，范围：1~32字节

/*-----------------------------
   用户配置部分
   修改 SDA / SCL 所在端口和引脚
------------------------------*/
#define NRF_CE_PORT 		GPIOB
#define NRF_CE_PIN  		GPIO_PIN_1

typedef enum {
    NRF24L01_TX_OK = 1,        // 发送成功
    NRF24L01_TX_MAX_RT = 2,    // 达到最大重发次数
    NRF24L01_TX_INVALID = 3,   // 状态寄存器值异常
    NRF24L01_TX_TIMEOUT = 4    // 发送超时
} NRF24L01_TxStatus;


/*外部可调用全局数组***********/

extern uint8_t NRF24L01_TxAddress[];
extern uint8_t NRF24L01_TxPacket[];

extern uint8_t NRF24L01_RxAddress[];
extern uint8_t NRF24L01_RxPacket[];

/***********外部可调用全局数组*/


/*函数声明*********************/

/*指令实现*/
uint8_t NRF24L01_ReadReg(uint8_t RegAddress);
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data);
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_FlushTx(void);
void NRF24L01_FlushRx(void);
uint8_t NRF24L01_ReadStatus(void);

/*功能函数*/
void NRF24L01_PowerDown(void);
void NRF24L01_StandbyI(void);
void NRF24L01_Rx(void);
void NRF24L01_Tx(void);

void NRF24L01_Init(void);
uint8_t NRF24L01_Send(void);
void NRF24L01_Send_IT(uint8_t *pData, uint8_t len);
uint8_t NRF24L01_Receive(void);
void NRF24L01_UpdateRxAddress(void);

/*********************函数声明*/
uint8_t NRF24L01_TestHardware(void);
#endif

