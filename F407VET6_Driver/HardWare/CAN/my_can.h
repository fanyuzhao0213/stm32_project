#ifndef MY_CAN_H
#define MY_CAN_H

#include "main.h"
#include "can.h"
#include "stdio.h"
#include "string.h"

/*
	测试宏定义
*/
// ==============================================
// CAN 过滤器配置宏定义
// 用于选择不同的测试模式和过滤器配置
// ==============================================

// ------------------------
// 接收所有 CAN ID
// ------------------------
//#define CAN_FILTER_ALL       
// 测试场景：
// 1. 测试 CAN 收发功能是否正常。
// 2. 无论标准帧还是扩展帧，都能接收。
// 3. 用于调试或初始阶段快速验证总线数据。

// ------------------------
// 标准帧列表模式（IDLIST）
// ------------------------
//#define CAN_FILTER_STD_LIST  
// 测试场景：
// 1. 接收多个指定的标准帧 ID，例如 0x123, 0x234, 0x345。
// 2. 用于功能验证：只接收特定消息，不关心总线上其它 ID。
// 3. 常用于模块间固定 ID 通信测试。

// ------------------------
// 标准帧掩码模式（IDMASK）
// ------------------------
//#define CAN_FILTER_STD_MASK  
// 测试场景：
// 1. 接收一定范围的标准帧 ID，例如 0x200~0x2FF。
// 2. 用掩码灵活匹配多个 ID，适合批量接收同类型数据。
// 3. 用于总线压力测试或某类数据帧筛选。

// ------------------------
// 扩展帧列表模式（IDLIST）
// ------------------------
//#define CAN_FILTER_EXT_LIST  
// 测试场景：
// 1. 接收多个特定 29 位扩展帧 ID，例如 0x12345678, 0x0789ABCD。
// 2. 用于模块间扩展帧通信测试。
// 3. 验证扩展帧的解析、发送与接收功能。

// ------------------------
// 扩展帧掩码模式（IDMASK）
// ------------------------
#define CAN_FILTER_EXT_MASK  
// 测试场景：
// 1. 接收一定范围的扩展帧 ID，例如 0x12345600 ~ 0x123456FF。
// 2. 用掩码匹配一类扩展帧，适合批量或分类接收。
// 3. 用于压力测试或筛选扩展帧消息。



// 计算帧数量
#define CAN_TEST_FRAME_COUNT (sizeof(can_test_frames) / sizeof(CAN_Frame_t))
extern uint8_t can_test_data_index;
// 定义 CAN 数据帧结构体
typedef struct
{
    uint32_t StdId;     // 标准 ID
    uint32_t ExtId;     // 扩展 ID（如果 IDE=CAN_ID_EXT 时使用）
    uint8_t  IDE;       // 标识符类型（CAN_ID_STD 或 CAN_ID_EXT）
    uint8_t  RTR;       // 帧类型（CAN_RTR_DATA 或 CAN_RTR_REMOTE）
    uint8_t  DLC;       // 数据长度（0~8）
    uint8_t  Data[8];   // 数据内容
} CAN_Frame_t;

extern CAN_Frame_t can_test_frames[];
void CAN_Loopback_Test_Init(void);
HAL_StatusTypeDef CAN_Send_Message(uint32_t ide, uint32_t id, uint32_t rtr, uint8_t *data, uint8_t len);
void CAN_Send_Test_Frames(void);
void OLED_Show_CAN_Data_Str(uint8_t row_start, uint8_t *rxData, uint8_t dlc);
#endif

