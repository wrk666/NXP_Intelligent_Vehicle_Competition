#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "common.h"
#include "headfile.h"
	   		   
#define u8      uint8 
#define u16     uint16
#define u32     uint32

#define IIC_GPIO       GPIOA_BASE_PTR ////////GPIOA16  GPIOA17对应的是PTC0  PTC1(板)，详细见技术手册，更改管脚请在IIC_init()里面一并更改
#define IIC_SCLpin     16                   //操作寄存器对应的与库函数不同，寄存器清单上只有PORT：ABC，见技术手册
#define IIC_SDApin     17
#define IIC_SDA_H       (IIC_GPIO->PDOR |= (1 << IIC_SDApin))
#define IIC_SCL_H       (IIC_GPIO->PDOR |= (1 << IIC_SCLpin))
#define IIC_SDA_L       (IIC_GPIO->PDOR &= ~(1 << IIC_SDApin))
#define IIC_SCL_L       (IIC_GPIO->PDOR &= ~(1 << IIC_SCLpin))
#define IIC_SDA_OUT()  {IIC_GPIO->PIDR |= (1 << IIC_SDApin); IIC_GPIO->PDDR |= (1 << IIC_SDApin);}
#define IIC_SDA_IN()   {IIC_GPIO->PIDR &= ~(1 << IIC_SDApin); IIC_GPIO->PDDR &= ~(1 << IIC_SDApin);}

#define DELAY_US  

//IO方向设置 SCL C0 ; SDA C1   
#define SDA_IN()    gpio_ddr(C1,GPI)
#define SDA_OUT()   gpio_ddr(C1,GPO)

//IO操作函数
#define READ_SDA    gpio_get(C1)
//#define IIC_SCL    PTE3_OUT //SCL
//#define IIC_SDA    PTE5_OUT //SDA

//IIC所有操作函数
void IIC_Delay(void);				//MPU IIC延时函数
void IIC_Init(void);                            //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

#endif
















