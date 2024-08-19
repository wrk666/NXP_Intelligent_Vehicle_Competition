#include "mpu9250.h"
#include "mpuiic.h"
#include "mpuiic.h"

#include "common.h"
#include "headfile.h"
#include "stdbool.h"


/*int16 gyroy_new=0,gyroy_pointer=0;
int16* queue_pointer=&gyroy_pointer;  
int16* H=&gyroy_pointer;  //����ָ��(Ϊ�˱�������ͬ�ĵ�ַ)
int16* T=&gyroy_pointer;*/

extern float pitch,roll,yaw;
extern short aacx,aacy,aacz;	        //  Gyro
extern short gyrox,gyroy,gyroz;        //  Acceleration
extern int16 gyroy1[3];  //�˲�����
extern int16 gyroy_new;
//��ʼ��MPU9250
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU9250_Init(void)
{
    u8 res=0;
    IIC_Init();     //��ʼ��IIC����
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//��λMPU9250
    pit_delay_ms(pit1,100);  //��ʱ100ms
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//����MPU9250
    MPU_Set_Gyro_Fsr(3);			  	//�����Ǵ�����,��2000dps
	  MPU_Set_Accel_Fsr(0);		     	 	//���ٶȴ�����,��2g
    MPU_Set_Rate(50);			     	 	//���ò�����50Hz
    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //�ر������ж�
	  MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C��ģʽ�ر�
	  MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
	  MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT���ŵ͵�ƽ��Ч������bypassģʽ������ֱ�Ӷ�ȡ������
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //��ȡMPU6500��ID
    if(res==MPU6500_ID) //����ID��ȷ
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//����CLKSEL,PLL X��Ϊ�ο�
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//���ٶ��������Ƕ�����
        MPU_Set_Rate(50);						       	//���ò�����Ϊ50Hz   
    }else return 1;
 
    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//��ȡAK8963 ID   
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//����AK8963Ϊ���β���ģʽ
    }else return 1;

    return 0;
}
/*�Լ��жϺ���*/
void SetDMP_Interrupt(void)
{
  
      //1001 1100  ѡ���жϵ�ƽ���߼���ƽΪ0  50us��   ��ѯ�����Զ����
  MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0x9c);	//�����ж�
  MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0x01);	//��λMPU6050
//  MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//��λMPU6050
  
}


//����MPU9250�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU9250���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}

//����MPU9250�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}

//����MPU9250�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
short MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU9250_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=21+((double)raw)/333.87;  
    return temp*100;;
}
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res; 
	res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;
}

///**
//  * @brief  Calculate the queue data
//  * @param  int16* M    : The original pointer
//  *         int16* P    :The head pointer
//            int16* Q   :The tail pointer
//            int16 data : measured gyroy data
//  * @retval none
//  */
//void Queue_IO(int16* M, int16* P ,int16* Q, int16 data)
//{
//  if(P=Q)  //��һ������
//  {
//    *P=data;
//  }
//  else
//  *(++Q)=data;
//  
//  if(Q>M+4)   //�����˶�β����ת������
//  {
//    P++;   //����ָ�����
//    a=*Q;  //���ݶ�βֵ
//    Q=M;   //��βָ��ת������
//    *Q=a;  //���׵�ַ��ֵΪ�ղŵĶ�βֵ
//  }
//  if(P>M+4)
//  {
//    //��βָ�����
//    a=*P;  //���ݶ�βֵ
//    P=M;   //��βָ��ת������
//    *P=a;  //���׵�ַ��ֵΪ�ղŵĶ�βֵ
//  }
//}






//500ȡ��ֵ
//0  1  2  3  4  5   gyro1
// 0  1  2  3  4     Bias

/*���ڶ�βֵ�ĸ������⣺֮ǰ�����˸��¶�βֵ��������һ�μ���ʱ��βֵ��Ϊ���˲�����ĵ����ڶ���ֵ��
�����Ұ��˲����ֵ�����˲�����Ķ�βֵʱ���������������²��ֵ�������ڵ�ֵƫ�����Ļ�����βֵ��
��������������ֵ��һ���������߲���
*/

