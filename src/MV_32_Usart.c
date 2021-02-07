#include "Usart.h"
#include "steer.h"
#include <math.h>
#include "string.h"
#include "Step.h"
#include "OLED_SHOW.h"
#include "OLED_SPI.h"
#include "Beep_LED.h"

extern uint32_t Laser_Detect(void);
extern void Light_Control(uint8_t Status);
extern osThreadId_t Semaphore_Step_Control;
extern osThreadId_t Semaphore_Step_Control_Done;
extern uint32_t Laser_Detect_Once(void);


#define Pi 	3.141592653
uint16_t Communication_Busy;
uint16_t TISubject=0;
osSemaphoreId_t sem_Communication_Work;

double Pre_X_Angle=0,X_Angle=0;

//换算实际距离
static int IF_Length(uint8_t sort,uint32_t Distance,uint16_t Sum_Pix)
{
  double K = 4.493562352+(-0.000901990)*Distance;
  switch(sort)
  {
    case 0x01://圆
    {
      double R = sqrt(1000*Sum_Pix/Pi);
      double D = (R/K)*2;
      return (int)D;
    }
    case 0x02://三角形
    {
      double a = sqrt(1000*Sum_Pix*4/1.7320508);
      a= a/K;
      return (int)(a);
    }
    case 0x03://正方形
    {
      double a = sqrt(1000*Sum_Pix);
      double b = a/K;
      return (int)b;
//      return (int)(a/K);
    }
  }
}

int avDis[61],isValid[61];
int judge(void)
{
  //每间隔10度测量距离，然后判断是否到达背景板或目标
  //return 0 -->无法判断 1-->发现背景板或目标
  int i,flag;
  
  flag = isValid[0];
  if(flag) return 1;//目标在-30度附近 
  for(i=10;i<=curAngle/100+30;i+=10)
  {
    if(flag!=isValid[i])
    {
      return 1;
    }
  }
  return 0;
}

