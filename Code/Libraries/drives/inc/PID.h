
#include "headfile.h"

#define KP 0
#define KI 1
#define KD 2
#define KT 3
#define KB 4
#define KF 5

typedef struct PID
{ 
        float iError;
	float SumError;	//ŒÛ≤Ó¿€º∆	
	float LastError;	//Error[-1]
	float PrevError;	//Error[-2]	
	float LastData;	//Speed[-1]
        float Error_Rate;
        float fP;
        float fD;
} PID;

typedef struct
{
 float Input_Butter[3];
 float Output_Butter[3];
}Butter_BufferData;   

typedef struct
{
 const float a[3];
 const float b[3];
}Butter_Parameter;   //

extern Butter_BufferData Gyro_BufferData;
extern Butter_Parameter  Gyro_Parameter;
extern Butter_Parameter Control_Device_Div_LPF_Parameter;
extern Butter_Parameter Control_Device_Err_LPF_Parameter;

extern float Data_Observe[2];

float PID_Realize(PID *sprt, float *PID, float NowData, float Point);
float PID_Increase(PID *sprt, float *PID, float NowData, float Point);
float PlacePID_Control(PID *sprt, float *PID, float NowPiont, float SetPoint);

float Control_Device_LPF(float curr_inputer,Butter_BufferData *Buffer,Butter_Parameter *Parameter);
float PID_Butterworth(PID *sprt, float *PID, float NowData, float Point);