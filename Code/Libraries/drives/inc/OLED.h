
#ifndef _SEEKFREE_OELD_H
#define _SEEKFREE_OELD_H

//----宏定义OLED引脚----	 
#define  OLED_SCL_PIN	A2//CLK
#define  OLED_SDA_PIN	A3//MOSI
#define  OLED_DC_PIN	D3
#define  OLED_CS_PIN	D2

//OLED X坐标（6*8），一共21个字一行
#define bit1        0
#define bit2        bit1+6
#define bit3        bit2+6
#define bit4        bit3+6
#define bit5        bit4+6
#define bit6        bit5+6
#define bit7        bit6+6
#define bit8        bit7+6
#define bit9        bit8+6
#define bit10       bit9+6
#define bit11       bit10+6
#define bit12       bit11+6
#define bit13       bit12+6
#define bit14       bit13+6
#define bit15       bit14+6
#define bit16       bit15+6
#define bit17       bit16+6
#define bit18       bit17+6
#define bit19       bit18+6
#define bit20       bit19+6
#define bit21       bit20+6
#define X(x)        x*6 - 6
#define OLEDXEND    X(21)

void Dly_ms(uint16 ms);
void OLED_Init(void);
void OLED_Fill(uint8 dat);
void OLED_WrDat(uint8 data);
void OLED_Wr6Dat(uint8 data);
void OLED_PutPixel(uint8 x,uint8 y);
void OLED_ClrPixel(uint8 x,uint8 y);
void OLED_Set_Pos(uint8 x, uint8 y);
void OLED_P6x8Str(uint8 x,uint8 y,const uint8 ch[]);
void OLED_P8x16Str(uint8 x,uint8 y,uint8 ch[]);
void OLED_HEXACSII(uint16 hex,uint8* Print);
void OLED_Print_Num(uint8 x, uint8 y, uint16 num);
void OLED_Print_Num1(uint8 x, uint8 y, int16 num);
void dis_bmp(uint16 high, uint16 width, uint8 *p,uint8 value);

void OLED_P6x8char(uint8 x,uint8 y,uint8 ch);           //打印一个字符,printf重定向使用
uint8_t* float2str(double num,int n);                    //浮点数转字符串，printf使用
int Fputc(int ch, FILE *f);      //printf重定向

//OLED下的printf，只是多加了x,y的定位,会和printf冲突
signed int OLED_printf(uint8_t x,uint8_t y,const char *pFormat, ...); 

uint8_t getx();
uint8_t gety();

/********************************************************************/

#endif
