//#ifndef ADDITION_H
//#define ADDITION_H

#define  YELLOW 0x01
#define  WHITE  0x02
#define  BLUE   0x03
#define  GREEN  0x04

void KBI_Init(void);
void Bee_Init(void);
void LED_Init(void);
void Bee_Alarm(void);
void Bee_DeAlarm(void);
void Addition_Init(void);
void LED_ON(uint8 Color);
void LED_OFF(uint8 Color);
//void Encoder_Init(void);
void GetMotorPulse(void);


//#endif