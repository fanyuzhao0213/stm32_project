#include "systerm_daolibai.h"
#include "adc.h"

uint8_t RunState;

#define CENTER_ANGLE		ParamMenu.Items[0].Num		
										//���ĽǶ�ֵ���Ȱ�K2�ٰ�K3����������С���ȿ��������
										//ֵ��ŷ�Χ��1900~2100������ֵ2048

#define ANGLE_A		1700				//�������ĽǶ����ޣ�һ�㲻��
#define ANGLE_B		2300				//�������ĽǶ����ޣ�һ�㲻��


#define START_PWM			ParamMenu.Items[1].Num
										//����ʱ�����ȣ��������˵�������̫�͵�С
										//ֵ��ŷ�Χ��35����СЩ��40���ȴ�Щ
										
#define START_TIME			ParamMenu.Items[2].Num
										//����ʱÿ����ת��ʱ�䣬һ�㲻��

#define OFFSET_PWM			ParamMenu.Items[3].Num
										//�����תʱ��PWM��ƫ�ƣ�һ�㲻��

PID_t AnglePID = {				//����ǶȻ�PID�ṹ�����������ʼ�����ֳ�Ա
	.pid_target= 0,
	.pid_kp = 0.25,
	.pid_ki = 0.009,
	.pid_kd = 0.41,
	.pid_output_max = 100,
	.pid_output_min = -100,
};

static PID_t LocationPID = {	//����λ�û�PID�ṹ�����������ʼ�����ֳ�Ա
	.pid_target = 0,
	.pid_kp = 0.52,
	.pid_ki = 0.01,
	.pid_kd = 4.56,
	.pid_output_max = 100,							//��ת���ٶȣ�ֵ��ŷ�Χ��50~100
	.pid_output_min = -100,							//��ת���ٶȣ�ֵ��ŷ�Χ��-50~-100
	.pid_err_int_limit = 100,						//���ַ�����ֵ
};


void PID_daolibai_SaveParam(void)	//��������������ڵ�Kp��Ki��Kd�����ĽǶȡ��������ȡ�����ʱ�䡢PWMƫ�ƣ�
{
	/*ʹ��uint16_t *���͵�ָ�룬ָ�򸡵������Ѹ������Ĵ洢���ݣ�����Store_Data������*/
	/*������ռ��4�ֽڣ�Store_Dataһ������2�ֽڣ������Ҫ����Store_Data�������洢һ��������*/
	/*��Ҫ����Ĳ����������٣���˴洢λ�����Լ�����ģ����ظ�����*/
	FlashStore_Data[21] = *((uint16_t *)&AnglePID.pid_kp);
	FlashStore_Data[22] = *((uint16_t *)&AnglePID.pid_kp + 1);
	FlashStore_Data[23] = *((uint16_t *)&AnglePID.pid_ki);
	FlashStore_Data[24] = *((uint16_t *)&AnglePID.pid_ki + 1);
	FlashStore_Data[25] = *((uint16_t *)&AnglePID.pid_kd);
	FlashStore_Data[26] = *((uint16_t *)&AnglePID.pid_kd + 1);

	FlashStore_Data[31] = *((uint16_t *)&LocationPID.pid_kp);
	FlashStore_Data[32] = *((uint16_t *)&LocationPID.pid_kp + 1);
	FlashStore_Data[33] = *((uint16_t *)&LocationPID.pid_ki);
	FlashStore_Data[34] = *((uint16_t *)&LocationPID.pid_ki + 1);
	FlashStore_Data[35] = *((uint16_t *)&LocationPID.pid_kd);
	FlashStore_Data[36] = *((uint16_t *)&LocationPID.pid_kd + 1);
	
	FlashStore_Data[41] = *((uint16_t *)&ParamMenu.Items[0].Num);
	FlashStore_Data[42] = *((uint16_t *)&ParamMenu.Items[0].Num + 1);
	FlashStore_Data[43] = *((uint16_t *)&ParamMenu.Items[1].Num);
	FlashStore_Data[44] = *((uint16_t *)&ParamMenu.Items[1].Num + 1);
	FlashStore_Data[45] = *((uint16_t *)&ParamMenu.Items[2].Num);
	FlashStore_Data[46] = *((uint16_t *)&ParamMenu.Items[2].Num + 1);
	FlashStore_Data[47] = *((uint16_t *)&ParamMenu.Items[3].Num);
	FlashStore_Data[48] = *((uint16_t *)&ParamMenu.Items[3].Num + 1);
	
	/*��Store_Data�����ݣ����浽STM32�ڲ���FLASH�У�ʵ�ֵ��粻��ʧ*/
	FlashStore_Save();
}

