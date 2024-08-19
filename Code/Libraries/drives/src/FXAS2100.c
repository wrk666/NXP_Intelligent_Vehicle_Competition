/*********************************************************************************************************************
 *
 * @file       		FXAS2100
 * @author     		Alex
 * @version    		v1.0
 * @Software 		IAR 8.1
 * @date       		2017-11-9
 ********************************************************************************************************************/

#include "common.h"
#include "headfile.h"


bool FXAS2100_init(void)
{
  uint8 ErrCount = 0;
  while(IIC_Read_Reg(IIC_FXAS2100_ADR, FXAS2100_ID) != 0xD7)   /////ȷ��оƬID
  {
    ErrCount++;
    if(ErrCount > 5)
      return false;
  }
  if(IIC_Write_Reg(IIC_FXAS2100_ADR, FXAS2100_CTRL_REG1, 0x00) == false)return false;  ///stand by
  pit_delay_ms(pit1,10);
  if(IIC_Write_Reg(IIC_FXAS2100_ADR, FXAS2100_CTRL_REG0, 0x01) == false)return false;   ///1000dps
  pit_delay_ms(pit1,10);
  if(IIC_Write_Reg(IIC_FXAS2100_ADR, FXAS2100_CTRL_REG2, 0x03) == false)return false;  
  pit_delay_ms(pit1,10);
  if(IIC_Write_Reg(IIC_FXAS2100_ADR, FXAS2100_CTRL_REG1, 0x02) == false)return false;   ////800hz
  pit_delay_ms(pit1,10);
  return true;
}

//////�����ȡ������false
bool FXAS2100_DataRead(FXAS2100Datatypedef *Q)
{
  uint8 datatemp[6] = {0};
  if(IIC_Read_Buff(IIC_FXAS2100_ADR, FXAS2100_XDATA, datatemp, 6) == false)return false;
  Q->GYROXdata = (float)((int16)((datatemp[0] << 8) | datatemp[1]));
  Q->GYROYdata = (float)((int16)((datatemp[2] << 8) | datatemp[3]));
  Q->GYROZdata = (float)((int16)((datatemp[4] << 8) | datatemp[5]));
  return true;
}

