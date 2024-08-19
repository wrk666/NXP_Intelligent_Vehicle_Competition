#include "headfile.h"
#include "Keyboard.h"
#include "OLED.h"
#include "isr.h"



void Key_IO_Init(){
  
     port_pull(KeyUPin);
     port_pull(KeyDPin);
     port_pull(KeyLPin);
     port_pull(KeyRPin);
     port_pull(KeyCPin);
        

     port_pull(B2);
    
     gpio_init(KeyUPin,GPI,0);
     gpio_init(KeyDPin,GPI,0);
     gpio_init(KeyLPin,GPI,0);
     gpio_init(KeyRPin,GPI,0);
     gpio_init(KeyCPin,GPI,0);	
     
     gpio_init(B2,GPI,1);
              
     gpio_init(BeepPin,GPO,0);	

     
}
void Beep(uint8_t time_ms){

     gpio_set(BeepPin,1);
     
     systick_delay_ms(time_ms);
     
     gpio_set(BeepPin,0);
     

}
uint8_t GetKeyInpt(){
       
     static uint8_t Key_x[] = {'U','D','L','R','C'}; //返回值
     
     uint8_t key_input[5] = {0}; //按键当前的输入状态

     key_input[0] = Key_Up;
     key_input[1] = Key_Down;
     key_input[2] = Key_Left;
     key_input[3] = Key_Right;
     key_input[4] = Key_Center;
     
     uint8_t i = 0;
     for(i = 0;i<5;i++)
     {

	if( 0 == key_input[i])
        {//某个按键被按下

	   systick_delay_ms(20); //消抖
	   return Key_x[i];  //返回按键值
	}     
     
     }
     return ' ';
}
     

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//配置系统在线调参界面核心函数



//通过修改这个使能菜单数组，可以设置某个参数号下是否能有二级菜单。1表示有二级菜单，0表示没有
uint8_t KeyInput = 0; //按键的输入
int8_t cursor_x = 0,cursor_y = 0; //光标的X,Y坐标
uint8_t state = 0; //调参状态 0 1 2 对应处于一级菜单 二级菜单 和二级菜单调整参数 三个状态
int8_t ParameterNo = 0; //一级菜单参数号
int8_t Parameter2No = 0; //二级菜单的参数号
uint8_t Pre = 5;//每次加/减的精度
uint8_t en_sec_menu[16]={1,1,1,1,1,1,1,1,0,0,0 ,0 ,1 ,0 ,1 ,0 }; //使能二级菜单
//          ParameterNo: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
uint8_t PWM_flag = 0;

