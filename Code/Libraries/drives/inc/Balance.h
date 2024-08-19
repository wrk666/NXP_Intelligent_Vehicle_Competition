#ifndef __BALANCE_H__
#define __BALANCE_H__

#include "headfile.h"

extern float Tar_Ang_Vel,Target_Angle;
extern PID Angle_PID, Gyro_PID, Speed_PID, Dir_Error_PID, Dir_Gyro_PID;
extern float Angle[4], Bal_Gyro[4], Speed[4], Dir_Error[8][3], Dir_Gyro[4] , Gyro_Ratio[5]; 

extern uint8 Speed_Ratio;

extern int16 time_set;
/***************************** Balance Parameter*****************************/ 

extern float Bal_PWM_Out,Increase;
extern vuint8 Bal_Time_Flag;
void Bal_Control(void);

/***************************** Speed Parameter*****************************/ 
extern int16 Last_Speed_PWM;
extern int16 Speed_PWM;
extern float Last_Car_Speed;             
extern int16 Speed_Set;     
extern vuint8  Speed_Time_Flag ;         
extern vint16   Speed_PWM_Output_Period ; 
extern int16 Car_Speed_Min;
extern uint8 speed_each,run_flag;
extern uint8 Ang_put;
void Speed_Control(void);
void Speed_PWM_Output(void);

/***************************** Direction Parameter*****************************/
extern float Direction_PID_Kp;
extern float Direction_PID_Kd;
extern float AngleSpeed_X;
extern uint8 Dir_PWM_Output_Period;
extern int16 Direction_PWM_Out;
extern int32 Direction_Parameter;
extern float Radius;
extern vuint8 Dir_Time_Flag;
extern int16 Direction_PWM;
extern float Indside_Error_Ratio;
extern float Outside_Errer_Ratio;
extern float Gyro_Inside_Erate;

void Direction_Control(void);
void Direction_PWM_Output(void);

extern float Difference,Difference_old;  //ÖÐÏß
extern float Dir_Outside_Out;
#endif