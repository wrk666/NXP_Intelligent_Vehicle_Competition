#define  PI            3.1415926f
#define  PERIODHZ      (float)250       /////采样频率
#define  PERIODS       (float)0.004           ////采样周期
#include "FXAS2100.h"
#include "FXOS8700.h"

typedef struct{
  float W;
  float X;
  float Y;
  float Z;
}QuaternionTypedef;

typedef struct{ 
  float Pitch;  //俯仰角
  float Yaw;    //偏航角
  float Roll;   //翻滚角
}EulerAngleTypedef;


typedef struct{
  float Xdata;
  float Ydata;
  float Zdata;
}AttitudeDatatypedef;

extern QuaternionTypedef    Quaternion;   //四元数
extern EulerAngleTypedef    EulerAngle;   //欧拉角
extern QuaternionTypedef    AxisAngle;    //轴角
extern EulerAngleTypedef    EulerAngleRate;//当前欧拉角速度

extern QuaternionTypedef    MeaQuaternion;
extern EulerAngleTypedef    MeaEulerAngle;
extern QuaternionTypedef    MeaAxisAngle;

extern QuaternionTypedef    ErrQuaternion;
extern EulerAngleTypedef    ErrEulerAngle;
extern QuaternionTypedef    ErrAxisAngle;
extern AttitudeDatatypedef         Acc;
extern AttitudeDatatypedef         Gyro;
//extern vuint8  PWM_flag;
extern volatile bool IsAttitudeinit;
extern AttitudeDatatypedef    GyroOffset;

extern FXAS2100Datatypedef      FXAS2100_Data;   //FXAS2100数据结构体(包含三轴陀螺数据)
extern FXOS8700Datatypedef      FXOS8700_Data;   //FXOS8700数据结构体(包含六轴加计磁力数据)
extern EulerAngleTypedef      SystemAttitude;            /////姿态角
extern EulerAngleTypedef      SystemAttitudeRate;        //姿态角速度

extern vuint8   IsGyroOffsetReset;

extern void Quaternion_init();

extern void Attitude_UpdateGyro(void);

extern void Attitude_UpdateAcc(void);
void AT_Init();
void Angle_Fusion(void);
//extern void Attitude_Calculate(void);