//void Gyro_Smoothing_filtering(int16* array, int16 data)
//{  
//  uint8 i=0;
//  int Gyro_Bias,Bias_ave;  //�ֲ�������������
//  for(i=0;i<5;i++)
//  {
//    gyroy1[i]=gyroy1[i+1];   //6��һ�㣬�����ȳ�
//  }
//  gyroy1[5]=gyroy;   //��β��ֵ����
//  Bias_ave=(gyroy1[4]-gyroy1[0])*1.0/4;   //ƫ���ֵ
////  for(i=0;i<4;i++)   //��ȥ��β��ֵ,5��ֵ���4�����
////  {
////    
////    Gyro_Bias[i]=gyroy1[i+1]-gyroy1[i];
////    Bias_sum+=Gyro_Bias[i];   //0 1 2 3(���ĸ�)
////    if(i==3)  Bias_ave=(Bias_sum*1.0)/4;  //ǰ�ĸ�ƫ���ֵ
////  }
//  Gyro_Bias=gyroy1[5]-gyroy1[4];  //��βֵƫ��(���ж��Ƿ��˵�)
//
//  if(Bias_ave>0)  //ƫ���ֵ������
//  {
//    if(Gyro_Bias>0)  //Bias_ave>0,Gyro_Bias>0
//    {
//      /*�������س������ϼ��*/
//      if( Gyro_Bias>Bias_ave*3 )   //����ֵ�Ĳ��̫�����ϴα����㣬���¼����ϴ�ƫ��
//      {
//        gyroy_new=gyroy1[4]+Bias_ave;  //��ƫ���ֵԤ���β��ֵ
//        gyroy1[5]=gyroy_new;  //��βֵУ������
//      }
//        
//      else
//        gyroy_new=gyroy1[5];   //��������
//    }
//    else    //Bias_ave>0,Gyro_Bias<0
//    {  
//      //��ȥ���ڶ�βֵ��ȡ����ɵ���һ�εĶ�βƫ���С����
//      if( ((Bias_ave+Gyro_Bias)<40)&&((Bias_ave+Gyro_Bias)>-40) )  
//          gyroy_new=gyroy1[5];   //��������
//      else  /*�������س����½����*/
//      {
//        gyroy_new=gyroy1[4]+Bias_ave;  //��ƫ���ֵԤ���β��ֵ
//        gyroy1[5]=gyroy_new;  //��βֵУ������
//      }
//    }
//  }
//  else   //ƫ���ֵС����
//  {
//    if(Gyro_Bias<0)  //Bias_ave<0,Gyro_Bias<0
//    {
//      /*���½��س������¼��*/
//      if( Gyro_Bias<Bias_ave*3 )   //����ֵ�Ĳ��̫�����ϴα����㣬���¼����ϴ�ƫ��
//      {
//        gyroy_new=gyroy1[4]+Bias_ave;  //��ƫ���ֵԤ���β��ֵ
//        gyroy1[5]=gyroy_new;  //��βֵУ������
//      }
//      else
//        gyroy_new=gyroy1[5];   //��������
//    }
//    else    //Bias_ave<0,Gyro_Bias>0
//    {
//     //��ȥ���ڶ�βֵ��ȡ����ɵ���һ�εĶ�βƫ���С����
//      if( ((Bias_ave+Gyro_Bias)<40)&&((Bias_ave+Gyro_Bias)>-40) )
//        gyroy_new=gyroy1[5];   //��������
//      else   /*���½��س����������*/
//      {
//        gyroy_new=gyroy1[4]+Bias_ave;  //��ƫ���ֵԤ���β��ֵ
//        gyroy1[5]=gyroy_new;  //��βֵУ������
//      }
//    }
//  }
//  Gyro_Bias=0;
//  Bias_ave=0;
//}


