#include "my_can.h"


// 定义 CAN 发送和接收的头结构体
CAN_TxHeaderTypeDef TxHeader;     // 发送头
CAN_RxHeaderTypeDef RxHeader;     // 接收头
uint8_t TxData[8];                // 发送数据缓冲区
uint8_t RxData[8];                // 接收数据缓冲区
uint32_t TxMailbox;               // 发送邮箱（CAN 发送缓冲区标识）


/*   
 字段说明：
 *     StdId  - 标准 ID（11 位）
 *     ExtId  - 扩展 ID（29 位，当 IDE = CAN_ID_EXT 时使用）
 *     IDE    - 标识符类型：
 *                  CAN_ID_STD  标准帧
 *                  CAN_ID_EXT  扩展帧
 *     RTR    - 帧类型：
 *                  CAN_RTR_DATA   数据帧
 *                  CAN_RTR_REMOTE 遥控帧
 *     DLC    - 数据长度 (0~8)
 *     Data[] - 数据内容
*/

uint8_t can_test_data_index = 0;
// 定义不同的测试帧（数据帧 + 遥控帧）
CAN_Frame_t can_test_frames[] =
{
    /* 数据帧示例（标准帧，ID=0x321，8字节数据） */
    {0x345, 0x00, CAN_ID_STD, CAN_RTR_DATA, 8, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}},
	/* 数据帧示例（标准帧，ID=0x123，8字节数据） */
	{0x123, 0x00, CAN_ID_STD, CAN_RTR_DATA, 8, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}},
	{0x666, 0x00, CAN_ID_STD, CAN_RTR_DATA, 8, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}},
	{0x777, 0x00, CAN_ID_STD, CAN_RTR_DATA, 8, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}},
	{0x888, 0x00, CAN_ID_STD, CAN_RTR_DATA, 8, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}},
	{0x321, 0x00, CAN_ID_STD, CAN_RTR_DATA, 8, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}},
    /* 遥控帧示例（标准帧，ID=0x456，无数据） */
    {0x456, 0x00, CAN_ID_STD, CAN_RTR_REMOTE, 0, {0}},
	{0x345, 0x00, CAN_ID_STD, CAN_RTR_REMOTE, 0, {0}},
    /* 数据帧示例（扩展帧，ExtID=0x1ABCDE，4字节数据） */
    {0x00,  0x1A0002, CAN_ID_EXT, CAN_RTR_DATA, 8, {0x12, 0x21, 0x33, 0x44,0x12, 0x21, 0x33, 0x44}},
	{0x00,  0x2ABCDE, CAN_ID_EXT, CAN_RTR_DATA, 8, {0x12, 0x21, 0x33, 0x44,0x12, 0x21, 0x33, 0x44}},
	{0x00,  0x1AACDE, CAN_ID_EXT, CAN_RTR_DATA, 8, {0x12, 0x21, 0x33, 0x44,0x12, 0x21, 0x33, 0x44}},
	{0x00,  0x1BBCDE, CAN_ID_EXT, CAN_RTR_DATA, 8, {0x12, 0x21, 0x33, 0x44,0x12, 0x21, 0x33, 0x44}},
	{0x00,  0x1A0001, CAN_ID_EXT, CAN_RTR_DATA, 8, {0x12, 0x21, 0x33, 0x44,0x12, 0x21, 0x33, 0x44}},
	{0x00,  0x4ABCDE, CAN_ID_EXT, CAN_RTR_DATA, 8, {0x12, 0x21, 0x33, 0x44,0x12, 0x21, 0x33, 0x44}},
	/* 遥控帧示例（扩展帧，ExtID=0x1ABCDE） */
	{0x00,  0x2ABCDE, CAN_ID_EXT, CAN_RTR_REMOTE, 8, {0xAB, 0xCD, 0x33, 0x44,0xAB, 0xCD, 0x33, 0x44}},
	{0x00,  0x1A0000, CAN_ID_EXT, CAN_RTR_REMOTE, 8, {0xAB, 0xCD, 0x33, 0x44,0xAB, 0xCD, 0x33, 0x44}},
	{0x00,  0x1A0001, CAN_ID_EXT, CAN_RTR_REMOTE, 8, {0xAB, 0xCD, 0x33, 0x44,0xAB, 0xCD, 0x33, 0x44}},
	{0x00,  0x1A9999, CAN_ID_EXT, CAN_RTR_REMOTE, 8, {0xAB, 0xCD, 0x33, 0x44,0xAB, 0xCD, 0x33, 0x44}},
};


