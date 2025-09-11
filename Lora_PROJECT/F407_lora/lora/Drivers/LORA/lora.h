#ifndef __LORA_H__
#define __LORA_H__
/***********************************************************************************************************************************
 ** 【文件名称】  lora.h
 ** 【编写人员】  魔女开发板团队
 ** 【淘    宝】  魔女开发板      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** 【功能描述】  定义引脚、定义全局结构体、声明全局函数
 **
 ** 【适用平台】  STM32F103 + 标准库v3.5 + keil5 + 安信可LoRa模块Ra-01
 **     
 ** 【设备重点】  1- 引脚： 3V3,GND,NSS,SCK,MOSI,MISO,DIO0(IRQ),RES
 **               2- DIO-0用于指示接收和发送状态；
 **               3- LORA调制方式
 **
 ** 【代码重点】  1- SPI通信
 **               2- W25Q256的地址, 与32,64,128等不同 
 ** 　　　　　　　3- 为简化整个W25Q128的代码，已封装成3个全局函数，只用这3个函数，即可完成对其存取操作
 **                  初始化  ：  vW25qx_Init()　
 **                  读取数据：  vW25qxx_ReadBuffer( u32 addr, uint8_t *p, u16 num)　 // 数据地址，缓存指针，字节数
 **                  写入数据：  vW25qxx_WriteBuffer(u32 addr, uint8_t *p, u16 num)   // 数据地址，缓存指针，字节数    
 **   
 ** 【函数解释】  1- 发送数据：vLora_TxData(要发送数据的首地址, 字节数);
 **               2- 接收数据：步骤(1): 按需询问接收标志：xLora.ReceivedFlag，是否等于1，
 **                            步骤(2): 如果标志==1, 所接收到的数据存放于xLora.ReceivedBuf[256]数组中，按需处理即可；
 **                            步骤(3): 把接收标志清零，即：xLora.ReceivedFlag=0; 
 **
 ** 【移植方法】  添加文件：
 **               1:把 hardware/lora文件夹，复制到你的工程文件夹下；
 **               2:在keil工程中，打开Options, 在C/C++页面中，打开Include Paths 添加lora文件夹位置；
 **               3:在Keil工程中，引用lora.c，在main.c中添加 #include "lora.h"
 **               修改配置
 **               1:引脚修改：在lora.h中修改
 **               2:参数修改：在引用Lora_Init()时，设置3个参数( fre:频率_MHz,   sf:扩频因子,   bw:带宽)；更详细的参数可在lora.c中修改，但只要前述三个参数双方一致，即可通信
 ** 
 ** 【更新记录】  2020-07-24  依据厂家代码完成重写
 **               2021-11-01  完善注释、代码结构
 **
***********************************************************************************************************************************/
#include "main.h"



/*****************************************************************************
 ** 移植 修改区
 ** 引脚定义： SPI通信引脚+模块控制引脚 
 ** 这里只定义引脚，芯片参数在sx1276.c文件开头配置
****************************************************************************/
#define LORA_SPI_PORT         SPI1                 // SPIx

#define LORA_SCK_GPIO         GPIOA                // SPI_SCK
#define LORA_SCK_PIN          GPIO_Pin_5

#define LORA_MISO_GPIO        GPIOA                // SPI_MISO
#define LORA_MISO_PIN         GPIO_Pin_6

#define LORA_MOSI_GPIO        GPIOA                // SPI_MOSI
#define LORA_MOSI_PIN         GPIO_Pin_7

#define LORA_CS_GPIO          GPIOB                // LORA NSS
#define LORA_CS_PIN           GPIO_Pin_9  

#define LORA_RESET_GPIO       GPIOA                // LORA Reset
#define LORA_RESET_PIN        GPIO_Pin_15
  
#define LORA_DIO0_GPIO        GPIOA                // LORA DIO0 IRQ  注意： 其它的外部中断线冲突
#define LORA_DIO0_PIN         GPIO_Pin_8
//#define LORA_DIO0_EXTI_TRIM   EXTI_TRIGGER_RISING  // 上升沿触发
//#define LORA_DIO0_IRQN        EXTI9_5_IRQn         // 中断向量
//#define LORA_DIO0_HANDLER     EXTI9_5_IRQHandler   // 中断服务函数
// end 移植 *********************


/*****************************************************************************
 ** 定义新数据、类型
****************************************************************************/
#define RF_BUF_SIZE   256              // RF数据包大小(模块配备了256Byte的RAM缓存，该缓存仅能通过LoRa模式访问)   

typedef struct                         // SX1278通信状态、数据
{
    // 记录当前状态
    uint8_t InitOK;                         //  LORA初始化状态， 0:失败， 1:成功
    //uint8_t Busy;                           
    uint8_t RxMode;
    uint8_t TxMode; 
    // 接收到新的数据
    uint8_t ReceivedFlag;              // 接收到新数据的标志，0_无数据，1_有新数据；使用说明：当LORA接收到新数据时，会在中断中把变量=1，并把新数据存放到数组receivedBuf中；外部程序可以按需询问本变量值，当处理完receivedBuf中数据后，需要手动=0；
    uint8_t ReceivedBuf[RF_BUF_SIZE];  // 存放接收到新数据的缓存区；可以外部询问if(receivedFlag==1)以判断是否接收到新数据，处理数据后，buf可不清理，但Flag必须清0；
    uint8_t ReceivedSize;              // 最后一包数据的有效负载长度(字节数)    
}xLora_TypeDef;
extern xLora_TypeDef xLora;            // SX1278在LORA调制方式下的通信状态，作用范围：全局





/*****************************************************************************
 ** 声明  全局函数
 ** 数量：5个
****************************************************************************/
static void SPI1_Reset( void );
static void ReadForRegister( uint8_t addr, uint8_t *buffer, uint8_t size );
static void writeToRegister( uint8_t addr, uint8_t *buffer, uint8_t size );
// 设备配置部分
static void  sx1276_SetOpMode( uint8_t opMode );  // 设置 操作模式
static void  SX1276LoRaSetRFFrequency( uint32_t freq );
static void  SX1276LoRaSetSignalBandwidth( uint8_t bw );
static void  SX1276LoRaSetSpreadingFactor( uint8_t factor );
static void  SX1276LoRaSetErrorCoding( uint8_t value );
static void  SX1276LoRaSetPacketCrcOn( bool enable );
static void  SX1276LoRaSetImplicitHeaderOn( bool enable );
static void  SX1276LoRaSetPAOutput( uint8_t outputPin );
static void  SX1276LoRaSetSymbTimeout( uint16_t value );
static void  SX1276LoRaSetLowDatarateOptimize( bool enable );
void Lora_Init(  uint32_t fre, uint8_t sf, uint8_t bw);  // 初始化 SX1276为LORA模式， fre:频率_MHz, sf:扩频因子, bw:带宽
void Lora_StandbyMode(void);              // 设备进入: 待机模式
void Lora_RxMode(void);                   // 设备进入: 接收模式
void Lora_TxData(uint8_t* buf, uint8_t size);  // 发送数据, 注意：发送成功后自动进入接收模式
void LORA_DIO0_HANDLER(void);
//int8_t  Lora_GetSNR (void) ;            // 信噪比
//double  Lora_GetRSSI(void) ;            // 信号强度
//uint8_t Lora_GetGain( void );           // 信号增益
#endif 

