#include "mpuiic.h"
#include "common.h"
#include "headfile.h"
//#include "delay.h"

//��ʼ��IIC
void IIC_Init(void)
{					     
  gpio_init(C0,GPO,0);  //��Ϊ���
  gpio_init(C1,GPO,0);
  
  port_pull(C0);  //��������
  port_pull(C1);
  
  IIC_SCL_H;
  IIC_SDA_H;
  					 //PB10,PB11 �����
 
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	IIC_SDA_OUT();     //sda�����
        IIC_SDA_H; 	  
	IIC_SCL_H;
	pit_delay_us(pit1,4);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	pit_delay_us(pit1,4);
        IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	IIC_SDA_OUT();//sda�����
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
	pit_delay_us(pit1,4);
	IIC_SCL_H; 
	IIC_SDA_H;//����I2C���߽����ź�
	pit_delay_us(pit1,4);						   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC_SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_H;	pit_delay_us(pit1,1);  
	IIC_SCL_H;	pit_delay_us(pit1,1); 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_L;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL_L;
	IIC_SDA_OUT();
	IIC_SDA_L;
	pit_delay_us(pit1,2);
	IIC_SCL_H;
	pit_delay_us(pit1,2);
	IIC_SCL_L;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_L;
	IIC_SDA_OUT();
	IIC_SDA_H;
	pit_delay_us(pit1,2);
	IIC_SCL_H;
	pit_delay_us(pit1,2);
	IIC_SCL_L;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    IIC_SDA_OUT(); 	    
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
      if((txd&0x80)>>7)  IIC_SDA_H;
      else  IIC_SDA_L;
//        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
	pit_delay_us(pit1,2);
		IIC_SCL_H;
	pit_delay_us(pit1,2);
		IIC_SCL_L;
	pit_delay_us(pit1,2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	IIC_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL_L;
	pit_delay_us(pit1,2);
	IIC_SCL_H;
        receive<<=1;
        if(READ_SDA)receive++;   
	pit_delay_us(pit1,1);
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}


