void PID_daolibai_LoadParam(void)	//���ز����������ڵ�Kp��Ki��Kd�����ĽǶȡ��������ȡ�����ʱ�䡢PWMƫ�ƣ�
{
	/*ʹ��uint16_t *���͵�ָ�룬����Store_Data���������ݣ�ƴ��Ϊ32λ*/
	/*��ǿתΪfloat�����ɻָ�ԭʼ�����float��������*/
	uint32_t Temp;
	Temp = (FlashStore_Data[22] << 16) | FlashStore_Data[21];
	AnglePID.pid_kp = *(float *)&Temp;
	Temp = (FlashStore_Data[24] << 16) | FlashStore_Data[23];
	AnglePID.pid_ki = *(float *)&Temp;
	Temp = (FlashStore_Data[26] << 16) | FlashStore_Data[25];
	AnglePID.pid_kd = *(float *)&Temp;
	
	Temp = (FlashStore_Data[32] << 16) | FlashStore_Data[31];
	LocationPID.pid_kp = *(float *)&Temp;
	Temp = (FlashStore_Data[34] << 16) | FlashStore_Data[33];
	LocationPID.pid_ki = *(float *)&Temp;
	Temp = (FlashStore_Data[36] << 16) | FlashStore_Data[35];
	LocationPID.pid_kd = *(float *)&Temp;
	
	Temp = (FlashStore_Data[42] << 16) | FlashStore_Data[41];
	ParamMenu.Items[0].Num = *(float *)&Temp;
	Temp = (FlashStore_Data[44] << 16) | FlashStore_Data[43];
	ParamMenu.Items[1].Num = *(float *)&Temp;
	Temp = (FlashStore_Data[46] << 16) | FlashStore_Data[45];
	ParamMenu.Items[2].Num = *(float *)&Temp;
	Temp = (FlashStore_Data[48] << 16) | FlashStore_Data[47];
	ParamMenu.Items[3].Num = *(float *)&Temp;
}

void systerm_daolibai_Init(void)
{
	/*��������ʾ�����ʼ��*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "                ", OLED_8X16);
	OLED_ShowString(0, 16, "                ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "                ", OLED_8X16);
	OLED_Update();
	
	/*�ǶȻ���ʼĿ��ֵ�趨Ϊ���ĽǶ�*/
	AnglePID.pid_target = CENTER_ANGLE;
}

