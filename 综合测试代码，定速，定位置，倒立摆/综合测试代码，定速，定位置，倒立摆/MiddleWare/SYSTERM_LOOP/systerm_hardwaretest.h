#ifndef SYSTERM_HARDWARE_TEST_H
#define SYSTERM_HARDWARE_TEST_H

#include "main.h"

/* 按键测试 */
void KeyTest_Init(void);
void KeyTest_Loop(void);

/* 电位器测试 */
void RPTest_Init(void);
void RPTest_Loop(void);

/* 电机测试 */
void PWMTest_Init(void);
void PWMTest_Loop(void);
void PWMTest_Exit(void);

/* 编码器测试 */
void EncoderTest_Init(void);
void EncoderTest_Loop(void);

/* 角度传感器测试 */
void AngleTest_Init(void);
void AngleTest_Loop(void);

#endif

