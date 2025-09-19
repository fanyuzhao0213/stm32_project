#ifndef MY_TASK_H
#define MY_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


/* ÈÎÎñ¾ä±ú */
extern osThreadId_t myMainTask1Handle;
extern osThreadId_t myMainTask2Handle;

extern osSemaphoreId_t keySemHandle;



/* ÊôĞÔ */
extern const osThreadAttr_t myMainTask1_attributes;
extern const osThreadAttr_t myMainTask2_attributes;
extern const osSemaphoreAttr_t keySem_attributes;

extern void MyMainTask1(void *argument);
extern void MyMainTask2(void *argument);
#endif

