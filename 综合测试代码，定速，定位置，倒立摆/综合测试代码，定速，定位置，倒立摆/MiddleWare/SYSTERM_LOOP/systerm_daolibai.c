#include "systerm_daolibai.h"
#include "adc.h"

uint8_t RunState;

#define CENTER_ANGLE		ParamMenu.Items[0].Num		
										//中心角度值，先按K2再按K3，先慢后快调小，先快后慢调大
										//值大概范围：1900~2100，理想值2048

#define ANGLE_A		1700				//立起来的角度下限，一般不调
#define ANGLE_B		2300				//立起来的角度上限，一般不调


#define START_PWM			ParamMenu.Items[1].Num
										//启动时的力度，启动不了调大，启动太猛调小
										//值大概范围：35力度小些，40力度大些
										
#define START_TIME			ParamMenu.Items[2].Num
										//启动时每次旋转的时间，一般不调

#define OFFSET_PWM			ParamMenu.Items[3].Num
										//电机旋转时，PWM的偏移，一般不调

PID_t AnglePID = {				//定义角度环PID结构体变量，并初始化部分成员
	.pid_target= 0,
	.pid_kp = 0.25,
	.pid_ki = 0.009,
	.pid_kd = 0.41,
	.pid_output_max = 100,
	.pid_output_min = -100,
};

static PID_t LocationPID = {	//定义位置环PID结构体变量，并初始化部分成员
	.pid_target = 0,
	.pid_kp = 0.52,
	.pid_ki = 0.01,
	.pid_kd = 4.56,
	.pid_output_max = 100,							//旋转的速度，值大概范围：50~100
	.pid_output_min = -100,							//旋转的速度，值大概范围：-50~-100
	.pid_err_int_limit = 100,						//积分分离阈值
};


void PID_daolibai_SaveParam(void)	//保存参数（倒立摆的Kp、Ki、Kd、中心角度、启动力度、启动时间、PWM偏移）
{
	/*使用uint16_t *类型的指针，指向浮点数，把浮点数的存储内容，存入Store_Data数组中*/
	/*浮点数占用4字节，Store_Data一个数据2字节，因此需要两个Store_Data数据来存储一个浮点数*/
	/*需要保存的参数数量较少，因此存储位置是自己分配的，不重复即可*/
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
	
	/*将Store_Data的数据，保存到STM32内部的FLASH中，实现掉电不丢失*/
	FlashStore_Save();
}

void PID_daolibai_LoadParam(void)	//加载参数（倒立摆的Kp、Ki、Kd、中心角度、启动力度、启动时间、PWM偏移）
{
	/*使用uint16_t *类型的指针，读出Store_Data的两个数据，拼接为32位*/
	/*再强转为float，即可恢复原始保存的float类型数据*/
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
	/*倒立摆显示界面初始化*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "                ", OLED_8X16);
	OLED_ShowString(0, 16, "                ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "                ", OLED_8X16);
	OLED_Update();
	
	/*角度环初始目标值设定为中心角度*/
	AnglePID.pid_target = CENTER_ANGLE;
}

