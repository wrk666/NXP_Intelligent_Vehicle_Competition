#include "headfile.h"

//±àÂëÆ÷·½Ïò
uint8 pole_left=0,pole_right=0;
int LeftMotorPulse=0,RightMotorPulse=0;


  
void Bee_Init(void)
{
  gpio_init (D1, GPO,0);
}

void LED_Init(void)
{
    gpio_init (D0, GPO,1);  //Yellow
    gpio_init (D5, GPO,1);  //White
    gpio_init (A1, GPO,1);  //Blue
    gpio_init (B2, GPO,1);  //Green
}


void Bee_Alarm(void)
{
  gpio_set(D1,1);
}

void Bee_DeAlarm(void)
{
  gpio_set(D1,0);
}

void LED_ON(uint8 Color)
{
  switch(Color)
  {
  case YELLOW:     gpio_set(D0,0);  break;
  case WHITE:      gpio_set(D5,0);  break;
  case BLUE:       gpio_set(A1,0);  break;
  case GREEN:      gpio_set(B2,0);  break;
  default:  break;
  }
}

void LED_OFF(uint8 Color)
{
  switch(Color)
  {
  case YELLOW:     gpio_set(D0,1);  break;
  case WHITE:      gpio_set(D5,1);  break;
  case BLUE:       gpio_set(A1,1);  break;
  case GREEN:      gpio_set(B2,1);  break;
  default:  break;
  }
}


void Addition_Init(void)
{
  Bee_Init();
  LED_Init();
}


void GetMotorPulse(void)
{
    pole_left=gpio_get(C5);  //ÓÒ
    pole_right=gpio_get(E1); //×ó
     
    RightMotorPulse=ftm_count_get(ftm1);   //E7  ÓÒ
    LeftMotorPulse=ftm_count_get(ftm0);  //E0  ×ó
    (pole_left==0)?(LeftMotorPulse=LeftMotorPulse):(LeftMotorPulse=-LeftMotorPulse);
    (pole_right==1)?(RightMotorPulse=RightMotorPulse):(RightMotorPulse=-RightMotorPulse);
    ftm_count_clean(ftm0);
    ftm_count_clean(ftm1);
}










