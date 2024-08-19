#include "headfile.h"
#include "common.h"

float Data_Observe[2] = {0};

//位置式PID控制
float PID_Realize(PID *sprt, float *PID, float NowData, float Point)
{
	float 	Realize;	//最后得出的实际增量

	sprt->iError = Point - NowData;	// 计算当前误差

        sprt->Error_Rate = sprt->iError - sprt->LastError;
        
	Realize = PID[KP] * sprt->iError + PID[KD] * sprt->Error_Rate;

	sprt->LastError = sprt->iError;		  	// 更新上次误差
        
	sprt->LastData  = NowData;			// 更新上次数据

	return Realize;	// 返回实际值
}


// 增量式PID电机控制
float PID_Increase(PID *sprt, float *PID, float NowData, float Point)
{
	float 	Increase;	//最后得出的实际增量

	sprt->iError = Point - NowData;	// 计算当前误差
        
        sprt->Error_Rate = sprt->iError - sprt->LastError ;

	Increase =  PID[KP] * (sprt->iError - sprt->LastError)+ PID[KI] * sprt->iError
                                + PID[KD] * (sprt->iError - sprt->PrevError);   //???
//			  + PID[KD] * (sprt->iError - 2 * sprt->LastError + sprt->PrevError);
	
	sprt->PrevError = sprt->LastError;	// 更新前次误差
        
	sprt->LastError = sprt->iError;		  	// 更新上次误差
	
	return Increase;	// 返回增量
}


// 位置式动态PID控制
float PlacePID_Control(PID *sprt, float *PID, float NowPiont, float SetPoint)
{
	float 	Actual;	//最后得出的实际输出值
	float   Kp;		//动态P
        
	sprt->iError = SetPoint - NowPiont;	//计算当前误差
        
        sprt->Error_Rate = sprt->iError - sprt->LastError ;
        
	Kp = 1.0 * ( sprt->iError * sprt->iError ) / PID[KP] + PID[KI];
        
	Actual = Kp * sprt->iError + PID[KD] * sprt->Error_Rate;//只用PD
        
	sprt->LastError = sprt->iError;		//更新上次误差

	return Actual;
}




/************************巴特沃斯低通滤波器******************/

Butter_BufferData Gyro_BufferData;    //输入输出缓冲
Butter_Parameter  Gyro_Parameter;     //巴特沃斯低通后得到的微分项

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
{           //@param 1：角速度偏差微分   2：//控制器低通输入输出缓冲  3：//巴特沃斯低通后得到的微分项,20hz
        /* 加速度计Butterworth滤波 */
	/* 获取最新x(n) */
        Buffer->Input_Butter[2]=curr_inputer;  //LPF输入更新
	/* Butterworth滤波 */
        Buffer->Output_Butter[2]=
         Parameter->b[0] * Buffer->Input_Butter[2]
        +Parameter->b[1] * Buffer->Input_Butter[1]
	+Parameter->b[2] * Buffer->Input_Butter[0]
        -Parameter->a[1] * Buffer->Output_Butter[1]
        -Parameter->a[2] * Buffer->Output_Butter[0];
	/* x(n) 序列保存 */
        Buffer->Input_Butter[0]=Buffer->Input_Butter[1];  //输入保存
        Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
	/* y(n) 序列保存 */
        Buffer->Output_Butter[0]=Buffer->Output_Butter[1];  //输出保存
        Buffer->Output_Butter[1]=Buffer->Output_Butter[2];
        return (Buffer->Output_Butter[2]);  //返回最新输出
}

float PID_Butterworth(PID *sprt, float *PID, float NowData, float Point)
{
	float 	Realize;	//最后得出的实际增量

	sprt->iError = Point - NowData;	// 计算当前误差

        sprt->Error_Rate = sprt->iError - sprt->LastError;
        
        sprt->fP = PID[KP] * sprt->iError;
        
          //Butterworth微分项更新于第一项
        /*sprt->fD = PID[KD] * Control_Device_LPF(sprt->Error_Rate,  //角速度偏差微分
                                  &Gyro_BufferData,    //控制器低通输入输出缓冲
                                  &Gyro_Parameter);//巴特沃斯低通后得到的微分项,20hz*/
        
        sprt->fD = PID[KD] * sprt->Error_Rate;
          
	Realize =  sprt->fP + sprt->fD;

	sprt->LastError = sprt->iError;		  	// 更新上次误差
        
	sprt->LastData  = NowData;			// 更新上次数据

	return Realize;	// 返回实际值
}

