#include "common.h"
#include "headfile.h"
#include "Keyboard.h"

/***************************** Paramters Of Tracking_Sensor*****************************************/


float Tracking_ADC_Val_Sum = 0;
float Tracking_ADC_Val_Sub = 0;
float Tracking_ADC_Val_Error = 0;
float Tracking_ADC_Val_Error_Set[10] = {0};                  //Inductor's Error Set

int32 Tracking_ADC_Val_Original_L[10] = {0};                  //Original Value Of Inductor
int32 Tracking_ADC_Val_Original_R[10] = {0};
int32 Tracking_ADC_Val_Original_M[10] = {0};
int32 Tracking_ADC_Val_Original_A[10] = {0};
int32 Tracking_ADC_Val_Original_B[10] = {0};


int16  Weight_Array[10]={4,6,7,8,10,15,20,30,200,700};
static int16  Weighted_Ration_add = 1000;

int32 Tracking_ADC_Weight_Sum_L = 0;                          //Weight Accumulation Of Inductor
int32 Tracking_ADC_Weight_Sum_R = 0;
int32 Tracking_ADC_Weight_Sum_M = 0;
int32 Tracking_ADC_Weight_Sum_A = 0;
int32 Tracking_ADC_Weight_Sum_B = 0;

int32 Tracking_Sensor_Data_L = 0;                             //Using Value Of Inductor
int32 Tracking_Sensor_Data_R = 0;                             
int32 Tracking_Sensor_Data_M = 0; 
int32 Tracking_Sensor_Data_A = 0; 
int32 Tracking_Sensor_Data_B = 0; 


int32 Tracking_Sensor_Data_L_Temp[10] = {0};                  //Temporary Using Value Array
int32 Tracking_Sensor_Data_R_Temp[10] = {0};
int32 Tracking_Sensor_Data_M_Temp[10] = {0};
int32 Tracking_Sensor_Data_A_Temp[10] = {0};
int32 Tracking_Sensor_Data_B_Temp[10] = {0};


int32 Tracking_Sensor_Data_L_OBS = 0;                         //Observation Of Inductor's Using Value
int32 Tracking_Sensor_Data_R_OBS = 0;                             
int32 Tracking_Sensor_Data_M_OBS = 0;
int32 Tracking_Sensor_Data_A_OBS = 0;
int32 Tracking_Sensor_Data_B_OBS = 0;


uint8 Round_num=0;
uint8 Roundabout_flag=0;
uint8 Right_Round=0,Left_Round=0;
uint8 Judge_flag=0,In_End=0;
int In_Pulses=0,Out_Pulses=0;
float Out_Ratio=0;
uint32 Test_Pulses=0;
uint8 Mag_Sign_flag=0, Mag_lost_times=0;
float In_rate = 0.175,OUT_rate = 0.4;


