/*********************************************************************************************************************
 *
 * @file       		FXOS8700
 * @author     		Alex
 * @version    		v1.0
 * @Software 		IAR 8.1
 * @date       		2017-11-9
 ********************************************************************************************************************/
#include "common.h"
#include "headfile.h"


bool FXOS8700_init(void)
{
  uint8 ErrCount = 0;
  while(IIC_Read_Reg(IIC_FXOS8700_ADR, FXOS8700_ID) != 0xC7)   /////确认芯片ID
  {
    ErrCount++;
    if(ErrCount > 5)
      return false;
  }

  if(IIC_Write_Reg(IIC_FXOS8700_ADR, FXOS8700_SYSMOD, 0x00) == false)return false;          ///active
  pit_delay_ms(pit1,10);
  if(IIC_Write_Reg(IIC_FXOS8700_ADR, FXOS8700CQ_M_CTRL_REG1, 0x1F) == false)return false;       //acc低噪声模式(限于2,4G)，并唤醒
  pit_delay_ms(pit1,10);
  if(IIC_Write_Reg(IIC_FXOS8700_ADR, FXOS8700CQ_M_CTRL_REG2, 0x20) == false)return false;   //mag oversampling 200Hz,hybrid mode
  pit_delay_ms(pit1,10);
  if(IIC_Write_Reg(IIC_FXOS8700_ADR, FXOS8700_XYZ_DATA_CFG, 0x01) == false)return false;   
  pit_delay_ms(pit1,10);
  if(IIC_Write_Reg(IIC_FXOS8700_ADR, FX0S8700_CTRL_REG1, 0x0D) == false)return false;    // f_read=0 for normal 16 bit reads
  pit_delay_ms(pit1,10);
  
  return true;
}


//////错误读取将返回false
bool FXOS8700_DataRead(FXOS8700Datatypedef *Q, uint8 type)
{
  uint8 datatemp[6] = {0};
  uint8 datatemp1[6] ={0};
  if(IIC_Read_Buff(IIC_FXOS8700_ADR, FXOS8700_ACC_XDATA, datatemp, 6) == false)return false;
  Q->ACCXdata = (float)( (int16)( (datatemp[0] << 8) | datatemp[1]) >> 2);
  Q->ACCYdata = (float)( (int16)( (datatemp[2] << 8) | datatemp[3]) >> 2);
  Q->ACCZdata = (float)( (int16)( (datatemp[4] << 8) | datatemp[5]) >> 2);
  if(type==1)
  {
  
    if(IIC_Read_Buff(IIC_FXOS8700_ADR, FXOS8700_MAG_XDATA , datatemp1, 6) == false)return false;
      Q->MAGXdata =  ( datatemp1[0] << 8 ) | datatemp1[1] ;
      Q->MAGYdata =  ( datatemp1[2] << 8 ) | datatemp1[3] ;
      Q->MAGZdata =  ( datatemp1[4] << 8 ) | datatemp1[5] ;
      
  }
  return true;
}



bool FXOS8700_Mag_Read(FXOS8700Datatypedef *Q)
{
    uint8 datatemp1[6] ={0};
      if(IIC_Read_Buff(IIC_FXOS8700_ADR, FXOS8700_MAG_XDATA , datatemp1, 6) == false)return false;
      Q->MAGXdata = (float)((int16)( (datatemp1[0] << 8) | datatemp1[1] ) );
      Q->MAGYdata = (float)((int16)( (datatemp1[2] << 8) | datatemp1[3] ) );
      Q->MAGZdata = (float)((int16)( (datatemp1[4] << 8) | datatemp1[5] ) );
}

