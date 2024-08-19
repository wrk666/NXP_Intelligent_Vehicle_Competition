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
       
     static uint8_t Key_x[] = {'U','D','L','R','C'}; //����ֵ
     
     uint8_t key_input[5] = {0}; //������ǰ������״̬

     key_input[0] = Key_Up;
     key_input[1] = Key_Down;
     key_input[2] = Key_Left;
     key_input[3] = Key_Right;
     key_input[4] = Key_Center;
     
     uint8_t i = 0;
     for(i = 0;i<5;i++)
     {

	if( 0 == key_input[i])
        {//ĳ������������

	   systick_delay_ms(20); //����
	   return Key_x[i];  //���ذ���ֵ
	}     
     
     }
     return ' ';
}
     

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//����ϵͳ���ߵ��ν�����ĺ���



//ͨ���޸����ʹ�ܲ˵����飬��������ĳ�����������Ƿ����ж����˵���1��ʾ�ж����˵���0��ʾû��
uint8_t KeyInput = 0; //����������
int8_t cursor_x = 0,cursor_y = 0; //����X,Y����
uint8_t state = 0; //����״̬ 0 1 2 ��Ӧ����һ���˵� �����˵� �Ͷ����˵��������� ����״̬
int8_t ParameterNo = 0; //һ���˵�������
int8_t Parameter2No = 0; //�����˵��Ĳ�����
uint8_t Pre = 5;//ÿ�μ�/���ľ���
uint8_t en_sec_menu[16]={1,1,1,1,1,1,1,1,0,0,0 ,0 ,1 ,0 ,1 ,0 }; //ʹ�ܶ����˵�
//          ParameterNo: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
uint8_t PWM_flag = 0;