/**
 * @brief  配置 CAN 过滤器
 * @note   这里配置为接收所有 ID 的数据
 */
void CAN_Filter_Config(void)
{
    CAN_FilterTypeDef canFilter;

    canFilter.FilterActivation = ENABLE;              // 使能过滤器
    canFilter.FilterFIFOAssignment = CAN_RX_FIFO0;     // 绑定到 FIFO0
	
#ifdef CAN_FILTER_ALL
    // ================================
    // 接收所有 CAN ID（无过滤）
    // ================================
    // 过滤器编号为 0（STM32 的 CAN 可以有多个过滤器组）
    canFilter.FilterBank = 0;
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;     // 32 位过滤器
    // 使用掩码模式（IDMASK），但掩码为 0，表示不屏蔽任何 ID
    canFilter.FilterMode = CAN_FILTERMODE_IDMASK;

    // 标准帧 ID 高低位都置 0
    canFilter.FilterIdHigh = 0x0000;
    canFilter.FilterIdLow  = 0x0000;

    // 掩码全为 0，意味着不屏蔽任何位（接受所有 ID）
    canFilter.FilterMaskIdHigh = 0x0000;
    canFilter.FilterMaskIdLow  = 0x0000;

    HAL_CAN_ConfigFilter(&hcan, &canFilter);

#elif defined(CAN_FILTER_STD_LIST)
    // ================================
    // 标准帧列表模式，接收多个特定 ID
	// 列表模式默认只匹配数据帧，如果要接收 Remote Frame，需要单独用掩码模式配置
    // ================================
	#if 0
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;     // 32 位过滤器
	canFilter.FilterBank = 0;                // 每个 ID 用一个过滤器
	canFilter.FilterMode = CAN_FILTERMODE_IDLIST; // 列表模式
	// 标准帧 ID 左移 5 位存入高位寄存器
	// STM32 标准帧过滤器格式：FilterIdHigh = StdId[10:5] << 5
	canFilter.FilterIdHigh = 0x345 << 5;
	canFilter.FilterIdLow  = 0;
	canFilter.FilterMaskIdHigh = 0x123 << 5;
	canFilter.FilterMaskIdLow  = 0x0000;
	HAL_CAN_ConfigFilter(&hcan, &canFilter);
	#else
	/*0过滤器*/
	canFilter.FilterScale = CAN_FILTERSCALE_16BIT;     // 16 位过滤器
	canFilter.FilterBank = 0;                // 每个 ID 用一个过滤器
	canFilter.FilterMode = CAN_FILTERMODE_IDLIST; // 列表模式
	// 标准帧 ID 左移 5 位存入高位寄存器
	// STM32 标准帧过滤器格式：FilterIdHigh = StdId[10:5] << 5
	canFilter.FilterIdHigh = 0x345 << 5;
	canFilter.FilterIdLow  = 0x321 << 5;
	canFilter.FilterMaskIdHigh = 0x123 << 5;
	canFilter.FilterMaskIdLow  = 0x888 << 5;
	HAL_CAN_ConfigFilter(&hcan, &canFilter);
	
	/*1过滤器*/
	canFilter.FilterScale = CAN_FILTERSCALE_16BIT;     // 16 位过滤器
	canFilter.FilterBank = 1;                // 每个 ID 用一个过滤器
	canFilter.FilterMode = CAN_FILTERMODE_IDLIST; // 列表模式
	// 标准帧 ID 左移 5 位存入高位寄存器
	// STM32 标准帧过滤器格式：FilterIdHigh = StdId[10:5] << 5
	canFilter.FilterIdHigh = 0x345 << 5;
	canFilter.FilterIdLow  = 0x777 << 5;
	canFilter.FilterMaskIdHigh = 0x666 << 5;
	canFilter.FilterMaskIdLow  = 0x888 << 5;
	HAL_CAN_ConfigFilter(&hcan, &canFilter);
	#endif

		
#elif defined(CAN_FILTER_STD_MASK)
    // ================================
    // 标准帧掩码模式（接收一定范围的 ID）
	/*
		Bit 0-1   : reserved
		Bit 2     : RTR (0=数据帧, 1=遥控帧)
		Bit 3     : IDE (0=标准帧, 1=扩展帧)
	*/
    // ================================
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;     // 32 位过滤器
		
    canFilter.FilterBank = 0;
    canFilter.FilterMode = CAN_FILTERMODE_IDMASK; // 掩码模式

    // 起始 ID（0x200），左移 5 位符合 STM32 标准帧 ID 存储方式
    canFilter.FilterIdHigh = 0x300 << 5;
//    canFilter.FilterIdLow  = CAN_RTR_REMOTE;
	canFilter.FilterIdLow  = 0;
	/*
		掩码设置需要将高3位写1，0x700 高三位就是1
	*/
    // 掩码设置为 0x700 左移 5 位
    // 掩码位为 1 的位置表示忽略，对应 0 的位才比较
    // 所以实际接收 ID 范围：0x200 ~ 0x2FF
    canFilter.FilterMaskIdHigh = 0x700 << 5;
//    canFilter.FilterMaskIdLow  = CAN_RTR_REMOTE;
	canFilter.FilterIdLow  = 0;
    HAL_CAN_ConfigFilter(&hcan, &canFilter);

#elif defined(CAN_FILTER_EXT_LIST)
    // ================================
    // 扩展帧列表模式，接收多个特定 29 位 ID
    // ================================
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;     // 32 位过滤器
	canFilter.FilterBank = 0;
	canFilter.FilterMode = CAN_FILTERMODE_IDLIST; // 列表模式
	// 高 16 位存入 FilterIdHigh（取 ID[28:13]）
	canFilter.FilterIdHigh = (0x1ABCDE >> 13) & 0xFFFF;
	// 低 16 位存入 FilterIdLow（取 ID[12:0] << 3 并标记为扩展帧）
	canFilter.FilterIdLow  = ((0x1ABCDE << 3) & 0xFFF8) | CAN_ID_EXT;
	canFilter.FilterMaskIdHigh = (0x3ABCDE >> 13) & 0xFFFF;
	canFilter.FilterMaskIdLow  = ((0x3ABCDE << 3) & 0xFFF8) | CAN_ID_EXT;
	HAL_CAN_ConfigFilter(&hcan, &canFilter);
	
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;     // 32 位过滤器
	canFilter.FilterBank = 1;
	canFilter.FilterMode = CAN_FILTERMODE_IDLIST; // 列表模式
	// 高 16 位存入 FilterIdHigh（取 ID[28:13]）
	canFilter.FilterIdHigh = (0x2ABCDE >> 13) & 0xFFFF;
	canFilter.FilterIdLow  = ((0x2ABCDE << 3) & 0xFFF8) | CAN_ID_EXT | CAN_RTR_REMOTE;
	HAL_CAN_ConfigFilter(&hcan, &canFilter);
	

#elif defined(CAN_FILTER_EXT_MASK)
    // ================================
    // 扩展帧掩码模式，接收一定范围的 29 位 ID
    // ================================
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;     // 32 位过滤器
	
    canFilter.FilterBank = 0;
    canFilter.FilterMode = CAN_FILTERMODE_IDMASK; // 掩码模式

	// 设置要匹配的 ID：
	// - 高 16 位 = 0x001A（左移 3 位后 = 0x00D0）
	// - 低 16 位不关心（允许任意 xxxx）
	// - 设置 IDE 位（扩展帧）
	uint32_t ext_id_high = 0x001A << (16 + 3);  // 0x001A0000 << 3 = 0x00D00000
	canFilter.FilterIdHigh = (ext_id_high >> 16) & 0xFFFF;  // 高 16 位 = 0x00D0
//	canFilter.FilterIdLow = (ext_id_high & 0xFFFF) | CAN_ID_EXT;  // 低 16 位 + IDE 位
	canFilter.FilterIdLow = (ext_id_high & 0xFFFF) | CAN_ID_EXT | CAN_RTR_DATA;  // 低16位 + IDE + RTR_DATA
	// 设置掩码：
	// - 高 16 位必须匹配 0x00D0（即原始高 16 位 = 0x001A）
	// - 低 16 位不关心（允许任意 xxxx）
	// - 强制数据帧（过滤 RTR 帧）
	// 掩码：高16位必须匹配，低16位忽略 + IDE必须=1 + RTR必须=0
	
	/*过滤遥控帧*/
//	uint32_t mask = (0xFFFF0000 << 3) | CAN_ID_EXT | CAN_RTR_REMOTE;  
	uint32_t mask = (0xFFFF0000 << 3) | CAN_ID_EXT;  
	

	canFilter.FilterMaskIdHigh = (mask >> 16) & 0xFFFF;  // 严格匹配高 16 位
	canFilter.FilterMaskIdLow =  mask & 0xFFFF;   // 低 16 位不关心（允许任意 xxxx）

	HAL_CAN_ConfigFilter(&hcan, &canFilter);

#else
    // ================================
    // 默认接收所有 ID（和 CAN_FILTER_ALL 相同）
    // ================================
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;     // 32 位过滤器
	
    canFilter.FilterBank = 0;
    canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
    canFilter.FilterIdHigh = 0x0000;
    canFilter.FilterIdLow  = 0x0000;
    canFilter.FilterMaskIdHigh = 0x0000;
    canFilter.FilterMaskIdLow  = 0x0000;
    HAL_CAN_ConfigFilter(&hcan, &canFilter);
#endif


//    // 应用过滤器配置
//    HAL_CAN_ConfigFilter(&hcan, &canFilter);
}

