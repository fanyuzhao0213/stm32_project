#ifndef __PID_H
#define __PID_H
#include "stm32f1xx_hal.h"

/**
 * @brief PID �������ṹ��
 */
typedef struct {
    float pid_target;           // Ŀ��ֵ
    float pid_actual;           // ʵ��ֵ
    float pid_output;           // ���ֵ

    float pid_err_now;          // ��ǰ��� e(k)
    float pid_err_last;         // ��һ����� e(k-1)
    float pid_err_sum;          // ������ ��e(k)

    float pid_err_int_limit;    // ������������ֵ�������ֱ��ͣ�0 ��ʾ�����ƣ�

    float pid_kp;               // ����ϵ��
    float pid_ki;               // ����ϵ��
    float pid_kd;               // ΢��ϵ��

    float pid_output_max;       // ������ֵ
    float pid_output_min;       // �����Сֵ
} PID_t;


/**
 * @brief ��� PID �������ڲ�״̬
 * @param pid PID ����������ָ��
 */
void PID_Reset(PID_t *pid);

/**
 * @brief ���� PID ���������
 * @param pid PID ����������ָ��
 */
void PID_Compute(PID_t *pid, uint8_t OutInLimitationFlag);

#endif
