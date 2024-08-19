#include "isr.h"
#include "headfile.h"

                                                 //  15   14   13    12
vuint32 flag;            //�������ͱ���Ϊuint32 FFFF (0    0    0     0)FFF->&0x0000(1110)000
vuint8  PWM_flag=0;

//void PIT_CH0_IRQHandler(void)
//{
//    PIT_FlAG_CLR(pit0);
//    
//    Attitude_Calculate();  //��̬����
//    
//    
//}

void PIT_CH1_IRQHandler(void)
{
    PIT_FlAG_CLR(pit1);

}

void IRQ_IRQHandler(void)
{
    CLEAR_IRQ_FLAG;
    
}


void KBI1_IRQHandler(void)
{
   flag = READ_KBI1_FLAG;  //�ȶ�ȡ��־λ
   flag&=0xE3000000;
   switch(flag)
  {
    case 0x01000000: 
      LED_ON(YELLOW);
      break; //Start
    case 0x02000000: LED_ON(BLUE);break; //D
    case 0x20000000:      
      Bee_Alarm(); 
      pit_delay_ms(pit1,1000);
      Bee_DeAlarm();
      PWM_flag=1; 
      break; //B
    case 0x40000000:      
      LED_ON(WHITE);
      break; //C
    case 0x80000000: break; //A
    default:break;
   }
    CLEAN_KBI1_FLAG;        //Ȼ�������־λ  
    
}





/*
�жϺ������ƣ��������ö�Ӧ���ܵ��жϺ���
Sample usage:��ǰ���������ڶ�ʱ�� ͨ��0���ж�
void PIT_CH0_IRQHandler(void)
{
    ;
}
�ǵý����жϺ������־λ

FTMRE_IRQHandler      
PMC_IRQHandler        
IRQ_IRQHandler        
I2C0_IRQHandler       
I2C1_IRQHandler       
SPI0_IRQHandler       
SPI1_IRQHandler       
UART0_IRQHandler 
UART1_IRQHandler 
UART2_IRQHandler 
ADC0_IRQHandler       
ACMP0_IRQHandler      
FTM0_IRQHandler       
FTM1_IRQHandler       
FTM2_IRQHandler       
RTC_IRQHandler        
ACMP1_IRQHandler      
PIT_CH0_IRQHandler    
PIT_CH1_IRQHandler    
KBI0_IRQHandler       
KBI1_IRQHandler       
Reserved26_IRQHandler 
ICS_IRQHandler        
WDG_IRQHandler        
PWT_IRQHandler        
MSCAN_Rx_IRQHandler   
MSCAN_Tx_IRQHandler   
*/