void oneDetectDis(void)
{
  int i,index,tempAngle;
  float temp;
//  tempAngle = curAngle;
  index = curAngle/100+30;
//  temp=Laser_Detect_Once();
//  temp = temp * cos(tempAngle) * 180 / Pi;
//  avDis[index]= (int)temp;
  avDis[index]=Laser_Detect_Once();
  if(avDis[index]<=3*1000)
    isValid[index]=1;
}
int scan(int dir)
{
  int startAngle,i,j,endAngle;
  int Dvalue = 0,DvalueFlag=0,Avalue;
  startAngle = curAngle;
  if(dir>0)
    endAngle =30*100;
  j = curAngle/100+30;
  for(i=startAngle+100;i<=endAngle;i+=100)
  {
    Step_Control(i);
    osDelay(1);
    oneDetectDis();
    
    if(isValid[j+1]==0)
    {//发现有背景板但无目标
      osMutexAcquire(mutex_OLED,100);
      OLED_Clear();
      OLED_ShowString(0,0,"no target",12,BackOff);
      OLED_Refresh_Gram();
      osMutexRelease(mutex_OLED);
      Sound_Light_Control();
      return -1;
    }

    Dvalue = avDis[j+1]-avDis[j];j++;
    if(Dvalue<0) 
      Avalue = -Dvalue;
    else
      Avalue = Dvalue;
    
    if(Avalue < 20) {
      //水平
      continue;
    }else if(Avalue<55+40)
    {
      //找到平面目标
      osMutexAcquire(mutex_OLED,100);
      OLED_Clear();
      OLED_ShowString(0,0,"found palne object",12,BackOff);
      OLED_Refresh_Gram();
      osMutexRelease(mutex_OLED);
      Sound_Light_Control();
      return 1;
    }
    else
    {
      //找到球
      osMutexAcquire(mutex_OLED,100);
      OLED_Clear();
      OLED_ShowString(0,0,"found ball",12,BackOff);
      OLED_Refresh_Gram();
      osMutexRelease(mutex_OLED);
      Sound_Light_Control();
      return 2;
    }
  }
}
uint8_t oled[32];
void Plane_Usart(void)
{
  uint8_t MyUSART_RxBuffer[32];
	uint8_t Send_TakePhotos[4] = {0x5A,0x5A,0xA3,0xC0};//发送1
	uint8_t Send_1[4] = {0x5A,0x5A,0xA1,0xC0};
	uint8_t rxCount;
	uint16_t temp;
	uint32_t Distance;
	int lenght;
	double X_dis;
	int angle;
	int t;
	
	memset(MyUSART_RxBuffer,0,sizeof(MyUSART_RxBuffer));
	rxCount = 0;
	
	MyUSART_Send(Send_TakePhotos,sizeof(Send_TakePhotos));
	rxCount = MyUSART_Receive(MyUSART_RxBuffer);
//      MyUSART_Send(MyUSART_RxBuffer,rxCount);
	if(MyUSART_RxBuffer[0] == 0X5A && MyUSART_RxBuffer[1] == 0X5A && MyUSART_RxBuffer[rxCount-1] == 0XC0)
	{
		X_dis = (MyUSART_RxBuffer[3]<<8)|MyUSART_RxBuffer[4];
		if(MyUSART_RxBuffer[2] == 0x00)
		{
			Distance = Laser_Detect();
			double  K = 4.493562352+(-0.000901990)*Distance;
			X_dis = X_dis/K;
			angle = 180*(atan(X_dis/Distance))/Pi*100;
		}
		else if(MyUSART_RxBuffer[2] == 0x01)
		{
			Distance = Laser_Detect();
			double K = 4.493562352+(-0.000901990)*Distance;
			X_dis = (X_dis)/K;
			angle = 180*(atan(X_dis/Distance))/Pi*100;
			angle = -angle;
		}
	}
	
	memset(MyUSART_RxBuffer,0,sizeof(MyUSART_RxBuffer));
	rxCount = 0;
	
	Step_Control(curAngle+angle);
	
	MyUSART_Send(Send_1,sizeof(Send_1));
	rxCount = MyUSART_Receive(MyUSART_RxBuffer);
	if(MyUSART_RxBuffer[0] == 0X5A && MyUSART_RxBuffer[1] == 0X5A && MyUSART_RxBuffer[rxCount-1] == 0XC0)
	{
		Distance = Laser_Detect();
		temp = (uint16_t)(MyUSART_RxBuffer[3]<<8) + MyUSART_RxBuffer[4];
		lenght = IF_Length(MyUSART_RxBuffer[2],Distance,temp);
	}
	//增加oled显示程序
}

void Ball_Usart(void)
{
  uint8_t MyUSART_RxBuffer[32];
	uint8_t Send_TakePhotos[4] = {0x5A,0x5A,0xA4,0xC0};//发送1
	uint8_t rxCount;
	uint16_t temp;
	uint32_t Distance;
	int lenght;
	double X_dis;
	int angle;
	
	memset(MyUSART_RxBuffer,0,sizeof(MyUSART_RxBuffer));
	rxCount = 0;
	
	MyUSART_Send(Send_TakePhotos,sizeof(Send_TakePhotos));
	rxCount = MyUSART_Receive(MyUSART_RxBuffer);
//      MyUSART_Send(MyUSART_RxBuffer,rxCount);
	if(MyUSART_RxBuffer[0] == 0X5A && MyUSART_RxBuffer[1] == 0X5A && MyUSART_RxBuffer[rxCount-1] == 0XC0)
	{
		X_dis = (MyUSART_RxBuffer[3]<<8)|MyUSART_RxBuffer[4];
		if(MyUSART_RxBuffer[2] == 0x00)
		{
			Distance = Laser_Detect();
			double K = 4.493562352+(-0.000901990)*Distance;
			X_dis = X_dis*K;
			angle = 180*(atan(X_dis/Distance))/Pi*100;
		}
		else if(MyUSART_RxBuffer[2] == 0x01)
		{
			Distance = Laser_Detect();
			double K = 4.493562352+(-0.000901990)*Distance;
			X_dis = X_dis*K;
			angle = 180*(atan(X_dis/Distance))/Pi*100;
			angle = -angle;
		}
	}
	
	memset(MyUSART_RxBuffer,0,sizeof(MyUSART_RxBuffer));
	rxCount = 0;
	
	Step_Control(curAngle+angle);
	
//	MyUSART_Send(Send_TakePhotos,sizeof(Send_TakePhotos));
//	rxCount = MyUSART_Receive(MyUSART_RxBuffer);
//	if(MyUSART_RxBuffer[0] == 0X5A && MyUSART_RxBuffer[1] == 0X5A && MyUSART_RxBuffer[rxCount-1] == 0XC0)
//	{
//		Distance = Laser_Detect();
//		temp = (uint16_t)(MyUSART_RxBuffer[3]<<8) + MyUSART_RxBuffer[4];
//		lenght = IF_Length(MyUSART_RxBuffer[2],Distance,temp);
//	}
	//增加oled显示程序
}