void systerm_daolibai_Loop(void)
{
	uint16_t read_RP_buf[4];
	/*这些Flag标志位用于判断对应的数据是实心填充还是空心填充*/
	/*因为电位器旋钮可以调节参数，但是初始电位器的位置可能偏差很大*/
	/*如果始终允许电位器调节对应的参数*/
	/*那么模式刚进入时，参数会迅速跳变为电位器所对应的值，这不利于快速测试*/
	/*因此这里的效果是，先旋转电位器，使电位器的位置与当前值对应，之后才允许电位器调节参数*/
	/*电位器的位置与当前值对应时，Flag为1，数据为实心填充*/
	/*电位器的位置与当前值不对应时，Flag为0，数据为空心填充*/
	static uint8_t RPFlag;
	static uint8_t KpFlag1, KiFlag1, KdFlag1;
	static uint8_t KpFlag2, KiFlag2, KdFlag2;
	
	float RP_Kp, RP_Ki, RP_Kd;
	
	if (Key_Check(1, KEY_SINGLE))		//K1按下
	{
		if (RunState == 0)
		{
			RunState = 13;				//开始启摆
										//转入状态13，可以让倒立摆以向左施加瞬时驱动力开始运行
										//使摆杆离开角度传感器盲区，避免盲区干扰
		}
		else
		{
			RunState = 0;				//手动停止
		}
	}
	if (Key_Check(2, KEY_SINGLE))		//K2按下
	{
		LocationPID.pid_target += 408;		//横杆正转一圈
		if (LocationPID.pid_target > 8999)
		{
			LocationPID.pid_target = 8999;
		}
	}
	if (Key_Check(3, KEY_SINGLE))		//K3按下
	{
		LocationPID.pid_target -= 408;		//横杆反转一圈
		if (LocationPID.pid_target < -8999)
		{
			LocationPID.pid_target = -8999;
		}
	}
	if (Key_Check(4, KEY_SINGLE))		//K4按下
	{
		RPFlag ++;						//RPFlag在0、1、2、3之间变化
										//分别表示不调节、调节内环、调节外环和菜单调节
		if (RPFlag == 3)				//如果RPFlag为3
		{
			RPFlag = 0;
			nextSystemStage = SYSTEM_STAGE_MENU;			//进入菜单调节模式
		}
		
		KpFlag1 = 0;
		KiFlag1 = 0;
		KdFlag1 = 0;
		KpFlag2 = 0;
		KiFlag2 = 0;
		KdFlag2 = 0;
	}
	if (Key_Check(4, KEY_LONG))			//K4长按
	{
		PID_daolibai_SaveParam();				//保存参数
		
		/*OLED提示已保存参数*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "     [提示]     ", OLED_8X16);
		OLED_ShowString(0, 16, "   已保存参数   ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "             K4>", OLED_8X16);
		OLED_Update();
		
		/*等待K4按下*/
		while (Key_Check(4, KEY_SINGLE) == 0);
		Key_Clear();
		
		/*恢复倒立摆界面*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "                ", OLED_8X16);
		OLED_ShowString(0, 16, "                ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "                ", OLED_8X16);
		OLED_Update();
	}
	
	if (RPFlag == 1)				//RPFlag为1，表示进入内环参数调节模式
	{

		Read_ADC1_MultiChannel_Average(read_RP_buf);
		RP_Kp = read_RP_buf[0] / 4095.0 * 1;		//读取电位器的值并转换范围
		RP_Ki = read_RP_buf[1] / 4095.0 * 0.5;
		RP_Kd = read_RP_buf[2] / 4095.0 * 1;
		
		if (fabs(RP_Kp - AnglePID.pid_kp) < 0.03)		//如果RP1旋钮的值与Kp参数对应
		{
			KpFlag1 = 1;		//允许RP1调节Kp
		}
		if (fabs(RP_Ki - AnglePID.pid_ki) < 0.003)		//如果RP2旋钮的值与Ki参数对应
		{
			KiFlag1 = 1;		//允许RP2调节Ki
		}
		if (fabs(RP_Kd - AnglePID.pid_kd) < 0.03)		//如果RP3旋钮的值与Kd参数对应
		{
			KdFlag1 = 1;		//允许RP3调节Kd
		}
		
		if (KpFlag1)			//实现RP1调节Kp
		{
			AnglePID.pid_kp = RP_Kp;
		}
		if (KiFlag1)			//实现RP2调节Ki
		{
			AnglePID.pid_ki = RP_Ki;
		}
		if (KdFlag1)			//实现RP3调节Kd
		{
			AnglePID.pid_kd = RP_Kd;
		}
	}
	if (RPFlag == 2)				//RPFlag为2，表示进入外环参数调节模式
	{
		Read_ADC1_MultiChannel_Average(read_RP_buf);
		RP_Kp = read_RP_buf[0] / 4095.0 * 5;		//读取电位器的值并转换范围
		RP_Ki = read_RP_buf[1] / 4095.0 * 1;
		RP_Kd = read_RP_buf[2] / 4095.0 * 10;

		if (fabs(RP_Kp - LocationPID.pid_kp) < 0.03)		//如果RP1旋钮的值与Kp参数对应
		{
			KpFlag2 = 1;		//允许RP1调节Kp
		}
		if (fabs(RP_Ki - LocationPID.pid_ki) < 0.003)		//如果RP2旋钮的值与Ki参数对应
		{
			KiFlag2 = 1;		//允许RP2调节Ki
		}
		if (fabs(RP_Kd - LocationPID.pid_kd) < 0.03)		//如果RP3旋钮的值与Kd参数对应
		{
			KdFlag2 = 1;		//允许RP3调节Kd
		}
		
		if (KpFlag2)			//实现RP1调节Kp
		{
			LocationPID.pid_kp = RP_Kp;
		}
		if (KiFlag2)			//实现RP2调节Ki
		{
			LocationPID.pid_ki = RP_Ki;
		}
		if (KdFlag2)			//实现RP3调节Kd
		{
			LocationPID.pid_kd = RP_Kd;
		}
	}
	
	/*LED指定当前允许状态*/
	if (RunState == 0)
	{
		LED_OFF();
	}
	else
	{
		LED_ON();
	}
	
	/*OLED显示PID各个参数*/
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
	
	/*根据各个标志位的值，在对应数据上显示空心填充或者实心填充，以指示是否正在调节此参数*/
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
	
	/*OLED更新*/
	OLED_Update();
}

void systerm_daolibai_Exit(void)
{
	/*退出倒立摆模式时，执行一下收尾工作*/
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
	
	/*每隔1ms，程序执行到这里一次*/
	
	/*获取实际值*/
	LocationPID.pid_actual = Encoder_GetLocation();
	AnglePID.pid_actual = Read_Angle_Value();
	
	/*以下是状态机代码，即根据运行状态的不同，执行不同的功能*/
	if (RunState == 0)			//当前处于状态0，停止状态
	{
		Motor_SetPWM(0);
	}
	else if (RunState == 1)		//当前处于状态1，判断状态
	{
		Count0 ++;
		if (Count0 > 40)		//if每隔40ms进一次
		{
			Count0 = 0;
			
			/*获取本次、上次、上上次的角度值，以40ms为间隔，连续采样3次角度值*/
			AD[2] = AD[1];
			AD[1] = AD[0];
			AD[0] = AnglePID.pid_actual;
			
			/*如果2次角度值均位于中心区间*/
			if (AD[0] > ANGLE_A && AD[0] < ANGLE_B &&
				AD[1] > ANGLE_A && AD[1] < ANGLE_B)
			{
				/*一些变量初始化归零，避免错误的初值影响PID程序运行*/
				LocationPID.pid_actual = 0;
				Encoder_SetLocation(0);
				PID_Reset(&AnglePID);
				PID_Reset(&LocationPID);
				
				/*启摆完成*/
				RunState = 2;		//状态转入2，执行PID控制程序
			}
			
			/*判断摆杆当前是否处于右侧最高点*/
			if (AD[0] >= ANGLE_B && AD[1] >= ANGLE_B && AD[2] >= ANGLE_B && //3次角度值均位于右侧区间
				AD[1] < AD[0] - 2 && AD[1] < AD[2] - 2)		//且中间一次角度值是最小的（此处额外加了2的余量），表示摆杆处于右侧最高点
			{
				RunState = 11;		//状态转入11，执行向左施加瞬时驱动力的程序
			}
			
			/*判断摆杆当前是否处于左侧最高点*/
			if (AD[0] <= ANGLE_A && AD[1] <= ANGLE_A && AD[2] <= ANGLE_A && //3次角度值均位于左侧区间
				AD[1] > AD[0] + 2 && AD[1] > AD[2] + 2)		//且中间一次角度值是最大的（此处额外加了2的余量），表示摆杆处于左侧最高点
			{
				RunState = 21;		//状态转入21，执行向右施加瞬时驱动力的程序
			}
		}
		
	}
	
	else if (RunState == 11)		//当前处于状态11，向左摆动
	{
		Motor_SetPWM(START_PWM);
		TimeCount = START_TIME;
		RunState = 12;
	}
	else if (RunState == 12)		//当前处于状态12，延时
	{
		TimeCount --;
		if (TimeCount == 0)
		{
			RunState = 13;
		}
	}
	else if (RunState == 13)		//当前处于状态13，向右摆动
	{
		Motor_SetPWM(-START_PWM);
		TimeCount = START_TIME;
		RunState = 14;
	}
	else if (RunState == 14)		//当前处于状态14，延时
	{
		TimeCount --;
		if (TimeCount == 0)
		{
			Motor_SetPWM(0);
			RunState = 1;
		}
	}
	
	else if (RunState == 21)		//当前处于状态21，向右摆动
	{
		Motor_SetPWM(-START_PWM);
		TimeCount = START_TIME;
		RunState = 22;
	}
	else if (RunState == 22)		//当前处于状态22，延时
	{
		TimeCount --;
		if (TimeCount == 0)
		{
			RunState = 23;
		}
	}
	else if (RunState == 23)		//当前处于状态23，向左摆动
	{
		Motor_SetPWM(START_PWM);
		TimeCount = START_TIME;
		RunState = 24;
	}
	else if (RunState == 24)		//当前处于状态24，延时
	{
		TimeCount --;
		if (TimeCount == 0)
		{
			Motor_SetPWM(0);
			RunState = 1;
		}
	}
	
	
	else if (RunState == 2)			//当前处于状态2，倒立摆
	{
		Count1 ++;
		if (Count1 > 50)			//if每隔50ms进一次
		{
			Count1 = 0;
			
			/*外环位置环PID调控*/
			PID_Compute(&LocationPID, 0);
			
			/*外环的输出值作用于内环的目标值，组成串级PID结构*/
			AnglePID.pid_target = CENTER_ANGLE - LocationPID.pid_output;
		}
		
		Count2 ++;
		if (Count2 > 5)				//if每隔5ms进一次
		{
			Count2 = 0;
			
			/*摆杆倒下自动停止PID程序*/
			if (AnglePID.pid_actual > 3000 || AnglePID.pid_actual < 1000)
			{
				RunState = 0;
			}
			
			/*内环角度环PID调控*/
			PID_Compute(&AnglePID, 0);
			
			/*输出PWM偏移*/
			if (AnglePID.pid_output == 0)
			{
				Motor_SetPWM(0);		//角度环的输出值给到电机PWM
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