//���ν����У�
//���¿����л�ѡ�еĲ���
//���������ʼ��OLED����OLED����֮���ʼ������ʹСҺ���ظ�������ʾ��
//�������ļ��������һ���˵����Ҷ�Ӧ������ʹ���˶����˵��Ļ�������뵽�����˵��С��ڶ����˵��У�������Ϊ '>' ��ʱ�� �������ļ� ���Ϊ '-'
//���ʱ�����ͨ���ϡ��¼��޸Ķ�Ӧ�Ĳ������ٰ�һ�����ļ� ������±�� '>' ����������ɣ�����ֿ���ͨ���ϡ��¼�ѡ�����
//��������Ҽ��������Լ����壬�ҵĳ��������������ܳ�
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigParameterCore(){
	
        KeyInput = GetKeyInpt();
//////////////////////////////// �����ǵ��ν���Ĳ����߼�����   //////////////////////////////////////////////////////////////////////////////////////////
	if(KeyInput == 'L') 
        {
          OLED_Init(); 
          In_or_Not = 0;
          Beep(10);
        }//�����ʼ��Һ���� ��ֹҺ������

	//�ƶ����
	if(state == 0){ //��һ���˵��У�state = 0��

		if(KeyInput == 'U') {OLED_printf(cursor_x,cursor_y," ");ParameterNo--;Beep(30);}
		else if(KeyInput == 'D') {OLED_printf(cursor_x,cursor_y," ");ParameterNo++;Beep(30);}
	}
	else if(state == 1){ //�ڶ����˵���

		if(KeyInput == 'U') {OLED_printf(cursor_x,cursor_y," "); Parameter2No--;Beep(30);}
		else if(KeyInput == 'D') {OLED_printf(cursor_x,cursor_y," ");Parameter2No++;Beep(30);}
	}

	//�����޸�  ParameterNo �ķ�Χ����������Щһ��������ӵ�ж����˵� �����Ϊ ParameterNo < 15 �����е�һ���˵���ӵ�ж�������
	//��һЩ��־λ��Ҫ����ʹ�ã�����ÿ�ν�������˵���̫�鷳��ʱ�򣬿���������һ�������λ�ã�ֱ����һ�������޸ı�־λ��������������˵���
	if(state == 0 && en_sec_menu[ParameterNo] == 1){ //���ʹ���˶����˵��ͽ�������˵�
		//����״̬��������˵�
		if(KeyInput == 'C'){state = 1;OLED_Fill(0);Beep(30); } //����
	}
	else if(state == 1){ //�����˵���

		//��ʾ���ؼ�
		OLED_printf(X(2),7,"Back");

		if(Parameter2No!= 6){ //û�й��ѡ�з���ʱ���������ļ������޸Ĳ���

                  if(KeyInput == 'C') {state = 2; Beep(30);}//����״̬ ��ʼ�޸Ĳ���
		}
		else if(Parameter2No== 6){ //ѡ���˷���
			if(KeyInput == 'C'){state = 0;Parameter2No = 0;OLED_Fill(0);Beep(30);} //����һ���˵� ����
		}
	}
	else if(state == 2){if(KeyInput == 'C') {state = 1;Beep(30);}} //�޸Ĳ�����ɺ��ٰ�һ�����ļ�ȷ���޸�
//////////////////////////////// ���ν���Ĳ����߼����ý���   //////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// ������ͨ��������ȷ�����   ///////////////////////////////////////////////////////////////////////////////////////////////////
	if(state == 0){ //һ���˵��Ĳ���

		//��������λ
		if(ParameterNo > 14) ParameterNo = 0;
		else if(ParameterNo < 0) ParameterNo = 14;
		//ͨ��������ȷ���������
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
	else if(state == 1){ //�����˵��Ĳ���

		//��������λ
		if(Parameter2No > 6) Parameter2No = 0;
		else if(Parameter2No < 0) Parameter2No = 6;
		//ͨ��������ȷ���������
		if(Parameter2No == 0) {cursor_x = 0;cursor_y = 1;}
		else if(Parameter2No == 1) {cursor_x = 0;cursor_y = 2;}
		else if(Parameter2No == 2) {cursor_x = 0;cursor_y = 3;}
		else if(Parameter2No == 3) {cursor_x = 0;cursor_y = 4;}
		else if(Parameter2No == 4) {cursor_x = 0;cursor_y = 5;}
		else if(Parameter2No == 5) {cursor_x = 0;cursor_y = 6;}
		else if(Parameter2No == 6) {cursor_x = 0;cursor_y = 7;}
	}
///////////////////////////////////// ���λ��ȷ������ ///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////// ��ʾ���  ///////////////////////////////////////////////////////////////////////////////////////////////////
	//��ʾ���
	if(state == 0)
		OLED_printf(cursor_x,cursor_y,">");
	else if(state == 1)
		OLED_printf(cursor_x,cursor_y,">");
	else if(state == 2)
		OLED_P6x8char(cursor_x,cursor_y,'*');
///////////////// ��ʾ������ ///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////  �������ܰ����Ҽ�    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(KeyInput == 'R') {  //�����Ҽ�
        Beep(10);
        PWM_flag =1 ;
        Motor_Stop_Flag = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ͨ�������޸�ĳ��������ֵ
//���������parameter_type ptype ����Ҫ�޸ĵ�������������ͣ�ö�ٶ�����ͷ�ļ���
//                                int8_t��    �ñ���Ϊchar��
//                                uint8_t��   �ñ���Ϊunsigned char��
//                                int16_t��   �ñ���Ϊshort��
//                                uint16_t��  �ñ���Ϊunsigned short��
//                                int32_t��   �ñ���Ϊint��
//                                uint32_t��  �ñ���Ϊunsigned int��
//                                pfloat��    �ñ���Ϊfloat��
//                                pdouble��   �ñ���Ϊdouble��
//
//         void* ppara��Ҫ�޸ĵı���
//
//         double precision ��ÿ�μ�/���ľ���
//
//         uint8_t keyx ������������ֵ
//
//������������
//�÷�ʵ����ChangeParameter(pdouble,&p,0.05,KeyInput); //�޸�double���ͱ���p��ֵ��ÿ���޸�0.05����������Ϊ'U'/'D'��ʱ���޸ģ�
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

//�޸�ĳ��־λ��ֵ��һ������û�ж����˵���λ�ã�
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

//���������Ч����ʾ
void clean_invalid_char(uint8_t endx){

	uint8_t i = 0;
	for(i = getx();i<=endx;i+=6){

		OLED_P6x8char(i,gety(),' ');
	}
}

uint8_t M_Flag = 0;
int16_t Track = 80;
int16_t Class=0;
//���ν���
void OLED_ConfigParameter()
{
	ConfigParameterCore(); //���ν���ĺ��ģ�������ƹ���λ�ã������˵�֮���л�

        if(PWM_flag==0)
        {
             //��ʾһ���˵�����    
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
             //��ʾ�����˵�����
          else if(state == 1 || state == 2){ 

		switch(ParameterNo)
		{
		//PID�Ĳ���
		case 0:
			//ParameterNo = 0�Ķ����˵�����
		       OLED_printf(X(7),0,"M_PID");
			//��ʾ��������
                       OLED_printf(X(2),1,"Bal_AP=%.2f",Angle[0]);
                       OLED_printf(X(2),2,"Bal_GP=%.2f",Bal_Gyro[0]);
                       OLED_printf(X(2),3,"Spe_P=%.2f",Speed[0]);
                       OLED_printf(X(2),4,"Spe_P=%d",Ang_put);
                       
                       clean_invalid_char(OLEDXEND);

			//�޸ĸ�������
                       if(state == 2)
                       {

            	          if(Parameter2No == 0)ChangeParameterVal(pfloat,&Angle[0],1,KeyInput);  //�Ƕ�P
                          else if(Parameter2No == 1)ChangeParameterVal(pfloat,&Bal_Gyro[0],1,KeyInput); //ֱ�����ٶ�P
                          else if(Parameter2No == 2)ChangeParameterVal(pfloat,&Speed[0],1,KeyInput);  //�ٶ�P
                          else if(Parameter2No == 3)ChangeParameterVal(puint8_t,&Ang_put,1,KeyInput);  //�ٶ�P

                          
                        }
                       OLED_printf(X(2),1,"Bal_AP=%.2f",Angle[0]);
                       OLED_printf(X(2),2,"Bal_GP=%.2f",Bal_Gyro[0]);
                       OLED_printf(X(2),3,"Spe_P=%.2f",Speed[0]);
                       OLED_printf(X(2),4,"Spe_P=%d",Ang_put);
                       
                       clean_invalid_char(OLEDXEND);       
                       break;
		
		case 1:
			//ParameterNo = 0�Ķ����˵�����
		       OLED_printf(X(7),0,"Dir_GY_PID");
			//��ʾ��������
                       OLED_printf(X(2),1,"P=%.2f",Dir_Gyro[0]);
                       OLED_printf(X(2),2,"D=%.2f",Dir_Gyro[2]);
                       
                       clean_invalid_char(OLEDXEND);       

			//�޸ĸ�������
                       if(state == 2)
                       {

            	          if(Parameter2No == 0) ChangeParameterVal(pfloat,&Dir_Gyro[0],1,KeyInput);  //�����ڻ�P
            	          else if(Parameter2No == 1) ChangeParameterVal(pfloat,&Dir_Gyro[2],1,KeyInput);  //�����ڻ�D
                          }
                       OLED_printf(X(2),1,"P=%.2f",Dir_Gyro[0]);
                       OLED_printf(X(2),2,"D=%.2f",Dir_Gyro[2]);
                       clean_invalid_char(OLEDXEND);                                  
                       break;
		case 2:
			//ParameterNo = 0�Ķ����˵�����
		       OLED_printf(X(7),0,"Roundabout");
			//��ʾ��������
                       OLED_printf(X(2),1,"LR=%4d",LR);
                       OLED_printf(X(2),2,"BA_IN=%4d",BA_IN);
                       OLED_printf(X(2),3,"BA_OUT=%4d",BA_OUT);
                       OLED_printf(X(2),4,"Time=%4d",time_set);
                       OLED_printf(X(2),5,"In_rate=%2f",In_rate);                   
                       OLED_printf(X(2),6,"OUT_rate=%2f",OUT_rate);
                       clean_invalid_char(OLEDXEND);

			//�޸ĸ�������
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
			//ParameterNo = 0�Ķ����˵�����
		       OLED_printf(X(7),0,"Display");
			//��ʾ��������
                       OLED_printf(X(2),1,"Pitch=%.2f",SystemAttitude.Pitch);
                       OLED_printf(X(2),2,"Roll=%.2f",SystemAttitude.Roll);
                       OLED_printf(X(2),3,"Yaw=%.2f",SystemAttitude.Yaw);                 
                       clean_invalid_char(OLEDXEND);                                 
		       break;   
		}                

	}

//////////////////////  �޸�û�ж����˵���λ�õĲ���   ////////////////////////////////////////////////////////////////////////
	if(state == 0)
        {
		//û�ж����˵���λ��
		if(en_sec_menu[ParameterNo] == 0)
                {
			switch(ParameterNo)
			{
                        case 13:
			}
		}
	}
      }//PWM_flag





    
////////////////////////  û�ж����˵���λ�õĲ��� ����  ////////////////////////////////////////////////////////////////////////
}
     
     
     
     
     
     
     
     
     
     
     