#include "headfile.h"
#include "OLED.h"
#include "isr.h"
#include "Keyboard.h"

//8.19

int main(void)
{
    get_clk();
    
    AT_Init();
    
    PWM_Init();
    
    OLED_Init();
    
    Key_IO_Init();
    
    Blue_Tooth_Init();
    
    Tracking_Sensor_Init();
    
    pit_init_ms( pit0 , 2 );
    set_irq_priority( PIT_CH0_IRQn,1 );
    enable_irq( PIT_CH0_IRQn );
    
//    Motor_Start();
    
//    gpio_init(E1,GPO,1);
    
    while(1)
    {
     
     if( STOP_Distance==1 )   //达到一定速度，走一段距离后检测停车
     {
        MAG_STOP() ;
        Chase_STOP(); 
     }
//     if(Car_Speed>130)
//     {
//
//     }
     
//      Gear_Protect();
//     Start_Car();
     OLED_ConfigParameter();         //User Interface
     
     /*FXOS8700_DataRead(&FXOS8700_Data, 1);
     pit_delay_ms(pit1,10);
     send_flag=5;*/
     



      switch(send_flag)
      {
        case 1:
                Out_Data[0] = SystemAttitude.Pitch;
                Out_Data[1] = SystemAttitude.Roll;
                Out_Data[2] = SystemAttitude.Yaw;  //Yaw角的数据均为左正右负
                Out_Data[3] = 0;
                send_flag=0;
                OutPut_Data(Out_Data);
                break;
        case 2:
                Out_Data[0] = Judge_flag;//Mag_flag;//Dir_Error_PID.Error_Rate;//Roundabout_flag;106
                Out_Data[1] = Tracking_Sensor_Data_R_OBS;//Tracking_Sensor_Data_R_OBS;//Car_Speed_Min;//In_times;
                Out_Data[2] = Tracking_Sensor_Data_B_OBS;//Judge_flag;//Difference;//Tracking_Sensor_Data_R_OBS;
                Out_Data[3] = Tracking_Sensor_Data_A_OBS;//In_Pulses;//Out_times;//Tracking_Sensor_Data_M_OBS;//Roundabout_ave;
                send_flag=0;
                OutPut_Data(Out_Data);
                break;
        case 3 :
                Out_Data[0] = Car_Speed_Min;
                Out_Data[1] = MOTOR_Speed_Left;
                Out_Data[2] = MOTOR_Speed_Right;
                Out_Data[3] = Gyro_PID.iError;
                send_flag=0;
                OutPut_Data(Out_Data);
                break;
        case 4 :
                Out_Data[0] = Car_Speed;//Dir_Gyro_PID.fP;//SystemAttitudeRate.Yaw;   //+-500波动不大
                Out_Data[1] = Speed_Ratio;//Dir_Gyro_PID.fD;//Car_Speed; //Dir_Outside_Out;               //+-180
                Out_Data[2] = Dir_Gyro_PID.Error_Rate;//Dir_Gyro_PID.Error_Rate;
                Out_Data[3] = Gyro_BufferData.Input_Butter[1];//Dir_Outside_Out - SystemAttitudeRate.Yaw; //e2/[kp]
                send_flag=0;
                OutPut_Data(Out_Data);
                break;
        case 5 :
                Out_Data[0] = Car_Speed;//SystemAttitude. Pitch;//   //+-500波动不大
                Out_Data[1] = FXOS8700_Data.MAGXdata;//Car_Speed; //Dir_Outside_Out;               //+-180
                Out_Data[2] = FXOS8700_Data.MAGYdata;//Dir_Gyro_PID.Error_Rate;
                Out_Data[3] = FXOS8700_Data.MAGZdata;//Dir_Outside_Out - SystemAttitudeRate.Yaw; //e2/[kp]
                send_flag=0;
                OutPut_Data(Out_Data);
                break;
        default: break;
      }
       
       
   }
   
}


