#include "OLED_SHOW.h"
#include "OLED_SPI.h"
#include "TTP229.H"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "Beep_LED.h"
#include "Step.h"

extern void Light_Control(uint8_t Status);

extern uint16_t TISubject;

extern uint16_t Communication_Busy;
extern osSemaphoreId_t sem_Communication_Work;

uint8_t oledbuf[32];

#define k 0.0011

void MENU_Working(void)
{
  osMutexAcquire(mutex_OLED,100);
  OLED_ShowString(0,0,"Welcome",12,BackOff);
  OLED_ShowString(0,12,"Key4 Start 1 or 2",12,BackOff);
  OLED_ShowString(0,24,"Key8 Start 3 or 4",12,BackOff);
  OLED_Refresh_Gram();
  osMutexRelease(mutex_OLED);
}

void Excute(void)
{
  osMutexAcquire(mutex_OLED,100);
  OLED_Clear();
  OLED_ShowString(0,0,"Device is working...",12,BackOff);
  OLED_Refresh_Gram();
  osSemaphoreRelease(sem_Communication_Work);
  osMutexRelease(mutex_OLED);
  osDelay(100);
}

void Show_Result(uint32_t Distance ,uint8_t Type,int len)
{
  osDelay(1000);
  osMutexAcquire(mutex_OLED,100);
  OLED_Clear();
  switch(Type)
  {
    case 0x01: OLED_ShowString(0,0,"Type : Circle",12,BackOff);break;
    case 0x02: OLED_ShowString(0,0,"Type : Triangle",12,BackOff);break;
    case 0x03: OLED_ShowString(0,0,"Type : Square",12,BackOff);break;
  }
  switch(Type)
  {
    case 0x01: 
      sprintf(oledbuf,"Radius : %4d mm",len);
      OLED_ShowString(0,12,oledbuf,12,BackOff);break;
    case 0x02: 
    case 0x03: 
      sprintf(oledbuf,"Length : %4d mm",len);
      OLED_ShowString(0,12,oledbuf,12,BackOff);break;
  }
  sprintf(oledbuf,"Distance : %5d mm",Distance);
  OLED_ShowString(0,24,oledbuf,12,BackOff);
  OLED_Refresh_Gram();
  osMutexRelease(mutex_OLED);
  Sound_Light_Control();
  Light_Control(1);
}

extern double angle;
void TouchKey_Handle(void *argument)
{
  osDelay(100);
	MENU_Working();
	while(1){
		if(isHandUP==0 && Communication_Busy == 0) 
		{
			switch(NowKeyNum){
        case 4://3/4Ã‚
          TISubject=2;
          Excute();
					break;
        case 1: //1°¢2Ã‚
          TISubject=1;
          Excute();
          break;
        case 16: //πÈ÷–
          Step_Control(0);
          osDelay(500);
          break;
        default:
          break;
			}   
		}
		osDelay(100);
	}
}



