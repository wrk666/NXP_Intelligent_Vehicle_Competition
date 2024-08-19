#include "common.h"
#define Tracking_Sensor_L            ADC0_SE12                 

#define Tracking_Sensor_R            ADC0_SE13          //15
  
#define Tracking_Sensor_M            ADC0_SE6          //13

#define Tracking_Sensor_A            ADC0_SE14          //13

#define Tracking_Sensor_B            ADC0_SE15         //13

extern int32 Tracking_Sensor_Data_L_Temp[10];                  //Temporary Using Value Array
extern int32 Tracking_Sensor_Data_R_Temp[10];
extern int32 Tracking_Sensor_Data_M_Temp[10];
extern int32 Tracking_Sensor_Data_A_Temp[10];
extern int32 Tracking_Sensor_Data_B_Temp[10];

extern float Tracking_ADC_Val_Error;
extern float Tracking_ADC_Val_Error_Set[10];                    //Inductor's Error Set
extern int32 Tracking_Sensor_Data_M_Temp[10];
extern int32 Tracking_Sensor_Data_L_OBS;                         //Observation Of Inductor's Using Value
extern int32 Tracking_Sensor_Data_R_OBS;                             
extern int32 Tracking_Sensor_Data_M_OBS;
extern int32 Tracking_Sensor_Data_A_OBS,Tracking_Sensor_Data_B_OBS;
extern uint8 Round_num;
extern uint8 Roundabout_flag;
extern int In_times,Out_times;
extern uint8 Right_Round,Left_Round;
extern uint8 Judge_flag,In_End;
extern int In_Pulses,Out_Pulses;
extern uint32 Test_Pulses;
extern uint8 Mag_Sign_flag;
extern int LR,BA_IN,BA_OUT;
extern float In_rate ,OUT_rate;
extern uint8 In_or_Not;

extern uint8 Round_Flag;
void Tracking_Sensor_Init(void);
void Tracking_Sensor_Detect(void);
void Cal_Roundabout(void);
void Difference_Cal(void);
void Round_Deal(void);
void Mag_Sign(void);