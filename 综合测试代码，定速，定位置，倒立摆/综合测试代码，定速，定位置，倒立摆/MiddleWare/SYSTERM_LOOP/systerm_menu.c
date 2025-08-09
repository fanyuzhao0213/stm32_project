#include "systerm_menu.h"

/*����˵��б�ṹ��*/
struct Menu_List ParamMenu = {
	.Name = "��������",
	
	.WindowIndex = 0,
	.WindowLength = 4,
	
	.ItemIndex = 0,
	.ItemLength = 4,
	.Items = {
		{.Name = "���ĽǶ�", .Type = 2,
		 .Num = 2050, .NumMax = 2200, .NumMin = 1900, .NumStep = 10, .NumDisplayFormat = "%04.0f"},
		
		{.Name = "��������", .Type = 2,
		 .Num = 35, .NumMax = 100, .NumMin = 0, .NumStep = 1, .NumDisplayFormat = "%03.0f"},
		
		{.Name = "����ʱ��", .Type = 2,
		 .Num = 90, .NumMax = 200, .NumMin = 0, .NumStep = 1, .NumDisplayFormat = "%03.0f"},
		
		{.Name = "PWMƫ��", .Type = 2,
		 .Num = 0, .NumMax = 50, .NumMin = 0, .NumStep = 1, .NumDisplayFormat = "%03.0f"},
	}
};


struct Menu_List *ListStack[32];		//�б�ջ�������б���
int16_t pListStack = 0;					//ָʾ��ǰ�б�λ���б�ջ�ĵڼ���

void Menu_Display(void)		//��ʾ�˵�
{
	OLED_Clear();
	
	struct Menu_List *pList = ListStack[pListStack];
	
	for (uint8_t i = 0; i < pList->WindowLength; i ++)		//ѭ����ʾ�б��ÿһ��
	{
		int16_t j = pList->WindowIndex + i;					//��ȡ���������
		
		struct Menu_Item Item = pList->Items[j];			//�ҵ�����
		
		OLED_Printf(0, 16 * i, OLED_8X16, "%s", Item.Name);	//��ʾ���������
		
		if (Item.Type == 1)					//�����Ŀ����Ϊ1�������ͣ�
		{
			OLED_Printf(80, 16 * i, OLED_8X16, "%d", Item.ON_OFF);					//��ʾ����ֵ
		}
		else if (Item.Type == 2)			//�����Ŀ����Ϊ2����ֵ�ͣ�
		{
			OLED_Printf(80, 16 * i, OLED_8X16, *Item.NumDisplayFormat, Item.Num);	//��ʾ��ֵ
			
			if (Item.NumSetFlag)			//�����������ֵģʽ
			{
				OLED_Printf(72, 16 * i, OLED_8X16, "<");			//����ֵ������ʾ������ͷ
				OLED_Printf(128 - 8, 16 * i, OLED_8X16, ">");
			}
		}
	}
	
	int16_t Cursor = pList->ItemIndex - pList->WindowIndex;			//������λ��
	
	OLED_ReverseArea(0, 16 * Cursor, 128, 16);						//�����ѡ�е���Ŀ��ɫ��ʾ
		
	OLED_Update();			//OLED����
}

void Menu_Up(void)			//�ϼ�����
{
	struct Menu_List *pList = ListStack[pListStack];
	
	struct Menu_Item *Item = &pList->Items[pList->ItemIndex];
	
	if (Item->NumSetFlag)	//��ǰ��������ֵģʽ
	{
		Item->Num -= Item->NumStep;		//��С��ǰ�����ֵ
		if (Item->Num < Item->NumMin)
		{
			Item->Num = Item->NumMin;
		}
	}
	else					//���򣬼���ǰ����������ֵģʽ
	{
		pList->ItemIndex --;			//��Ŀ������1
		
		if (pList->ItemIndex < 0)		//����Ѿ��ǵ�һ����
		{
			pList->ItemIndex = pList->ItemLength - 1;		//��Ŀ��������Ϊĩβ
			pList->WindowIndex = pList->ItemLength - pList->WindowLength;	//������������Ϊĩβ
		}
		else							//���򣬼�û���Ƶ���һ��
		{
			int16_t Cursor = pList->ItemIndex - pList->WindowIndex;			//������λ��
			if (Cursor < 0)				//�������Ѿ��Ƴ���Ļ��
			{
				pList->WindowIndex --;	//�򴰿�������1���б������ƶ�
			}
		}
	}
}