void Gyro_Smoothing_filtering(int16* array, int16 data)
{
    uint8 i=0;
    int16 Bias_pre,Bias_later;
  for(i=0;i<2;i++)
  {
    gyroy1[i]=gyroy1[i+1];   //3��һ�㣬�����ȳ�
  }
  gyroy1[2]=gyroy;   //��β��ֵ����
  Bias_pre=gyroy1[1]-gyroy1[0];    if(Bias_pre<0)  Bias_pre=-Bias_pre;
  Bias_later=gyroy1[2]-gyroy1[1];  if(Bias_later<0)  Bias_later=-Bias_later;
//  if((Bias_pre>100)&&(Bias_later>100))   
  if( gyroy1[1]==0 ||((Bias_pre>70)&&(Bias_later>70)) )  //��Ϊ0���߳���ƫ����ֵ
  {
      
    gyroy1[1]=(gyroy1[0]+gyroy1[2])/2;
    gyroy_new=gyroy1[1];
  }
  else
    gyroy_new=gyroy1[1];
//  if(gyroy1[1]=0)  
//  {
//    gyroy1=(gyroy1[0]+gyroy1[2])/2;
//    gyroy_new=gyroy1[1];
//  }
  
}


void Queue_In(int16* array,int16 data)
{
    uint8 i=0;
      for(i=0;i<4;i++)    //����Ԫ����͵�ַ�ƶ�
  {
    gyroy1[i]=gyroy1[i+1];   //5��һ�㣬�����ȳ�
  }
  gyroy1[4]=gyroy;  //��β��ֵ����
}

//������С���˷����������ƽ���˲�
void cubicSmooth5 ( int16 in[],int16 out[], int N )  
{  

    int i;
    if ( N < 5 )  
    {  
        for ( i = 0; i <= N - 1; i++ )  
            out[i] = in[i];  
    }  
    
    else   
    {  
        out[0] = (69.0 * in[0] + 4.0 * in[1] - 6.0 * in[2] + 4.0 * in[3] - in[4]) / 70.0;  
        out[1] = (2.0 * in[0] + 27.0 * in[1] + 12.0 * in[2] - 8.0 * in[3] + 2.0 * in[4]) / 35.0;  
        for ( i = 2; i <= N - 3; i++ )  
        {  
            out[i] = (-3.0 * (in[i - 2] + in[i + 2])+ 12.0 * (in[i - 1] + in[i + 1]) + 17.0 * in[i] ) / 35.0;  
        }  
        out[N - 2] = (2.0 * in[N - 5] - 8.0 * in[N - 4] + 12.0 * in[N - 3] + 27.0 * in[N - 2] + 2.0 * in[N - 1]) / 35.0;  
        out[N - 1] = (- in[N - 5] + 4.0 * in[N - 4] - 6.0 * in[N - 3] + 4.0 * in[N - 2] + 69.0 * in[N - 1]) / 70.0;  
    }  
     
}



//u8 Gyro_Filtering(short *Gyro)
//{   
////  short E;
////  short EC;
////  short Pre_Gyro;
////  short Pre_Pre_Gyro;
////  E = abs(*Gyro - Pre_Gyro);
////  
////  if(E < 6000){
////    
////    Pre_Gyro = *Gyro;
////    return *Gyro;
////    
////  }
////  else{
////    Pre_Gyro = *Gyro;
//   uint8 i=0;
//   short Gyro_Output;
//   short Bias_gyro_new,Bias_gyro_old;
//
//
//  for(i=0;i<2;i++)
//  {
//    Gyro[i] = Gyro[i+1];   //����һ�㣬�����ȳ�
//  }
//  
//  Gyro[2] = *Gyro;   //��β��ֵ����
//
//   Bias_gyro_new=Gyro[2]-Gyro[1];  if(Bias_gyro_new<0) Bias_gyro_new=-Bias_gyro_new;
//  Bias_gyro_old=Gyro[1]-Gyro[0];  if(Bias_gyro_old<0) Bias_gyro_old=-Bias_gyro_old;
//  
//  if( Bias_gyro_new > Bias_gyro_old+900 )   //����ֵ�Ĳ��̫�����ϴα����㣬���¼����ϴ�ƫ��
//    Gyro_Output=Gyro[1]+(Gyro[1]-Gyro[0]);
//  else 
//    Gyro_Output=Gyro[2];
//
////  Bias_gyro_new = abs(Gyro[2]-Gyro[1]);
//// // if(Bias_gyro_new<0) Bias_gyro_new=-Bias_gyro_new;
//////  Bias_gyro_old = abs(Gyro[1]-Gyro[0]);
////  //if(Bias_gyro_old<0) Bias_gyro_old=-Bias_gyro_old;
////  
////  if( Bias_gyro_new < 6000 ){   //����ֵ�Ĳ��̫�����ϴα����㣬���¼����ϴ�ƫ��
////    
////     Gyro_Output = Gyro[2];
////  
////  }
////  else 
////    Gyro_Output = Gyro[1];
//
//  // gyroy_output = Gyro[2];
//   return Gyro_Output;
//}