//调参界面中：
//上下可以切换选中的参数
//按下左键初始化OLED（在OLED花屏之后初始化可以使小液晶回复正常显示）
//按下中心键，如果在一级菜单中且对应参数号使能了二级菜单的话，会进入到二级菜单中。在二级菜单中，如果光标为 '>' 的时候 按下中心键 会变为 '-'
//这个时候可以通过上、下键修改对应的参数，再按一次中心键 光标重新变回 '>' 参数设置完成，光标又可以通过上、下键选择参数
//如果按下右键，可以自己定义，我的程序里是启动智能车
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigParameterCore(){
	
        KeyInput = GetKeyInpt();
//////////////////////////////// 以下是调参界面的操作逻辑设置   //////////////////////////////////////////////////////////////////////////////////////////
	if(KeyInput == 'L') 
        {
          OLED_Init(); 
          In_or_Not = 0;
          Beep(10);
        }//左键初始化液晶屏 防止液晶花屏

	//移动光标
	if(state == 0){ //在一级菜单中（state = 0）

		if(KeyInput == 'U') {OLED_printf(cursor_x,cursor_y," ");ParameterNo--;Beep(30);}
		else if(KeyInput == 'D') {OLED_printf(cursor_x,cursor_y," ");ParameterNo++;Beep(30);}
	}
	else if(state == 1){ //在二级菜单中

		if(KeyInput == 'U') {OLED_printf(cursor_x,cursor_y," "); Parameter2No--;Beep(30);}
		else if(KeyInput == 'D') {OLED_printf(cursor_x,cursor_y," ");Parameter2No++;Beep(30);}
	}

	//在这修改  ParameterNo 的范围可以配置哪些一级参数号拥有二级菜单 如果改为 ParameterNo < 15 则所有的一级菜单都拥有二级界面
	//有一些标志位需要经常使用，但是每次进入二级菜单又太麻烦的时候，可以留几个一级界面的位置，直接在一级界面修改标志位，而不进入二级菜单。
	if(state == 0 && en_sec_menu[ParameterNo] == 1){ //如果使能了二级菜单就进入二级菜单
		//更改状态进入二级菜单
		if(KeyInput == 'C'){state = 1;OLED_Fill(0);Beep(30); } //清屏
	}
	else if(state == 1){ //二级菜单中

		//显示返回键
		OLED_printf(X(2),7,"Back");

		if(Parameter2No!= 6){ //没有光标选中返回时，按下中心键进入修改参数

                  if(KeyInput == 'C') {state = 2; Beep(30);}//更改状态 开始修改参数
		}
		else if(Parameter2No== 6){ //选中了返回
			if(KeyInput == 'C'){state = 0;Parameter2No = 0;OLED_Fill(0);Beep(30);} //返回一级菜单 清屏
		}
	}
	else if(state == 2){if(KeyInput == 'C') {state = 1;Beep(30);}} //修改参数完成后再按一下中心键确定修改
//////////////////////////////// 调参界面的操作逻辑设置结束   //////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// 以下是通过参数号确定光标   ///////////////////////////////////////////////////////////////////////////////////////////////////
	if(state == 0){ //一级菜单的参数

		//参数号限位
		if(ParameterNo > 14) ParameterNo = 0;
		else if(ParameterNo < 0) ParameterNo = 14;
		//通过参数号确定光标坐标
		if(ParameterNo == 0) {cursor_x = 0;cursor_y = 1;}
		else if(ParameterNo == 1) {cursor_x = 0;cursor_y = 2;}
		else if(ParameterNo == 2) {cursor_x = 0;cursor_y = 3;}
		else if(ParameterNo == 3) {cursor_x = 0;cursor_y = 4;}
		else if(ParameterNo == 4) {cursor_x = 0;cursor_y = 5;}
		else if(ParameterNo == 5) {cursor_x = 0;cursor_y = 6;}
		else if(ParameterNo == 6) {cursor_x = 0;cursor_y = 7;}
		else if(ParameterNo == 7) {cursor_x = 0;cursor_y = 7;}
		else if(ParameterNo == 8) {cursor_x = 64;cursor_y = 1;}
		else if(ParameterNo == 9) {cursor_x = 64;cursor_y = 2;}
		else if(ParameterNo == 10) {cursor_x = 64;cursor_y = 3;}
		else if(ParameterNo == 11) {cursor_x = 64;cursor_y = 4;}
		else if(ParameterNo == 12) {cursor_x = 64;cursor_y = 5;}
		else if(ParameterNo == 13) {cursor_x = 64;cursor_y = 6;}
		else if(ParameterNo == 14) {cursor_x = 64;cursor_y = 7;}
//		else if(ParameterNo == 15) {cursor_x = 64;cursor_y = 7;}
	}
	else if(state == 1){ //二级菜单的参数

		//参数号限位
		if(Parameter2No > 6) Parameter2No = 0;
		else if(Parameter2No < 0) Parameter2No = 6;
		//通过参数号确定光标坐标
		if(Parameter2No == 0) {cursor_x = 0;cursor_y = 1;}
		else if(Parameter2No == 1) {cursor_x = 0;cursor_y = 2;}
		else if(Parameter2No == 2) {cursor_x = 0;cursor_y = 3;}
		else if(Parameter2No == 3) {cursor_x = 0;cursor_y = 4;}
		else if(Parameter2No == 4) {cursor_x = 0;cursor_y = 5;}
		else if(Parameter2No == 5) {cursor_x = 0;cursor_y = 6;}
		else if(Parameter2No == 6) {cursor_x = 0;cursor_y = 7;}
	}
///////////////////////////////////// 光标位置确定结束 ///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////// 显示光标  ///////////////////////////////////////////////////////////////////////////////////////////////////
	//显示光标
	if(state == 0)
		OLED_printf(cursor_x,cursor_y,">");
	else if(state == 1)
		OLED_printf(cursor_x,cursor_y,">");
	else if(state == 2)
		OLED_P6x8char(cursor_x,cursor_y,'*');
///////////////// 显示光标结束 ///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////  保留功能按下右键    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(KeyInput == 'R') {  //按下右键
        Beep(10);
        PWM_flag =1 ;
        Motor_Stop_Flag = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//通过按键修改某个参数的值
//传入参数：parameter_type ptype ：需要修改的这个参数的类型，枚举定义在头文件中
//                                int8_t，    该变量为char型
//                                uint8_t，   该变量为unsigned char型
//                                int16_t，   该变量为short型
//                                uint16_t，  该变量为unsigned short型
//                                int32_t，   该变量为int型
//                                uint32_t，  该变量为unsigned int型
//                                pfloat，    该变量为float型
//                                pdouble，   该变量为double型
//
//         void* ppara：要修改的变量
//
//         double precision ：每次加/减的精度
//
//         uint8_t keyx ：按键的输入值
//
//传出参数：无
//用法实例：ChangeParameter(pdouble,&p,0.05,KeyInput); //修改double类型变量p的值，每次修改0.05（按键输入为'U'/'D'的时候修改）
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChangeParameterVal(parameter_type ptype,void* ppara,double precision,uint8_t keyx){

	if(keyx == 'U'){
                Beep(10);
		switch(ptype)
		{
		case pint8_t:     *(( int8_t *  )ppara) += precision;break;
		case puint8_t:    *(( uint8_t * )ppara) += precision;break;
		case pint16_t:    *(( int16_t * )ppara) += precision;break;
		case puint16_t:   *(( uint16_t *)ppara) += precision;break;
		case pint32_t:    *(( int32_t * )ppara) += precision;break;
		case puint32_t:   *(( uint32_t *)ppara) += precision;break;    
		case pfloat:      *(( float *   )ppara) += precision;break;
		case pdouble:     *(( double *  )ppara) += precision;break;
		}
	}
	else if(keyx == 'D'){
                Beep(10);
		switch(ptype)
		{
		case pint8_t:     *(( int8_t *  )ppara) -= precision;break;
		case puint8_t:    *(( uint8_t * )ppara) -= precision;break;
		case pint16_t:    *(( int16_t * )ppara) -= precision;break;
		case puint16_t:   *(( uint16_t *)ppara) -= precision;break;
		case pint32_t:    *(( int32_t * )ppara) -= precision;break;
		case puint32_t:   *(( uint32_t *)ppara) -= precision;break;    
		case pfloat:      *(( float *   )ppara) -= precision;break;
		case pdouble:     *(( double *  )ppara) -= precision;break;
		}
	}
}

//修改某标志位的值（一般用于没有二级菜单的位置）
void ChangeFlagVal(uint8_t *flag,uint8_t keyx,uint8_t min,uint8_t max){

     if(keyx == 'C'){
       
	(*flag)++;
         Beep(10);
     }
     if(*flag > max){
       *flag = min;
     }
     else if(*flag < min){ 
      *flag = max;
     }
}

//清除变量无效的显示
void clean_invalid_char(uint8_t endx){

	uint8_t i = 0;
	for(i = getx();i<=endx;i+=6){

		OLED_P6x8char(i,gety(),' ');
	}
}

uint8_t M_Flag = 0;
int16_t Track = 80;
int16_t Class=0;
//调参界面
void OLED_ConfigParameter()
{
	ConfigParameterCore(); //调参界面的核心，负责控制光标的位置，各级菜单之间切换

        if(PWM_flag==0)
        {
             //显示一级菜单内容    
        	if(state == 0)
                {
                    OLED_printf(X(8),0,"YZ Three");
                    
                    OLED_printf(X(2),1,"M_PID");  //0
                    OLED_printf(X(2),2,"GY_PID"); //1
                    OLED_printf(X(2),3,"Round");  //2
                    OLED_printf(X(2),4,"Display");//3
                    OLED_printf(X(2),5,"LR:%d",LR);
                    OLED_printf(X(2),6,"BA_IN:%d",BA_IN);
                    OLED_printf(X(2),7,"BA_OUT:%d",BA_OUT);
                    
                    OLED_printf(X(13),1,"L:%4d",Tracking_Sensor_Data_L_Temp[9]);
                    OLED_printf(X(13),2,"R:%4d",Tracking_Sensor_Data_R_Temp[9]);
                    OLED_printf(X(13),3,"B:%4d",Tracking_Sensor_Data_B_Temp[9]);
                    OLED_printf(X(13),4,"A:%4d",Tracking_Sensor_Data_A_Temp[9]);
                    OLED_printf(X(13),5,"J:%.2f",SystemAttitude.Pitch);
                    OLED_printf(X(13),6,"T:%4d",time_set);
//                    OLED_printf(X(13),3,"Pre:%d",Pre);
        
                    OLED_printf(X(13),7,"FSave");
                }
             //显示二级菜单内容
          else if(state == 1 || state == 2){ 

		switch(ParameterNo)
		{
		//PID的参数
		case 0:
			//ParameterNo = 0的二级菜单标题
		       OLED_printf(X(7),0,"M_PID");
			//显示各个参数
                       OLED_printf(X(2),1,"Bal_AP=%.2f",Angle[0]);
                       OLED_printf(X(2),2,"Bal_GP=%.2f",Bal_Gyro[0]);
                       OLED_printf(X(2),3,"Spe_P=%.2f",Speed[0]);
                       OLED_printf(X(2),4,"Spe_P=%d",Ang_put);
                       
                       clean_invalid_char(OLEDXEND);

			//修改各个参数
                       if(state == 2)
                       {

            	          if(Parameter2No == 0)ChangeParameterVal(pfloat,&Angle[0],1,KeyInput);  //角度P
                          else if(Parameter2No == 1)ChangeParameterVal(pfloat,&Bal_Gyro[0],1,KeyInput); //直立角速度P
                          else if(Parameter2No == 2)ChangeParameterVal(pfloat,&Speed[0],1,KeyInput);  //速度P
                          else if(Parameter2No == 3)ChangeParameterVal(puint8_t,&Ang_put,1,KeyInput);  //速度P

                          
                        }
                       OLED_printf(X(2),1,"Bal_AP=%.2f",Angle[0]);
                       OLED_printf(X(2),2,"Bal_GP=%.2f",Bal_Gyro[0]);
                       OLED_printf(X(2),3,"Spe_P=%.2f",Speed[0]);
                       OLED_printf(X(2),4,"Spe_P=%d",Ang_put);
                       
                       clean_invalid_char(OLEDXEND);       
                       break;
		
		case 1:
			//ParameterNo = 0的二级菜单标题
		       OLED_printf(X(7),0,"Dir_GY_PID");
			//显示各个参数
                       OLED_printf(X(2),1,"P=%.2f",Dir_Gyro[0]);
                       OLED_printf(X(2),2,"D=%.2f",Dir_Gyro[2]);
                       
                       clean_invalid_char(OLEDXEND);       

			//修改各个参数
                       if(state == 2)
                       {

            	          if(Parameter2No == 0) ChangeParameterVal(pfloat,&Dir_Gyro[0],1,KeyInput);  //方向内环P
            	          else if(Parameter2No == 1) ChangeParameterVal(pfloat,&Dir_Gyro[2],1,KeyInput);  //方向内环D
                          }
                       OLED_printf(X(2),1,"P=%.2f",Dir_Gyro[0]);
                       OLED_printf(X(2),2,"D=%.2f",Dir_Gyro[2]);
                       clean_invalid_char(OLEDXEND);                                  
                       break;
		case 2:
			//ParameterNo = 0的二级菜单标题
		       OLED_printf(X(7),0,"Roundabout");
			//显示各个参数
                       OLED_printf(X(2),1,"LR=%4d",LR);
                       OLED_printf(X(2),2,"BA_IN=%4d",BA_IN);
                       OLED_printf(X(2),3,"BA_OUT=%4d",BA_OUT);
                       OLED_printf(X(2),4,"Time=%4d",time_set);
                       OLED_printf(X(2),5,"In_rate=%2f",In_rate);                   
                       OLED_printf(X(2),6,"OUT_rate=%2f",OUT_rate);
                       clean_invalid_char(OLEDXEND);

			//修改各个参数
                       if(state == 2)
                       {

            	          if(Parameter2No == 0)ChangeParameterVal(pint16_t,&LR,100,KeyInput);
            	          else if(Parameter2No == 1)ChangeParameterVal(pint16_t,&BA_IN,100,KeyInput);
             	          else if(Parameter2No == 2)ChangeParameterVal(pint16_t,&BA_OUT,100,KeyInput);                         
             	          else if(Parameter2No == 3)ChangeParameterVal(pint16_t,&time_set,200,KeyInput);
                          else if(Parameter2No == 4)ChangeParameterVal(pfloat,&In_rate,0.01,KeyInput);
             	          else if(Parameter2No == 5)ChangeParameterVal(pfloat,&OUT_rate,0.01,KeyInput);

                        }
                       
                       OLED_printf(X(2),1,"LR=%4d",LR);
                       OLED_printf(X(2),2,"BA_IN=%4d",BA_IN);
                       OLED_printf(X(2),3,"BA_OUT=%4d",BA_OUT);
                       OLED_printf(X(2),4,"Time=%4d",time_set);    
                       OLED_printf(X(2),5,"In_rate=%2f",In_rate);                   
                       OLED_printf(X(2),6,"OUT_rate=%2f",OUT_rate);
                       
                       clean_invalid_char(OLEDXEND);                                 
		       break;   
                 case 3:
			//ParameterNo = 0的二级菜单标题
		       OLED_printf(X(7),0,"Display");
			//显示各个参数
                       OLED_printf(X(2),1,"Pitch=%.2f",SystemAttitude.Pitch);
                       OLED_printf(X(2),2,"Roll=%.2f",SystemAttitude.Roll);
                       OLED_printf(X(2),3,"Yaw=%.2f",SystemAttitude.Yaw);                 
                       clean_invalid_char(OLEDXEND);                                 
		       break;   
		}                

	}

//////////////////////  修改没有二级菜单的位置的参数   ////////////////////////////////////////////////////////////////////////
	if(state == 0)
        {
		//没有二级菜单的位置
		if(en_sec_menu[ParameterNo] == 0)
                {
			switch(ParameterNo)
			{
                        case 13:
			}
		}
	}
      }//PWM_flag





    
////////////////////////  没有二级菜单的位置的参数 结束  ////////////////////////////////////////////////////////////////////////
}
     
     
     
     
     
     
     
     
     
     
     