void Tracking_Sensor_Init(void)
{
    adc_init (Tracking_Sensor_L);
    adc_init (Tracking_Sensor_R);
    //adc_init (Tracking_Sensor_M);
    adc_init (Tracking_Sensor_A);
    adc_init (Tracking_Sensor_B);
    
//    adc_stop();
}
int q = 0;
void Tracking_Sensor_Detect(void)
{

  int i=0,j=0;
  
  for(i=0;i<9;i++)
  {
    
    Tracking_ADC_Val_Original_L[i] = Tracking_ADC_Val_Original_L[i+1];
    Tracking_ADC_Val_Original_R[i] = Tracking_ADC_Val_Original_R[i+1];
    Tracking_ADC_Val_Original_M[i] = Tracking_ADC_Val_Original_M[i+1];
    Tracking_ADC_Val_Original_A[i] = Tracking_ADC_Val_Original_A[i+1];
    Tracking_ADC_Val_Original_B[i] = Tracking_ADC_Val_Original_B[i+1];

  
  }
  
  Tracking_ADC_Val_Original_L[9] = adc_once(Tracking_Sensor_L,ADC_12bit);
  Tracking_ADC_Val_Original_R[9] = adc_once(Tracking_Sensor_R,ADC_12bit);
  Tracking_ADC_Val_Original_M[9] = adc_once(Tracking_Sensor_M,ADC_12bit);
  Tracking_ADC_Val_Original_A[9] = adc_once(Tracking_Sensor_A,ADC_12bit); 
  Tracking_ADC_Val_Original_B[9] = adc_once(Tracking_Sensor_B,ADC_12bit); 

    
//  adc_stop();

  for(j=0;j<10;j++)
  {
    Tracking_ADC_Weight_Sum_L += Tracking_ADC_Val_Original_L[j] * Weight_Array[j];
    Tracking_ADC_Weight_Sum_R += Tracking_ADC_Val_Original_R[j] * Weight_Array[j];
    Tracking_ADC_Weight_Sum_M += Tracking_ADC_Val_Original_M[j] * Weight_Array[j];
    Tracking_ADC_Weight_Sum_A += Tracking_ADC_Val_Original_A[j] * Weight_Array[j];
    Tracking_ADC_Weight_Sum_B += Tracking_ADC_Val_Original_B[j] * Weight_Array[j];
   
  }
  
   Tracking_Sensor_Data_L = Tracking_ADC_Weight_Sum_L / Weighted_Ration_add; 
   Tracking_Sensor_Data_R = Tracking_ADC_Weight_Sum_R / Weighted_Ration_add; 
   Tracking_Sensor_Data_M = Tracking_ADC_Weight_Sum_M / Weighted_Ration_add;
   Tracking_Sensor_Data_A = Tracking_ADC_Weight_Sum_A / Weighted_Ration_add;
   Tracking_Sensor_Data_B = Tracking_ADC_Weight_Sum_B / Weighted_Ration_add;
   
   
   
   for(i=0;i<10;i++)
   {
     
     Tracking_Sensor_Data_L_Temp[i] = Tracking_Sensor_Data_L_Temp[i+1];
     Tracking_Sensor_Data_R_Temp[i] = Tracking_Sensor_Data_R_Temp[i+1];
     Tracking_Sensor_Data_M_Temp[i] = Tracking_Sensor_Data_M_Temp[i+1];
     Tracking_Sensor_Data_A_Temp[i] = Tracking_Sensor_Data_A_Temp[i+1];
     Tracking_Sensor_Data_B_Temp[i] = Tracking_Sensor_Data_B_Temp[i+1];
   }
    Tracking_Sensor_Data_L_Temp[9] = Tracking_Sensor_Data_L ;
    Tracking_Sensor_Data_R_Temp[9] = Tracking_Sensor_Data_R ;
    Tracking_Sensor_Data_M_Temp[9] = Tracking_Sensor_Data_M ; 
    Tracking_Sensor_Data_A_Temp[9] = Tracking_Sensor_Data_A ; 
    Tracking_Sensor_Data_B_Temp[9] = Tracking_Sensor_Data_B ; 
    
    
    
    Tracking_Sensor_Data_L_OBS = Tracking_Sensor_Data_L;     //ADC值滑动赋值
    Tracking_Sensor_Data_R_OBS = Tracking_Sensor_Data_R;
    Tracking_Sensor_Data_M_OBS = Tracking_Sensor_Data_M;
    Tracking_Sensor_Data_A_OBS = Tracking_Sensor_Data_A;
    Tracking_Sensor_Data_B_OBS = Tracking_Sensor_Data_B;

    /***********************************/  //Class2:0.3&0.7进
    if(Judge_flag==1)
    {
      Class=3;
//      Dir_Error[Class][2]=10;   //6.13  4L  6B
      Tracking_ADC_Val_Sub =  sqrt( In_rate*Tracking_Sensor_Data_L + (1-In_rate)*Tracking_Sensor_Data_B ) - sqrt( In_rate * Tracking_Sensor_Data_R + (1-In_rate) * Tracking_Sensor_Data_A ) ;
      Tracking_ADC_Val_Sum = In_rate * ( Tracking_Sensor_Data_L + Tracking_Sensor_Data_R ) + (1-In_rate) * (Tracking_Sensor_Data_A + Tracking_Sensor_Data_B );
    }
    /*else
    {
      Tracking_ADC_Val_Sub = sqrt( 0.75*Tracking_Sensor_Data_L + 0.25*Tracking_Sensor_Data_B) - sqrt( 0.75*Tracking_Sensor_Data_R + 0.25*Tracking_Sensor_Data_A  );
      Tracking_ADC_Val_Sum = 0.75*(Tracking_Sensor_Data_L + Tracking_Sensor_Data_R )+ 0.25*(Tracking_Sensor_Data_A + Tracking_Sensor_Data_B );
    }*/
    else if(Judge_flag==2)  //出
    {
      Class=3;
//      Dir_Error[Class][2]=10;
//      Tracking_ADC_Val_Sub = (sqrt( Tracking_Sensor_Data_L-0.1*Tracking_Sensor_Data_B ) - sqrt(Tracking_Sensor_Data_R-0.1*Tracking_Sensor_Data_A)  );
//      Tracking_ADC_Val_Sum = (Tracking_Sensor_Data_L-0.1*Tracking_Sensor_Data_B + Tracking_Sensor_Data_R-0.1*Tracking_Sensor_Data_A );
      Tracking_ADC_Val_Sub =  ( sqrt( OUT_rate *Tracking_Sensor_Data_L) - sqrt( OUT_rate * Tracking_Sensor_Data_R) )  ;   //0.8  Class3
      Tracking_ADC_Val_Sum = OUT_rate * (Tracking_Sensor_Data_L + Tracking_Sensor_Data_R);
    }
    else if(Judge_flag==0)   //直道
    {
      Class=3;
//      Dir_Error[Class][2]=10;
      Tracking_ADC_Val_Sub = (  sqrt( Tracking_Sensor_Data_L ) - sqrt(Tracking_Sensor_Data_R)  );
      Tracking_ADC_Val_Sum = (Tracking_Sensor_Data_L + Tracking_Sensor_Data_R );
      /*Tracking_ADC_Val_Sub = sqrt( 0.9*Tracking_Sensor_Data_L + 0.1*Tracking_Sensor_Data_B) - sqrt( 0.9*Tracking_Sensor_Data_R + 0.1*Tracking_Sensor_Data_A ) ;
      Tracking_ADC_Val_Sum = 0.9*(Tracking_Sensor_Data_L + Tracking_Sensor_Data_R )+ 0.1*(Tracking_Sensor_Data_A + Tracking_Sensor_Data_B );*/
//        Tracking_ADC_Val_Sub =  sqrt( 0.2*Tracking_Sensor_Data_L+0.8*Tracking_Sensor_Data_B ) - sqrt( 0.2*Tracking_Sensor_Data_R + 0.8 * Tracking_Sensor_Data_A ) ;
//        Tracking_ADC_Val_Sum = 0.2 * ( Tracking_Sensor_Data_L + Tracking_Sensor_Data_R ) + 0.8 * (Tracking_Sensor_Data_A + Tracking_Sensor_Data_B );
    }
    
/**************************************/
//      Tracking_ADC_Val_Sub =  sqrt( 0.3*Tracking_Sensor_Data_L+0.7*Tracking_Sensor_Data_B ) - sqrt( 0.3 *Tracking_Sensor_Data_R + 0.7 * Tracking_Sensor_Data_A) ;
//      Tracking_ADC_Val_Sum = 0.3 * ( Tracking_Sensor_Data_L + Tracking_Sensor_Data_R ) + 0.7 * (Tracking_Sensor_Data_A + Tracking_Sensor_Data_B );
      Tracking_ADC_Val_Error = 10000 * Tracking_ADC_Val_Sub / Tracking_ADC_Val_Sum;  //差比和
  
  if(Tracking_ADC_Val_Error >= 500)
  {
   Tracking_ADC_Val_Error = 500;
  }
  if(Tracking_ADC_Val_Error <= -500)
  {
   Tracking_ADC_Val_Error = -500;
  }
  
  /*if(Judge_flag==2)
  {
    if(Tracking_ADC_Val_Error>=80)
      Tracking_ADC_Val_Error=80;
    else if(Tracking_ADC_Val_Error<=-80)
      Tracking_ADC_Val_Error=-80;
  }*/
  
  //中线值滑动赋值
   for(i=0;i<9;i++)
 {
   Tracking_ADC_Val_Error_Set[i] = Tracking_ADC_Val_Error_Set[i+1];
 }
  
  Tracking_ADC_Val_Error_Set[9] = Tracking_ADC_Val_Error;
  
  Tracking_ADC_Weight_Sum_L = 0;
  Tracking_ADC_Weight_Sum_R = 0;
  Tracking_ADC_Weight_Sum_M = 0;
  Tracking_ADC_Weight_Sum_A = 0;
  Tracking_ADC_Weight_Sum_B = 0;
  
  
  Mag_Sign();
  
  send_flag=2;
}


