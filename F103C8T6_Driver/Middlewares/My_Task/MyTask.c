#include "MyTask.h"


/*
	osThreadAttr_t.stack_size 的单位是 字节（Byte），不是字（word）。
	在 FreeRTOS 原生 API 里，xTaskCreate() 里传的栈大小是以 word 为单位（不是字节）。
	比如：
	xTaskCreate(vTaskCode, "task", 128, NULL, 2, NULL);
	这里的 128 实际上 = 128 * 4 = 512 字节。
*/


/*
	信号量相关定义
*/
osSemaphoreId_t keySemHandle;
const osSemaphoreAttr_t keySem_attributes = {
  .name = "keySem"
};



/* 任务句柄 */
osThreadId_t myMainTask1Handle;
osThreadId_t myMainTask2Handle;

/* 属性 */
const osThreadAttr_t myMainTask1_attributes = {
	.name       = "myTask1",
	.stack_size = 128 * 4,
	.priority   = (osPriority_t) osPriorityNormal1,
};
 
const osThreadAttr_t myMainTask2_attributes = {
	.name       = "myTask2",
	.stack_size = 128 * 4,
	.priority   = (osPriority_t) osPriorityNormal2,
};

  


/* 动态任务函数 */
void MyMainTask1(void *argument)
{
	printf("MyMainTask1 Start!\r\n");
	
	for(;;)
	{
		
		osDelay(500);
		// 等待按键信号量
        if (osSemaphoreAcquire(keySemHandle, osWaitForever) == osOK)
        {
            // 收到按键事件
			printf("KEY_1 Press Detected! \r\n");
			LED1_TOGGLE();; // 翻转 LED
			osDelay(500);
			LED1_TOGGLE();; // 翻转 LED
			/*变换测试数据，便于观察实验现象*/
			/*实际项目中，可以将待发送的数据赋值给NRF24L01_TxPacket数组*/
			NRF24L01_TxPacket[0] ++;
			NRF24L01_TxPacket[1] ++;
			NRF24L01_TxPacket[2] ++;
			NRF24L01_TxPacket[3] ++;
//			NRF24L01_TxPacket[31] = 0xEE;
//			NRF24L01_TxPacket[31] = 0xEE;
			/*调用NRF24L01_Send函数，发送数据，同时返回发送标志位，方便用户了解发送状态*/
			/*发送标志位与发送状态的对应关系，可以转到此函数定义上方查看*/
			SendFlag = NRF24L01_Send();
			printf("SendFlag:%d!\r\n",SendFlag);
			if (SendFlag == 1)			//发送标志位为1，表示发送成功
			{
				SendSuccessCount ++;	//发送成功计次变量自增
			}
			else						//发送标志位不为1，即2/3/4，表示发送不成功
			{
				SendFailedCount ++;		//发送失败计次变量自增
			}
			
			printf("SendSuccessCount：%d,SendFailedCount:%d!\r\n",SendSuccessCount,SendFailedCount);
            // TODO: 在这里处理按键功能
        }
	}
}

/* 动态任务函数 */
void MyMainTask2(void *argument)
{
	uint8_t i = 0;
	printf("MyMainTask2 Start!\r\n");
	for(;;)
	{
		/*主循环内循环执行NRF24L01_Receive函数，接收数据，同时返回接收标志位，方便用户了解接收状态*/
		/*接收标志位与接收状态的对应关系，可以转到此函数定义上方查看*/
		ReceiveFlag = NRF24L01_Receive();
		
		if (ReceiveFlag)				//接收标志位不为0，表示收到了一个数据包
		{
			if (ReceiveFlag == 1)		//接收标志位为1，表示接收成功
			{
				ReceiveSuccessCount ++;	//接收成功计次变量自增
			}
			else	//接收标志位不为0也不为1，即2/3，表示此次接收产生了错误，错误接收的数据不应该使用
			{
				ReceiveFailedCount ++;	//接收失败计次变量自增
			}
			
			printf("ReceiveSuccessCount%d,ReceiveFailedCount:%d!\r\n",ReceiveSuccessCount,ReceiveFailedCount);

			printf("Read NRF24L01 Data: \r\n");
			for (i = 0; i < 4; i++) printf("%02X ", NRF24L01_RxPacket[i]);
			printf("\r\n");
			
		}
		
		LED2_TOGGLE();; // 翻转 LED
		osDelay(500);
	}
}




