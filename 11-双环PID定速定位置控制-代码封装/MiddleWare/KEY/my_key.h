#ifndef MY_KEY_H_
#define MY_KEY_H_

#include "stm32f1xx_hal.h"

/// @brief ����״̬����
#define KEY_PRESSED             1   // �������£���Ч��
#define KEY_UNPRESSED           0   // ����δ����

/// @brief ����ʱ����ֵ����λ�����룩
#define KEY_TIME_DOUBLE         200     // ˫�������ʱ��
#define KEY_TIME_LONG           2000    // �����ж�ʱ��
#define KEY_TIME_REPEAT         100     // �����������ʱ��

/// @brief ��������
#define KEY_COUNT               4       // ��ǰ֧�ֵİ�������

/// @brief ������Ŷ���
#define KEY_1                   0
#define KEY_2                   1
#define KEY_3                   2
#define KEY_4                   3

/// @brief �����¼���־λ������ϣ�
#define KEY_HOLD                0x01    // ����������ס״̬�����֣�
#define KEY_DOWN                0x02    // �ոձ�����
#define KEY_UP                  0x04    // �ոձ��ͷ�
#define KEY_SINGLE              0x08    // �����¼�
#define KEY_DOUBLE              0x10    // ˫���¼�
#define KEY_LONG                0x20    // �����¼�
#define KEY_REPEAT              0x40    // ���������¼������ֲ���ʱ�ظ�������


/**
 * @brief ������ʼ�����������ð������ź��ж�
 */
void Key_Init(void);
/**
 * @brief ���ĳ��������״̬�����¼���־λ��
 * 
 * @param n     ������ţ�0 ~ KEY_COUNT-1��
 * @param Flag  Ҫ�����¼����ͣ��� KEY_SINGLE��KEY_LONG �ȣ�
 * @return uint8_t ����1��ʾ�¼�������0��ʾδ����
 */
uint8_t Key_Check(uint8_t n, uint8_t Flag);

void Key_Tick(void);
void my_key_test_func(void);
uint8_t my_get_key_num(void);
#endif  // MY_KEY_H_
