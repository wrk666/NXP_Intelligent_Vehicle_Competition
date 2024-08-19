#include "isr.h"
#include "headfile.h"
#include "Keyboard.h"

int t = 0;
int Out_Data[4]={0};
uint8 send_flag=0,
      speed_flag=0;
int stop_count=0;

void PIT_CH1_IRQHandler(void)
{
    PIT_FlAG_CLR(pit1);

}

void IRQ_IRQHandler(void)
{
    CLEAR_IRQ_FLAG;
    
}


void KBI0_IRQHandler(void)
{
    CLEAN_KBI0_FLAG;
    
}

void PIT_CH0_IRQHandler(void)
{ 
   
   PIT_FlAG_CLR(pit0);
   
   Bal_Time_Flag ++;
   Speed_Time_Flag ++;
   
   /*Dir_PWM_Output_Period++;
   Direction_PWM_Output();*/
   
   Angle_Fusion();
//   send_flag=1;
       
   
/*Balance*/

   Bal_Control();   //Inside_4ms  Outside_12ms
   
/*Speed*/

   if(Speed_Time_Flag >= 50)  //100ms speed control
      {         
        Speed_Control();
        Speed_Time_Flag = 0;
      }
   
/*Direction*/
   Tracking_Sensor_Detect();
   
   if(In_or_Not)
   {
    Cal_Roundabout(); 
   }
   
   Difference = 0.8 * Tracking_ADC_Val_Error_Set[9] + 0.12 * Tracking_ADC_Val_Error_Set[8] + 0.08 * Tracking_ADC_Val_Error_Set[7];  //+-400

//   Round_Deal();
   
   //send_flag=2;
   Dir_Time_Flag ++;
   Direction_Control();  //Inside_4ms  Outside_12ms
   
   if( PWM_flag>0 && Mag_Sign_flag>0 )
    {
      stop_count++;
      SET_PWM(Bal_PWM_Out , Direction_PWM_Out );//Bal_PWM_Out    Direction_PWM_Out
    }
   else if(Motor_Stop_Flag==1)
   {
     SET_PWM( 0 , Direction_PWM_Out );
   }
   
   
}