static void TISubject3(void)
{
  int i,angle,t;//角度值*100
	int rxCount=0;
  uint32_t Distance;
  uint16_t rec_final[5];

  int value;
  
//  TheFirst();
  
  memset(isValid,0,sizeof(isValid));
//  Step_Control(-30*100);//调到-30度
//  osDelay(50);
  
  for(i=-30;i<=30;i+=5)
  {
    angle = i*100;
    Step_Control(angle);//调好5度间隔角度
    osDelay(1);
    oneDetectDis();
    
    t =judge();
    if(t) break;
  }
  if(t==0) 
  {
    //无目标
    osMutexAcquire(mutex_OLED,100);
    OLED_Clear();
    OLED_ShowString(0,0,"no background plate",12,BackOff);
    OLED_Refresh_Gram();
    osMutexRelease(mutex_OLED);
    Sound_Light_Control();
  }
  else
  {
    value=scan(1);
		switch(value)
		{
			case 1://平面图形识别
			{
				Plane_Usart();
				break;
			}
			case 2://球识别
			{
				Ball_Usart();
				break;
			}
		}
  }
  
  //判断目标
  //前项差分
}

void Communication_Tasks (void *argument) 
{
	uint8_t MyUSART_RxBuffer[32];
	int rxCount=0;
	uint32_t Distance;
  int lenght;
	
////rec_1[0] 0为正，1为负；rec_1[1] 为X偏移高8位；rec_1[2] 为X偏移低8位；
//	uint16_t rec_1[3];
	
//rec_final[0] rec_final[1]具体形状类型（01：圆；02：三角；03：正方形；04：排球；05：足球；06：篮球）；
//rec_final[2] rec_final[3] 面积（pix*pix） High Low；
	
	
	for(int i=0;i<32;i++)
	{
		MyUSART_RxBuffer[i]=0;
	}
	sem_Communication_Work = osSemaphoreNew(1, 0, NULL);
	
	Communication_Busy = 0;
	while(1)
	{
		osSemaphoreAcquire (sem_Communication_Work, osWaitForever);
		Communication_Busy = 1;
    Light_Control(1);
    
    if(TISubject==1)
    {
      uint8_t Send_1[4] = {0x5A,0x5A,0xA2,0xC0};
      uint16_t temp;
      MyUSART_Send(Send_1,sizeof(Send_1));
      rxCount = MyUSART_Receive(MyUSART_RxBuffer);
//      MyUSART_Send(MyUSART_RxBuffer,rxCount);
      if(MyUSART_RxBuffer[0] == 0X5A && MyUSART_RxBuffer[1] == 0X5A && MyUSART_RxBuffer[rxCount-1] == 0XC0)
      {
        Distance = Laser_Detect();
        temp = (uint16_t)(MyUSART_RxBuffer[3]<<8) + MyUSART_RxBuffer[4];
        extern void Show_Result(uint32_t Distance ,uint8_t Type,int len);
        lenght = IF_Length(MyUSART_RxBuffer[2],Distance,temp);
        Show_Result(Distance,MyUSART_RxBuffer[2],lenght);
      }
      Communication_Busy = 0;
    }
    else if(TISubject == 2)
    {
      TISubject3();
      Communication_Busy = 0;
    }
    Light_Control(0x01);
		osDelay(10);
	}
}