//87°1200  1350
//85°1000  1300  1200
//必先判断环的方向

int LR=1300,BA_IN=550,BA_OUT=600;
uint8 In_or_Not = 1;

void Cal_Roundabout(void)
{
    if( (Tracking_Sensor_Data_L_Temp[9]>LR) && (Tracking_Sensor_Data_R_Temp[9]>LR) && (Tracking_Sensor_Data_B_Temp[9]<BA_IN) && (Tracking_Sensor_Data_A_Temp[9]<BA_IN) && (Judge_flag ==0 )  )
    {
      Judge_flag = 1;
    }
    else if( (Judge_flag==1) && (In_End==1) && (Out_Pulses==0) && (Tracking_Sensor_Data_B_Temp[9]>BA_OUT) && (Tracking_Sensor_Data_A_Temp[9]>BA_OUT) )
//    else if(Judge_flag_Sensor_Data_B_Temp[9]>600) && (Tracking_Sensor_Data_A_Temp[9]>600) )||
//           ( (Tracking==1 && (In_End==1) && (Out_Pulses==0) && 
//         ( ( (Tracking_Sensor_Data_L_Temp[9]>2000) || (Tracking_Sensor_Data_R_Temp[9]>2000) )  )  )
   //else if( (Judge_flag==1) && (In_End==1) && (Out_Pulses==0) && ( ((Tracking_Sensor_Data_A_Temp[9]>400)&&(Tracking_Sensor_Data_B_Temp[9]>800))||((Tracking_Sensor_Data_A_Temp[9]>800) &&(Tracking_Sensor_Data_B_Temp[9]>400)) )  )
    {
      In_Pulses=0;
      In_End=0;
      Judge_flag = 2 ;
//      gpio_set(BeepPin,1);
    }
    
    if(In_Pulses>=6500)  //进结束
    {
      In_End=1;
      In_Pulses=0;
    }
    if(Out_Pulses>=12000)  //出防误判结束
    {
      Out_Pulses=0;
      Judge_flag=0;
//      gpio_set(BeepPin,0);
    }

}


void Mag_Sign(void)
{
  if(Mag_lost_times)
  {
      if( (abs(Tracking_Sensor_Data_L_OBS)<20) && (abs(Tracking_Sensor_Data_R_OBS)<20)  )
      {
         Mag_lost_times++;
         
        if(Mag_lost_times==10)
       {
         Mag_lost_times=0;
         Mag_Sign_flag=0;
       }
         return;
      }
      else
      {
        Mag_lost_times=0;
        return;
      }

      
  }
  
  if( (abs(Tracking_Sensor_Data_L_OBS)>0) && (abs(Tracking_Sensor_Data_R_OBS)>0)  )
  {
    Mag_Sign_flag=1;
  }
  else
  {
    Mag_lost_times++;
  }    



}


