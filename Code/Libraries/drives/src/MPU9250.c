#include "mpu9250.h"
#include "mpuiic.h"
#include "mpuiic.h"

#include "common.h"
#include "headfile.h"
#include "stdbool.h"


/*int16 gyroy_new=0,gyroy_pointer=0;
int16* queue_pointer=&gyroy_pointer;  
int16* H=&gyroy_pointer;  //队列指针(为了被分配相同的地址)
int16* T=&gyroy_pointer;*/

extern float pitch,roll,yaw;
extern short aacx,aacy,aacz;	        //  Gyro
extern short gyrox,gyroy,gyroz;        //  Acceleration
extern int16 gyroy1[3];  //滤波数组
extern int16 gyroy_new;
//初始化MPU9250
//返回值:0,成功
//    其他,错误代码
u8 MPU9250_Init(void)
{
    u8 res=0;
    IIC_Init();     //初始化IIC总线
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//复位MPU9250
    pit_delay_ms(pit1,100);  //延时100ms
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//唤醒MPU9250
    MPU_Set_Gyro_Fsr(3);			  	//陀螺仪传感器,±2000dps
	  MPU_Set_Accel_Fsr(0);		     	 	//加速度传感器,±2g
    MPU_Set_Rate(50);			     	 	//设置采样率50Hz
    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //关闭所有中断
	  MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
	  MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	  MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //读取MPU6500的ID
    if(res==MPU6500_ID) //器件ID正确
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//加速度与陀螺仪都工作
        MPU_Set_Rate(50);						       	//设置采样率为50Hz   
    }else return 1;
 
    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//读取AK8963 ID   
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//设置AK8963为单次测量模式
    }else return 1;

    return 0;
}
/*自加中断函数*/
void SetDMP_Interrupt(void)
{
  
      //1001 1100  选择中断电平，逻辑电平为0  50us高   查询立即自动清除
  MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0x9c);	//设置中断
  MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0x01);	//复位MPU6050
//  MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
  
}


//设置MPU9250陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}
//设置MPU9250加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}

//设置MPU9250的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//设置数字低通滤波器  
}

//设置MPU9250的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
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
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
//  if(P=Q)  //第一次入列
//  {
//    *P=data;
//  }
//  else
//  *(++Q)=data;
//  
//  if(Q>M+4)   //若到了队尾，需转到队首
//  {
//    P++;   //队首指针后移
//    a=*Q;  //备份队尾值
//    Q=M;   //队尾指针转到队首
//    *Q=a;  //队首地址的值为刚才的队尾值
//  }
//  if(P>M+4)
//  {
//    //队尾指针后移
//    a=*P;  //备份队尾值
//    P=M;   //队尾指针转到队首
//    *P=a;  //队首地址的值为刚才的队尾值
//  }
//}






//500取中值
//0  1  2  3  4  5   gyro1
// 0  1  2  3  4     Bias

/*关于队尾值的更新问题：之前是忘了更新队尾值而导致下一次计算时队尾值变为了滤波数组的倒数第二个值，
但当我把滤波后的值赋给滤波数组的队尾值时，发现如果后面的新测的值与数组内的值偏差过大的话，队尾值就
进不来，导致数值往一个方向变或者不变
*/

//void Gyro_Smoothing_filtering(int16* array, int16 data)
//{  
//  uint8 i=0;
//  int Gyro_Bias,Bias_ave;  //局部变量无需清零
//  for(i=0;i<5;i++)
//  {
//    gyroy1[i]=gyroy1[i+1];   //6个一算，先入先出
//  }
//  gyroy1[5]=gyroy;   //队尾数值更新
//  Bias_ave=(gyroy1[4]-gyroy1[0])*1.0/4;   //偏差均值
////  for(i=0;i<4;i++)   //刨去队尾的值,5个值算出4个误差
////  {
////    
////    Gyro_Bias[i]=gyroy1[i+1]-gyroy1[i];
////    Bias_sum+=Gyro_Bias[i];   //0 1 2 3(共四个)
////    if(i==3)  Bias_ave=(Bias_sum*1.0)/4;  //前四个偏差均值
////  }
//  Gyro_Bias=gyroy1[5]-gyroy1[4];  //队尾值偏差(需判断是否滤掉)
//
//  if(Bias_ave>0)  //偏差均值大于零
//  {
//    if(Gyro_Bias>0)  //Bias_ave>0,Gyro_Bias>0
//    {
//      /*①上升沿出现向上尖峰*/
//      if( Gyro_Bias>Bias_ave*3 )   //若差值的差别太大则按上次变差计算，重新计算上次偏差
//      {
//        gyroy_new=gyroy1[4]+Bias_ave;  //按偏差均值预测队尾数值
//        gyroy1[5]=gyroy_new;  //队尾值校正更新
//      }
//        
//      else
//        gyroy_new=gyroy1[5];   //满足则不滤
//    }
//    else    //Bias_ave>0,Gyro_Bias<0
//    {  
//      //除去由于队尾值的取舍造成的下一次的队尾偏差的小波动
//      if( ((Bias_ave+Gyro_Bias)<40)&&((Bias_ave+Gyro_Bias)>-40) )  
//          gyroy_new=gyroy1[5];   //满足则不滤
//      else  /*②上升沿出现下降尖峰*/
//      {
//        gyroy_new=gyroy1[4]+Bias_ave;  //按偏差均值预测队尾数值
//        gyroy1[5]=gyroy_new;  //队尾值校正更新
//      }
//    }
//  }
//  else   //偏差均值小于零
//  {
//    if(Gyro_Bias<0)  //Bias_ave<0,Gyro_Bias<0
//    {
//      /*③下降沿出现向下尖峰*/
//      if( Gyro_Bias<Bias_ave*3 )   //若差值的差别太大则按上次变差计算，重新计算上次偏差
//      {
//        gyroy_new=gyroy1[4]+Bias_ave;  //按偏差均值预测队尾数值
//        gyroy1[5]=gyroy_new;  //队尾值校正更新
//      }
//      else
//        gyroy_new=gyroy1[5];   //满足则不滤
//    }
//    else    //Bias_ave<0,Gyro_Bias>0
//    {
//     //除去由于队尾值的取舍造成的下一次的队尾偏差的小波动
//      if( ((Bias_ave+Gyro_Bias)<40)&&((Bias_ave+Gyro_Bias)>-40) )
//        gyroy_new=gyroy1[5];   //满足则不滤
//      else   /*④下降沿出现上升尖峰*/
//      {
//        gyroy_new=gyroy1[4]+Bias_ave;  //按偏差均值预测队尾数值
//        gyroy1[5]=gyroy_new;  //队尾值校正更新
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
    gyroy1[i]=gyroy1[i+1];   //3个一算，先入先出
  }
  gyroy1[2]=gyroy;   //队尾数值更新
  Bias_pre=gyroy1[1]-gyroy1[0];    if(Bias_pre<0)  Bias_pre=-Bias_pre;
  Bias_later=gyroy1[2]-gyroy1[1];  if(Bias_later<0)  Bias_later=-Bias_later;
