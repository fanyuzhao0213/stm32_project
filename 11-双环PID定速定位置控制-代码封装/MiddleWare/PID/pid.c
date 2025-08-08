#include "pid.h"


/**
 * @brief  ��ʼ�� PID ������
 * @param  pid: PID �������ṹ��ָ��
 * @param  kp, ki, kd: PID ϵ��
 * @param  outMax: �������
 * @param  outMin: �������
 * @retval None
 */
void PID_Init(PID_t *pid, float kp, float ki, float kd, float outMax, float outMin)
{
    pid->PID_Kp = kp;
    pid->PID_Ki = ki;
    pid->PID_Kd = kd;
    pid->PID_OutMax = outMax;
    pid->PID_OutMin = outMin;

    // ��ʼ������״̬����
    pid->PID_Target = 0;
    pid->PID_Actual = 0;
    pid->PID_PwmOut = 0;
    pid->PID_Error = 0;
    pid->PID_LastError = 0;
    pid->PID_ErrorSum = 0;
}


/**
 * @brief  ���� PID ״̬�����֡�������㣩
 * @param  pid: PID �������ṹ��ָ��
 * @retval None
 */
void PID_Reset(PID_t *pid)
{
    pid->PID_PwmOut = 0;
    pid->PID_Error = 0;
    pid->PID_LastError = 0;
    pid->PID_ErrorSum = 0;
}

/**
 * @brief  ����ʱ�޸� PID ����
 * @param  pid: PID �������ṹ��ָ��
 * @param  kp, ki, kd: �µ� PID ϵ��
 * @retval None
 */
void PID_SetTunings(PID_t *pid, float kp, float ki, float kd)
{
    pid->PID_Kp = kp;
    pid->PID_Ki = ki;
    pid->PID_Kd = kd;
}


/**
 * @brief  ���� PID ���㣨λ��ʽ PID + ���ַ����� + ΢�ֵ�ͨ�˲���
 * @param  pid: PID �������ṹ��ָ��
 * @retval None
 */
void PID_Update(PID_t *pid)
{
    // 1. ������һ�ε�������΢������㣩
    pid->PID_LastError = pid->PID_Error;

    // 2. ���㱾����� = Ŀ��ֵ - ʵ��ֵ
    pid->PID_Error = pid->PID_Target - pid->PID_Actual;

    // 3. ���ּ��㣨Anti-windup ���ַ����ͣ�
    //    - �����δ�ﵽ������ʱ����������ۻ�
    //    - ��������������ڱ���ʱ�����ۼӣ����¿�������������
  // 3. ���ּ��㣨���ж��Ƿ�������֣�
    if (pid->PID_Ki != 0)
    {
        // �á�Ԥ��������ж��Ƿ��������
        float predict_output = pid->PID_Kp * pid->PID_Error
                             + pid->PID_Ki * pid->PID_ErrorSum
                             + pid->PID_Kd * (pid->PID_Error - pid->PID_LastError);

        if (!((predict_output >= pid->PID_OutMax && pid->PID_Error > 0) ||
              (predict_output <= pid->PID_OutMin && pid->PID_Error < 0)))
        {
            pid->PID_ErrorSum += pid->PID_Error;

            // �����޷�
            if (pid->PID_ErrorSum > ITERM_MAX) pid->PID_ErrorSum = ITERM_MAX;
            if (pid->PID_ErrorSum < ITERM_MIN) pid->PID_ErrorSum = ITERM_MIN;
        }
    }
    else
    {
        pid->PID_ErrorSum = 0;
    }

    // 4. ΢������㣨����ͨ�˲�����
    //    - ��ͳ D �e(k) - e(k-1)
    //    - ��ͨ�˲���ʽ��D_filt = �� * D_last + (1 - ��) * D_raw
    //    - �� ȡֵ 0.0 ~ 1.0��Խ�ӽ� 1.0 �˲�Խƽ�����ӳ�ҲԽ��
    static float d_last = 0;   // ��һ�ε�΢��ֵ
    float d_raw = pid->PID_Error - pid->PID_LastError;  // ԭʼ΢��
    float alpha = 0.8f;        // ��ͨ�˲�ϵ�����ɵ���
    float d_filt = alpha * d_last + (1.0f - alpha) * d_raw;
    d_last = d_filt;           // �����˲����΢��ֵ

    // 5. PID λ��ʽ���㹫ʽ
    pid->PID_PwmOut = pid->PID_Kp * pid->PID_Error
                    + pid->PID_Ki * pid->PID_ErrorSum
                    + pid->PID_Kd * d_filt;

    // 6. ����޷������� PWM �����Χ��
    if (pid->PID_PwmOut > pid->PID_OutMax) pid->PID_PwmOut = pid->PID_OutMax;
    if (pid->PID_PwmOut < pid->PID_OutMin) pid->PID_PwmOut = pid->PID_OutMin;
}


/**
 * @brief  ���� PID ���㣨λ�û� + �ٶȻ���
 * @param  cpid: ���� PID �ṹ��ָ��
 * @retval None
 */
void CascadePID_Update(CascadePID_t *cpid)
{
    // --- �⻷��λ�û��� ---
    PID_Update(&cpid->PositionPID);
	
	// ���룺Ŀ��λ�á�ʵ��λ��
    // ������ٶ�Ŀ��ֵ����Ϊ�ڻ����룩
    cpid->SpeedPID.PID_Target = cpid->PositionPID.PID_PwmOut;

    // --- �ڻ����ٶȻ��� ---
    // ���룺�ٶ�Ŀ��ֵ��ʵ���ٶ�
    // �����PWM ռ�ձ�
    PID_Update(&cpid->SpeedPID);
}
