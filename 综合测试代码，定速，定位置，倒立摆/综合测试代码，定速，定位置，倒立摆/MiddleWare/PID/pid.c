#include "pid.h"
#include <math.h>
#include <stddef.h>  // for NULL
#include "main.h"

/**
 * @brief ��� PID �������ڲ�״̬�������ͻ�������
 * @param pid PID �������ṹ��ָ��
 */
void PID_Reset(PID_t *pid)
{
    if (pid == NULL) return;

    pid->pid_err_now = 0.0f;
    pid->pid_err_last = 0.0f;
    pid->pid_err_sum = 0.0f;
}

/**
 * @brief ����Ŀ��ֵ��ʵ��ֵ���� PID ���
 *        ����ǰӦ������ pid->pid_target �� pid->pid_actual
 * @param pid PID �������ṹ��ָ��
 */
void PID_Compute(PID_t *pid, uint8_t OutInLimitationFlag)
{
    if (pid == NULL) return;

    // ��¼��һ�����
    pid->pid_err_last = pid->pid_err_now;

    // ��ǰ���
    pid->pid_err_now = pid->pid_target - pid->pid_actual;

	if(OutInLimitationFlag == 1)			//��������޷���־
	{
		/*��������*/
		if (fabs(pid->pid_err_now) < 5)		//���������ֵ�Ƚ�С
		{
			pid->pid_output = 0;				//��ֱ�����0�������е���
		}
		else
		{
			// ���ֲ��ִ���
			if (pid->pid_ki != 0.0f) {
				if (pid->pid_err_int_limit == 0.0f) {
					// �����ƻ����ۼ�
					pid->pid_err_sum += pid->pid_err_now;
				}
				else if (fabsf(pid->pid_err_now) < pid->pid_err_int_limit) {
					// �������ֵ��Χ�ڲŻ���
					pid->pid_err_sum += pid->pid_err_now;
				}
				else {
					// ������ֵ���������㣬������ֱ���
					pid->pid_err_sum = 0.0f;
				}
			} else {
				// ����ϵ��Ϊ0����������
				pid->pid_err_sum = 0.0f;
			}

			// PID ���㹫ʽ
			pid->pid_output = pid->pid_kp * pid->pid_err_now
							+ pid->pid_ki * pid->pid_err_sum
							+ pid->pid_kd * (pid->pid_err_now - pid->pid_err_last);
			
			
			/*���ƫ��*/
			if (pid->pid_output > 0.1)		//������Ϊ��
			{
				pid->pid_output += 6;		//��ֱ�Ӹ����ֵ����һ���̶�ƫ��
			}
			else if (pid->pid_output < -0.1)	//������Ϊ��
			{
				pid->pid_output -= 6;		//��ֱ�Ӹ����ֵ��ȥһ���̶�ƫ��
			}
			else				//���Ϊ0
			{
				pid->pid_output = 0;		//���0
			}
		}
	}
	else
	{
		// ���ֲ��ִ���
		if (pid->pid_ki != 0.0f) {
			if (pid->pid_err_int_limit == 0.0f) {
				// �����ƻ����ۼ�
				pid->pid_err_sum += pid->pid_err_now;
			}
			else if (fabsf(pid->pid_err_now) < pid->pid_err_int_limit) {
				// �������ֵ��Χ�ڲŻ���
				pid->pid_err_sum += pid->pid_err_now;
			}
			else {
				// ������ֵ���������㣬������ֱ���
				pid->pid_err_sum = 0.0f;
			}
		} else {
			// ����ϵ��Ϊ0����������
			pid->pid_err_sum = 0.0f;
		}

		// PID ���㹫ʽ
		pid->pid_output = pid->pid_kp * pid->pid_err_now
						+ pid->pid_ki * pid->pid_err_sum
						+ pid->pid_kd * (pid->pid_err_now - pid->pid_err_last);
	}


    // ���������Χ
    if (pid->pid_output > pid->pid_output_max) {
        pid->pid_output = pid->pid_output_max;
    } else if (pid->pid_output < pid->pid_output_min) {
        pid->pid_output = pid->pid_output_min;
    }
}
