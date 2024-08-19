#ifndef FXOS8700_H
#define FXOS8700_H





#define IIC_FXOS8700_ADR          0X1E
#define FXOS8700_ID               0x0D      /////ID  0xD7
#define FXOS8700_XYZ_DATA_CFG     0x0E      ////0x01  4G
#define FXOS8700_SYSMOD           0x0B
#define FX0S8700_CTRL_REG1        0x2A      //acc configure
#define FXOS8700_ACC_XDATA        0x01
#define FXOS8700_MAG_XDATA        0x33

#define FXOS8700CQ_M_CTRL_REG1    0x5B
#define FXOS8700CQ_M_CTRL_REG2    0x5C



typedef struct{
  float ACCXdata;
  float ACCYdata;
  float ACCZdata;
  int16 MAGXdata;
  int16 MAGYdata;
  int16 MAGZdata;
}FXOS8700Datatypedef;

bool FXOS8700_DataRead(FXOS8700Datatypedef *Q, uint8 type);
bool FXOS8700_init(void);
bool FXOS8700_Mag_Read(FXOS8700Datatypedef *Q );
#endif

