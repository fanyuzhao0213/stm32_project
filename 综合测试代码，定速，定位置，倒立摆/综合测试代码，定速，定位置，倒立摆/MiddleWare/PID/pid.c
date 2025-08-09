#include "pid.h"
#include <math.h>
#include <stddef.h>  // for NULL
#include "main.h"

/**
 * @brief 清除 PID 控制器内部状态，将误差和积分清零
 * @param pid PID 控制器结构体指针
 */
void PID_Reset(PID_t *pid)
{
    if (pid == NULL) return;

    pid->pid_err_now = 0.0f;
    pid->pid_err_last = 0.0f;
    pid->pid_err_sum = 0.0f;
}

/**
 * @brief 根据目标值和实际值计算 PID 输出
 *        调用前应先设置 pid->pid_target 和 pid->pid_actual
 * @param pid PID 控制器结构体指针
 */
void PID_Compute(PID_t *pid, uint8_t OutInLimitationFlag)
{
    if (pid == NULL) return;

    // 记录上一次误差
    pid->pid_err_last = pid->pid_err_now;

    // 当前误差
    pid->pid_err_now = pid->pid_target - pid->pid_actual;

	if(OutInLimitationFlag == 1)			//输入输出限幅标志
	{
		/*输入死区*/
		if (fabs(pid->pid_err_now) < 5)		//如果误差绝对值比较小
		{
			pid->pid_output = 0;				//则直接输出0，不进行调控
		}
		else
		{
			// 积分部分处理
			if (pid->pid_ki != 0.0f) {
				if (pid->pid_err_int_limit == 0.0f) {
					// 不限制积分累加
					pid->pid_err_sum += pid->pid_err_now;
				}
				else if (fabsf(pid->pid_err_now) < pid->pid_err_int_limit) {
					// 误差在阈值范围内才积分
					pid->pid_err_sum += pid->pid_err_now;
				}
				else {
					// 误差超出阈值，积分清零，避免积分饱和
					pid->pid_err_sum = 0.0f;
				}
			} else {
				// 积分系数为0，积分清零
				pid->pid_err_sum = 0.0f;
			}

			// PID 计算公式
			pid->pid_output = pid->pid_kp * pid->pid_err_now
							+ pid->pid_ki * pid->pid_err_sum
							+ pid->pid_kd * (pid->pid_err_now - pid->pid_err_last);
			
			
			/*输出偏移*/
			if (pid->pid_output > 0.1)		//如果输出为正
			{
				pid->pid_output += 6;		//则直接给输出值加上一个固定偏移
			}
			else if (pid->pid_output < -0.1)	//如果输出为负
			{
				pid->pid_output -= 6;		//则直接给输出值减去一个固定偏移
			}
			else				//输出为0
			{
				pid->pid_output = 0;		//输出0
			}
		}
	}
	else
	{
		// 积分部分处理
		if (pid->pid_ki != 0.0f) {
			if (pid->pid_err_int_limit == 0.0f) {
				// 不限制积分累加
				pid->pid_err_sum += pid->pid_err_now;
			}
			else if (fabsf(pid->pid_err_now) < pid->pid_err_int_limit) {
				// 误差在阈值范围内才积分
				pid->pid_err_sum += pid->pid_err_now;
			}
			else {
				// 误差超出阈值，积分清零，避免积分饱和
				pid->pid_err_sum = 0.0f;
			}
		} else {
			// 积分系数为0，积分清零
			pid->pid_err_sum = 0.0f;
		}

		// PID 计算公式
		pid->pid_output = pid->pid_kp * pid->pid_err_now
						+ pid->pid_ki * pid->pid_err_sum
						+ pid->pid_kd * (pid->pid_err_now - pid->pid_err_last);
	}


    // 限制输出范围
    if (pid->pid_output > pid->pid_output_max) {
        pid->pid_output = pid->pid_output_max;
    } else if (pid->pid_output < pid->pid_output_min) {
        pid->pid_output = pid->pid_output_min;
    }
}
