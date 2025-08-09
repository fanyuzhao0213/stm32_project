#ifndef SYSTERM_HARDWARE_TEST_H
#define SYSTERM_HARDWARE_TEST_H

#include "main.h"

/* �������� */
void KeyTest_Init(void);
void KeyTest_Loop(void);

/* ��λ������ */
void RPTest_Init(void);
void RPTest_Loop(void);

/* ������� */
void PWMTest_Init(void);
void PWMTest_Loop(void);
void PWMTest_Exit(void);

/* ���������� */
void EncoderTest_Init(void);
void EncoderTest_Loop(void);

/* �Ƕȴ��������� */
void AngleTest_Init(void);
void AngleTest_Loop(void);

#endif

