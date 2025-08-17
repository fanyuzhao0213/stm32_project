#include "NRF24L01.h"
#include "NRF24L01_REG.h"


/*引脚配置*********************/

/**
  * 函    数：NRF24L01写CE高低电平
  * 参    数：要写入CE的电平值，范围：0/1
  * 返 回 值：无
  * 说    明：当上层函数需要写CE时，此函数会被调用
  *           用户需要根据参数传入的值，将CE置为高电平或者低电平
  *           当参数传入0时，置CE为低电平，当参数传入1时，置CE为高电平
  */
void NRF24L01_W_CE(uint8_t BitValue)
{
	/*根据BitValue的值，将CE置高电平或者低电平*/
	HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, (GPIO_PinState)BitValue);
}

/**
  * 函    数：NRF24L01写CSN高低电平
  * 参    数：要写入CSN的电平值，范围：0/1
  * 返 回 值：无
  * 说    明：当上层函数需要写CSN时，此函数会被调用
  *           用户需要根据参数传入的值，将CSN置为高电平或者低电平
  *           当参数传入0时，置CSN为低电平，当参数传入1时，置CSN为高电平
  */
void NRF24L01_W_CSN(uint8_t BitValue)
{
	/*根据BitValue的值，将CSN置高电平或者低电平*/
	HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, (GPIO_PinState)BitValue);
}