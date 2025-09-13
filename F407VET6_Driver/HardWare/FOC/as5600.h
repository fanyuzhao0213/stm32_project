#ifndef __AS5600_H
#define __AS5600_H

#include "stm32f4xx_hal.h"
#include "math.h"
#include <stdio.h>

/* I2C 默认地址 */
#define AS5600_I2C_ADDR        (0x36 << 1)  // STM32 HAL 需要左移1位
#define AS5600_RAW_ANGLE_H     0x0C         // 高位寄存器


/**
 * @brief 初始化AS5600编码器设备
 * @param hi2c I2C句柄
 * @param i2c_addr I2C地址
 * @param index 编码器索引（0或1）
 */
void AS5600_InitDevice(I2C_HandleTypeDef *hi2c, uint8_t i2c_addr, uint8_t index);

/**
 * @brief 获取机械角度（0~2π）
 * @param index 编码器索引
 * @return 机械角度，单位弧度
 */
float AS5600_GetAngle(uint8_t index);

/**
 * @brief 获取未跟踪零点的角度
 * @param index 编码器索引
 * @return 电角度（未校准）
 */
float AS5600_GetAngleWithoutOffset(uint8_t index);

/**
 * @brief 设置零点偏移
 * @param index 编码器索引
 * @param offset_rad 偏移角度（弧度）
 */
void AS5600_SetOffset(uint8_t index, float offset_rad);

/**
 * @brief 获取累计角度
 * @param index 编码器索引
 * @return 累计旋转角度（弧度），包含多圈
 */
float AS5600_GetTotalAngle(uint8_t index);

/**
 * @brief 获取累计转圈数
 * @param index 编码器索引
 * @return 转圈数，可为负
 */
int32_t AS5600_GetTurnCount(uint8_t index);


void AS5600_Test(uint8_t index);
#endif
