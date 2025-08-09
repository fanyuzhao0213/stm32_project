#include "motorpid.h"

// 全局 PID 结构体初始化，字段名统一为小写
PID_t Speed_pid = {
    .pid_kp = 0.3f,
    .pid_ki = 0.3f,
    .pid_kd = 0.0f,
    .pid_output_max = 100.0f,
    .pid_output_min = -100.0f,
};

PID_t Location_pid = {
    .pid_kp = 0.4f,
    .pid_ki = 0.1f,
    .pid_kd = 0.2f,
    .pid_output_max = 100.0f,
    .pid_output_min = -100.0f,
    .pid_err_int_limit = 40.0f,  // 积分限幅阈值
};

// 存储索引宏定义，避免魔法数字
#define SPEED_PID_PARAM_START 1
#define POSITION_PID_PARAM_START 11
#define PID_PARAM_COUNT 6  // kp, ki, kd 每个float占2个uint16_t


// 将 float 拆成两个 uint16_t 保存到数组
static void save_pid_params(const PID_t *pid, uint16_t *store_data, int start_index) 
{
    const uint16_t *p_kp = (const uint16_t *)&pid->pid_kp;
    const uint16_t *p_ki = (const uint16_t *)&pid->pid_ki;
    const uint16_t *p_kd = (const uint16_t *)&pid->pid_kd;

    store_data[start_index]     = p_kp[0];
    store_data[start_index + 1] = p_kp[1];
    store_data[start_index + 2] = p_ki[0];
    store_data[start_index + 3] = p_ki[1];
    store_data[start_index + 4] = p_kd[0];
    store_data[start_index + 5] = p_kd[1];

    FlashStore_Save();
}

// 从数组恢复 float
static void load_pid_params(PID_t *pid, const uint16_t *store_data, int start_index) 
{
    uint32_t temp;

    temp = ((uint32_t)store_data[start_index + 1] << 16) | store_data[start_index];
    pid->pid_kp = *(float *)&temp;

    temp = ((uint32_t)store_data[start_index + 3] << 16) | store_data[start_index + 2];
    pid->pid_ki = *(float *)&temp;

    temp = ((uint32_t)store_data[start_index + 5] << 16) | store_data[start_index + 4];
    pid->pid_kd = *(float *)&temp;
}

// Speed PID 保存与加载
void PID_SpeedControl_SaveParams(void) 
{
    save_pid_params(&Speed_pid, FlashStore_Data, SPEED_PID_PARAM_START);
}

void PID_SpeedControl_LoadParams(void) 
{
    load_pid_params(&Speed_pid, FlashStore_Data, SPEED_PID_PARAM_START);
}

// Position PID 保存与加载
void PID_PositionControl_SaveParams(void) 
{
    save_pid_params(&Location_pid, FlashStore_Data, POSITION_PID_PARAM_START);
}

void PID_PositionControl_LoadParams(void) 
{
    load_pid_params(&Location_pid, FlashStore_Data, POSITION_PID_PARAM_START);
}

