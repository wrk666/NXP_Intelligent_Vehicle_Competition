/*************************************************************************************/
#include "common.h"
#include "headfile.h"
#include "codetab.h"
/********************
*
* ��IO��ģ��SPI
*
* P1.0�ڽ�ģ���ϵ� D/C
* P1.1�ڽ�ģ���ϵ� CS
* P1.2�ڽ�ģ���ϵ� MOSI
* P1.3�ڽ�ģ���ϵ� CLK
*
*********************/
//extern uint16 times_cnt;
#define PORT_DC     D3
#define PORT_CS     D2
#define PORT_MOSI   A3
#define PORT_CLK    A2
//#define PORT_DC     PTD2
//#define PORT_CS     PTD4
//#define PORT_MOSI   PTD7
//#define PORT_CLK    PTD8

//Һ�����ƿ���1�������궨�� 
#define	LCD_DC_SET  	gpio_set (PORT_DC, 1)  
#define	LCD_CS_SET  	gpio_set (PORT_CS, 1) 
#define	LCD_MOSI_SET  	gpio_set (PORT_MOSI, 1)    
#define	LCD_CLK_SET  	gpio_set (PORT_CLK, 1)     

//Һ�����ƿ���0�������궨��  
#define	LCD_DC_CLR  	gpio_set (PORT_DC, 0)    
#define	LCD_CS_CLR  	gpio_set (PORT_CS, 0)   
#define	LCD_MOSI_CLR  	gpio_set (PORT_MOSI, 0)   
#define	LCD_CLK_CLR  	gpio_set (PORT_CLK, 0) 

#define XLevelL			0x00
#define XLevelH			0x10
#define XLevel	    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	132
#define Max_Row			64
#define	Brightness	0xCF
#define X_WIDTH 		132
#define Y_WIDTH 		64

//����oled�Դ�
uint8 OLED_GRAM[128][8];
/**********************��ʱ����************************************/
void delay(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
/*********************LCDд����************************************/ 
void LCD_WrDat(unsigned char dat)	 
{
	unsigned char i;
	LCD_DC_SET;  
	for(i=0;i<8;i++) //����һ����λ���� ,ͨ�������ߵĸߵ͵�ƽ����������
	{
		if((dat << i) & 0x80)
		{
			LCD_MOSI_SET;
		}
		else  
                  LCD_MOSI_CLR;
                
		LCD_CLK_CLR;
		LCD_CLK_SET;
	}
}
/*********************LCDд����************************************/										
void LCD_WrCmd(unsigned char cmd)
{
	unsigned char i;
	LCD_DC_CLR;
	for(i=0;i<8;i++) //����һ����λ���� 
	{
		if((cmd << i) & 0x80)  //�����1
		{
                  LCD_MOSI_SET;  //ģ���������Ϊ�ߵ�ƽ
		}
		else   //���Ϊ0
		{
                  LCD_MOSI_CLR;   //ģ���������Ϊ�͵�ƽ
		}
	        LCD_CLK_CLR;  //ʱ���߲���һ��������
		LCD_CLK_SET;
	}
}
/*********************LCD ��������************************************/
void LCD_Set_Pos(unsigned char x, unsigned char y) 
{ 
	LCD_WrCmd(0xb0+y);
	LCD_WrCmd(((x&0xf0)>>4)|0x10);//LCD_WrCmd(((x&0xf0)>>4)|0x10);
	LCD_WrCmd((x&0x0f)|0x01);//LCD_WrCmd((x&0x0f)|0x01);
} 
/*********************LCDȫ��************************************/
void LCD_Fill(unsigned char bmp_dat) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		LCD_WrDat(bmp_dat);
	}
}
/*********************OLED����************************************/
void LCD_CLS(void)
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		LCD_WrDat(0);
	}
}
/*********************OLED��ʼ��************************************/
void LCD_Init(void)
{
	//delay(100);
	LCD_CS_CLR;
	LCD_WrCmd(0xae);//--turn off oled panel
	LCD_WrCmd(0x00);//---set low column address
	LCD_WrCmd(0x10);//---set high column address
	LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	LCD_WrCmd(0x81);//--set contrast control register
	LCD_WrCmd(0xcf); // Set SEG Output Current Brightness
	LCD_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	LCD_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	LCD_WrCmd(0xa6);//--set normal display
	LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	LCD_WrCmd(0x3f);//--1/64 duty
	LCD_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	LCD_WrCmd(0x00);//-not offset
	LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	LCD_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	LCD_WrCmd(0xd9);//--set pre-charge period
	LCD_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	LCD_WrCmd(0xda);//--set com pins hardware configuration
	LCD_WrCmd(0x12);
	LCD_WrCmd(0xdb);//--set vcomh
	LCD_WrCmd(0x40);//Set VCOM Deselect Level
	LCD_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	LCD_WrCmd(0x02);//
	LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
	LCD_WrCmd(0x14);//--set(0x10) disable
	LCD_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	LCD_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	LCD_WrCmd(0xaf);//--turn on oled panel
	LCD_Fill(0x00);  //��ʼ����
	LCD_Set_Pos(0,0); 	
} 
/***************������������ʾ6*8һ���׼ASCII�ַ���	��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void LCD_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		LCD_Set_Pos(x,y);
		for(i=0;i<6;i++)
		LCD_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
}
/*******************������������ʾ8*16һ���׼ASCII�ַ���	 ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void LCD_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		LCD_Set_Pos(x,y);
		for(i=0;i<8;i++)
		LCD_WrDat(F8X16[c*16+i]);
		LCD_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		LCD_WrDat(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}
/*****************������������ʾ16*16����  ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************************/
void LCD_P16x16Ch(unsigned char x,unsigned char y,unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	LCD_Set_Pos(x+2, y);
	for(wm = 0;wm < 16;wm++)
	{
		LCD_WrDat(F16x16[adder]);
		adder += 1;
	}
	LCD_Set_Pos(x+2,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		LCD_WrDat(F16x16[adder]);
		adder += 1;
	} 	  	
}
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		LCD_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	LCD_WrDat(BMP[j++]);	    	
	    }
	}
}



