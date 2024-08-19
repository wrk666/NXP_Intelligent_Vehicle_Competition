
//调参界面ChangeParameterVal()函数修改某变量的值的时候使用
typedef enum{

	pint8_t,
	puint8_t,
	pint16_t,
	puint16_t,
        pint32_t,
        puint32_t,
	pfloat,
	pdouble,
}parameter_type;

#define KeyUPin          H7
#define KeyDPin          H1
#define KeyRPin          H5
#define KeyLPin          H6
#define KeyCPin          H0
#define BeepPin          D1


#define Key_Up           gpio_get(H7)
#define Key_Down         gpio_get(H1)
#define Key_Right        gpio_get(H5)
#define Key_Left         gpio_get(H6)
#define Key_Center       gpio_get(H0)
extern uint8_t PWM_flag;

extern int16_t Class;

void Key_IO_Init();
uint8_t GetKeyInpt();
void clean_invalid_char(uint8_t endx);
void ChangeParameterVal(parameter_type ptype,void* ppara,double precision,uint8_t keyx);
void ChangeFlagVal(uint8_t *flag,uint8_t keyx,uint8_t min,uint8_t max);
void OLED_ConfigParameter();
void Beep(uint8_t time_ms);