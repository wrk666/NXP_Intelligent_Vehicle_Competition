#include "common.h"
#include "headfile.h"
#include "Keyboard.h"

uint8 STOP_Distance=0;
uint8 Motor_Stop_Flag=0;
uint8 Protect_Gear_flag=0;

void PWM_Init(void)
  {
    ftm_pwm_init(ftm2,ftm_ch2,3000,0);  //LN
    ftm_pwm_init(ftm2,ftm_ch3,3000,0);  //LP
    ftm_pwm_init(ftm2,ftm_ch4,3000,0);  //RP
    ftm_pwm_init(ftm2,ftm_ch5,3000,0);  //RN
    
    gpio_init(C5,GPI,1);  //R
    ftm_count_init(ftm1); 
    
    gpio_init(E1,GPI,1);  //L
    ftm_count_init(ftm0); 
    
  }

int16 LeftMotorPulse  = 0;
int16 RightMotorPulse = 0;

float Car_Speed = 0;             
float CarSpeed_Filter = 0;
float Left_Speed  = 0;         
float Right_Speed = 0;
float RealSpeed = 0;  
float CarSpeed_Old = 0;
float CarSpeed_New = 0;

void Get_CarSpeed(void)
{
     LeftMotorPulse = (gpio_get(E1) == 1? ftm_count_get(ftm0):(-ftm_count_get(ftm0)));
     
     ftm_count_clean(ftm0);
     
     RightMotorPulse = (gpio_get(C5) == 0? ftm_count_get(ftm1):(-ftm_count_get(ftm1)));
     
     ftm_count_clean(ftm1);
     
     Left_Speed = LeftMotorPulse ;
     Left_Speed = (Left_Speed > 400 ? 400 : Left_Speed);		//滤左编码器的噪声

     Right_Speed = RightMotorPulse ;
     Right_Speed = (Right_Speed > 400 ? 400 : Right_Speed);		//滤右编码器的噪声
     
     RealSpeed = (Left_Speed + Right_Speed) * 0.5;			//Real Speed
//	//速度采集梯度平滑，每次采集最大变化100
     CarSpeed_Old = CarSpeed_Filter;
     CarSpeed_New = RealSpeed;
	
     if(CarSpeed_New >= CarSpeed_Old)
     {
//       Beep(100);
       CarSpeed_Filter = ( (CarSpeed_New - CarSpeed_Old) > 30 ? (CarSpeed_Old + 30) : CarSpeed_New );
       
     }
     else
     {
       
       CarSpeed_Filter = ((CarSpeed_New - CarSpeed_Old) < -30 ? (CarSpeed_Old - 30) : CarSpeed_New);
       
     }
     
     Car_Speed = CarSpeed_Filter;


     /*Car_Speed_Min = 0.9 * Car_Speed + 0.1 * Car_Speed_Min;
     
     if(Car_Speed_Min <= 40){
        
        Car_Speed_Min = 40;
     }*/
}

char Left_Crazy = 0;	// 电机疯转
char Right_Crazy = 0;	// 电机疯转
int32 MOTOR_Duty_Left  = 0;
int32 MOTOR_Duty_Right = 0;
int32 MOTOR_Speed_Left = 0;
int32 MOTOR_Speed_Right = 0; 
int32 MOTOR_Speed_Left_Last = 0;
int32 MOTOR_Speed_Right_Last = 0;
int32 MOTOR_Left_Acc = 0;
int32 MOTOR_Right_Acc = 0;

uint32 Annulus_Error = 0;	// 环路误判控制


