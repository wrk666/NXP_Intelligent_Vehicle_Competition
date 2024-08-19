#include "headfile.h"
#include "common.h"

float Data_Observe[2] = {0};

//λ��ʽPID����
float PID_Realize(PID *sprt, float *PID, float NowData, float Point)
{
	float 	Realize;	//���ó���ʵ������

	sprt->iError = Point - NowData;	// ���㵱ǰ���

        sprt->Error_Rate = sprt->iError - sprt->LastError;
        
	Realize = PID[KP] * sprt->iError + PID[KD] * sprt->Error_Rate;

	sprt->LastError = sprt->iError;		  	// �����ϴ����
        
	sprt->LastData  = NowData;			// �����ϴ�����

	return Realize;	// ����ʵ��ֵ
}


// ����ʽPID�������
float PID_Increase(PID *sprt, float *PID, float NowData, float Point)
{
	float 	Increase;	//���ó���ʵ������

	sprt->iError = Point - NowData;	// ���㵱ǰ���
        
        sprt->Error_Rate = sprt->iError - sprt->LastError ;

	Increase =  PID[KP] * (sprt->iError - sprt->LastError)+ PID[KI] * sprt->iError
                                + PID[KD] * (sprt->iError - sprt->PrevError);   //???
//			  + PID[KD] * (sprt->iError - 2 * sprt->LastError + sprt->PrevError);
	
	sprt->PrevError = sprt->LastError;	// ����ǰ�����
        
	sprt->LastError = sprt->iError;		  	// �����ϴ����
	
	return Increase;	// ��������
}


// λ��ʽ��̬PID����
float PlacePID_Control(PID *sprt, float *PID, float NowPiont, float SetPoint)
{
	float 	Actual;	//���ó���ʵ�����ֵ
	float   Kp;		//��̬P
        
	sprt->iError = SetPoint - NowPiont;	//���㵱ǰ���
        
        sprt->Error_Rate = sprt->iError - sprt->LastError ;
        
	Kp = 1.0 * ( sprt->iError * sprt->iError ) / PID[KP] + PID[KI];
        
	Actual = Kp * sprt->iError + PID[KD] * sprt->Error_Rate;//ֻ��PD
        
	sprt->LastError = sprt->iError;		//�����ϴ����

	return Actual;
}




/************************������˹��ͨ�˲���******************/

Butter_BufferData Gyro_BufferData;    //�����������
Butter_Parameter  Gyro_Parameter;     //������˹��ͨ��õ���΢����

Butter_Parameter Control_Device_Div_LPF_Parameter=
{
 //200---20hz
  1,    -1.14298050254,   0.4128015980962,
  0.06745527388907,   0.1349105477781,  0.06745527388907
};

Butter_Parameter Control_Device_Err_LPF_Parameter=
{
  //200hz---2hz
  1,   -1.911197067426,   0.9149758348014,
  0.0009446918438402,  0.00188938368768,0.0009446918438402
};

float Control_Device_LPF(float curr_inputer,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
{           //@param 1�����ٶ�ƫ��΢��   2��//��������ͨ�����������  3��//������˹��ͨ��õ���΢����,20hz
        /* ���ٶȼ�Butterworth�˲� */
	/* ��ȡ����x(n) */
        Buffer->Input_Butter[2]=curr_inputer;  //LPF�������
	/* Butterworth�˲� */
        Buffer->Output_Butter[2]=
         Parameter->b[0] * Buffer->Input_Butter[2]
        +Parameter->b[1] * Buffer->Input_Butter[1]
	+Parameter->b[2] * Buffer->Input_Butter[0]
        -Parameter->a[1] * Buffer->Output_Butter[1]
        -Parameter->a[2] * Buffer->Output_Butter[0];
	/* x(n) ���б��� */
        Buffer->Input_Butter[0]=Buffer->Input_Butter[1];  //���뱣��
        Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
	/* y(n) ���б��� */
        Buffer->Output_Butter[0]=Buffer->Output_Butter[1];  //�������
        Buffer->Output_Butter[1]=Buffer->Output_Butter[2];
        return (Buffer->Output_Butter[2]);  //�����������
}

float PID_Butterworth(PID *sprt, float *PID, float NowData, float Point)
{
	float 	Realize;	//���ó���ʵ������

	sprt->iError = Point - NowData;	// ���㵱ǰ���

        sprt->Error_Rate = sprt->iError - sprt->LastError;
        
        sprt->fP = PID[KP] * sprt->iError;
        
          //Butterworth΢��������ڵ�һ��
        /*sprt->fD = PID[KD] * Control_Device_LPF(sprt->Error_Rate,  //���ٶ�ƫ��΢��
                                  &Gyro_BufferData,    //��������ͨ�����������
                                  &Gyro_Parameter);//������˹��ͨ��õ���΢����,20hz*/
        
        sprt->fD = PID[KD] * sprt->Error_Rate;
          
	Realize =  sprt->fP + sprt->fD;

	sprt->LastError = sprt->iError;		  	// �����ϴ����
        
	sprt->LastData  = NowData;			// �����ϴ�����

	return Realize;	// ����ʵ��ֵ
}

