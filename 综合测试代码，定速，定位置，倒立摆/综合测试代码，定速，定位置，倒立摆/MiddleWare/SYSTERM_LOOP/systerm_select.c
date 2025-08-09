#include "systerm_select.h"


void systerm_select_Init(void)
{
	/*选择系统界面初始化*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "   请选择系统   ", OLED_8X16);
	OLED_ShowString(0, 16, " K1：硬件测试   ", OLED_8X16);
	OLED_ShowString(0, 32, " K2：电机控制   ", OLED_8X16);
	OLED_ShowString(0, 48, " K3：倒立摆     ", OLED_8X16);
	OLED_Update();
}

void systerm_select_Loop(void)
{
	if (Key_Check(1, KEY_SINGLE))	//K1按下，切换到模式SYSTEM_STAGE_KEY_TEST（硬件测试）
	{
		nextSystemStage = SYSTEM_STAGE_KEY_TEST;
	}
	if (Key_Check(2, KEY_SINGLE))	//K2按下，切换到模式SYSTEM_STAGE_SPEED_CONTROL（电机控制的选择界面）
	{
		nextSystemStage = SYSTEM_STAGE_MOTOR_CONTROL;
	}
	if (Key_Check(3, KEY_SINGLE))	//K3按下，切换到模式SYSTEM_STAGE_DAOLIBAI_CONTROL（倒立摆）
	{
		nextSystemStage = SYSTEM_STAGE_DAOLIBAI_IDLE;
	}
	if (Key_Check(4, KEY_LONG))		//K4长按，重置参数
	{
		/*调用保存参数的函数，初始情况下保存的参数是程序里指定的默认参数*/
		PID_SpeedControl_SaveParams();
		PID_PositionControl_SaveParams();
		PID_daolibai_SaveParam();
		
		/*OLED提示已重置参数*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "     [提示]     ", OLED_8X16);
		OLED_ShowString(0, 16, "   已重置参数   ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "             K4>", OLED_8X16);
		OLED_Update();
		
		/*等待K4按键按下*/
		while (Key_Check(4, KEY_SINGLE) == 0);
		Key_Clear();
		
		/*重新初始化模式systerm_select，显示选择系统界面*/
		systerm_select_Init();
	}
}


void Location_Ready_Init(void)
{
	/*显示提示界面*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "     [提示]     ", OLED_8X16);
	OLED_ShowString(0, 16, "  请将设备置于  ", OLED_8X16);
	OLED_ShowString(0, 32, "   倒立摆状态   ", OLED_8X16);
	OLED_ShowString(0, 48, "             K4>", OLED_8X16);
	OLED_Update();
	
	/*等待K4按下*/
	while (Key_Check(4, KEY_SINGLE) == 0);
	Key_Clear();
	
	/*进入倒立摆模式*/
	nextSystemStage = SYSTEM_STAGE_DAOLIBAI_CONTROL;
	
	/*加载参数，恢复之前用户保存过的参数*/
	PID_daolibai_LoadParam();
}	

// 模式初始化（选择定速/定位置）
void Motor_Control_Ready_Init(void) 
{
    OLED_Clear();
    OLED_ShowString(0, 0,  "     [提示]     ", OLED_8X16);
    OLED_ShowString(0, 16, "  请将设备置于  ", OLED_8X16);
    OLED_ShowString(0, 32, "  电机控制状态  ", OLED_8X16);
    OLED_ShowString(0, 48, "             K4>", OLED_8X16);
    OLED_Update();

	/*等待K4单击*/
	while (Key_Check(4, KEY_SINGLE) == 0);
	Key_Clear();

    OLED_Clear();
    OLED_ShowString(0, 0,  "    电机控制    ", OLED_8X16);
    OLED_ShowString(0, 16, " K1：定速控制   ", OLED_8X16);
    OLED_ShowString(0, 32, " K2：定位置控制 ", OLED_8X16);
    OLED_Update();

    while (1) {
        if (Key_Check(1, KEY_SINGLE))
		{ 
			nextSystemStage = SYSTEM_STAGE_SPEED_CONTROL; 
			break; 
		}
        if (Key_Check(2, KEY_SINGLE)) 
		{ 
			nextSystemStage = SYSTEM_STAGE_LOCATION_CONTROL; 
			break; 
		}
    }
    Key_Clear();

    PID_SpeedControl_LoadParams();
    PID_PositionControl_LoadParams();
}