/**
 * @brief  初始化 CAN 为自发自收（回环）模式
 * @note   回环模式下发送的数据会直接进入接收 FIFO
 */
void CAN_Loopback_Test_Init(void)
{
    hcan.Init.Mode = CAN_MODE_LOOPBACK;   // 设置为回环模式
    if (HAL_CAN_Init(&hcan) != HAL_OK)
    {
        Error_Handler();                   // 初始化失败
    }

    CAN_Filter_Config();                   // 配置过滤器（接收所有 ID）

    if (HAL_CAN_Start(&hcan) != HAL_OK)    // 启动 CAN 外设
    {
        Error_Handler();
    }

    // 使能接收 FIFO0 消息挂起中断
    HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}


/**
 * @brief  发送结构体数组中的 CAN 帧
 */
void CAN_Send_Test_Frames(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8];
    uint32_t txMailbox;

	txHeader.StdId = can_test_frames[can_test_data_index].StdId;
	txHeader.ExtId = can_test_frames[can_test_data_index].ExtId;
	txHeader.IDE   = can_test_frames[can_test_data_index].IDE;
	txHeader.RTR   = can_test_frames[can_test_data_index].RTR;
	txHeader.DLC   = can_test_frames[can_test_data_index].DLC;
	txHeader.TransmitGlobalTime = DISABLE;

	if (txHeader.RTR == CAN_RTR_DATA)
	{
		memcpy(txData, can_test_frames[can_test_data_index].Data,
			   can_test_frames[can_test_data_index].DLC);
	}
	else
	{
		// 遥控帧不需要数据，保证 buffer 清零即可
		memset(txData, 0, sizeof(txData));
	}
	
	if (HAL_CAN_AddTxMessage(&hcan, &txHeader, txData, &txMailbox) != HAL_OK)
	{
		Error_Handler();
	}

