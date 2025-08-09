#include "systerm_locationcontrol.h"
#include "adc.h"


void motor_location_control_Init(void)
{
	/*��λ�ÿ�����ʾ�����ʼ��*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "   ��λ�ÿ���    ", OLED_8X16);
	OLED_ShowString(0, 16, "                ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "                ", OLED_8X16);
	OLED_Update();
}

void motor_location_control_Loop(void)
{
	uint16_t read_RP_buf[4];
	/*��ЩFlag��־λ�����ж϶�Ӧ��������ʵ����仹�ǿ������*/
	/*��Ϊ��λ����ť���Ե��ڲ��������ǳ�ʼ��λ����λ�ÿ���ƫ��ܴ�*/
	/*���ʼ�������λ�����ڶ�Ӧ�Ĳ���*/
	/*��ôģʽ�ս���ʱ��������Ѹ������Ϊ��λ������Ӧ��ֵ���ⲻ���ڿ��ٲ���*/
	/*��������Ч���ǣ�����ת��λ����ʹ��λ����λ���뵱ǰֵ��Ӧ��֮��������λ�����ڲ���*/
	/*��λ����λ���뵱ǰֵ��Ӧʱ��FlagΪ1������Ϊʵ�����*/
	/*��λ����λ���뵱ǰֵ����Ӧʱ��FlagΪ0������Ϊ�������*/
	static uint8_t RPFlag, KpFlag, KiFlag, KdFlag, TargetFlag;
	
	float RP_Kp, RP_Ki, RP_Kd, RP_Target;
	
	if (Key_Check(1, KEY_SINGLE))		//K1����
	{
		TargetFlag = 0;
		Location_pid.pid_target += 408;		//��λ�ÿ���Ŀ��ֵ��408
		if (Location_pid.pid_target > 816)
		{
			Location_pid.pid_target = 816;
		}
	}
	if (Key_Check(2, KEY_SINGLE))		//K2����
	{
		TargetFlag = 0;
		Location_pid.pid_target -= 408;		//��λ�ÿ���Ŀ��ֵ��408
		if (Location_pid.pid_target < -816)
		{
			Location_pid.pid_target = -816;
		}
	}
	if (Key_Check(3, KEY_SINGLE))		//K3����
	{
		TargetFlag = 0;
		Location_pid.pid_target = 0;			//��λ�ÿ���Ŀ��ֵ��0
	}
	if (Key_Check(4, KEY_SINGLE))		//K4����
	{
		RPFlag = !RPFlag;				//RPFlagȡ�ǣ���ʾ������˳���������ģʽ
		KpFlag = 0;
		KiFlag = 0;
		KdFlag = 0;
		TargetFlag = 0;
	}
	if (Key_Check(4, KEY_LONG))			//K4����
	{
		PID_PositionControl_SaveParams();			//�������
		
		/*OLED��ʾ�ѱ������*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "     [��ʾ]     ", OLED_8X16);
		OLED_ShowString(0, 16, "   �ѱ������   ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "             K4>", OLED_8X16);
		OLED_Update();
		
		/*�ȴ�K4����*/
		while (Key_Check(4, KEY_SINGLE) == 0);
		Key_Clear();
		
		/*�ָ���λ�ÿ��ƽ���*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "   ��λ�ÿ���   ", OLED_8X16);
		OLED_ShowString(0, 16, "                ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "                ", OLED_8X16);
		OLED_Update();
	}
	
	if (RPFlag)				//RPFlag��0����ʾ�����������ģʽ
	{
		Read_ADC1_MultiChannel_Average(read_RP_buf);
		RP_Kp = read_RP_buf[0] / 4095.0 * 3;		//��ȡ��λ����ֵ��ת����Χ
		RP_Ki = read_RP_buf[1] / 4095.0 * 3;
		RP_Kd = read_RP_buf[2] / 4095.0 * 3;
		RP_Target = read_RP_buf[3] / 4095.0 * 408 - 204;
		
		if (fabs(RP_Kp - Location_pid.pid_kp) < 0.05)		//���RP1��ť��ֵ��Kp������Ӧ
		{
			KpFlag = 1;		//����RP1����Kp
		}
		if (fabs(RP_Ki - Location_pid.pid_ki) < 0.05)		//���RP2��ť��ֵ��Ki������Ӧ
		{
			KiFlag = 1;		//����RP2����Ki
		}
		if (fabs(RP_Kd - Location_pid.pid_kd) < 0.05)		//���RP3��ť��ֵ��Kd������Ӧ
		{
			KdFlag = 1;		//����RP3����Kd
		}
		if (fabs(RP_Target - Location_pid.pid_target) < 5)	//���RP4��ť��ֵ��Target������Ӧ
		{
			TargetFlag = 1;	//����RP4����Target
		}
		
		if (KpFlag)			//ʵ��RP1����Kp
		{
			Location_pid.pid_kp = RP_Kp;
		}
		if (KiFlag)			//ʵ��RP2����Ki
		{
			Location_pid.pid_ki = RP_Ki;
		}
		if (KdFlag)			//ʵ��RP3����Kd
		{
			Location_pid.pid_kd = RP_Kd;
		}
		if (TargetFlag)		//ʵ��RP4����Target
		{
			Location_pid.pid_target = RP_Target;
		}
	}
	
	/*OLED��ʾPID��������*/
	OLED_Printf(0, 16, OLED_8X16, "Kp:%04.2f", Location_pid.pid_kp);
	OLED_Printf(0, 32, OLED_8X16, "Ki:%04.2f", Location_pid.pid_ki);
	OLED_Printf(0, 48, OLED_8X16, "Kd:%04.2f", Location_pid.pid_kd);
	OLED_Printf(64, 16, OLED_8X16, "Tar:%+04d", (int16_t)Location_pid.pid_target);
	OLED_Printf(64, 32, OLED_8X16, "Act:%+04d", (int16_t)Location_pid.pid_actual);
	OLED_Printf(64, 48, OLED_8X16, "Out:%+04d", (int16_t)Location_pid.pid_output);
	
	/*���ݸ�����־λ��ֵ���ڶ�Ӧ��������ʾ����������ʵ����䣬��ָʾ�Ƿ����ڵ��ڴ˲���*/
	if (RPFlag)
	{
		if (KpFlag)
		{
			OLED_ReverseArea(24, 17, 32, 15);
		}
		else
		{
			OLED_DrawRectangle(24, 17, 32, 15, OLED_UNFILLED);
		}
		if (KiFlag)
		{
			OLED_ReverseArea(24, 33, 32, 15);
		}
		else
		{
			OLED_DrawRectangle(24, 33, 32, 15, OLED_UNFILLED);
		}
		if (KdFlag)
		{
			OLED_ReverseArea(24, 49, 32, 15);
		}
		else
		{
			OLED_DrawRectangle(24, 49, 32, 15, OLED_UNFILLED);
		}
		if (TargetFlag)
		{
			OLED_ReverseArea(96, 17, 32, 15);
		}
		else
		{
			OLED_DrawRectangle(96, 17, 32, 15, OLED_UNFILLED);
		}
	}
	
	/*OLED����*/
	OLED_Update();
}

void motor_location_control_Tick(void)
{
	static uint16_t Count;
	Count ++;
	if (Count >= 40)		//�ƴη�Ƶ��40msִ��һ��PID����
	{
		Count = 0;
		
		/*PID����*/
		Location_pid.pid_actual = Encoder_GetLocation();	//��ȡʵ��λ��
		PID_Compute(&Location_pid, 1);			//PID����
		Motor_SetPWM(Location_pid.pid_output);	//PID���ֵ��������PWM
	}
}