//������  x=0~127     y=0~7
void oled_draw_line(uint8 x,uint8 y,uint8 data)
{
   LCD_Set_Pos(x,y);
   //OLED_WrDat(0xff);
   LCD_WrDat(data);
}
void oled_draw_point(uint8 x,uint8 y)
{
   uint8 y0,y1;
   y0=y/8;
   y1=y%8;
   
    if(y1==0)
    {
    LCD_Set_Pos(x,y0);
    LCD_WrDat(0x01);
    }
    else if(y1==1)
    {
    LCD_Set_Pos(x,y0);
    LCD_WrDat(0x02);
    }
    else if(y1==2)
    {
    LCD_Set_Pos(x,y0);
    LCD_WrDat(0x04);
    }
    else if(y1==3)
    {
    LCD_Set_Pos(x,y0);
    LCD_WrDat(0x08);
    }
    else if(y1==4)
    {
    LCD_Set_Pos(x,y0);
    LCD_WrDat(0x10);
    }
    else if(y1==5)
    {
    LCD_Set_Pos(x,y0);
    LCD_WrDat(0x20);
    }
    else if(y1==6)
    {
    LCD_Set_Pos(x,y0);
    LCD_WrDat(0x40);
    }
    else if(y1==7)
    {
    LCD_Set_Pos(x,y0);
    LCD_WrDat(0x80);
    }
}
			   
void OLED_DrawPoint(uint8 x,uint8 y,uint8 t)
{
	uint8 pos,bx,temp=0;
	if(x>127||y>63)return;
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

void OLED_Refresh_Gram(void)
{
	uint8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		LCD_WrCmd (0xb0+i);    
		LCD_WrCmd (0x00);      
		LCD_WrCmd (0x10);     
		for(n=0;n<128;n++)LCD_WrDat(OLED_GRAM[n][i]); 
	}   
}



