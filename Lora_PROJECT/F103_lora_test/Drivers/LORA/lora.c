/***********************************************************************************************************************************
 ** 【文件名称】  lora.c
 ** 【编写人员】  魔女开发板团队
 ** 【淘    宝】  魔女开发板      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** 【文件功能】  初始化GPIO、SPI, 各功能函数
 ** 【适用平台】  STM32F103 + 标准库v3.5 + keil5 
 **
 ** 【代码重点】  1- LORA调制方式
 **               2- Lora_Init()时，设置3个参数( fre:频率_MHz,   sf:扩频因子,   bw:带宽)；更详细的参数可在lora.c中修改，但只要前述三个参数双方一致，即可通信
 **
 ** 【更新记录】  
 ** 
************************************************************************************************************************************/
#include "lora.h"
#include "spi.h"


/*****************************************************************************
 ** 移植 修改区
 ** SX1278 通信参数
****************************************************************************/

/************************************************
 ** 新数据类型定义
 ***********************************************/
// SX1276参数结构体
typedef struct
{
    uint32_t RFFrequency;                // 无线通信频率
    int8_t Power;                       // 功率
    uint8_t SignalBw;                   // LORA 带宽[0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                                        // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]  
    uint8_t SpreadingFactor;            // 扩频因子 LORA [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    uint8_t ErrorCoding;                // LORA 纠错码 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    bool CrcOn;                         // CRC效验开关 [0: OFF, 1: ON]
    bool ImplicitHeaderOn;              // 隐藏头部信息开关 [0: OFF, 1: ON]
    bool RxSingleOn;                    // 接收单次模式\连续模式配置[0: Continuous, 1 Single]
    bool FreqHopOn;                     // 跳频模式开关 [0: OFF, 1: ON]
    uint8_t HopPeriod;                  // 跳频之间的周期长度 Hops every frequency hopping period symbols
    uint32_t TxPacketTimeout;           // 最大发送时间
    uint32_t RxPacketTimeout;           // 最大接收时间
    uint8_t PayloadLength;              // 数据长度
}xLoraSettings_TypeDef;



/************************************************
 ** 全局 变量
 ***********************************************/
xLora_TypeDef xLora;
xLora_TypeDef xLora2;


/************************************************
 ** 本地 变量
 ***********************************************/
static  uint8_t regTemp;
#define LORA_CS_LOW      		HAL_GPIO_WritePin(GPIOB, SPI1_NSS_Pin, GPIO_PIN_RESET);
#define LORA_CS_HIGH    		HAL_GPIO_WritePin(GPIOB, SPI1_NSS_Pin, GPIO_PIN_SET);

#define LORA2_CS_LOW      		HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_RESET);
#define LORA2_CS_HIGH    		HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_SET);

xLoraSettings_TypeDef xLoraSettings =
{
    461550000,     // fre---频率    【重要：为方便设置，这个参数在Lora_Init()调用时作为参数进行配置。这个的值无效，只作为示例保留】  86000000>值>360000000, 步进间隔 2000000
    20,            // 发射功率
    8,             // BW----信号频宽 【重要：为方便设置，这个参数在Lora_Init()调用时作为参数进行配置。这个的值无效，只作为示例保留】 [0:7.8kHz, 1:10.4kHz, 2:15.6kHz, 3:20.8kHz, 4:31.2kHz, 5:41.6kHz, 6: 62.5kHz, 7:125kHz, 8:250kHz, 9:500kHz, other: Reserved]
                   // 带宽也表示传输速率，这里的带宽指的是频宽，即频段的频率宽度，它决定了中心频率的上下频率。比如中心频率 433MHZ,带宽是2MHZ，则通信的频谱（信道）的频率为432MHZ~434MHZ*/
    11,            // SF----扩频因子 【重要：为方便设置，这个参数在Lora_Init()调用时作为参数进行配置。这个的值无效，只作为示例保留】 [6:64, 7:128, 8:256, 9:512, 10:1024, 11:2048, 12:4096  chips]
                   // 扩频设置用的，扩频因子越大，信噪比越高，传输速率越低，但是传输距离也会变远； 如果该值为6，则必须得打开下方的隐藏头部信息开关
    1,             // 纠错码 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]， 即有效信号和整个报文（数据包）的比值*/
    true,          // CRC效验开关[0: OFF, 1: ON]
    false,         // 隐藏头部信息开关 [0: OFF, 1: ON]
    0,             // 接收单次模式\连续模式配置 [0: Continuous, 1 Single],单次：接收一次就没了，持续：不停地
    0,             // FreqHopOn， 跳频模式开关 [0: OFF, 1: ON]              //跳频技术
    4,             // HopPeriod, 跳频之间的周期长度
    100000,        // 最大发送时间
    1000000,       // 最大接收时间
    128,           // 数据长度 (用于隐式头模式)
};

xLoraSettings_TypeDef xLoraSettings2 =
{
    461550000,     // fre---频率    【重要：为方便设置，这个参数在Lora_Init()调用时作为参数进行配置。这个的值无效，只作为示例保留】  86000000>值>360000000, 步进间隔 2000000
    20,            // 发射功率
    8,             // BW----信号频宽 【重要：为方便设置，这个参数在Lora_Init()调用时作为参数进行配置。这个的值无效，只作为示例保留】 [0:7.8kHz, 1:10.4kHz, 2:15.6kHz, 3:20.8kHz, 4:31.2kHz, 5:41.6kHz, 6: 62.5kHz, 7:125kHz, 8:250kHz, 9:500kHz, other: Reserved]
                   // 带宽也表示传输速率，这里的带宽指的是频宽，即频段的频率宽度，它决定了中心频率的上下频率。比如中心频率 433MHZ,带宽是2MHZ，则通信的频谱（信道）的频率为432MHZ~434MHZ*/
    11,            // SF----扩频因子 【重要：为方便设置，这个参数在Lora_Init()调用时作为参数进行配置。这个的值无效，只作为示例保留】 [6:64, 7:128, 8:256, 9:512, 10:1024, 11:2048, 12:4096  chips]
                   // 扩频设置用的，扩频因子越大，信噪比越高，传输速率越低，但是传输距离也会变远； 如果该值为6，则必须得打开下方的隐藏头部信息开关
    1,             // 纠错码 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]， 即有效信号和整个报文（数据包）的比值*/
    true,          // CRC效验开关[0: OFF, 1: ON]
    false,         // 隐藏头部信息开关 [0: OFF, 1: ON]
    0,             // 接收单次模式\连续模式配置 [0: Continuous, 1 Single],单次：接收一次就没了，持续：不停地
    0,             // FreqHopOn， 跳频模式开关 [0: OFF, 1: ON]              //跳频技术
    4,             // HopPeriod, 跳频之间的周期长度
    100000,        // 最大发送时间
    1000000,       // 最大接收时间
    128,           // 数据长度 (用于隐式头模式)
};



/************************************************
 ** 本地 函数
 ***********************************************/


