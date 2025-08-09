#include "systerm_select.h"


void systerm_select_Init(void)
{
	/*ѡ��ϵͳ�����ʼ��*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "   ��ѡ��ϵͳ   ", OLED_8X16);
	OLED_ShowString(0, 16, " K1��Ӳ������   ", OLED_8X16);
	OLED_ShowString(0, 32, " K2���������   ", OLED_8X16);
	OLED_ShowString(0, 48, " K3��������     ", OLED_8X16);
	OLED_Update();
}

void systerm_select_Loop(void)
{
	if (Key_Check(1, KEY_SINGLE))	//K1���£��л���ģʽSYSTEM_STAGE_KEY_TEST��Ӳ�����ԣ�
	{
		nextSystemStage = SYSTEM_STAGE_KEY_TEST;
	}
	if (Key_Check(2, KEY_SINGLE))	//K2���£��л���ģʽSYSTEM_STAGE_SPEED_CONTROL��������Ƶ�ѡ����棩
	{
		nextSystemStage = SYSTEM_STAGE_MOTOR_CONTROL;
	}
	if (Key_Check(3, KEY_SINGLE))	//K3���£��л���ģʽSYSTEM_STAGE_DAOLIBAI_CONTROL�������ڣ�
	{
		nextSystemStage = SYSTEM_STAGE_DAOLIBAI_IDLE;
	}
	if (Key_Check(4, KEY_LONG))		//K4���������ò���
	{
		/*���ñ�������ĺ�������ʼ����±���Ĳ����ǳ�����ָ����Ĭ�ϲ���*/
		PID_SpeedControl_SaveParams();
		PID_PositionControl_SaveParams();
		PID_daolibai_SaveParam();
		
		/*OLED��ʾ�����ò���*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "     [��ʾ]     ", OLED_8X16);
		OLED_ShowString(0, 16, "   �����ò���   ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "             K4>", OLED_8X16);
		OLED_Update();
		
		/*�ȴ�K4��������*/
		while (Key_Check(4, KEY_SINGLE) == 0);
		Key_Clear();
		
		/*���³�ʼ��ģʽsysterm_select����ʾѡ��ϵͳ����*/
		systerm_select_Init();
	}
}


void Location_Ready_Init(void)
{
	/*��ʾ��ʾ����*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "     [��ʾ]     ", OLED_8X16);
	OLED_ShowString(0, 16, "  �뽫�豸����  ", OLED_8X16);
	OLED_ShowString(0, 32, "   ������״̬   ", OLED_8X16);
	OLED_ShowString(0, 48, "             K4>", OLED_8X16);
	OLED_Update();
	
	/*�ȴ�K4����*/
	while (Key_Check(4, KEY_SINGLE) == 0);
	Key_Clear();
	
	/*���뵹����ģʽ*/
	nextSystemStage = SYSTEM_STAGE_DAOLIBAI_CONTROL;
	
	/*���ز������ָ�֮ǰ�û�������Ĳ���*/
	PID_daolibai_LoadParam();
}	

// ģʽ��ʼ����ѡ����/��λ�ã�
void Motor_Control_Ready_Init(void) 
{
    OLED_Clear();
    OLED_ShowString(0, 0,  "     [��ʾ]     ", OLED_8X16);
    OLED_ShowString(0, 16, "  �뽫�豸����  ", OLED_8X16);
    OLED_ShowString(0, 32, "  �������״̬  ", OLED_8X16);
    OLED_ShowString(0, 48, "             K4>", OLED_8X16);
    OLED_Update();

	/*�ȴ�K4����*/
	while (Key_Check(4, KEY_SINGLE) == 0);
	Key_Clear();

    OLED_Clear();
    OLED_ShowString(0, 0,  "    �������    ", OLED_8X16);
    OLED_ShowString(0, 16, " K1�����ٿ���   ", OLED_8X16);
    OLED_ShowString(0, 32, " K2����λ�ÿ��� ", OLED_8X16);
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