//  if((Bias_pre>100)&&(Bias_later>100))   
  if( gyroy1[1]==0 ||((Bias_pre>70)&&(Bias_later>70)) )  //若为0或者超过偏差阈值
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
      for(i=0;i<4;i++)    //所有元素向低地址移动
  {
    gyroy1[i]=gyroy1[i+1];   //5个一算，先入先出
  }
  gyroy1[4]=gyroy;  //队尾数值更新
}

//基于最小二乘法的五点三次平滑滤波
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
//    Gyro[i] = Gyro[i+1];   //三个一算，先入先出
//  }
//  
//  Gyro[2] = *Gyro;   //队尾数值更新
//
//   Bias_gyro_new=Gyro[2]-Gyro[1];  if(Bias_gyro_new<0) Bias_gyro_new=-Bias_gyro_new;
//  Bias_gyro_old=Gyro[1]-Gyro[0];  if(Bias_gyro_old<0) Bias_gyro_old=-Bias_gyro_old;
//  
//  if( Bias_gyro_new > Bias_gyro_old+900 )   //若差值的差别太大则按上次变差计算，重新计算上次偏差
//    Gyro_Output=Gyro[1]+(Gyro[1]-Gyro[0]);
//  else 
//    Gyro_Output=Gyro[2];
//
////  Bias_gyro_new = abs(Gyro[2]-Gyro[1]);
//// // if(Bias_gyro_new<0) Bias_gyro_new=-Bias_gyro_new;
//////  Bias_gyro_old = abs(Gyro[1]-Gyro[0]);
////  //if(Bias_gyro_old<0) Bias_gyro_old=-Bias_gyro_old;
////  
////  if( Bias_gyro_new < 6000 ){   //若差值的差别太大则按上次变差计算，重新计算上次偏差
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





//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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

//得到磁力计值(原始值)
//mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码

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
    MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963每次读完以后都需要重新设置为单次测量模式
    return res;
}

//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    u8 i;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    for(i=0;i<len;i++)
    {
        IIC_Send_Byte(buf[i]);  //发送数据
        if(IIC_Wait_Ack())      //等待ACK
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_Stop();
    return 0;
} 

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    IIC_Start();                
    IIC_Send_Byte((addr<<1)|1); //发送器件地址+读命令
    IIC_Wait_Ack();             //等待应答
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf=IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++;  
    }
    IIC_Stop();                 //产生一个停止条件
    return 0;       
//        u8 res;
//    IIC_Start();
//    IIC_Send_Byte((addr<<1)|0); //·￠?í?÷?tμ??·+D′?üá?
//    IIC_Wait_Ack();             //μè′yó|′e
//    IIC_Send_Byte(reg);         //D′??′??÷μ??·
//    IIC_Wait_Ack();             //μè′yó|′e
//	IIC_Start();                
//    IIC_Send_Byte((addr<<1)|1); //·￠?í?÷?tμ??·+?á?üá?
//    IIC_Wait_Ack();             //μè′yó|′e
//    res=IIC_Read_Byte(0);		//?áêy?Y,·￠?ínACK  
//    IIC_Stop();                 //2úéúò???í￡?1ì??t
//    return 0;  
}

//IIC写一个字节 
//devaddr:器件IIC地址
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    IIC_Send_Byte(data);        //发送数据
    if(IIC_Wait_Ack())          //等待ACK
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    return 0;
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
    u8 res;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    IIC_Wait_Ack();             //等待应答
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
	  IIC_Start();                
    IIC_Send_Byte((addr<<1)|1); //发送器件地址+读命令
    IIC_Wait_Ack();             //等待应答
    res=IIC_Read_Byte(0);		//读数据,发送nACK  
    IIC_Stop();                 //产生一个停止条件
    return res;  
}


//错误读取将返回false
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