#if 1  // 参数宏定义 
/*LoRa模式寄存器映射*/
//SX1276内部寄存器地址
#define REG_LR_FIFO                                 0x00 
//通用寄存器//通用寄存器
#define REG_LR_OPMODE                               0x01 
#define REG_LR_FRFMSB                               0x06 
#define REG_LR_FRFMID                               0x07
#define REG_LR_FRFLSB                               0x08 
//RF模块寄存器
//发射配置
#define REG_LR_PACONFIG                             0x09 
#define REG_LR_OCP                                  0x0B 
//接收配置
#define REG_LR_LNA                                  0x0C  // 增益设备, 默认已最大
//LoRa页面寄存器
#define REG_LR_FIFOADDRPTR                          0x0D  // FIFI缓冲区路SPI接口地址指针
#define REG_LR_FIFOTXBASEADDR                       0x0E  // FIFI缓冲区的写入基址
#define REG_LR_FIFORXBASEADDR                       0x0F 
#define REG_LR_FIFORXCURRENTADDR                    0x10 
#define REG_LR_IRQFLAGSMASK                         0x11   //IAQ标志屏蔽 
#define REG_LR_IRQFLAGS                             0x12 
#define REG_LR_NBRXBYTES                            0x13 
#define REG_LR_PKTSNRVALUE                          0x19 
#define REG_LR_PKTRSSIVALUE                         0x1A 
#define REG_LR_HOPCHANNEL                           0x1C   // 跳频
#define REG_LR_MODEMCONFIG1                         0x1D   // 数据包参数：带宽、纠错码、
#define REG_LR_MODEMCONFIG2                         0x1E   // 数据包参数：扩频因子、CRC
#define REG_LR_SYMBTIMEOUTLSB                       0x1F 
#define REG_LR_PREAMBLEMSB                          0x20 
#define REG_LR_PREAMBLELSB                          0x21 
#define REG_LR_PAYLOADLENGTH                        0x22  // 负载长度，在隐式报头时才有用
#define REG_LR_PAYLOADMAXLENGTH                     0x23 
#define REG_LR_HOPPERIOD                            0x24 
#define REG_LR_FIFORXBYTEADDR                       0x25
#define REG_LR_MODEMCONFIG3                         0x26
#define REG_LR_PADAC                                0x4D // 附加寄存器

// 工作模式
#define RFLR_OPMODE_MASK                            0xF8 // [2：0]
#define RFLR_OPMODE_SLEEP                           0x00 // 睡眠模式**
#define RFLR_OPMODE_STANDBY                         0x01 // 待机模式**
#define RFLR_OPMODE_SYNTHESIZER_TX                  0x02 // 频率合成器转换至Tx频率
#define RFLR_OPMODE_TRANSMITTER                     0x03 // 发送模式
#define RFLR_OPMODE_SYNTHESIZER_RX                  0x04 // 频率合成器转换至Rx频率
#define RFLR_OPMODE_RECEIVER                        0x05 // 接收模式**
// LoRa specific modes                                   
#define RFLR_OPMODE_RECEIVER_SINGLE                 0x06 // 单次接收模式 **  
#define RFLR_OPMODE_CAD                             0x07 // CAD模式 

//PA（功率放大器） 选择和输出功率控制设置相关宏定义 RegPaConfig（寄存器地址0X09）     
#define RFLR_PACONFIG_PASELECT_PABOOST              0x80 
////LNA（低噪声放大器 ）设置相关宏定义 RegLna（寄存器地址0X0C）
#define RFLR_LNA_GAIN_G1                            0x20 // Default
//FIFO 数据缓冲区中 SPI 接口地址指针（寄存器地址0X0D）
#define RFLR_FIFOADDRPTR                            0x00 // Default
//发送信息的起始位置
#define RFLR_FIFOTXBASEADDR                         0x80 // Default
//接收信息的起始位置
#define RFLR_FIFORXBASEADDR                         0x00 // Default

//关于中断打开相关的宏定义
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80 
#define RFLR_IRQFLAGS_RXDONE                        0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20 
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10 
#define RFLR_IRQFLAGS_TXDONE                        0x08 
#define RFLR_IRQFLAGS_CADDONE                       0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02 
#define RFLR_IRQFLAGS_CADDETECTED                   0x01 

// RegDioMapping1
#define RFLR_DIOMAPPING1_DIO0_MASK                  0x3F
#define RFLR_DIOMAPPING1_DIO0_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO0_01                    0x40
#define RFLR_DIOMAPPING1_DIO0_10                    0x80
#define RFLR_DIOMAPPING1_DIO0_11                    0xC0

#define RFLR_DIOMAPPING1_DIO1_MASK                  0xCF
#define RFLR_DIOMAPPING1_DIO1_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO1_01                    0x10
#define RFLR_DIOMAPPING1_DIO1_10                    0x20
#define RFLR_DIOMAPPING1_DIO1_11                    0x30

#define RFLR_DIOMAPPING1_DIO2_MASK                  0xF3
#define RFLR_DIOMAPPING1_DIO2_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO2_01                    0x04
#define RFLR_DIOMAPPING1_DIO2_10                    0x08
#define RFLR_DIOMAPPING1_DIO2_11                    0x0C

#define RFLR_DIOMAPPING1_DIO3_MASK                  0xFC
#define RFLR_DIOMAPPING1_DIO3_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO3_01                    0x01
#define RFLR_DIOMAPPING1_DIO3_10                    0x02
#define RFLR_DIOMAPPING1_DIO3_11                    0x03

#endif  // 结束：参数宏定义


//预先计算的信号带宽，用于计算RSSI值
const double SignalBwLog[] =
{
    3.8927900303521316335038277369285,  // 7.8 kHz
    4.0177301567005500940384239336392,  // 10.4 kHz
    4.193820026016112828717566631653,   // 15.6 kHz
    4.31875866931372901183597627752391, // 20.8 kHz
    4.4948500216800940239313055263775,  // 31.2 kHz
    4.6197891057238405255051280399961,  // 41.6 kHz
    4.795880017344075219145044421102,   // 62.5 kHz
    5.0969100130080564143587833158265,  // 125 kHz
    5.397940008672037609572522210551,   // 250 kHz
    5.6989700043360188047862611052755   // 500 kHz
};

//这些值需要试验测得
const double RssiOffsetLF[] =
{   
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
};



/******************************************************************************
 * 函  数： Lora_Init()
 * 功  能： 配置Lora, 完成后进入接收模式
 * 参  数： fre: 频率， 单位：MHz,   
 *          sf : 扩频因子  
 *          bw : 带宽
 * 返回值：
 * 备  注：                                           最后修改_2020年07月24日
 ******************************************************************************/  