/******* 电机速度测量 ********/
void Speed_Measure(void)
{
	int32 Pulses;
	static int32 Speed_Last = 0;
	static int32 Crazy_Count = 0;
	
	
	/******* 右电机速度相关控制 ********/
       Pulses = (gpio_get(C5) == 0? ftm_count_get(ftm1):(-ftm_count_get(ftm1)));
       
       ftm_count_clean(ftm1); 
       
	MOTOR_Speed_Right = Pulses;	// 得到右轮转速

	MOTOR_Right_Acc = MOTOR_Speed_Right - MOTOR_Speed_Right_Last;	// 计算加速度
	if (MOTOR_Right_Acc > 60)
	{
		Right_Crazy = 1;	// 疯转
	}
	if (MOTOR_Speed_Right > Speed_Set )
	{
		Right_Crazy = 2;	// 疯转
	}
//	if (MOTOR_Speed_Right < -350)
//	{
//		Right_Crazy = -1;	// 倒转
//	}
	
	if (Right_Crazy)
	{
		if (MOTOR_Right_Acc <= 60)
		{
			if (MOTOR_Speed_Right < Speed_Set  && MOTOR_Speed_Right > 0)
			{
				Right_Crazy = 0;
			}
		}
	}
	
	if (!Right_Crazy)
	{
		MOTOR_Speed_Right = MOTOR_Speed_Right*0.9 + MOTOR_Speed_Right_Last*0.1;
		MOTOR_Speed_Right_Last = MOTOR_Speed_Right;	// 更新右轮速度
	}
	else
	{
		MOTOR_Speed_Right = MOTOR_Speed_Right*0.5 + MOTOR_Speed_Right_Last*0.5;
		MOTOR_Speed_Right_Last = MOTOR_Speed_Right;	// 更新右轮速度
	}
	/******* 右电机速度相关控制结束 ********/
	
	/******* 左电机速度相关控制 ********/
        Pulses = (gpio_get(E1) == 1? ftm_count_get(ftm0):(-ftm_count_get(ftm0)));
     
        ftm_count_clean(ftm0);
        
        
			// 正交解码寄存器清零
	MOTOR_Speed_Left = Pulses;		// 得到左轮转速
	
	MOTOR_Left_Acc = MOTOR_Speed_Left - MOTOR_Speed_Left_Last;	// 计算加速度
	if (MOTOR_Left_Acc > 60)
	{
		Left_Crazy = 1;
	}
	if (MOTOR_Speed_Left > Speed_Set )
	{
		Left_Crazy = 2;
	}
//	if (MOTOR_Speed_Left < -350)
//	{
//		Left_Crazy = -1;
//	}
	
	if (Left_Crazy)
	{
		if (MOTOR_Left_Acc <= 60)
		{
			if (MOTOR_Speed_Left < Speed_Set  && MOTOR_Speed_Left > 0)
			{
				Left_Crazy = 0;
			}
		}
	}
	
	if (!Left_Crazy)
	{
		MOTOR_Speed_Left = 0.9*MOTOR_Speed_Left + 0.1*MOTOR_Speed_Left_Last;	// 低通滤波
		MOTOR_Speed_Left_Last = MOTOR_Speed_Left;	// 更新左轮速度
	}
	else
	{
		MOTOR_Speed_Left = 0.5*MOTOR_Speed_Left + 0.5*MOTOR_Speed_Left_Last;	// 低通滤波
		MOTOR_Speed_Left_Last = MOTOR_Speed_Left;	// 更新左轮速度
	}

	
	
	/******* 左电机速度相关控制结束 ********/


	
	/******* 电机疯转特殊处理 ********/
	if ((Left_Crazy > 0) && (Right_Crazy > 0))
	{
		Car_Speed = Speed_Set;			// 两边都疯转，使用上次速度作为当前实际速度
	}
	else if (Left_Crazy)
	{
		if (MOTOR_Speed_Right > Speed_Set)
		{
			Car_Speed = Speed_Set;
		}
		else
		{
			Car_Speed = MOTOR_Speed_Right;	// 左电机疯转，使用上次速度作为当前实际速度
		}
	}
	else if (Right_Crazy)
	{
		if (MOTOR_Speed_Left > Speed_Set)
		{
			Car_Speed = Speed_Set;
		}
		else
		{
			Car_Speed = MOTOR_Speed_Left;
		}
	}
	else
	{
		Car_Speed = (MOTOR_Speed_Left + MOTOR_Speed_Right) / 2;	
	}
	
	Car_Speed = Car_Speed *0.9 + Speed_Last * 0.1;
	Speed_Last = Car_Speed;
        
     if(Judge_flag==1)      //4000->80cm   50->1cm
     {
       In_Pulses+=Car_Speed;
     }
     else if(Judge_flag==2)
     {
       Out_Pulses+=Car_Speed;
     }
     
     Test_Pulses+=Car_Speed;
     
     
     
}