//void show_imgb_in_oled()
//{
// 
//  uint8_t *imbuff = imgbuff;
// char x=0,y=0;
// for(x=0;x<60;x++)
// {    imbuff++;
//      for(y=7;y>=0;y--)
//      {
//           oled_draw_line(x,y,*imbuff);
//           imbuff++;
//      }
//      imbuff++;
// }
//               
//}
void show_img_in_oled(void *imgp)//
{
  int32 i,j;
  uint8_t *imbuff = imgp;
   for(i=0;i<128;i++)
  {
    for(j=0;j<8;j++)
    {
       OLED_GRAM[i][j]=0;
    }
  }
  for(i=59;i>=0;i--)
  {
    for(j=0;j<80;j++)
    {
       if(*imbuff==0) //0�ڵ�
       OLED_DrawPoint(j,i,1);
       imbuff++;
    }
  }
  
  OLED_Refresh_Gram(); 
}

void Show_bit(uint8_t num , uint8_t x ,uint8_t y)    //��ʾ����
{ 
  uint8_t  X=0;
  X = num;
  switch (X)
  {
    case 0:
      LCD_P6x8Str(x,y,"0");
      break;
    case 1:
      LCD_P6x8Str(x,y,"1");
      break;
    case 2:
      LCD_P6x8Str(x,y,"2");
      break;
    case 3:
      LCD_P6x8Str(x,y,"3");
      break;
    case 4:
      LCD_P6x8Str(x,y,"4");
      break;
    case 5:
      LCD_P6x8Str(x,y,"5");
      break;
    case 6:
      LCD_P6x8Str(x,y,"6");
      break;
    case 7:
      LCD_P6x8Str(x,y,"7");
      break;
    case 8:
      LCD_P6x8Str(x,y,"8");
      break;
    case 9:
      LCD_P6x8Str(x,y,"9");
      break;
    case 10:
      LCD_P6x8Str(x,y," ");
      break;
    default :
      break;
  }
}

void Get_Bit(float num ,uint8 x ,uint8 y)  //��ȡ���ֵ�λ
{
  uint8 a,b,c,d,e,f;
  //��������
  if(num>0)
  {
    LCD_P6x8Str(x+6,y,"+");
//        LCD_P6x8Str(x,y,"+");

     if(num>999)  //��������������λ��  6X8��ʾ
     {  
        a = ((int)num)/1000;                              if(a>0) {Show_bit( a, x+6 ,y);}
        b = ((int)num%1000)/100;                           if(a>0||b>0) {Show_bit( b, x+12 ,y); }
        c = ((int)num%100)/10;                             Show_bit( c, x+18 ,y);
        d =  (int)num%10;                                 Show_bit(d,x+24 ,y);
        LCD_P6x8Str(x+24,y,".");
        LCD_P6x8Str(x+30,y,"0");
     }
    else   //����������С����λ��
    {
      
      a = ((int)num)/100;                              if(a>0) {Show_bit( a, x+12 ,y);}   else Show_bit(0, x+12 ,y);
      b = ((int)num%100)/10;                           if(a>0||b>0) {Show_bit( b, x+18 ,y); }  else Show_bit(0, x+18 ,y);
      c = ((int)num)%10;                                       Show_bit( c, x+24 ,y);
      LCD_P6x8Str(x+30,y,".");
      d = ((int)(num*10))%10; Show_bit(d,x+36 ,y);
      e = ((int)(num*100))%10; Show_bit(e,x+42 ,y);
      f = ((int)(num*1000))%10; Show_bit(f,x+48,y);
      
    }
  }
  //���Ǹ���
  else
  {
    num =(0.0-num);
    LCD_P6x8Str(x+6,y,"-");
    a = ((int)num)/100;                                  if(a>0) { Show_bit( a, x+12 ,y);}   else Show_bit(' ', x+12 ,y);
    b = ((int)num%100)/10;                               if(a>0||b>0) { Show_bit( b, x+18 ,y);}   else Show_bit(0, x+18 ,y);
    c = ((int)num)%10;                                             Show_bit( c, x+24 ,y); 
    LCD_P6x8Str(x+30,y,".");
    d = ((int)(num*10))%10; Show_bit(d,x+36 ,y);
    e = ((int)(num*100))%10; Show_bit(e,x+42 ,y);
    f = ((int)(num*1000))%10; Show_bit(f,x+48,y);
  }
  
}


