#include "systerm_hardwaretest.h"
#include "adc.h"

void KeyTest_Init(void)
{
	/*按键测试界面初始化*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "    KEY TEST    ", OLED_8X16);
	OLED_ShowString(0, 16, "K1 K2 K3        ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "1/5          K4>", OLED_8X16);
	OLED_Update();
}

void KeyTest_Loop(void)
{
	/*按键测试*/
	if (Key_Check(1, KEY_SINGLE))	//K1按下，K1所在位置反色闪烁一次
	{
		OLED_ReverseArea(0, 16, 16, 16);
		OLED_Update();
		HAL_Delay(100);
		OLED_ReverseArea(0, 16, 16, 16);
		OLED_Update();
	}
	if (Key_Check(2, KEY_SINGLE))	//K2按下，K2所在位置反色闪烁一次
	{
		OLED_ReverseArea(24, 16, 16, 16);
		OLED_Update();
		HAL_Delay(100);
		OLED_ReverseArea(24, 16, 16, 16);
		OLED_Update();
	}
	if (Key_Check(3, KEY_SINGLE))	//K3按下，K3所在位置反色闪烁一次
	{
		OLED_ReverseArea(48, 16, 16, 16);
		OLED_Update();
		HAL_Delay(100);
		OLED_ReverseArea(48, 16, 16, 16);
		OLED_Update();
	}
	if (Key_Check(4, KEY_SINGLE))	//K4按下，切换到下一个测试界面
	{
		nextSystemStage = SYSTEM_STAGE_RP_TEST;
	}
}

void RPTest_Init(void)
{
	/*电位器测试界面初始化*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "    RP  TEST    ", OLED_8X16);
	OLED_ShowString(0, 16, "                ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "2/5          K4>", OLED_8X16);
	OLED_Update();
}

void RPTest_Loop(void)
{
	uint16_t read_RP_buf[4];
	
	if (Key_Check(4, KEY_SINGLE))	//K4按下，切换到下一个测试界面
	{
		nextSystemStage = SYSTEM_STAGE_PWM_TEST;
	}

	Read_ADC1_MultiChannel_Average(read_RP_buf);
	/*循环读取电位器旋钮的AD值并显示在OLED上*/
	OLED_Printf(0, 16, OLED_8X16, "1:%04d  2:%04d", read_RP_buf[0],read_RP_buf[1]);
	OLED_Printf(0, 32, OLED_8X16, "3:%04d  4:%04d", read_RP_buf[2],read_RP_buf[3]);
	OLED_Update();
}

void PWMTest_Init(void)
{
	/*电机测试界面初始化*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "   MOTOR TEST   ", OLED_8X16);
	OLED_ShowString(0, 16, "                ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "3/5          K4>", OLED_8X16);
	OLED_Update();
}

int16_t PWM_Duty;

void PWMTest_Loop(void)
{
	if (Key_Check(1, KEY_SINGLE))	//K1按下，PWM_Duty加5
	{
		PWM_Duty += 5;
	}
	if (Key_Check(2, KEY_SINGLE))	//K2按下，PWM_Duty减5
	{
		PWM_Duty -= 5;
	}
	if (Key_Check(3, KEY_SINGLE))	//K3按下，PWM_Duty归0
	{
		PWM_Duty = 0;
	}
	if (Key_Check(4, KEY_SINGLE))	//K4按下，切换到下一个测试界面
	{
		nextSystemStage = SYSTEM_STAGE_ENCODER_TEST;
	}

	Motor_SetPWM(PWM_Duty);		//把PWM_Duty变量的值赋值给左侧两个电机接口

	OLED_Printf(0, 16, OLED_8X16, "PWM:%+04d", PWM_Duty);	//OLED显示PWM_Duty值
	OLED_Update();
}

void PWMTest_Exit(void)
{
	/*电机测试模式收尾工作*/
	PWM_Duty = 0;					//在此模式退出后，电机自动停止
	Motor_SetPWM(PWM_Duty);
}


void EncoderTest_Init(void)
{
	/*编码器测试界面初始化*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "  ENCODER TEST   ", OLED_8X16);
	OLED_ShowString(0, 16, "                ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "4/5          K4>", OLED_8X16);
	OLED_Update();

	/*初始位置归0*/
	Encoder_SetLocation(0);
}

void EncoderTest_Loop(void)
{
	if (Key_Check(4, KEY_SINGLE))	//K4按下，切换到下一个测试界面
	{
		nextSystemStage = SYSTEM_STAGE_ANGLE_TEST;
	}

	/*循环读取两个编码器的位置值并显示在OLED上*/
	OLED_Printf(0, 16, OLED_8X16, "EA:%+06d", Encoder_GetLocation());	//左侧两个电机接口的编码器
	OLED_Update();
}

#include "adc.h"
void AngleTest_Init(void)
{
	/*角度传感器测试界面初始化*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "   ANGLE TEST   ", OLED_8X16);
	OLED_ShowString(0, 16, "                ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "5/5          K4>", OLED_8X16);
	OLED_Update();
}

void AngleTest_Loop(void)
{
	if (Key_Check(4, KEY_SINGLE))	//K4按下，切换到下一个测试界面
	{
		nextSystemStage = SYSTEM_STAGE_SELECTION;
	}

	/*循环读取角度传感器的AD值并显示在OLED上*/
	OLED_Printf(0, 16, OLED_8X16, "AD1:%04d", Read_Angle_Value());
	OLED_Update();
}