/*void Class_Calculate(void)
{
	if (Car_Speed_Min < 160)
	{
		Class = 0;
	}
	else if (Car_Speed_Min < 180)
	{
		Class = 1;
	}
	else if (Car_Speed_Min < 180)
	{
		Class = 1;
	}
	else if (Car_Speed_Min < 220)
	{
		Class = 1;
	}
	else if (Car_Speed_Min >= 220)
	{      
		Class = 1;
	}
}*/

/*//基本思想：陀螺仪数值小，路径给宽，保证不抖；
//陀螺仪数值大，结合速度，速度小，路径给宽；速度大，则转弯需灵敏，给窄
void Class_Calculate(void)
{
  if( abs(SystemAttitudeRate.Yaw) < 75 )   //80
  {
        Class = 0;
  }
  else if( abs(SystemAttitudeRate.Yaw) <120 )
  {
        if(Car_Speed<110)
            Class = 0;
        else if(Car_Speed<140)
            Class = 1;
  }
  else if( abs(SystemAttitudeRate.Yaw) < 160 )
  {
        if(Car_Speed<110)
            Class = 0;
        else if(Car_Speed<130)
            Class = 1;
        else if(Car_Speed<160)
            Class = 2;
  }
    else if( abs(SystemAttitudeRate.Yaw) >= 160 )
    {
        if(Car_Speed<110)
            Class = 0;
        else if(Car_Speed<130)
            Class = 1;
        else if(Car_Speed<160)
            Class = 2;
        else if(Car_Speed>=160)
            Class = 3;
    }
}*/


void Class_Calculate(void)
{
  if(Car_Speed<110)
  {
    Class = 0;
//    Dir_Gyro[0]=55;
  }
  else if(Car_Speed<130)
  {
    if( abs(SystemAttitudeRate.Yaw) <55 )
    {
        Class = 0;
//        Dir_Gyro[0]=55;
    }

    else if( abs(SystemAttitudeRate.Yaw) <130 )
    {
        Class = 1;
//        Dir_Gyro[0]=55;
    }

  }
  else if(Car_Speed<160)
  {
    if( abs(SystemAttitudeRate.Yaw) <75 )
    {
            Class = 0;
//            Dir_Gyro[0]=55;
    }

    else if( abs(SystemAttitudeRate.Yaw) <130 )
    {
      Class = 1;
//      Dir_Gyro[0]=55;
    }
    else if( abs(SystemAttitudeRate.Yaw) <160 )
    {
      Class = 2;
//      Dir_Gyro[0]=55;
    }
  }
  else if(Car_Speed>=160)
  {
    if( abs(SystemAttitudeRate.Yaw) <75 )
    {
      Class = 0;
//      Dir_Gyro[0]=55;
    }
    else if( abs(SystemAttitudeRate.Yaw) <130 )
    {
      Class = 1;
//      Dir_Gyro[0]=55;
    }
    else if( abs(SystemAttitudeRate.Yaw) <160 )
    {
      Class = 2;
//      Dir_Gyro[0]=55;
    }
    else if( abs(SystemAttitudeRate.Yaw) >=160 )
    {
      Class = 3;
//      Dir_Gyro[0]=60;
    }
  }
  
  
}



 void Right_Motor_Control(float Val)
{
  
   if(Val >= 0){   
      
           
      Val = Val < 400 ? 400 : Val;
      Val = Val > 9500 ? 9500 : Val;
  
      ftm_pwm_duty(ftm2, ftm_ch4,Val);       // Right(P)
      ftm_pwm_duty(ftm2, ftm_ch5, 0);        // Right(N)
  }
     if(Val < 0){    
      
      Val = -Val;
      
      
      Val = Val < 400 ? 400 : Val;
      Val = Val > 9500 ? 9500 : Val;
  
      ftm_pwm_duty(ftm2, ftm_ch4,0);        // Right(P)
      ftm_pwm_duty(ftm2, ftm_ch5,Val);      // Right(N)
  }
}
void Left_Motor_Control(float Val)
{
  
   if(Val >= 0)
   {
     
    
      Val = Val < 450 ? 450 : Val;
      Val = Val > 9800 ? 9800 : Val;
  
      ftm_pwm_duty(ftm2, ftm_ch3, Val);          // Left(P)
      ftm_pwm_duty(ftm2, ftm_ch2, 0);        // Left(N)
  }
     if(Val < 0){    
   
      Val = -Val;
       
      
      Val = Val < 450 ? 450 : Val;
      Val = Val > 9800 ? 9800 : Val;
  
      ftm_pwm_duty(ftm2, ftm_ch3, 0);      // Left(P)
      ftm_pwm_duty(ftm2, ftm_ch2, Val);        // Left(N)
  }
}


