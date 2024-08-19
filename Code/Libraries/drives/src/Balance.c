#include "headfile.h"
#include "Keyboard.h"

/*PID*/
PID Angle_PID,
    Gyro_PID,
    Speed_PID,
    Dir_Error_PID,
    Dir_Gyro_PID;

float Angle[4] =     { 30  , 0 , 0 , 0 };         //Bal outside loop
float Bal_Gyro[4] =  { 30  , 0 , 0 , 0 };         //Bal inside loop
float Speed[4] =     { 30  , 0 , 0 , 0 };         //Speed loop

int16 time_set=4000;

//100 400->1.37
//90  400->2.31
//80  400->3.32
//74  400->3.96
// 68   400->4.66
//  60  400->5.72
//52 400->6.98
float Dir_Error[8][3] = { { 3800,  1.37 ,10  },    //0->100
                           { 3800 , 2.31, 10 },    //1->90
                           { 3800 , 2.80, 10 },    //2->85
                           { 3800 , 3.32 ,9 },    //3->80    17->10
                           { 3800 , 3.96 ,13 },    //4->74    2277 , 3.0 ,10
                           { 3800 , 4.30 ,15 },    //5->71
                           { 3800 , 4.66 ,10 },    //6->68
                           { 3800 , 3.96 ,10} };  //7->80

float Dir_Gyro[4] =  { 25 , 0 , -4 , 0 };        //Direction inside loop  10

float Gyro_Ratio[5] = { 0.7 , 0.6 , 0.5 , 0.49 ,0.49  };
float Dir_lock[4]={20, 0 , 0 , 0 };

/*Balance*/
vuint8 Bal_Time_Flag = 0;                     //Period Of Balance Control
float Bal_PWM_Out = 0,
      Increase=0,
      Tar_Ang_Vel=0,
      Target_Angle=0;  //870
int16  Zero_Angle = 71;

/*Speed*/
vuint8  Speed_Time_Flag = 0;
vint16  Speed_PWM_Output_Period = 0;
int16 Last_Speed_PWM = 0;
int16 Speed_PWM = 0;
int16 Last_Speed_PID_Out=0;
int16 Speed_Set = 500;      //?????
int16 Speed_PID_Out = 0;
int16 Car_Speed_Min=0;

/*Direction*/
int32 Direction_Parameter=0;
float Radius=0;  //???????
float Dir_Outside_Out=0;
float Difference=0, Difference_old;  //????
int16 Direction_PWM = 0;
int16 Last_Direction_PWM = 0;
uint8 Dir_PWM_Output_Period = 0;
int16 Direction_PWM_Out = 0;
vuint8 Dir_Time_Flag = 0;

/*************************************** Bal_Control *********************************************/
void Bal_Control(void)
{

    if( Bal_Time_Flag==2||Bal_Time_Flag==4||Bal_Time_Flag==6 )
    {
/*12ms angle_control*/
      if(Bal_Time_Flag == 6)
      {
        Bal_Time_Flag=0;
        
        Speed_Measure();
          if( Car_Speed >130 && stop_count>=time_set)
        {
          STOP_Distance = 1;
          Protect_Gear_flag = 1;
        }
        
        Tar_Ang_Vel = PID_Realize( &Angle_PID, Angle , 0 , SystemAttitude.Pitch-Target_Angle);
        
        /*Tar_Ang_Vel = range_protect( (int32)Tar_Ang_Vel ,-550, 550);//???????*/
//        send_flag=3;
      
      }
      
/*gyro 4ms control*/
      /*Increase = PID_Increase( &Gyro_PID, Bal_Gyro , 0 , SystemAttitudeRate.Pitch + Tar_Ang_Vel );
      Bal_PWM_Out += Increase;*/
      
      Bal_PWM_Out = PID_Realize( &Gyro_PID, Bal_Gyro , -SystemAttitudeRate.Pitch , Tar_Ang_Vel );
      
      Bal_PWM_Out = range_protect( Bal_PWM_Out, -9500, 9500);//???????
      
//      send_flag=3;
    }

}

/************************************ Speed Control *************************************************/

uint8 Ang_put=20;

void Speed_Control(void)
{   

  Speed_PID_Out = PID_Realize( &Speed_PID , Speed , Car_Speed ,  Speed_Set );
  
  Speed_PID_Out = 0.3 * Last_Speed_PID_Out + 0.7 * Speed_PID_Out;
  Last_Speed_PID_Out = Speed_PID_Out;
  
  Target_Angle = range_protect( Speed_PID_Out , -2, Ang_put);//???????(?????????????)
  Target_Angle += Zero_Angle;	// ???????????????
  
  Target_Angle = range_protect( Target_Angle , 75 ,120 );
  
  Car_Speed_Min = 0.9 * Car_Speed + 0.1 * Car_Speed_Min;
  
     if(Car_Speed_Min <= 40)
     {
        Car_Speed_Min = 40;
     }
//  send_flag=4;
  
}

/********************************* Direction Control *****************************************/

uint8 Speed_Ratio = 175;

void Direction_Control(void)  //
{
  
//        Dir_Outside_Out = Radius;   //lock the direction
  
//        Indside_Error_Ratio = ( ( Car_Speed * Car_Speed )/( Speed_Set * Speed_Set )+1 );
  
     SystemAttitudeRate.Yaw = range_protect( SystemAttitudeRate.Yaw, -400, 400 );
  
     if( Dir_Time_Flag==2||Dir_Time_Flag==4 )
     {
       
//8ms error control
         if( Dir_Time_Flag == 4 )
        {
          
          Dir_Time_Flag = 0;
          
          //Class_Calculate();
          
          Radius = PlacePID_Control( &Dir_Error_PID , Dir_Error[Class] , 0 , Difference );   //double gyro loop
          
          /*Radius = PID_Realize( &Dir_Error_PID , Dir_lock , SystemAttitude.Yaw , 0 ) ;*/
          
          Radius = range_protect( Radius, -400, 400 );
          
//          if( Car_Speed_Min < 170 )    Speed_Ratio = 170;
          if( Car_Speed_Min<190 )  Speed_Ratio = 175;
          else if( Car_Speed_Min>190 ) Speed_Ratio = 175;
          
          Dir_Outside_Out = Radius * ( 1.0 * Car_Speed_Min/Speed_Ratio );
          
//          Dir_Outside_Out = range_protect(Dir_Outside_Out, -400, 400 );
       
        }
        
//4ms gyro control
       Dir_PWM_Output_Period = 0;  //4ms???????2ms??????
       
       Last_Direction_PWM = Direction_PWM;   //?????????????????
       
       /*Dir_Outside_Out = Radius;   //lock the direction*/
       
       Direction_Parameter = PID_Butterworth( &Dir_Gyro_PID, Dir_Gyro,  SystemAttitudeRate.Yaw , Dir_Outside_Out);	//the gyro_Yaw range from -500 to 500,left+ right-
       
       Direction_Parameter -= Difference * abs(Difference)/150;
       
       Direction_PWM = Direction_PWM*0.3 + Direction_Parameter*0.7;	//????????????????
       
       Direction_PWM = range_protect(Direction_PWM, -10000, 10000 );
       
       Direction_PWM_Out = Direction_PWM;

       //send_flag=4;
     }
}



void Direction_PWM_Output(void)
{
    float Error_Value;
    
    Error_Value = Direction_PWM - Last_Direction_PWM;
    
    Direction_PWM_Out = Error_Value * (Dir_PWM_Output_Period + 1) / 2 + Last_Direction_PWM;
    
}