//	printf("CAN Frame %d sent! ID: 0x%X\r\n", can_test_data_index, 
//			   (txHeader.IDE == CAN_ID_STD) ? txHeader.StdId : txHeader.ExtId);
	
	can_test_data_index ++;
	can_test_data_index = can_test_data_index % CAN_TEST_FRAME_COUNT;
}



/**
标准帧最大 0x7FF（11 位）
扩展帧最大 0x1FFFFFFF（29 位）
 * @brief  发送一帧 CAN 消息（支持标准帧、扩展帧、数据帧、遥控帧）
 * @param  ide     CAN_ID_STD 或 CAN_ID_EXT
 * @param  id      标准 ID（11 位）或扩展 ID（29 位）
 * @param  rtr     CAN_RTR_DATA（数据帧）或 CAN_RTR_REMOTE（遥控帧）
 * @param  data    数据缓冲区指针（遥控帧可传 NULL）
 * @param  len     数据长度（0~8）
 * @retval HAL 状态
 */
HAL_StatusTypeDef CAN_Send_Message(uint32_t ide, uint32_t id, uint32_t rtr, uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef tx_header;
    uint32_t tx_mailbox;

    if (len > 8) len = 8;  // CAN 最大 8 字节

    tx_header.DLC = len;
    tx_header.IDE = ide;   // CAN_ID_STD 或 CAN_ID_EXT
    tx_header.RTR = rtr;   // CAN_RTR_DATA 或 CAN_RTR_REMOTE
    tx_header.TransmitGlobalTime = DISABLE; // 不发送时间戳

    // 区分标准帧和扩展帧
    if (ide == CAN_ID_STD)
    {
        tx_header.StdId = (uint16_t)(id & 0x7FF);
    }
    else
    {
        tx_header.ExtId = (uint32_t)(id & 0x1FFFFFFF);
    }

    return HAL_CAN_AddTxMessage(&hcan, &tx_header, data, &tx_mailbox);
}