//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
        
        *ax=*ax/16363.5*9.8;
        *ay=*ay/16363.5*9.8;
        *az=*az/16363.5*9.8;
        
        
    return res;
}

//�õ�������ֵ(ԭʼֵ)
//mx,my,mz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������

u8 MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
    u8 buf[6],res;  
//        gg = MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
//        res= gg;
	res=MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
	if(res!=0)
	{
		*mx=((u16)buf[1]<<8)|buf[0];  
		*my=((u16)buf[3]<<8)|buf[2];  
		*mz=((u16)buf[5]<<8)|buf[4];
	} 	
    MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963ÿ�ζ����Ժ���Ҫ��������Ϊ���β���ģʽ
    return res;
}

//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    u8 i;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    for(i=0;i<len;i++)
    {
        IIC_Send_Byte(buf[i]);  //��������
        if(IIC_Wait_Ack())      //�ȴ�ACK
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_Stop();
    return 0;
} 

//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    IIC_Start();                
    IIC_Send_Byte((addr<<1)|1); //����������ַ+������
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);//������,����nACK 
		else *buf=IIC_Read_Byte(1);		//������,����ACK  
		len--;
		buf++;  
    }
    IIC_Stop();                 //����һ��ֹͣ����
    return 0;       
//        u8 res;
//    IIC_Start();
//    IIC_Send_Byte((addr<<1)|0); //����?��?��?t��??��+D��?����?
//    IIC_Wait_Ack();             //�̨���y��|��e
//    IIC_Send_Byte(reg);         //D��??��??�¦�??��
//    IIC_Wait_Ack();             //�̨���y��|��e
//	IIC_Start();                
//    IIC_Send_Byte((addr<<1)|1); //����?��?��?t��??��+?��?����?
//    IIC_Wait_Ack();             //�̨���y��|��e
//    res=IIC_Read_Byte(0);		//?����y?Y,����?��nACK  
//    IIC_Stop();                 //2��������???����?1��??t
//    return 0;  
}

//IICдһ���ֽ� 
//devaddr:����IIC��ַ
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    IIC_Send_Byte(data);        //��������
    if(IIC_Wait_Ack())          //�ȴ�ACK
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    return 0;
}

//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
    u8 res;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
	  IIC_Start();                
    IIC_Send_Byte((addr<<1)|1); //����������ַ+������
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    res=IIC_Read_Byte(0);		//������,����nACK  
    IIC_Stop();                 //����һ��ֹͣ����
    return res;  
}


//�����ȡ������false
bool MPU9250_DataRead(MPU9250Datatypedef *Q)
{
  uint8 Register[6] = {0};
  if(MPU_Read_Len(MPU9250_ADDR, MPU_GYRO_XOUTH_REG, 6,Register ) != 0)return false; 
  Q->GYROXdata = (float)((int16)((Register[0] << 8) | Register[1]));
  Q->GYROYdata = (float)((int16)((Register[2] << 8) | Register[3]));
  Q->GYROZdata = (float)((int16)((Register[4] << 8) | Register[5]));
  if(MPU_Read_Len(MPU9250_ADDR, MPU_ACCEL_XOUTH_REG, 6,Register ) != 0)return false; 
  Q->ACCXdata = (float)((int16)((Register[0] << 8) | Register[1]));
  Q->ACCYdata = (float)((int16)((Register[2] << 8) | Register[3]));
  Q->ACCZdata = (float)((int16)((Register[4] << 8) | Register[5]));
  return true;
}



