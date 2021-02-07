#ifndef __OLED_SHOW1_H
#define __OLED_SHOW1_H

#include "stm32f4xx.h"
#include "cmsis_os2.h"



void MENU_Working(void);
void MENU_Stop(void);
void ParaMenu_Select(void);
void ParaMenu_Show(void);
void Row_select(int i,int clear);
void MENU_Shift(int NowKeyNum);
void MENU_Add(int NowKeyNum);
void MENU_Cancel(void);
void MENU_SaveParameter(void);

void TakePhoto(void);
void Excute(void);

void TouchKey_Handle(void *argument);
#endif	/* __TTP229_H */