void systerm_daolibai_Loop(void)
{
	uint16_t read_RP_buf[4];
	/*��ЩFlag��־λ�����ж϶�Ӧ��������ʵ����仹�ǿ������*/
	/*��Ϊ��λ����ť���Ե��ڲ��������ǳ�ʼ��λ����λ�ÿ���ƫ��ܴ�*/
	/*���ʼ�������λ�����ڶ�Ӧ�Ĳ���*/
	/*��ôģʽ�ս���ʱ��������Ѹ������Ϊ��λ������Ӧ��ֵ���ⲻ���ڿ��ٲ���*/
	/*��������Ч���ǣ�����ת��λ����ʹ��λ����λ���뵱ǰֵ��Ӧ��֮��������λ�����ڲ���*/
	/*��λ����λ���뵱ǰֵ��Ӧʱ��FlagΪ1������Ϊʵ�����*/
	/*��λ����λ���뵱ǰֵ����Ӧʱ��FlagΪ0������Ϊ�������*/
	static uint8_t RPFlag;
	static uint8_t KpFlag1, KiFlag1, KdFlag1;
	static uint8_t KpFlag2, KiFlag2, KdFlag2;
	
	float RP_Kp, RP_Ki, RP_Kd;
	
	if (Key_Check(1, KEY_SINGLE))		//K1����
	{
		if (RunState == 0)
		{
			RunState = 13;				//��ʼ����
										//ת��״̬13�������õ�����������ʩ��˲ʱ��������ʼ����
										//ʹ�ڸ��뿪�Ƕȴ�����ä��������ä������
		}
		else
		{
			RunState = 0;				//�ֶ�ֹͣ
		}
	}
	if (Key_Check(2, KEY_SINGLE))		//K2����
	{
		LocationPID.pid_target += 408;		//�����תһȦ
		if (LocationPID.pid_target > 8999)
		{
			LocationPID.pid_target = 8999;
		}
	}
	if (Key_Check(3, KEY_SINGLE))		//K3����
	{
		LocationPID.pid_target -= 408;		//��˷�תһȦ
		if (LocationPID.pid_target < -8999)
		{
			LocationPID.pid_target = -8999;
		}
	}
	if (Key_Check(4, KEY_SINGLE))		//K4����
	{
		RPFlag ++;						//RPFlag��0��1��2��3֮��仯
										//�ֱ��ʾ�����ڡ������ڻ��������⻷�Ͳ˵�����
		if (RPFlag == 3)				//���RPFlagΪ3
		{
			RPFlag = 0;
			nextSystemStage = SYSTEM_STAGE_MENU;			//����˵�����ģʽ
		}
		
		KpFlag1 = 0;
		KiFlag1 = 0;
		KdFlag1 = 0;
		KpFlag2 = 0;
		KiFlag2 = 0;
		KdFlag2 = 0;
	}
	if (Key_Check(4, KEY_LONG))			//K4����
	{
		PID_daolibai_SaveParam();				//�������
		
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
		
		/*�ָ������ڽ���*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "                ", OLED_8X16);
		OLED_ShowString(0, 16, "                ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "                ", OLED_8X16);
		OLED_Update();
	}
	
	if (RPFlag == 1)				//RPFlagΪ1����ʾ�����ڻ���������ģʽ
	{

		Read_ADC1_MultiChannel_Average(read_RP_buf);
		RP_Kp = read_RP_buf[0] / 4095.0 * 1;		//��ȡ��λ����ֵ��ת����Χ
		RP_Ki = read_RP_buf[1] / 4095.0 * 0.5;
		RP_Kd = read_RP_buf[2] / 4095.0 * 1;
		
		if (fabs(RP_Kp - AnglePID.pid_kp) < 0.03)		//���RP1��ť��ֵ��Kp������Ӧ
		{
			KpFlag1 = 1;		//����RP1����Kp
		}
		if (fabs(RP_Ki - AnglePID.pid_ki) < 0.003)		//���RP2��ť��ֵ��Ki������Ӧ
		{
			KiFlag1 = 1;		//����RP2����Ki
		}
		if (fabs(RP_Kd - AnglePID.pid_kd) < 0.03)		//���RP3��ť��ֵ��Kd������Ӧ
		{
			KdFlag1 = 1;		//����RP3����Kd
		}
		
		if (KpFlag1)			//ʵ��RP1����Kp
		{
			AnglePID.pid_kp = RP_Kp;
		}
		if (KiFlag1)			//ʵ��RP2����Ki
		{
			AnglePID.pid_ki = RP_Ki;
		}
		if (KdFlag1)			//ʵ��RP3����Kd
		{
			AnglePID.pid_kd = RP_Kd;
		}
	}
	if (RPFlag == 2)				//RPFlagΪ2����ʾ�����⻷��������ģʽ
	{
		Read_ADC1_MultiChannel_Average(read_RP_buf);
		RP_Kp = read_RP_buf[0] / 4095.0 * 5;		//��ȡ��λ����ֵ��ת����Χ
		RP_Ki = read_RP_buf[1] / 4095.0 * 1;
		RP_Kd = read_RP_buf[2] / 4095.0 * 10;

		if (fabs(RP_Kp - LocationPID.pid_kp) < 0.03)		//���RP1��ť��ֵ��Kp������Ӧ
		{
			KpFlag2 = 1;		//����RP1����Kp
		}
		if (fabs(RP_Ki - LocationPID.pid_ki) < 0.003)		//���RP2��ť��ֵ��Ki������Ӧ
		{
			KiFlag2 = 1;		//����RP2����Ki
		}
		if (fabs(RP_Kd - LocationPID.pid_kd) < 0.03)		//���RP3��ť��ֵ��Kd������Ӧ
		{
			KdFlag2 = 1;		//����RP3����Kd
		}
		
		if (KpFlag2)			//ʵ��RP1����Kp
		{
			LocationPID.pid_kp = RP_Kp;
		}
		if (KiFlag2)			//ʵ��RP2����Ki
		{
			LocationPID.pid_ki = RP_Ki;
		}
		if (KdFlag2)			//ʵ��RP3����Kd
		{
			LocationPID.pid_kd = RP_Kd;
		}
	}
	
	/*LEDָ����ǰ����״̬*/
	if (RunState == 0)
	{
		LED_OFF();
	}
	else
	{
		LED_ON();
	}
	
	/*OLED��ʾPID��������*/
	OLED_DrawLine(63, 0, 63, 63);
	
	OLED_Printf(0, 0, OLED_6X8, "Angle");
	OLED_Printf(0, 8+4, OLED_6X8, "Kp:%.3f", AnglePID.pid_kp);
	OLED_Printf(0, 16+4, OLED_6X8, "Ki:%.3f", AnglePID.pid_ki);
	OLED_Printf(0, 24+4, OLED_6X8, "Kd:%.3f", AnglePID.pid_kd);
	OLED_Printf(0, 40, OLED_6X8, "Tar:%+05d", (int16_t)AnglePID.pid_target);
	OLED_Printf(0, 48, OLED_6X8, "Act:%+05d", (int16_t)AnglePID.pid_actual);
	OLED_Printf(0, 56, OLED_6X8, "Out:%+05d", (int16_t)AnglePID.pid_output);
	
	OLED_Printf(68, 0, OLED_6X8, "Location");
	OLED_Printf(68, 8+4, OLED_6X8, "Kp:%.3f", LocationPID.pid_kp);
	OLED_Printf(68, 16+4, OLED_6X8, "Ki:%.3f", LocationPID.pid_ki);
	OLED_Printf(68, 24+4, OLED_6X8, "Kd:%.3f", LocationPID.pid_kd);
	OLED_Printf(68, 40, OLED_6X8, "Tar:%+05d", (int16_t)LocationPID.pid_target);
	OLED_Printf(68, 48, OLED_6X8, "Act:%+05d", (int16_t)LocationPID.pid_actual);
	OLED_Printf(68, 56, OLED_6X8, "Out:%+05d", (int16_t)LocationPID.pid_output);	
	
	/*���ݸ�����־λ��ֵ���ڶ�Ӧ��������ʾ����������ʵ����䣬��ָʾ�Ƿ����ڵ��ڴ˲���*/
	if (RPFlag == 1)
	{
		if (KpFlag1)
		{
			OLED_ReverseArea(18, 8+4, 30, 7);
		}
		else
		{
			OLED_DrawRectangle(18, 8+4, 30, 7, OLED_UNFILLED);
		}
		if (KiFlag1)
		{
			OLED_ReverseArea(18, 16+4, 30, 7);
		}
		else
		{
			OLED_DrawRectangle(18, 16+4, 30, 7, OLED_UNFILLED);
		}
		if (KdFlag1)
		{
			OLED_ReverseArea(18, 24+4, 30, 7);
		}
		else
		{
			OLED_DrawRectangle(18, 24+4, 30, 7, OLED_UNFILLED);
		}
	}
	if (RPFlag == 2)
	{
		if (KpFlag2)
		{
			OLED_ReverseArea(86, 8+4, 30, 7);
		}
		else
		{
			OLED_DrawRectangle(86, 8+4, 30, 7, OLED_UNFILLED);
		}
		if (KiFlag2)
		{
			OLED_ReverseArea(86, 16+4, 30, 7);
		}
		else
		{
			OLED_DrawRectangle(86, 16+4, 30, 7, OLED_UNFILLED);
		}
		if (KdFlag2)
		{
			OLED_ReverseArea(86, 24+4, 30, 7);
		}
		else
		{
			OLED_DrawRectangle(86, 24+4, 30, 7, OLED_UNFILLED);
		}
	}
	
	/*OLED����*/
	OLED_Update();
}

void systerm_daolibai_Exit(void)
{
	/*�˳�������ģʽʱ��ִ��һ����β����*/
	RunState = 0;
	Motor_SetPWM(0);
	LED_OFF();
}

void systerm_daolibai_Tick(void)
{
	static uint8_t Count0;
	static uint8_t Count1;
	static uint8_t Count2;
	
	static uint16_t AD[3];
	static int16_t TimeCount;
	
	/*ÿ��1ms������ִ�е�����һ��*/
	
	/*��ȡʵ��ֵ*/
	LocationPID.pid_actual = Encoder_GetLocation();
	AnglePID.pid_actual = Read_Angle_Value();
	
	/*������״̬�����룬����������״̬�Ĳ�ͬ��ִ�в�ͬ�Ĺ���*/
	if (RunState == 0)			//��ǰ����״̬0��ֹͣ״̬
	{
		Motor_SetPWM(0);
	}
	else if (RunState == 1)		//��ǰ����״̬1���ж�״̬
	{
		Count0 ++;
		if (Count0 > 40)		//ifÿ��40ms��һ��
		{
			Count0 = 0;
			
			/*��ȡ���Ρ��ϴΡ����ϴεĽǶ�ֵ����40msΪ�������������3�νǶ�ֵ*/
			AD[2] = AD[1];
			AD[1] = AD[0];
			AD[0] = AnglePID.pid_actual;
			
			/*���2�νǶ�ֵ��λ����������*/
			if (AD[0] > ANGLE_A && AD[0] < ANGLE_B &&
				AD[1] > ANGLE_A && AD[1] < ANGLE_B)
			{
				/*һЩ������ʼ�����㣬�������ĳ�ֵӰ��PID��������*/
				LocationPID.pid_actual = 0;
				Encoder_SetLocation(0);
				PID_Reset(&AnglePID);
				PID_Reset(&LocationPID);
				
				/*�������*/
				RunState = 2;		//״̬ת��2��ִ��PID���Ƴ���
			}
			
			/*�жϰڸ˵�ǰ�Ƿ����Ҳ���ߵ�*/
			if (AD[0] >= ANGLE_B && AD[1] >= ANGLE_B && AD[2] >= ANGLE_B && //3�νǶ�ֵ��λ���Ҳ�����
				AD[1] < AD[0] - 2 && AD[1] < AD[2] - 2)		//���м�һ�νǶ�ֵ����С�ģ��˴��������2������������ʾ�ڸ˴����Ҳ���ߵ�
			{
				RunState = 11;		//״̬ת��11��ִ������ʩ��˲ʱ�������ĳ���
			}
			
			/*�жϰڸ˵�ǰ�Ƿ��������ߵ�*/
			if (AD[0] <= ANGLE_A && AD[1] <= ANGLE_A && AD[2] <= ANGLE_A && //3�νǶ�ֵ��λ���������
				AD[1] > AD[0] + 2 && AD[1] > AD[2] + 2)		//���м�һ�νǶ�ֵ�����ģ��˴��������2������������ʾ�ڸ˴��������ߵ�
			{
				RunState = 21;		//״̬ת��21��ִ������ʩ��˲ʱ�������ĳ���
			}
		}
		
	}
	
	else if (RunState == 11)		//��ǰ����״̬11������ڶ�
	{
		Motor_SetPWM(START_PWM);
		TimeCount = START_TIME;
		RunState = 12;
	}
	else if (RunState == 12)		//��ǰ����״̬12����ʱ
	{
		TimeCount --;
		if (TimeCount == 0)
		{
			RunState = 13;
		}
	}
	else if (RunState == 13)		//��ǰ����״̬13�����Ұڶ�
	{
		Motor_SetPWM(-START_PWM);
		TimeCount = START_TIME;
		RunState = 14;
	}
	else if (RunState == 14)		//��ǰ����״̬14����ʱ
	{
		TimeCount --;
		if (TimeCount == 0)
		{
			Motor_SetPWM(0);
			RunState = 1;
		}
	}
	
	else if (RunState == 21)		//��ǰ����״̬21�����Ұڶ�
	{
		Motor_SetPWM(-START_PWM);
		TimeCount = START_TIME;
		RunState = 22;
	}
	else if (RunState == 22)		//��ǰ����״̬22����ʱ
	{
		TimeCount --;
		if (TimeCount == 0)
		{
			RunState = 23;
		}
	}
	else if (RunState == 23)		//��ǰ����״̬23������ڶ�
	{
		Motor_SetPWM(START_PWM);
		TimeCount = START_TIME;
		RunState = 24;
	}
	else if (RunState == 24)		//��ǰ����״̬24����ʱ
	{
		TimeCount --;
		if (TimeCount == 0)
		{
			Motor_SetPWM(0);
			RunState = 1;
		}
	}
	
	
	else if (RunState == 2)			//��ǰ����״̬2��������
	{
		Count1 ++;
		if (Count1 > 50)			//ifÿ��50ms��һ��
		{
			Count1 = 0;
			
			/*�⻷λ�û�PID����*/
			PID_Compute(&LocationPID, 0);
			
			/*�⻷�����ֵ�������ڻ���Ŀ��ֵ����ɴ���PID�ṹ*/
			AnglePID.pid_target = CENTER_ANGLE - LocationPID.pid_output;
		}
		
		Count2 ++;
		if (Count2 > 5)				//ifÿ��5ms��һ��
		{
			Count2 = 0;
			
			/*�ڸ˵����Զ�ֹͣPID����*/
			if (AnglePID.pid_actual > 3000 || AnglePID.pid_actual < 1000)
			{
				RunState = 0;
			}
			
			/*�ڻ��ǶȻ�PID����*/
			PID_Compute(&AnglePID, 0);
			
			/*���PWMƫ��*/
			if (AnglePID.pid_output == 0)
			{
				Motor_SetPWM(0);		//�ǶȻ������ֵ�������PWM
			}
			else if (AnglePID.pid_output > 0)
			{
				Motor_SetPWM(AnglePID.pid_output + OFFSET_PWM);
			}
			else if (AnglePID.pid_output < 0)
			{
				Motor_SetPWM(AnglePID.pid_output - OFFSET_PWM);
			}
		}
	}
}