float PWM = 0;
float PWM_left = 0;
float PWM_right = 0;

void SET_PWM(float Bal_value, float Dir_value )
{
  if(Tracking_ADC_Val_Error >= 0){
//  
      PWM_right = Bal_value + Dir_value;
      PWM_left = Bal_value - Dir_value;
//  
  }
  if(Tracking_ADC_Val_Error < 0){
    
    Dir_value = -Dir_value;
    
      PWM_right = Bal_value - Dir_value;
      PWM_left = Bal_value + Dir_value;
  
  }  
  
  Left_Motor_Control(PWM_left);
  
  Right_Motor_Control(PWM_right);   
    
}    
  
void Motor_Stop(void)
{   
//    systick_delay_ms(10);
    
    ftm_pwm_duty(ftm2, ftm_ch2, 0);            //Left(N)
    ftm_pwm_duty(ftm2, ftm_ch3, 0);            //Left(P)

    ftm_pwm_duty(ftm2, ftm_ch5,0);             //Right(N)    
    ftm_pwm_duty(ftm2, ftm_ch4,0);             //Right(P)
    
    
}
void Motor_Start(void)
{   
    ftm_pwm_duty(ftm2, ftm_ch2,0);          //Left(N)
    ftm_pwm_duty(ftm2, ftm_ch3,300);             //Left(P)
    ftm_pwm_duty(ftm2, ftm_ch4,300);          //Right(P)
    ftm_pwm_duty(ftm2, ftm_ch5,0);             //Right(N)

}

void Gear_Protect(void)  //保护齿轮
{
     if( Protect_Gear_flag  && PWM_flag )
     {
       if(  Car_Speed<-10 )
       {
         Beep(200);
         Motor_Stop();
         PWM_flag = 0;
         Protect_Gear_flag = 0;
       }
     }
}

/******** 限幅保护 *********/
float range_protect(float duty, float min, float max)//限幅保护
{
	if (duty >= max)
	{
		return max;
	}
	if (duty <= min)
	{
		return min;
	}
	else
	{
		return duty;
	}
}


void MAG_STOP(void)
{
    uint8 flag = 1 ; //按键的输入
    flag = gpio_get(B2);
  	if(flag == 0)
        {     
          Motor_Stop();
          STOP_Distance=0;
          PWM_flag = 0;
          Motor_Stop_Flag = 1;
          Beep(10);
        }
  
     /*if(FXOS8700_Data.MAGXdata==0)  return;
     
     else if( IsGyroOffsetReset==0 && STOP_Flag==1 )
     {
        if( (FXOS8700_Data.MAGXdata<-3500) )  //|| (FXOS8700_Data.MAGXdata>-300)
        {
          Motor_Stop();
          PWM_flag = 0;
          Beep(10);
        }
     }*/
   
}


void Chase_STOP(void)
{
  if( Mag_Sign_flag==0 )
  {
      Motor_Stop();
      STOP_Distance=0;
      PWM_flag = 0;
      //Motor_Stop_Flag = 1;
      Beep(1000);
  }
}


void Start_Car(void)
{
    uint8_t KeyInput=1 ; //按键的输入
    KeyInput = gpio_get(H5);
  	if(KeyInput == 0)
        {  //按下右键
          Beep(100);
          PWM_flag = 1;
          Motor_Stop_Flag = 0;
        }
}

