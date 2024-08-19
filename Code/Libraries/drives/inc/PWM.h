#define Ratio_Encoder 203 / (1250*0.008);
extern float Car_Speed;
extern float CarSpeed_Filter;
extern int16 LeftMotorPulse;         
extern int16 RightMotorPulse;
extern float Left_Speed ;         
extern float Right_Speed;
extern float RealSpeed ;
extern uint8 Protect_Gear_flag;
//extern uint8_t Class;

extern int32 MOTOR_Speed_Left;
extern int32 MOTOR_Speed_Right; 

extern uint8 STOP_Distance;
extern uint8 Motor_Stop_Flag;

void PWM_Init(void);
void Get_CarSpeed(void);
void Class_Calculate(void);
void SET_PWM(float bal_value, float speed_value );
void Encoder_Init(void);

void Right_Motor_Control(float Val);
void Left_Motor_Control(float Val);
void SET_PWM(float Bal_value, float Dir_value );
void Motor_Stop(void);
void Motor_Start(void);
void Gear_Protect(void);
void Speed_Measure(void);
void MAG_STOP(void);
void Chase_STOP(void);
void Start_Car(void);

float range_protect(float duty, float min, float max);//ÏÞ·ù±£»¤