void Menu_Down(void)			//�¼�����
{
	struct Menu_List *pList = ListStack[pListStack];
	
	struct Menu_Item *Item = &pList->Items[pList->ItemIndex];
	
	if (Item->NumSetFlag)		//��ǰ��������ֵģʽ
	{
		Item->Num += Item->NumStep;		//����ǰ�����ֵ
		if (Item->Num > Item->NumMax)
		{
			Item->Num = Item->NumMax;
		}
	}
	else					//���򣬼���ǰ����������ֵģʽ
	{
		pList->ItemIndex ++;			//��Ŀ������1
		
		if (pList->ItemIndex >= pList->ItemLength)			//����Ѿ������һ����
		{
			pList->ItemIndex = 0;		//��Ŀ��������
			pList->WindowIndex = 0;		//������������
		}
		else							//���򣬼�û���Ƶ����һ��
		{
			int16_t Cursor = pList->ItemIndex - pList->WindowIndex;			//������λ��
			if (Cursor >= pList->WindowLength)				//�������Ѿ��Ƴ���Ļ��
			{
				pList->WindowIndex ++;	//�򴰿�������1���б������ƶ�
			}
		}
	}
}

void Menu_Enter(void)			//ȷ�ϼ�����
{
	/*�ҵ�����ȷ�ϼ�����Ŀ*/
	struct Menu_Item *Item = &ListStack[pListStack]->Items[ListStack[pListStack]->ItemIndex];
	
	if (Item->Function != 0)
	{
		Item->Function();		//����ж��幦�ܺ�������ִ�й��ܺ���
	}
	if (Item->Type == 1)		//�����Ŀ������1�������ͣ�
	{
		Item->ON_OFF = !Item->ON_OFF;			//���л�����ֵ
	}
	else if (Item->Type == 2)	//�����Ŀ������2����ֵ�ͣ�
	{
		Item->NumSetFlag = !Item->NumSetFlag;	//���л�������ֵģʽ��־λ
	}
	else						//���򣬼�����Ϊ��ͨ��Ŀ
	{
		if (Item->NextList != 0)				//���������һ���б�
		{
			pListStack ++;		//�б���
			ListStack[pListStack] = Item->NextList;		//���󶨵���һ���б���ʾ�ڵ�ǰ�б�֮��
		}
	}
}

void Menu_Back(void)			//���ؼ�����
{
	/*�ҵ����·��ؼ�����Ŀ*/
	struct Menu_Item *Item = &ListStack[pListStack]->Items[ListStack[pListStack]->ItemIndex];
	
	/*ȡ��������ֵģʽ��־λ*/
	Item->NumSetFlag = 0;
	
	/*����б�����ֹһ�㣬���Ƴ����ϲ��б�������һ���б�*/
	if (pListStack > 0)
	{
		pListStack --;
	}
}

void Menu_Init(void)
{
	/*�б�ģʽ��ʼ��*/
	ListStack[pListStack] = &ParamMenu;
	
	OLED_Clear();
	OLED_Update();
}

void Menu_Loop(void)
{
	if (Key_Check(1, KEY_SINGLE))		//K1����
	{
		Menu_Up();						//ִ���б��ϼ����µĹ���
	}
	if (Key_Check(2, KEY_SINGLE))		//K2����
	{
		Menu_Down();					//ִ���б��¼����µĹ���
	}
	if (Key_Check(3, KEY_SINGLE))		//K3����
	{
		Menu_Enter();					//ִ���б�ȷ�ϼ����µĹ���
	}
	if (Key_Check(4, KEY_SINGLE))		//K4����
	{
		Menu_Back();					//ִ���б��ؼ����µĹ���
		nextSystemStage = SYSTEM_STAGE_DAOLIBAI_CONTROL;				//Ŀǰֻ�õ���һ���б����ֱ�ӻ��˵�������ģʽ
	}
	
	Menu_Display();						//��ʾ�б�
}

void Menu_Exit(void)
{
	
}
