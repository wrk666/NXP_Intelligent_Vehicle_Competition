#ifndef SOFTIIC_H
#define SOFTIIC_H

#include "common.h"
#include "headfile.h"


#if (defined(MK60F15) || defined(MK66F18) || defined(MK60D10) || defined(MK60DZ10))
#include "MK60_PIT.h"                         /////ʹ�õ����е���ʱ����
/*���I2C��������*/
#define IICDelay      8              
/**************************************/
/*  K66 210Mʱ��ֵ��Ϊ3-15    ���150us��ɲɼ�
*/
/*************************************/
#define IICCLOCKON     (SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK)
#define IIC_PORT       PORTC_BASE_PTR
#define IIC_GPIO       PTC_BASE_PTR

#define IIC_SCLpin     1
#define IIC_SDApin     0
#define IIC_SDA_OUT()  {(IIC_GPIO->PDDR |= (1 << IIC_SDApin));}
#define IIC_SDA_IN()   {(IIC_GPIO->PDDR &= ~(1 << IIC_SDApin));}
#define IIC_DATA       ((IIC_GPIO->PDIR >> IIC_SDApin) & 0x01)
#define IIC_SDA_H      (IIC_GPIO->PDOR |= (1 << IIC_SDApin))
#define IIC_SCL_H      (IIC_GPIO->PDOR |= (1 << IIC_SCLpin))
#define IIC_SDA_L      (IIC_GPIO->PDOR &= ~(1 << IIC_SDApin))
#define IIC_SCL_L      (IIC_GPIO->PDOR &= ~(1 << IIC_SCLpin))



#elif (defined(S9KEAZ1284))


#define IICDelay       8 
#define IIC_GPIO       GPIOA_BASE_PTR ////////A12  A13��Ӧ����B4   B5����ϸ�������ֲᣬ���Ĺܽ�����IIC_init()����һ������
#define IIC_SCLpin     16                   //�����Ĵ�����Ӧ����⺯����ͬ���Ĵ����嵥��ֻ��PORT��ABC���������ֲ�
#define IIC_SDApin     17



#define IIC_SDA_OUT()   {IIC_GPIO->PIDR |= (1 << IIC_SDApin); IIC_GPIO->PDDR |= (1 << IIC_SDApin);}
#define IIC_SDA_IN()    {IIC_GPIO->PIDR &= ~(1 << IIC_SDApin); IIC_GPIO->PDDR &= ~(1 << IIC_SDApin);}
#define IIC_DATA        ((IIC_GPIO->PDIR >> IIC_SDApin) & 0x01)
#define IIC_SDA_H       (IIC_GPIO->PDOR |= (1 << IIC_SDApin))
#define IIC_SCL_H       (IIC_GPIO->PDOR |= (1 << IIC_SCLpin))
#define IIC_SDA_L       (IIC_GPIO->PDOR &= ~(1 << IIC_SDApin))
#define IIC_SCL_L       (IIC_GPIO->PDOR &= ~(1 << IIC_SCLpin))


#endif



uint8 IIC_Read_Reg(uint8 addr, uint8 offset);
bool IIC_Write_Reg(uint8 addr, uint8 offset, uint8 data);
bool IIC_Read_Buff(uint8 addr, uint8 offset, uint8* buff, uint8 size);
void Soft_IIC_init(void);
#endif