#include "as5600.h"
#include "my_foc.h"
#include "i2c.h"

/* ======================= AS5600 设备结构体 =======================
 * 用于保存每个AS5600传感器的状态信息
 * 每个编码器实例一个结构体，最多支持两个
 */
typedef struct {
    I2C_HandleTypeDef *hi2c;  // 指向对应的I2C句柄，例如 &hi2c1 或 &hi2c2
    uint8_t i2c_addr;         // AS5600 I2C设备地址，默认0x36<<1
    float angle_offset;       // 零点偏移，用于归一化输出角度
    uint16_t last_raw;        // 上一次读取的原始角度，用于判断跨圈
    int32_t turns;            // 记录累计圈数，可正可负
} AS5600_Device;

/* ======================= 全局设备数组 =======================
 * 这里定义两个AS5600设备实例，分别对应 index = 0 和 index = 1
 */
static AS5600_Device dev_list[2];

/* ======================= 内部工具函数 =======================
 * @brief 读取AS5600原始角度寄存器值
 * @param dev 指向AS5600_Device实例
 * @return 原始角度值 (0-4095)
 *
 * AS5600输出为12位数字量，对应0~360°。
 * 分辨率：360° / 4096 ≈ 0.0879°
 */
static uint16_t _read_raw_angle(AS5600_Device *dev) {
    uint8_t buf[2];

    // 通过I2C读取角度寄存器，高位寄存器地址为 0x0C
    if(HAL_I2C_Mem_Read(dev->hi2c, dev->i2c_addr, AS5600_RAW_ANGLE_H, 1, buf, 2, 100) != HAL_OK) {
        return 0; // 读取失败返回0，表示无效值
    }

    // 将两个字节拼接成12位有效数据
    return ((uint16_t)buf[0] << 8 | buf[1]) & 0x0FFF;
}

/* ======================= 外部接口函数 ======================= */

/**
 * @brief 初始化一个AS5600设备
 * @param hi2c     该AS5600使用的I2C句柄
 * @param i2c_addr AS5600 I2C地址，默认 (0x36<<1)
 * @param index    设备索引：0 或 1
 *
 * 注意：index 不能大于1，否则不进行初始化
 */
void AS5600_InitDevice(I2C_HandleTypeDef *hi2c, uint8_t i2c_addr, uint8_t index) {
    if(index >= 2) return;  // 防止数组越界

    // 保存设备的基本配置
    dev_list[index].hi2c = hi2c;
    dev_list[index].i2c_addr = i2c_addr;
    dev_list[index].angle_offset = 0.0f;
    dev_list[index].last_raw = _read_raw_angle(&dev_list[index]);  // 初始化上次角度
    dev_list[index].turns = 0;

    // 检测设备是否通信正常
    uint8_t test;
    if(HAL_I2C_Mem_Read(hi2c, i2c_addr, AS5600_RAW_ANGLE_H, 1, &test, 1, 100) != HAL_OK) {
        rtt_printf("AS5600[%d] init failed!\r\n", index); // 调试信息
    } else {
        rtt_printf("AS5600[%d] init OK\r\n", index);
    }
}

/**
 * @brief 获取当前机械角度
 * @param index 编码器索引（0或1）
 * @return 机械角度，单位：弧度，范围 [0, 2π)
 */
float AS5600_GetAngle(uint8_t index) {
    if(index >= 2) return 0;
    uint16_t raw = _read_raw_angle(&dev_list[index]);
    return ((float)raw / 4096.0f) * 2.0f * M_PI;
}

/**
 * @brief 获取减去零点偏移后的角度
 * @param index 编码器索引（0或1）
 * @return 归一化后的角度，单位：弧度
 */
float AS5600_GetAngleWithoutOffset(uint8_t index) {
    if(index >= 2) return 0;
    return AS5600_GetAngle(index) - dev_list[index].angle_offset;
}

/**
 * @brief 设置当前编码器的零点偏移
 * @param index      编码器索引（0或1）
 * @param offset_rad 零点偏移角度，单位：弧度
 */
void AS5600_SetOffset(uint8_t index, float offset_rad) {
    if(index >= 2) return;
    dev_list[index].angle_offset = offset_rad;
}

/**
 * @brief 获取累计旋转角度
 * @param index 编码器索引（0或1）
 * @return 累计机械角度，单位：弧度
 *
 * 功能说明：
 * 1. 通过检测跨圈跳变来判断是正向还是反向转过零点。
 * 2. 将转圈数累加到输出角度中，得到连续角度。
 * 3. 适用于速度计算或需要超过 0~2π 范围的控制。
 */
float AS5600_GetTotalAngle(uint8_t index) {
    if(index >= 2) return 0;
    AS5600_Device *dev = &dev_list[index];

    // 读取当前原始角度
    uint16_t raw = _read_raw_angle(dev);

    // 计算相邻两次读取的差值
    int16_t delta = raw - dev->last_raw;

    // 处理跨圈：AS5600范围0-4095，当从4095跳回0或从0跳到4095时
    if(delta > CROSS_THRESHOLD) {
        dev->turns--;  // 逆时针跨圈
    } else if(delta < -CROSS_THRESHOLD) {
        dev->turns++;  // 顺时针跨圈
    }

    // 更新 last_raw
    dev->last_raw = raw;

    // 当前单圈角度 (0~2π)
    float mech_angle = ((float)raw / 4096.0f) * 2.0f * M_PI;

    // 总角度 = 当前单圈角度 + 转圈累计值
    return mech_angle + dev->turns * 2.0f * M_PI;
}

/**
 * @brief 获取累计转圈数
 * @param index 编码器索引（0或1）
 * @return 转圈数，可正可负
 */
int32_t AS5600_GetTurnCount(uint8_t index) {
    if(index >= 2) return 0;
    return dev_list[index].turns;
}

/**
 * @brief AS5600 测试函数
 * @param index 编码器索引（0或1）
 *
 * 功能：
 *  1. 每隔 100ms 打印一次当前角度、总角度、转圈数。
 *  2. 验证 AS5600 驱动的基本功能是否正常。
 */
void AS5600_Test(uint8_t index)
{
    if(index >= 2) {
        rtt_printf("[AS5600] Invalid index!\r\n");
        return;
    }

    rtt_printf("[AS5600] Start test on index %d ...\r\n", index);

    while(1)
    {
        // 当前单圈机械角度（0~2π）
        float angle_now = AS5600_GetAngle(index);

        // 累计旋转角度
        float total_angle = AS5600_GetTotalAngle(index);

        // 转圈数
        int32_t turn_count = AS5600_GetTurnCount(index);

        // 角度换算为度
        float angle_deg = angle_now * (180.0f / M_PI);

        // 打印调试信息
        rtt_printf("Angle: %.2f deg | Total: %.2f rad | Turns: %ld\r\n",
                   angle_deg, total_angle, turn_count);

        HAL_Delay(100); // 延时 100ms
    }
}