void OLED_start(void)
{

       gpio_init (PORT_CLK, GPO,0);  //     CLK
       gpio_set (PORT_CLK, 1);
       gpio_init (PORT_MOSI, GPO,0);  //     MOSI
       gpio_set (PORT_MOSI, 1);
       gpio_init (PORT_CS, GPO,0); //     CS
       gpio_set (PORT_CS, 0);
       gpio_init (PORT_DC, GPO,0); //      DC
       gpio_set (PORT_DC, 1);
       
       LCD_Init(); //OLED��ʼ��  

}

extern float pitch,roll,yaw; 		//ŷ����
extern double Angle_aac;
extern float Angle_gyro;
extern short static_gyroy;
extern int16 count_right,count_left;
extern int count_speed;  //����������ٶ�
extern uint8 flag_key;
extern float Bal_Kp,Bal_Kd,Speed_Kp,Speed_Ki;
extern float Angle_Zero;   //��װ���
extern int Speed_set;   //�ٶ��趨ֵ
extern float Angle_set;  //�Ƕ��趨  
extern float Integral;   //�ٷ�Ki�Ļ���
extern float Bias;  //�ٶ�ƫ��
extern float Left_Dead_PWM,Right_Dead_PWM;  //��������
extern float Angle,Angle_dot ;//�Ƕȣ� ���ٶ� ��ȫ�ֱ���

extern EulerAngleTypedef      SystemAttitude;            /////��̬��
extern EulerAngleTypedef      SystemAttitudeRate;        /////��̬���ٶ�
void show_tirtle(void)
{
    
    LCD_P6x8Str(0,0,"pitch:");
    LCD_P6x8Str(0,1,"flag_key:");
//    LCD_P6x8Str(0,1,"count_right:");
    LCD_P6x8Str(0,2,"Bal_Kp:");
    LCD_P6x8Str(0,3,"Bal_Kd:");
    LCD_P6x8Str(0,4,"Speed_Kp:");
    

    LCD_P6x8Str(0,5,"Speed_Ki:");
    LCD_P6x8Str(0,6,"Angle:");
//    LCD_P6x8Str(0,6,"Angle_set:");
//    LCD_P6x8Str(0,6,"count_right:");
//    LCD_P6x8Str(0,7,"count_left:");
    
//    LCD_P6x8Str(0,7,"Speed_set:");
    LCD_P6x8Str(0,7,"Angle_Zero:");
//    LCD_P6x8Str(0,7,"Integral:");
//    LCD_P6x8Str(0,7,"Bias:");
    
//    LCD_P6x8Str(0,6,"Left_Dead_PWM:");
//    LCD_P6x8Str(0,7,"Right_Dead_PWM:");
//    LCD_P6x8Str(0,6,"count_right:");
//    LCD_P6x8Str(0,7,"count_left:");
    
}



void oled_shownum(void)
{


//  LCD_P6x8Str(0,2,"Angle_aac:");
//  Get_Bit(Angle_aac,70 ,2);

//  LCD_P6x8Str(0,2,"Angle_gyro:");
//  Get_Bit(Angle_gyro,70 ,2);

//  LCD_P6x8Str(0,1,"pitch:");
  Get_Bit(SystemAttitude.Pitch,70 ,0);
        
//  LCD_P6x8Str(0,2,"count_right:");
//  Get_Bit(flag_key,70 ,1);
//
//  Get_Bit(Bal_Kp,70 ,2);
//  
//  Get_Bit(Bal_Kd,70 ,3);
//  
//  Get_Bit(Speed_Kp,70 ,4);
//  
//  
//  Get_Bit(Speed_Ki,70 ,5);
//  Get_Bit(Angle,70 ,6);
//
//  
//  Get_Bit(Angle_Zero,70 ,7);

  

}