void Lora_Init( uint32_t fre, uint8_t sf, uint8_t bw)
{    
	printf("sx1278-Lora初始化\r\n\n");
	xLoraSettings .RFFrequency = fre*1000*1000;
    xLoraSettings .SpreadingFactor = sf;
    xLoraSettings .SignalBw=bw;
	
	MX_SPI1_Init();
    SPI1_Reset( );                                         			// 重置SX1276        
    ReadForRegister(0x06, &regTemp ,1);                             // 测试spi是否能读写数据
    while(regTemp  != 0x6C)
    {
        printf("    连接失败，将退出SX1276始化!\r\n\n");               
        return;
    }
     
    sx1276_SetOpMode( RFLR_OPMODE_SLEEP );                          // 1_进入睡眠模式,才可以修改OPMODE     
    
    ReadForRegister (0x01, &regTemp , 1);                           // 2_设置调制方式为LORA：RegOpMode_寄存器0x01_[7]，1:LORA，0:FSK
    regTemp |= 0x80 ;        
    writeToRegister( 0x01, &regTemp , 1);
    
    // 6_设置LNA增益【7：5】、电流，RegLna_寄存器_0x0C
    regTemp  = RFLR_LNA_GAIN_G1;                                    // LNA增益默认也是最大值 ， 数据手册_113页
    writeToRegister ( REG_LR_LNA , &regTemp , 1);                   // 把LNA新值 写入寄存器
    
    // 射频设置
    // 影响通信速率和距离的两大重点：扩频因子，带宽， 扩频因子越大越远，但慢， 带宽越宽越快，但近， 两者相反
    SX1276LoRaSetRFFrequency(     xLoraSettings.RFFrequency );      // 设置 载波频率
    SX1276LoRaSetSpreadingFactor( xLoraSettings.SpreadingFactor );  // 设置 扩频因子?? SF6只在隐式报头模式下工作  默认7
    SX1276LoRaSetErrorCoding(     xLoraSettings.ErrorCoding );      // 纠错码 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]， 即有效信号和整个报文（数据包）的比值*/
    SX1276LoRaSetPacketCrcOn(     xLoraSettings.CrcOn );            // CRC效验开关
    SX1276LoRaSetSignalBandwidth( xLoraSettings.SignalBw );         // 带宽   

    SX1276LoRaSetImplicitHeaderOn(xLoraSettings.ImplicitHeaderOn ); // 隐藏头部信息开关  默认关
    SX1276LoRaSetSymbTimeout( 0x3FF );                              // 斜升斜降时间为500us
        
    writeToRegister( 0x22, &xLoraSettings.PayloadLength , 1 );      // 设置负载长度（仅在隐藏头部开启时有用）
    
    SX1276LoRaSetLowDatarateOptimize( true );                       // ？？？设置最大接收超时时间为2^8+1
   
    SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST );        // 选择 PA_BOOST 管脚输出信号 1?PA_BOOST 引脚。输出功率不得超过+20dBm
    regTemp = 0x87;
    writeToRegister( REG_LR_PADAC, &regTemp , 1);                   // 最大功率可，达到+20dMb
    regTemp = 0x8F;    
    writeToRegister( REG_LR_PACONFIG, &regTemp , 1);                // 设置发射功率 , 20
    
    //低速率优化，当传输速率低于300时，要打开
    //ReadForRegister (0x26, &regTemp , 1);
    //regTemp = (regTemp & 0xF7) | ( 1<<3);
    //writeToRegister (0x26, &regTemp, 1);       

    regTemp = 0xFF;                                   // 清理中断标志
    writeToRegister(REG_LR_IRQFLAGS ,&regTemp, 1);        // 清理中断标志
        
    xLora.InitOK = 1;                                 // 初始化完成标志        
    Lora_RxMode();                                    // 进入持续接收模式    
    printf("     检测成功；已进入接收状态中\r\n\n");   
}    


/******************************************************************************
 * 函  数： Lora2_Init()
 * 功  能： 配置Lora, 完成后进入接收模式
 * 参  数： fre: 频率， 单位：MHz,   
 *          sf : 扩频因子  
 *          bw : 带宽
 * 返回值：
 * 备  注：                                           最后修改_2020年07月24日
 ******************************************************************************/  
void Lora_Init2( uint32_t fre, uint8_t sf, uint8_t bw)
{    
	printf("sx1278-Lora2初始化\r\n\n");
	xLoraSettings2 .RFFrequency = fre*1000*1000;
    xLoraSettings2 .SpreadingFactor = sf;
    xLoraSettings2 .SignalBw=bw;
	
	MX_SPI2_Init();
    SPI1_Reset2( );                                                        // 重置SX1276        
    ReadForRegister2(0x06, &regTemp ,1);                             // 测试spi是否能读写数据
    while(regTemp  != 0x6C)
    {
        printf("  连接失败，将退出SX1276 Lora2初始化!\r\n\n");               
        return;
    }
     
    sx1276_SetOpMode2( RFLR_OPMODE_SLEEP );                          // 1_进入睡眠模式,才可以修改OPMODE     
    
    ReadForRegister2 (0x01, &regTemp , 1);                           // 2_设置调制方式为LORA：RegOpMode_寄存器0x01_[7]，1:LORA，0:FSK
    regTemp |= 0x80 ;        
    writeToRegister2( 0x01, &regTemp , 1);
    
    // 6_设置LNA增益【7：5】、电流，RegLna_寄存器_0x0C
    regTemp  = RFLR_LNA_GAIN_G1;                                    // LNA增益默认也是最大值 ， 数据手册_113页
    writeToRegister2 ( REG_LR_LNA , &regTemp , 1);                   // 把LNA新值 写入寄存器
    
    // 射频设置
    // 影响通信速率和距离的两大重点：扩频因子，带宽， 扩频因子越大越远，但慢， 带宽越宽越快，但近， 两者相反
    SX1276LoRaSetRFFrequency2(     xLoraSettings2.RFFrequency );      // 设置 载波频率
    SX1276LoRaSetSpreadingFactor2( xLoraSettings2.SpreadingFactor );  // 设置 扩频因子?? SF6只在隐式报头模式下工作  默认7
    SX1276LoRaSetErrorCoding2(     xLoraSettings2.ErrorCoding );      // 纠错码 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]， 即有效信号和整个报文（数据包）的比值*/
    SX1276LoRaSetPacketCrcOn2(     xLoraSettings2.CrcOn );            // CRC效验开关
    SX1276LoRaSetSignalBandwidth2( xLoraSettings2.SignalBw );         // 带宽   

    SX1276LoRaSetImplicitHeaderOn2(xLoraSettings2.ImplicitHeaderOn ); // 隐藏头部信息开关  默认关
    SX1276LoRaSetSymbTimeout2( 0x3FF );                              // 斜升斜降时间为500us
        
    writeToRegister2( 0x22, &xLoraSettings2.PayloadLength , 1 );      // 设置负载长度（仅在隐藏头部开启时有用）
    
    SX1276LoRaSetLowDatarateOptimize2( true );                       // ？？？设置最大接收超时时间为2^8+1
   
    SX1276LoRaSetPAOutput2( RFLR_PACONFIG_PASELECT_PABOOST );        // 选择 PA_BOOST 管脚输出信号 1?PA_BOOST 引脚。输出功率不得超过+20dBm
    regTemp = 0x87;
    writeToRegister2( REG_LR_PADAC, &regTemp , 1);                   // 最大功率可，达到+20dMb
    regTemp = 0x8F;    
    writeToRegister2( REG_LR_PACONFIG, &regTemp , 1);                // 设置发射功率 , 20
    
    //低速率优化，当传输速率低于300时，要打开
    //ReadForRegister (0x26, &regTemp , 1);
    //regTemp = (regTemp & 0xF7) | ( 1<<3);
    //writeToRegister (0x26, &regTemp, 1);       

    regTemp = 0xFF;                                   // 清理中断标志
    writeToRegister2(REG_LR_IRQFLAGS ,&regTemp, 1);        // 清理中断标志
        
    xLora2.InitOK = 1;                                 // 初始化完成标志        
    Lora_RxMode2();                                    // 进入持续接收模式    
    printf("     检测成功；已进入接收状态中\r\n\n");   
}    

