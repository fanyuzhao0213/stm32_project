#ifndef __PID_H
#define __PID_H



#define ITERM_MAX  500.0f   // ���������ֵ���ɵ�
#define ITERM_MIN -500.0f


typedef struct {
    float PID_Target;     // Ŀ��ֵ
    float PID_Actual;     // ʵ��ֵ
    float PID_PwmOut;        // PID���

    float PID_Kp;         // ����ϵ��
    float PID_Ki;         // ����ϵ��
    float PID_Kd;         // ΢��ϵ��

    float PID_Error;      // �������
    float PID_LastError;  // �ϴ����
    float PID_ErrorSum;   // ������

    float PID_OutMax;     // �������
    float PID_OutMin;     // �������
} PID_t;


// ���� PID �ṹ�壨λ�û�+�ٶȻ���
typedef struct {
    PID_t PositionPID; // �⻷
    PID_t SpeedPID;    // �ڻ�
} CascadePID_t;



// === �������ܺ��� ===
void PID_Init(PID_t *pid, float kp, float ki, float kd, float outMax, float outMin);
void PID_Reset(PID_t *pid);
void PID_SetTunings(PID_t *pid, float kp, float ki, float kd);

// === ���㺯�� ===
void PID_Update(PID_t *pid);					// ���� PID ����
void CascadePID_Update(CascadePID_t *cpid);		// ���� PID ����
#endif


