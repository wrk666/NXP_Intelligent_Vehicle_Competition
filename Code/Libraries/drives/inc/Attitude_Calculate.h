#define  PI            3.1415926f
#define  PERIODHZ      (float)250       /////����Ƶ��
#define  PERIODS       (float)0.004           ////��������
#include "FXAS2100.h"
#include "FXOS8700.h"

typedef struct{
  float W;
  float X;
  float Y;
  float Z;
}QuaternionTypedef;

typedef struct{ 
  float Pitch;  //������
  float Yaw;    //ƫ����
  float Roll;   //������
}EulerAngleTypedef;


typedef struct{
  float Xdata;
  float Ydata;
  float Zdata;
}AttitudeDatatypedef;

extern QuaternionTypedef    Quaternion;   //��Ԫ��
extern EulerAngleTypedef    EulerAngle;   //ŷ����
extern QuaternionTypedef    AxisAngle;    //���
extern EulerAngleTypedef    EulerAngleRate;//��ǰŷ�����ٶ�

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

extern FXAS2100Datatypedef      FXAS2100_Data;   //FXAS2100���ݽṹ��(����������������)
extern FXOS8700Datatypedef      FXOS8700_Data;   //FXOS8700���ݽṹ��(��������Ӽƴ�������)
extern EulerAngleTypedef      SystemAttitude;            /////��̬��
extern EulerAngleTypedef      SystemAttitudeRate;        //��̬���ٶ�

extern vuint8   IsGyroOffsetReset;

extern void Quaternion_init();

extern void Attitude_UpdateGyro(void);

extern void Attitude_UpdateAcc(void);
void AT_Init();
void Angle_Fusion(void);
//extern void Attitude_Calculate(void);