//往寄存器的地址中连续写入几个字节的数据，此函数通常也用来配置寄存器，
//此函数用来与下面的函数配合，当SX1276寄存器是16位的时候，这时候8个字
//节不够用，所以就得用这个函数，往寄存器中写入多个字节。
static void writeToRegister( uint8_t addr, uint8_t *buffer, uint8_t size )
{    
    // 设置 SPI通信配置，重要，为防多个设备共用一个SPI，设备每次读写，都要调用本函数
    MX_SPI1_Init(); 
    
    LORA_CS_LOW;
    SPI_TransmitOneByte( addr | 0x80 );     // 送发的第一个为地址， 【7】值  0：读， 1写
    for(uint8_t i = 0; i < size; i++ )
    {
        SPI_TransmitOneByte( buffer[i] );
    }
    LORA_CS_HIGH;
}

static void writeToRegister2( uint8_t addr, uint8_t *buffer, uint8_t size )
{    
    // 设置 SPI通信配置，重要，为防多个设备共用一个SPI，设备每次读写，都要调用本函数
    MX_SPI2_Init(); 
    
    LORA2_CS_LOW;
    SPI2_TransmitOneByte( addr | 0x80 );     // 送发的第一个为地址， 【7】值  0：读， 1写
    for(uint8_t i = 0; i < size; i++ )
    {
        SPI2_TransmitOneByte( buffer[i] );
    }
    LORA2_CS_HIGH;
}

//下面2个函数与上面2个读寄存器函数同理，不过它是用来读寄存器值
static void ReadForRegister( uint8_t addr, uint8_t *buffer, uint8_t size )
{     
    // 设置 SPI通信配置，重要，为防多个设备共用一个SPI，设备每次读写，都要调用本函数
    MX_SPI1_Init(); 
    
    LORA_CS_LOW;
    SPI_TransmitOneByte( addr & 0x7F );       // 送发的第一个为地址， 【7】值  0：读， 1写
    for( uint8_t i = 0; i < size; i++ )
    {
        buffer[i] = SPI_ReceiveOneByte();
    }    
    LORA_CS_HIGH;
}

//下面2个函数与上面2个读寄存器函数同理，不过它是用来读寄存器值
static void ReadForRegister2( uint8_t addr, uint8_t *buffer, uint8_t size )
{     
    // 设置 SPI通信配置，重要，为防多个设备共用一个SPI，设备每次读写，都要调用本函数
    MX_SPI2_Init(); 
    
    LORA2_CS_LOW;
    SPI2_TransmitOneByte( addr & 0x7F );       // 送发的第一个为地址， 【7】值  0：读， 1写
    for( uint8_t i = 0; i < size; i++ )
    {
        buffer[i] = SPI2_ReceiveOneByte();
    }    
    LORA2_CS_HIGH;
}