/**
 * @brief  接收中断回调函数
 * @param  hcan CAN 句柄
 * @note   当 FIFO0 有数据时自动调用
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8] = {0};

    // 读取 FIFO0 中的数据
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK)
    {
        Error_Handler();
    }

    // 打印接收到的 ID
    if (rxHeader.IDE == CAN_ID_STD)
        printf("CAN message received: StdID=0x%03X DLC=%d", rxHeader.StdId, rxHeader.DLC);
    else
        printf("CAN message received: ExtID=0x%08X DLC=%d", rxHeader.ExtId, rxHeader.DLC);

	// 打印数据
	if (rxHeader.RTR == CAN_RTR_DATA)
	{
		printf("Data=");
		for (uint8_t i = 0; i < rxHeader.DLC; i++)
		{
			printf("%02X ", rxData[i]);
		}
	}
    printf("\r\n");

    // OLED 显示 ID 和类型（第 8 行）
    char oledBuf[32];
    snprintf(oledBuf, sizeof(oledBuf), "ID: %s %06X", 
             (rxHeader.IDE == CAN_ID_STD) ? "STD" : "EXT", 
             (rxHeader.IDE == CAN_ID_STD) ? rxHeader.StdId : rxHeader.ExtId);
    OLED_ShowString(2, 16, oledBuf, OLED_8X16);

    // OLED 显示数据长度（DLC，第 16 行）
    snprintf(oledBuf, sizeof(oledBuf), "Len:%d", rxHeader.DLC);
    OLED_ShowString(2, 32, oledBuf, OLED_8X16);

	// OLED 显示数据内容（第 4 行开始，每行固定显示 8 个字节，不足补 0）
	OLED_Show_CAN_Data_Str(48, rxData, rxHeader.DLC);
    OLED_Update();
}

/**
 * @brief  根据 DLC 和数据生成 OLED 显示字符串
 * @param  row_start: OLED 起始行（像素坐标）
 * @param  rxData: 数据数组
 * @param  dlc: 数据长度（1~8）
 */
void OLED_Show_CAN_Data_Str(uint8_t row_start, uint8_t *rxData, uint8_t dlc)
{
    char oledBuf[64]; // 足够容纳格式化字符串
    char *ptr = oledBuf;
    
    if (dlc > 8) dlc = 8; // 限制最大 8 字节

    // 前置补零
    for (uint8_t i = 0; i < 8 - dlc; i++)
    {
        ptr += snprintf(ptr, sizeof(oledBuf) - (ptr - oledBuf), "00");
    }

    // 输出实际数据
    for (uint8_t i = 0; i < dlc; i++)
    {
        if (i == dlc - 1) // 最后一个字节不加 &
            snprintf(ptr, sizeof(oledBuf) - (ptr - oledBuf), "%02X", rxData[i]);
        else
            ptr += snprintf(ptr, sizeof(oledBuf) - (ptr - oledBuf), "%02X", rxData[i]);
    }

    // 显示到 OLED
    OLED_ShowString(0, row_start, oledBuf, OLED_8X16);
}
