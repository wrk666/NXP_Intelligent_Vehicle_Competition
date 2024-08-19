#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "common.h"
#include "headfile.h"
	   		   
#define u8      uint8 
#define u16     uint16
#define u32     uint32

#define IIC_GPIO       GPIOA_BASE_PTR ////////GPIOA16  GPIOA17��Ӧ����PTC0  PTC1(��)����ϸ�������ֲᣬ���Ĺܽ�����IIC_init()����һ������
#define IIC_SCLpin     16                   //�����Ĵ�����Ӧ����⺯����ͬ���Ĵ����嵥��ֻ��PORT��ABC���������ֲ�
#define IIC_SDApin     17
#define IIC_SDA_H       (IIC_GPIO->PDOR |= (1 << IIC_SDApin))
#define IIC_SCL_H       (IIC_GPIO->PDOR |= (1 << IIC_SCLpin))
#define IIC_SDA_L       (IIC_GPIO->PDOR &= ~(1 << IIC_SDApin))
#define IIC_SCL_L       (IIC_GPIO->PDOR &= ~(1 << IIC_SCLpin))
#define IIC_SDA_OUT()  {IIC_GPIO->PIDR |= (1 << IIC_SDApin); IIC_GPIO->PDDR |= (1 << IIC_SDApin);}
#define IIC_SDA_IN()   {IIC_GPIO->PIDR &= ~(1 << IIC_SDApin); IIC_GPIO->PDDR &= ~(1 << IIC_SDApin);}

#define DELAY_US  

//IO�������� SCL C0 ; SDA C1   
#define SDA_IN()    gpio_ddr(C1,GPI)
#define SDA_OUT()   gpio_ddr(C1,GPO)

//IO��������
#define READ_SDA    gpio_get(C1)
//#define IIC_SCL    PTE3_OUT //SCL
//#define IIC_SDA    PTE5_OUT //SDA

//IIC���в�������
void IIC_Delay(void);				//MPU IIC��ʱ����
void IIC_Init(void);                            //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

#endif
















