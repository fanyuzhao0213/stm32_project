#include "systerm_locationcontrol.h"
#include "adc.h"


void motor_location_control_Init(void)
{
	/*定位置控制显示界面初始化*/
	OLED_Clear();
	OLED_ShowString(0, 0,  "   定位置控制    ", OLED_8X16);
	OLED_ShowString(0, 16, "                ", OLED_8X16);
	OLED_ShowString(0, 32, "                ", OLED_8X16);
	OLED_ShowString(0, 48, "                ", OLED_8X16);
	OLED_Update();
}

void motor_location_control_Loop(void)
{
	uint16_t read_RP_buf[4];
	/*这些Flag标志位用于判断对应的数据是实心填充还是空心填充*/
	/*因为电位器旋钮可以调节参数，但是初始电位器的位置可能偏差很大*/
	/*如果始终允许电位器调节对应的参数*/
	/*那么模式刚进入时，参数会迅速跳变为电位器所对应的值，这不利于快速测试*/
	/*因此这里的效果是，先旋转电位器，使电位器的位置与当前值对应，之后才允许电位器调节参数*/
	/*电位器的位置与当前值对应时，Flag为1，数据为实心填充*/
	/*电位器的位置与当前值不对应时，Flag为0，数据为空心填充*/
	static uint8_t RPFlag, KpFlag, KiFlag, KdFlag, TargetFlag;
	
	float RP_Kp, RP_Ki, RP_Kd, RP_Target;
	
	if (Key_Check(1, KEY_SINGLE))		//K1按下
	{
		TargetFlag = 0;
		Location_pid.pid_target += 408;		//定位置控制目标值加408
		if (Location_pid.pid_target > 816)
		{
			Location_pid.pid_target = 816;
		}
	}
	if (Key_Check(2, KEY_SINGLE))		//K2按下
	{
		TargetFlag = 0;
		Location_pid.pid_target -= 408;		//定位置控制目标值减408
		if (Location_pid.pid_target < -816)
		{
			Location_pid.pid_target = -816;
		}
	}
	if (Key_Check(3, KEY_SINGLE))		//K3按下
	{
		TargetFlag = 0;
		Location_pid.pid_target = 0;			//定位置控制目标值归0
	}
	if (Key_Check(4, KEY_SINGLE))		//K4按下
	{
		RPFlag = !RPFlag;				//RPFlag取非，表示进入或退出参数调节模式
		KpFlag = 0;
		KiFlag = 0;
		KdFlag = 0;
		TargetFlag = 0;
	}
	if (Key_Check(4, KEY_LONG))			//K4长按
	{
		PID_PositionControl_SaveParams();			//保存参数
		
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
		
		/*恢复定位置控制界面*/
		OLED_Clear();
		OLED_ShowString(0, 0,  "   定位置控制   ", OLED_8X16);
		OLED_ShowString(0, 16, "                ", OLED_8X16);
		OLED_ShowString(0, 32, "                ", OLED_8X16);
		OLED_ShowString(0, 48, "                ", OLED_8X16);
		OLED_Update();
	}
	
	if (RPFlag)				//RPFlag非0，表示进入参数调节模式
	{
		Read_ADC1_MultiChannel_Average(read_RP_buf);
		RP_Kp = read_RP_buf[0] / 4095.0 * 3;		//读取电位器的值并转换范围
		RP_Ki = read_RP_buf[1] / 4095.0 * 3;
		RP_Kd = read_RP_buf[2] / 4095.0 * 3;
		RP_Target = read_RP_buf[3] / 4095.0 * 408 - 204;
		
		if (fabs(RP_Kp - Location_pid.pid_kp) < 0.05)		//如果RP1旋钮的值与Kp参数对应
		{
			KpFlag = 1;		//允许RP1调节Kp
		}
		if (fabs(RP_Ki - Location_pid.pid_ki) < 0.05)		//如果RP2旋钮的值与Ki参数对应
		{
			KiFlag = 1;		//允许RP2调节Ki
		}
		if (fabs(RP_Kd - Location_pid.pid_kd) < 0.05)		//如果RP3旋钮的值与Kd参数对应
		{
			KdFlag = 1;		//允许RP3调节Kd
		}
		if (fabs(RP_Target - Location_pid.pid_target) < 5)	//如果RP4旋钮的值与Target参数对应
		{
			TargetFlag = 1;	//允许RP4调节Target
		}
		
		if (KpFlag)			//实现RP1调节Kp
		{
			Location_pid.pid_kp = RP_Kp;
		}
		if (KiFlag)			//实现RP2调节Ki
		{
			Location_pid.pid_ki = RP_Ki;
		}
		if (KdFlag)			//实现RP3调节Kd
		{
			Location_pid.pid_kd = RP_Kd;
		}
		if (TargetFlag)		//实现RP4调节Target
		{
			Location_pid.pid_target = RP_Target;
		}
	}
	
	/*OLED显示PID各个参数*/
	OLED_Printf(0, 16, OLED_8X16, "Kp:%04.2f", Location_pid.pid_kp);
	OLED_Printf(0, 32, OLED_8X16, "Ki:%04.2f", Location_pid.pid_ki);
	OLED_Printf(0, 48, OLED_8X16, "Kd:%04.2f", Location_pid.pid_kd);
	OLED_Printf(64, 16, OLED_8X16, "Tar:%+04d", (int16_t)Location_pid.pid_target);
	OLED_Printf(64, 32, OLED_8X16, "Act:%+04d", (int16_t)Location_pid.pid_actual);
	OLED_Printf(64, 48, OLED_8X16, "Out:%+04d", (int16_t)Location_pid.pid_output);
	
	/*根据各个标志位的值，在对应数据上显示空心填充或者实心填充，以指示是否正在调节此参数*/
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
	
	/*OLED更新*/
	OLED_Update();
}

void motor_location_control_Tick(void)
{
	static uint16_t Count;
	Count ++;
	if (Count >= 40)		//计次分频，40ms执行一次PID调控
	{
		Count = 0;
		
		/*PID调控*/
		Location_pid.pid_actual = Encoder_GetLocation();	//获取实际位置
		PID_Compute(&Location_pid, 1);			//PID计算
		Motor_SetPWM(Location_pid.pid_output);	//PID输出值作用与电机PWM
	}
}

