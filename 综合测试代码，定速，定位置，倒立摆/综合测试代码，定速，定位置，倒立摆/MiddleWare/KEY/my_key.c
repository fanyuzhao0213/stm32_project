#include "my_key.h"
#include "OLED.h"
#include "main.h"

/// @brief ������־���飨ÿ��λ����һ��״̬���� KEY_DOWN��KEY_UP �ȣ�
uint8_t Key_Flag[KEY_COUNT];


/// @brief ��ȡָ��������ǰ״̬���Ƿ��£�
/// @param n �������
/// @return KEY_PRESSED �� KEY_UNPRESSED
uint8_t Key_GetState(uint8_t n)
{
    if (n == KEY_1)
    {
        return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == 0 ? KEY_PRESSED : KEY_UNPRESSED;
    }
    else if (n == KEY_2)
    {
        return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == 0 ? KEY_PRESSED : KEY_UNPRESSED;
    }
    if (n == KEY_3)
    {
        return HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == 0 ? KEY_PRESSED : KEY_UNPRESSED;
    }
    else if (n == KEY_4)
    {
        return HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == 0 ? KEY_PRESSED : KEY_UNPRESSED;
    }
    return KEY_UNPRESSED;
}

/// @brief ��鰴���Ƿ񴥷����ض��¼����� KEY_DOWN��KEY_LONG��
/// @param n �������
/// @param Flag Ҫ���İ����¼�����
/// @return 1: �¼�������0: δ����
uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
    if (Key_Flag[n-1] & Flag)
    {
        if (Flag != KEY_HOLD)  // HOLD ״̬�����
        {
            Key_Flag[n-1] &= ~Flag;
        }
        return 1;
    }
    return 0;
}

/**
  * ��    ����������������¼���־λ
  * ��    ������
  * �� �� ֵ����
  */
void Key_Clear(void)
{
	uint8_t i;
	for (i = 1; i < 5; i ++)
	{
		Key_Flag[i] = 0;		//�������K1��K2��K3��K4���е��¼���־λ
	}
}

/// @brief ����״̬ɨ�����¼�ʶ�𣬽���ÿ 5~10ms ����һ��
void Key_Tick(void)
{
    static uint8_t count = 0;
    static uint8_t curr_state[KEY_COUNT] = {0};
    static uint8_t prev_state[KEY_COUNT] = {0};
    static uint8_t step[KEY_COUNT] = {0};       // ״̬��״̬��0~4
    static uint16_t timer[KEY_COUNT] = {0};     // ����;��ʱ��

    for (uint8_t i = 0; i < KEY_COUNT; i++)
    {
        if (timer[i] > 0)
        {
            timer[i]--;
        }
    }

    count++;
    if (count >= 20)  // ��Լ 20ms��20 * 1ms��
    {
        count = 0;

        for (uint8_t i = 0; i < KEY_COUNT; i++)
        {
            prev_state[i] = curr_state[i];		//��ǰ��״̬��ֵ���ϴ�״̬����ط���������
            curr_state[i] = Key_GetState(i);

            // ����״̬������λ
            if (curr_state[i] == KEY_PRESSED)			//������£����ñ��ֱ�־λ
                Key_Flag[i] |= KEY_HOLD;
            else
                Key_Flag[i] &= ~KEY_HOLD;				//����ɿ������ñ��ֱ�־λ

            // ���ؼ�⣨���£��������ǰ�ǰ��£���һ����û���£�����Ϊ�����ǰ��µ�״̬
            if (curr_state[i] == KEY_PRESSED && prev_state[i] == KEY_UNPRESSED)
                Key_Flag[i] |= KEY_DOWN;				//���ؼ�⣨���£�

            // ���ؼ�⣨�ɿ���
            if (curr_state[i] == KEY_UNPRESSED && prev_state[i] == KEY_PRESSED)
                Key_Flag[i] |= KEY_UP;

            // ״̬������֧�ֵ�����˫����������������
            switch (step[i])
            {
                case 0: // �ȴ�����
                    if (curr_state[i] == KEY_PRESSED)
                    {
                        timer[i] = KEY_TIME_LONG;		//���ó���ʱ�䣬��ʼ����ʱ
                        step[i] = 1;
                    }
                    break;

                case 1: // ����Ƿ��ɿ���ʱ��������
                    if (curr_state[i] == KEY_UNPRESSED)
                    {
                        timer[i] = KEY_TIME_DOUBLE;		//����˫����ʱ����ʼ����ʱ
                        step[i] = 2;  // �ȴ��ڶ��ΰ���
                    }
                    else if (timer[i] == 0)
                    {
                        Key_Flag[i] |= KEY_LONG;
                        timer[i] = KEY_TIME_REPEAT;
                        step[i] = 4;  // ��������ģʽ
                    }
                    break;

                case 2: // �ȴ��ڶ��ΰ��£�˫���жϣ�
                    if (curr_state[i] == KEY_PRESSED)
                    {
                        Key_Flag[i] |= KEY_DOUBLE;
                        step[i] = 3;  // �ȴ��ͷ�
                    }
                    else if (timer[i] == 0)
                    {
                        Key_Flag[i] |= KEY_SINGLE;
                        step[i] = 0;
                    }
                    break;

                case 3: // �ڶ��ΰ��º�ȴ��ͷ�
                    if (curr_state[i] == KEY_UNPRESSED)
                    {
                        step[i] = 0;
                    }
                    break;

                case 4: // ��������״̬
                    if (curr_state[i] == KEY_UNPRESSED)
                    {
                        step[i] = 0;
                    }
                    else if (timer[i] == 0)
                    {
                        Key_Flag[i] |= KEY_REPEAT;
                        timer[i] = KEY_TIME_REPEAT;  // ��װ��ʱ��
                    }
                    break;
            }
        }
    }
}


void my_key_test_func(void)
{
	for (int i = 0; i < KEY_COUNT; i++)
	{
		if (Key_Check(i, KEY_DOWN))
		{
			OLED_Printf(0, 16, OLED_6X8, "KEY[%d]: DOWN", i);
			OLED_Update();
		}
		if (Key_Check(i, KEY_UP))
		{
			OLED_Printf(0, 24, OLED_6X8, "KEY[%d]: UP  ", i);
			OLED_Update();
		}
		if (Key_Check(i, KEY_SINGLE))
		{
			OLED_Printf(0, 32, OLED_6X8, "KEY[%d]: CLICK ", i);
			OLED_Update();
		}
		if (Key_Check(i, KEY_DOUBLE))
		{
			OLED_Printf(0, 40, OLED_6X8, "KEY[%d]: DOUBLE", i);
			OLED_Update();
		}
		if (Key_Check(i, KEY_LONG))
		{
			OLED_Printf(0, 48, OLED_6X8, "KEY[%d]: LONG  ", i);
			OLED_Update();
		}

		if (Key_Check(i, KEY_REPEAT))
		{
			OLED_Printf(0, 56, OLED_6X8, "KEY[%d]: REPEAT", i);
			OLED_Update();
		}
	}
	HAL_Delay(20); // �����������̫��
}

uint8_t my_get_key_num(void)
{
	int i;
    for (i = 1; i < KEY_COUNT; i++)
    {
        if (Key_Check(i, KEY_DOWN))
        {
//            OLED_Clear();  // ��ѡ�������Ļ��������жѵ�
//            OLED_Printf(0, 16, OLED_6X8, "KEY[%d] DOWN", i);
//            OLED_Update();
            break; // �ҵ�һ�����µļ����˳�
        }
    }
	return i;
}