static void SPI1_Reset( void )
{
	HAL_GPIO_WritePin(Lora_Reset_GPIO_Port, Lora_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
    
	HAL_GPIO_WritePin(Lora_Reset_GPIO_Port, Lora_Reset_Pin, GPIO_PIN_SET);
	HAL_Delay(200);	 
}

static void SPI1_Reset2( void )
{
	HAL_GPIO_WritePin(Lora_Reset2_GPIO_Port, Lora_Reset2_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
    
	HAL_GPIO_WritePin(Lora_Reset2_GPIO_Port, Lora_Reset2_Pin, GPIO_PIN_SET);
	HAL_Delay(200);	 
}

//设置频率    
static void SX1276LoRaSetRFFrequency( uint32_t freq )
{    
    uint8_t F[3];
    freq = ( uint32_t )( ( double )freq / ( double )61.03515625f );  // 61.03515625来源，查看数据手册111页
    F[0] = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    F[1] = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    F[2] = ( uint8_t )( freq & 0xFF );
    writeToRegister( 0x06, F, 3 ); // 把高中低位，写到寄存器
}

//设置带宽
static void SX1276LoRaSetSignalBandwidth( uint8_t bw )
{
    ReadForRegister( 0x1D, &regTemp , 1 );   // 读取设备中寄存器的值 
    regTemp  = ( regTemp  & 0x0F ) | ( bw << 4 ); // 把新值写到相应的数据位中
    writeToRegister( 0x1D, &regTemp , 1);   // 把新值写入到设备
    //LoRaSettings.SignalBw = bw;
}

// 设置扩频因子
// 扩频因子[6:64,  7:128,  8:256,  9:512,  10:1024,  11:2048,  12:4096  chips]
// 影响空中速率和距离的两大重点：扩频因子、带宽，
// 扩频因子越大，抗干扰越好，距离越远，但传输速率就越小
// 带宽值越大，  速率越高，  距离越小
static void SX1276LoRaSetSpreadingFactor( uint8_t factor )
{
    if( factor > 12 )   factor = 12;    
    if( factor < 6 )    factor = 6;  
    
    // 设置数据包长度， 值的关系暂不明白
    ReadForRegister (0x31, &regTemp ,1);
    if( factor == 6 )   
        regTemp = (regTemp & 0xF8) | 5;             
    else       
        regTemp = (regTemp & 0xF8) | 3;
    writeToRegister (0x31, &regTemp , 1);         

    // 写入新的扩频值
    ReadForRegister ( 0x1E, &regTemp , 1);
    regTemp =  (regTemp & 0x0F) | (factor << 4);
    writeToRegister (0x1E, &regTemp , 1);  
}

// 纠错码 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]， 默认1：4/5
// 即有效信号和整个报文（数据包）的比值*/
static void SX1276LoRaSetErrorCoding( uint8_t value )
{
    ReadForRegister( 0x1D, &regTemp , 1 );  // 读取设备寄存器的值 
    regTemp  = ( regTemp  & 0xF1 ) | ( value << 1 );  // 设置新值 
    writeToRegister( 0x1D, &regTemp ,1 );  // 把新值写入到设备    
}

//CRC校验使能
static void SX1276LoRaSetPacketCrcOn( bool enable )
{
    ReadForRegister( REG_LR_MODEMCONFIG2, &regTemp ,1 );    // 读取设备寄存器的值 
    regTemp  = ( regTemp  & 0xFB ) | ( enable << 2);   // 把新值写到相应的数据位中
    writeToRegister( REG_LR_MODEMCONFIG2, &regTemp , 1);   // 把新值写入到设备    
}

//隐藏头部信息开关  默认关
static void SX1276LoRaSetImplicitHeaderOn( bool enable )
{   
    ReadForRegister( 0x1D, &regTemp ,1 );  // 读取设备寄存器的值 
    regTemp = ( regTemp & 0xFE) | ( enable );   // 把新值写到相应的数据位中
    writeToRegister(0x1D, &regTemp ,1 );  // 把新值写入到设备
}



//PA选择和输出功率控制, RegPaConfig_寄存器_0x09, [7]
static void SX1276LoRaSetPAOutput( uint8_t outputPin )
{
    ReadForRegister( REG_LR_PACONFIG, &regTemp ,1 );  // 读取寄存器的值
    regTemp  = (regTemp  & 0x7F ) | outputPin;   // 设置 RegPaConfig_寄存器_0x09,[7]
    writeToRegister(REG_LR_PACONFIG, &regTemp, 1 );
}

//设置接收超时时间
static void SX1276LoRaSetSymbTimeout( uint16_t value )
{
    uint8_t reg[2];
    ReadForRegister( 0x1E, reg, 2 );   // 需要读2个寄存器值 ， 因为TimeOut值为10位
    reg[0] = ( reg[0] & 0xFC ) | ( ( value >> 8 ) & ~0xFC );
    reg[1] = value & 0xFF;
    writeToRegister( 0x1E, reg, 2);  // 把新值写入到设备寄存器
}

//设置最大接收超时时间为2^8+1
static void SX1276LoRaSetLowDatarateOptimize( bool enable )
{  
    ReadForRegister( REG_LR_MODEMCONFIG3, &regTemp, 1 );   // 读取寄存器的值
    regTemp = ( regTemp & 0xF7 ) | ( enable << 3 );
    writeToRegister( REG_LR_MODEMCONFIG3, &regTemp,1 );   // 把新值写入到设备寄存器
}


/*****************************************************************
* 功能描述：设置LoRa工作模式, RegOpMode_0x01_[2:0]，主要是五种
* 入口参数：RFLR_OPMODE_SLEEP                 --    睡眠模式！！
            RFLR_OPMODE_STANDBY                 --    待机模式！！
            RFLR_OPMODE_SYNTHESIZER_TX       -- 频率合成发送模式
            RFLR_OPMODE_TRANSMITTER             --    发送模式！！
            RFLR_OPMODE_SYNTHESIZER_RX       -- 频率合成接收模式
            RFLR_OPMODE_RECEIVER             --    持续接收模式！！
            RFLR_OPMODE_RECEIVER_SINGLE      -- 单次接收模式！！
            RFLR_OPMODE_CAD                  -- 信号活动检测模式
* 返回值：        无
*******************************************************************/
static void sx1276_SetOpMode( uint8_t opMode )
{
    ReadForRegister( 0x01, &regTemp, 1 );
    regTemp &= 0xF8;
    regTemp |= (uint8_t)opMode ;
    writeToRegister( 0x01, &regTemp , 1 );     // 把新工作模式值，写到寄存器REG_LR_OPMODE_0x01_[2:0]，主要是五种
}


/*
 *
 *	SPI2相关函数
 *
 *
*/

//设置频率    
static void SX1276LoRaSetRFFrequency2( uint32_t freq )
{    
    uint8_t F[3];
    freq = ( uint32_t )( ( double )freq / ( double )61.03515625f );  // 61.03515625来源，查看数据手册111页
    F[0] = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    F[1] = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    F[2] = ( uint8_t )( freq & 0xFF );
    writeToRegister2( 0x06, F, 3 ); // 把高中低位，写到寄存器
}

//设置带宽
static void SX1276LoRaSetSignalBandwidth2( uint8_t bw )
{
    ReadForRegister2( 0x1D, &regTemp , 1 );   // 读取设备中寄存器的值 
    regTemp  = ( regTemp  & 0x0F ) | ( bw << 4 ); // 把新值写到相应的数据位中
    writeToRegister2( 0x1D, &regTemp , 1);   // 把新值写入到设备
    //LoRaSettings.SignalBw = bw;
}

// 设置扩频因子
// 扩频因子[6:64,  7:128,  8:256,  9:512,  10:1024,  11:2048,  12:4096  chips]
// 影响空中速率和距离的两大重点：扩频因子、带宽，
// 扩频因子越大，抗干扰越好，距离越远，但传输速率就越小
// 带宽值越大，  速率越高，  距离越小
static void SX1276LoRaSetSpreadingFactor2( uint8_t factor )
{
    if( factor > 12 )   factor = 12;    
    if( factor < 6 )    factor = 6;  
    
    // 设置数据包长度， 值的关系暂不明白
    ReadForRegister2 (0x31, &regTemp ,1);
    if( factor == 6 )   
        regTemp = (regTemp & 0xF8) | 5;             
    else       
        regTemp = (regTemp & 0xF8) | 3;
    writeToRegister2 (0x31, &regTemp , 1);         

    // 写入新的扩频值
    ReadForRegister2 ( 0x1E, &regTemp , 1);
    regTemp =  (regTemp & 0x0F) | (factor << 4);
    writeToRegister2 (0x1E, &regTemp , 1);  
}

// 纠错码 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]， 默认1：4/5
// 即有效信号和整个报文（数据包）的比值*/
static void SX1276LoRaSetErrorCoding2( uint8_t value )
{
    ReadForRegister2( 0x1D, &regTemp , 1 );  // 读取设备寄存器的值 
    regTemp  = ( regTemp  & 0xF1 ) | ( value << 1 );  // 设置新值 
    writeToRegister2( 0x1D, &regTemp ,1 );  // 把新值写入到设备    
}

//CRC校验使能
static void SX1276LoRaSetPacketCrcOn2( bool enable )
{
    ReadForRegister2( REG_LR_MODEMCONFIG2, &regTemp ,1 );    // 读取设备寄存器的值 
    regTemp  = ( regTemp  & 0xFB ) | ( enable << 2);   // 把新值写到相应的数据位中
    writeToRegister2( REG_LR_MODEMCONFIG2, &regTemp , 1);   // 把新值写入到设备    
}

//隐藏头部信息开关  默认关
static void SX1276LoRaSetImplicitHeaderOn2( bool enable )
{   
    ReadForRegister2( 0x1D, &regTemp ,1 );  // 读取设备寄存器的值 
    regTemp = ( regTemp & 0xFE) | ( enable );   // 把新值写到相应的数据位中
    writeToRegister2(0x1D, &regTemp ,1 );  // 把新值写入到设备
}



//PA选择和输出功率控制, RegPaConfig_寄存器_0x09, [7]
static void SX1276LoRaSetPAOutput2( uint8_t outputPin )
{
    ReadForRegister2( REG_LR_PACONFIG, &regTemp ,1 );  // 读取寄存器的值
    regTemp  = (regTemp  & 0x7F ) | outputPin;   // 设置 RegPaConfig_寄存器_0x09,[7]
    writeToRegister2(REG_LR_PACONFIG, &regTemp, 1 );
}

//设置接收超时时间
static void SX1276LoRaSetSymbTimeout2( uint16_t value )
{
    uint8_t reg[2];
    ReadForRegister2( 0x1E, reg, 2 );   // 需要读2个寄存器值 ， 因为TimeOut值为10位
    reg[0] = ( reg[0] & 0xFC ) | ( ( value >> 8 ) & ~0xFC );
    reg[1] = value & 0xFF;
    writeToRegister2( 0x1E, reg, 2);  // 把新值写入到设备寄存器
}

//设置最大接收超时时间为2^8+1
static void SX1276LoRaSetLowDatarateOptimize2( bool enable )
{  
    ReadForRegister2( REG_LR_MODEMCONFIG3, &regTemp, 1 );   // 读取寄存器的值
    regTemp = ( regTemp & 0xF7 ) | ( enable << 3 );
    writeToRegister2( REG_LR_MODEMCONFIG3, &regTemp,1 );   // 把新值写入到设备寄存器
}


/*****************************************************************
* 功能描述：设置LoRa工作模式, RegOpMode_0x01_[2:0]，主要是五种
* 入口参数：RFLR_OPMODE_SLEEP                 --    睡眠模式！！
            RFLR_OPMODE_STANDBY                 --    待机模式！！
            RFLR_OPMODE_SYNTHESIZER_TX       -- 频率合成发送模式
            RFLR_OPMODE_TRANSMITTER             --    发送模式！！
            RFLR_OPMODE_SYNTHESIZER_RX       -- 频率合成接收模式
            RFLR_OPMODE_RECEIVER             --    持续接收模式！！
            RFLR_OPMODE_RECEIVER_SINGLE      -- 单次接收模式！！
            RFLR_OPMODE_CAD                  -- 信号活动检测模式
* 返回值：        无
*******************************************************************/
static void sx1276_SetOpMode2( uint8_t opMode )
{
    ReadForRegister2( 0x01, &regTemp, 1 );
    regTemp &= 0xF8;
    regTemp |= (uint8_t)opMode ;
    writeToRegister2( 0x01, &regTemp , 1 );     // 把新工作模式值，写到寄存器REG_LR_OPMODE_0x01_[2:0]，主要是五种
}

//获取LNA（信号放大）增益（001=最大增益，010=最大增益-6dB，011=最大增益-12dB..........）
#if 0
static uint8_t Lora_GetGain( void )                    
{    
    ReadForRegister( REG_LR_LNA, &regTemp , 1);  // 0x0C, 增益设置, 复位后默认最大
    return( regTemp >> 5 ) & 0x07;
}

/********************************************************************
    * 功能描述：    获取调用本函数前最后接到那包数据的信噪比
    * 入口参数：    无
    * 返回值：    信噪比，有正有负
*********************************************************************/
static int8_t Lora_GetSNR (void)                                                
{
    int8_t _SNR;        // 信噪比结果
    uint8_t rxSnrEstimate;   // 信噪比原值
    // 取最后一包数据的信噪比                                                 
    ReadForRegister(REG_LR_PKTSNRVALUE, &rxSnrEstimate , 1);            
    // 信噪比原值<0，负数                
    if( rxSnrEstimate & 0x80 )  
    {
        _SNR = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;    
        _SNR = -_SNR;
    }
    //信噪比原值>0，正数
    else                        
    {        
        _SNR = ( rxSnrEstimate & 0xFF ) >> 2;            
    }
    return _SNR;
}


/*******************************************************************************
 * 功能: 最后接到1包数据的信号强度
 * 备注：本代码在原厂代码中复制，暂未用到
 * 参数: 无
 * 返回: 信号强度，有正有负
********************************************************************************/
static double Lora_GetRSSI(void)                                             
{
    double _RSSI;
    int8_t _SNR = Lora_GetSNR();  
    if(  _SNR < 0 )
    {
        // 功率：P=-174（dBm） + BW(dB) + NF(dB) + SNR(dB);
        // 在信号被噪声淹没的情况下用此公式反推RSSI，前三项是热噪声功率,最后一项是信噪比
        // 信号被噪声淹没
        _RSSI = (-174) + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + 4.0 + (double)_SNR;
    }
    else
    {
        //信号强于噪声
        ReadForRegister( REG_LR_PKTRSSIVALUE, &regTemp ,1);
        _RSSI = RssiOffsetLF[LoRaSettings.SignalBw] + (double)regTemp ;
    }
    return _RSSI;
}
#endif



/******************************************************************************
 * 描  述：    进入待机状态、睡眠状态
 * 参  数：    
 * 返回值：    
*******************************************************************************/
void Lora_StandbyMode(void)
{    
    if(xLora.InitOK == 0) 
    {
        printf("\r\nLORA 初始化失败, 不能进入待机模式！\r\n");
        return;
    }
    regTemp = 0xFF;
    writeToRegister( REG_LR_IRQFLAGS, &regTemp  ,1);     // 清除发送完成中断  
    sx1276_SetOpMode( RFLR_OPMODE_STANDBY );          // 通过在数据包发送后立即关闭发射机来优化功耗 
   
    xLora. TxMode =0;    
    xLora .RxMode=0;    
}

void Lora_StandbyMode2(void)
{    
    if(xLora2.InitOK == 0) 
    {
        printf("\r\nLORA 初始化失败, 不能进入待机模式！\r\n");
        return;
    }
    regTemp = 0xFF;
    writeToRegister2( REG_LR_IRQFLAGS, &regTemp  ,1);     // 清除发送完成中断  
    sx1276_SetOpMode2( RFLR_OPMODE_STANDBY );          // 通过在数据包发送后立即关闭发射机来优化功耗 
   
    xLora2. TxMode =0;    
    xLora2 .RxMode=0;    
}

/******************************************************************************
 * 函  数： Lora_TxData()
 * 功  能： 发送指定长度数据
 * 参  数： char *buf   待发送数据
 *          uint8_t size    等发送数据字节数量
 * 返回值：
 * 备  注：                                            最后修改_2020年07月24日
 ******************************************************************************/  
void Lora_TxData(uint8_t* buf, uint8_t size)
{    
    if(xLora.InitOK == 0) 
    {
        printf("\r\nLORA 初始化失败, 不能发送！\r\n");
        return;
    }  
    
    if(xLora.TxMode  == 1 ) 
    {
        printf("上次发送还没完成，本次发送失败\r\n");
        return;  
    }     
    
    if(size==0) return;        
    
    sx1276_SetOpMode( RFLR_OPMODE_STANDBY );            // 待机模式   
    
    regTemp  = 0;                                      
    writeToRegister( REG_LR_HOPPERIOD, &regTemp ,1 );       // 写入跳频周期_0x24, 默认为0, 即不做频率跳变
    
    regTemp  = RFLR_IRQFLAGS_RXTIMEOUT         |        // 中断选择
               RFLR_IRQFLAGS_RXDONE            |
               RFLR_IRQFLAGS_PAYLOADCRCERROR   |
               RFLR_IRQFLAGS_VALIDHEADER       |
               //RFLR_IRQFLAGS_TXDONE          |        // 开启发送完成中断
               RFLR_IRQFLAGS_CADDONE           |
               RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL|
               RFLR_IRQFLAGS_CADDETECTED;
    writeToRegister( 0x11, &regTemp, 1 );                   // 将中断值写到寄存器_0x11   
    
    // 中断映射至 IO:0~3            【7：6】DIO0 接收完成中断    【5：4】DIO1 RxTimeout中断  【3：2】DIO2 FhssChange中断  【1：0】DIO3 CadDone中断
    regTemp  = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
    writeToRegister( 0x40, &regTemp, 1 ); 
    
    regTemp  = 0x00;                                                    
    writeToRegister( REG_LR_FIFOTXBASEADDR, &regTemp , 1);  // 0x0E FIFI缓冲区的写入基址    
    writeToRegister( REG_LR_FIFOADDRPTR, &regTemp, 1);      // 0x0D FIFI缓冲区路SPI接口地址指针
   
    regTemp  =  size ;                                  // 初始化负载大小
    writeToRegister( REG_LR_PAYLOADLENGTH, &regTemp , 1 );  // 写入负载长度,隐式报头才有用     
 
    writeToRegister(0, (uint8_t*)buf,  size );                        // 将要发送的数组写fifo中      
    
    xLora.TxMode =1; 
    sx1276_SetOpMode( RFLR_OPMODE_TRANSMITTER );        // 设置为:发送模式 
}

void Lora_TxData2(uint8_t* buf, uint8_t size)
{    
    if(xLora2.InitOK == 0) 
    {
        printf("\r\nLORA2 初始化失败, 不能发送！\r\n");
        return;
    }  
    
    if(xLora2.TxMode  == 1 ) 
    {
        printf("LORA2 上次发送还没完成，本次发送失败\r\n");
        return;  
    }     
    
    if(size==0) return;        
    
    sx1276_SetOpMode2( RFLR_OPMODE_STANDBY );            // 待机模式   
    
    regTemp  = 0;                                      
    writeToRegister2( REG_LR_HOPPERIOD, &regTemp ,1 );       // 写入跳频周期_0x24, 默认为0, 即不做频率跳变
    
    regTemp  = RFLR_IRQFLAGS_RXTIMEOUT         |        // 中断选择
               RFLR_IRQFLAGS_RXDONE            |
               RFLR_IRQFLAGS_PAYLOADCRCERROR   |
               RFLR_IRQFLAGS_VALIDHEADER       |
               //RFLR_IRQFLAGS_TXDONE          |        // 开启发送完成中断
               RFLR_IRQFLAGS_CADDONE           |
               RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL|
               RFLR_IRQFLAGS_CADDETECTED;
    writeToRegister2( 0x11, &regTemp, 1 );                   // 将中断值写到寄存器_0x11   
    
    // 中断映射至 IO:0~3            【7：6】DIO0 接收完成中断    【5：4】DIO1 RxTimeout中断  【3：2】DIO2 FhssChange中断  【1：0】DIO3 CadDone中断
    regTemp  = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
    writeToRegister2( 0x40, &regTemp, 1 ); 
    
    regTemp  = 0x00;                                                    
    writeToRegister2( REG_LR_FIFOTXBASEADDR, &regTemp , 1);  // 0x0E FIFI缓冲区的写入基址    
    writeToRegister2( REG_LR_FIFOADDRPTR, &regTemp, 1);      // 0x0D FIFI缓冲区路SPI接口地址指针
   
    regTemp  =  size ;                                  // 初始化负载大小
    writeToRegister2( REG_LR_PAYLOADLENGTH, &regTemp , 1 );  // 写入负载长度,隐式报头才有用     
 
    writeToRegister2(0, (uint8_t*)buf,  size );                        // 将要发送的数组写fifo中      
    
    xLora2.TxMode =1; 
    sx1276_SetOpMode2( RFLR_OPMODE_TRANSMITTER );        // 设置为:发送模式 
}
/******************************************************************************
 * 函  数： Lora_RxData()
 * 功  能： 持续接收模式
 * 参  数： 无
 * 返回值：
 * 备  注：                                             最后修改_2020年07月24日
 ******************************************************************************/  
void Lora_RxMode(void)
{
    if(xLora.InitOK == 0) 
    {
        printf("\r\nLORA 初始化失败，不能设置为接收模式！\r\n");
        return;
    }     

    sx1276_SetOpMode( RFLR_OPMODE_STANDBY );           // 待机模式, 有些寄存器只有在待机、睡眠状态才能修改      
   
    regTemp = 0;                                       // 原：255,写入跳频周期_0x24, 默认为0, 即不做频率跳变
    writeToRegister( REG_LR_HOPPERIOD, &regTemp  , 1);       
       
    regTemp  = RFLR_IRQFLAGS_RXTIMEOUT |                  // 超时中断                   RegIrqFlagsMask寄存器0x11, 0:开启， 1：关闭 
               //RFLR_IRQFLAGS_RXDONE  |                  // 数据包接收完成中断
               //RFLR_IRQFLAGS_PAYLOADCRCERROR |          // 负载CRC错误中断 
               RFLR_IRQFLAGS_VALIDHEADER |                // Rx 模式下接收到的有效报头
               RFLR_IRQFLAGS_TXDONE      |                // FIFO 负载发送完成中断
               RFLR_IRQFLAGS_CADDONE     |                // CAD 完成中断屏蔽
               RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL|          // FHSS改变信道中断
               RFLR_IRQFLAGS_CADDETECTED;                 // 检测到 CAD 中断
    writeToRegister( REG_LR_IRQFLAGSMASK, &regTemp ,1 );  // 新的中断值写入到设备中断寄存器
    
    // 中断映射 【7：6】DIO0 接收完成中断   【5：4】DIO1 RxTimeout中断  【3：2】DIO2 FhssChange中断  【1：0】DIO3 CadDone中断   
    regTemp  = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
    writeToRegister ( 0x40, &regTemp ,1);

    // FIFO中指针位置, 尝试修改过，不影响
    //SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;
    //sx1276_SpiWriteByte( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr ); 
          
    regTemp = 0xFF;                                   // 清理中断标志位
    writeToRegister(REG_LR_IRQFLAGS , &regTemp , 1);      
    
    sx1276_SetOpMode( RFLR_OPMODE_RECEIVER );    
    xLora .RxMode =1;    
}

void Lora_RxMode2(void)
{
    if(xLora2.InitOK == 0) 
    {
        printf("\r\nLORA2 初始化失败，不能设置为接收模式！\r\n");
        return;
    }     

    sx1276_SetOpMode2( RFLR_OPMODE_STANDBY );           // 待机模式, 有些寄存器只有在待机、睡眠状态才能修改      
   
    regTemp = 0;                                       // 原：255,写入跳频周期_0x24, 默认为0, 即不做频率跳变
    writeToRegister2( REG_LR_HOPPERIOD, &regTemp  , 1);       
       
    regTemp  = RFLR_IRQFLAGS_RXTIMEOUT |                  // 超时中断                   RegIrqFlagsMask寄存器0x11, 0:开启， 1：关闭 
               //RFLR_IRQFLAGS_RXDONE  |                  // 数据包接收完成中断
               //RFLR_IRQFLAGS_PAYLOADCRCERROR |          // 负载CRC错误中断 
               RFLR_IRQFLAGS_VALIDHEADER |                // Rx 模式下接收到的有效报头
               RFLR_IRQFLAGS_TXDONE      |                // FIFO 负载发送完成中断
               RFLR_IRQFLAGS_CADDONE     |                // CAD 完成中断屏蔽
               RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL|          // FHSS改变信道中断
               RFLR_IRQFLAGS_CADDETECTED;                 // 检测到 CAD 中断
    writeToRegister2( REG_LR_IRQFLAGSMASK, &regTemp ,1 );  // 新的中断值写入到设备中断寄存器
    
    // 中断映射 【7：6】DIO0 接收完成中断   【5：4】DIO1 RxTimeout中断  【3：2】DIO2 FhssChange中断  【1：0】DIO3 CadDone中断   
    regTemp  = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
    writeToRegister2 ( 0x40, &regTemp ,1);

    // FIFO中指针位置, 尝试修改过，不影响
    //SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;
    //sx1276_SpiWriteByte( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr ); 
          
    regTemp = 0xFF;                                   // 清理中断标志位
    writeToRegister2(REG_LR_IRQFLAGS , &regTemp , 1);      
    
    sx1276_SetOpMode2( RFLR_OPMODE_RECEIVER );    
    xLora2 .RxMode =1;    
}

/******************************************************************************
 * 函  数： LORA_DIO0_HANDLER()
 * 功  能： 中断服务函数
 *          本函数用于处理发送、接收数据，如果接收到数据，输出至串口
 * 参  数： 
 * 返回值：
 * 备  注：                                             最后修改_2020年07月24日
 ******************************************************************************/  
void LORA_DIO0_HANDLER(void)
{
    // 发送完成中断 
    if(xLora .TxMode ==1)   
    {      
		printf("xLora .TxMode start!\r\n");	
        regTemp = 0xFF;                   // 清除寄存器中断标志：发送完成标志   
        writeToRegister( 0x12, &regTemp , 1);                 
        xLora.TxMode  =0;                 // 更新工作状态       
                 
        Lora_RxMode();                    // 进入:持续接收模式  
        xLora .RxMode = 1 ;               // 更新工作状态
              
        return;                           // 返回
    }
    
    // 接收完数据包 ： 数据存入:RxBuffer, 字节数存入:RxSize 
    if(xLora .RxMode ==1)
    {       
		printf("xLora .RxMode start!\r\n");			
        regTemp = 0xFF;
        writeToRegister( 0x12, &regTemp ,1 );                           // 清除寄存器中断标志：接收到数据包            
     
        if( xLoraSettings.ImplicitHeaderOn == true )                    // 隐藏头部格式 
            xLora.ReceivedSize = xLoraSettings.PayloadLength ;       
        else                                                            // 显式头部格式 
            ReadForRegister( REG_LR_NBRXBYTES, &xLora.ReceivedSize, 1 );// 0x13,最后一包数据负载字节数
       
        ReadForRegister( REG_LR_FIFORXCURRENTADDR, &regTemp ,1);        // 读取到的最后一个数据包，在FIFO中的地址起始位置（指针） 
        writeToRegister( REG_LR_FIFOADDRPTR, &regTemp ,1 );             // 设置FIFO中开始读数据的位置
        ReadForRegister(0, xLora.ReceivedBuf, xLora.ReceivedSize );     // 读取数据fifo， 从FIFO中指定起始位置，读取N字节到RxBuffer  
        xLora.ReceivedFlag=1;                                           // 把标志置1, 0_无数据，1_接收到新数据的),        
        
        //结果处理, 数据字节数: RxSize ; 数据缓冲区: RxBuffer        
        printf("lora 接收到 %d 个字节数据: %s\r\n",xLora.ReceivedSize,xLora.ReceivedBuf);    
    }
}

void LORA_DIO0_HANDLER2(void)
{
    // 发送完成中断 
    if(xLora2 .TxMode ==1)   
    {   
		printf("xLora2 .TxMode start!\r\n");				
        regTemp = 0xFF;                   // 清除寄存器中断标志：发送完成标志   
        writeToRegister2( 0x12, &regTemp , 1);                 
        xLora2.TxMode  =0;                 // 更新工作状态       
                 
        Lora_RxMode2();                    // 进入:持续接收模式  
        xLora2 .RxMode = 1 ;               // 更新工作状态
              
        return;                           // 返回
    }
    
    // 接收完数据包 ： 数据存入:RxBuffer, 字节数存入:RxSize 
    if(xLora2 .RxMode ==1)
    {    
		printf("xLora2 .RxMode start!\r\n");		        
        regTemp = 0xFF;
        writeToRegister2( 0x12, &regTemp ,1 );                           // 清除寄存器中断标志：接收到数据包            
     
        if( xLoraSettings2.ImplicitHeaderOn == true )                    // 隐藏头部格式 
            xLora2.ReceivedSize = xLoraSettings2.PayloadLength ;       
        else                                                            // 显式头部格式 
            ReadForRegister2( REG_LR_NBRXBYTES, &xLora2.ReceivedSize, 1 );// 0x13,最后一包数据负载字节数
       
        ReadForRegister2( REG_LR_FIFORXCURRENTADDR, &regTemp ,1);        // 读取到的最后一个数据包，在FIFO中的地址起始位置（指针） 
        writeToRegister2( REG_LR_FIFOADDRPTR, &regTemp ,1 );             // 设置FIFO中开始读数据的位置
        ReadForRegister2(0, xLora2.ReceivedBuf, xLora2.ReceivedSize );     // 读取数据fifo， 从FIFO中指定起始位置，读取N字节到RxBuffer  
        xLora2.ReceivedFlag=1;                                           // 把标志置1, 0_无数据，1_接收到新数据的),        
        
        //结果处理, 数据字节数: RxSize ; 数据缓冲区: RxBuffer        
        printf("lora2 接收到 %d 个字节数据: %s\r\n",xLora2.ReceivedSize,xLora2.ReceivedBuf);  
    }
}


