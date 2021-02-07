#ifndef __TTP229_H
#define __TTP229_H

#include "stm32f4xx.h"
#include "cmsis_os2.h"
extern osSemaphoreId_t sid_TTP229_Semaphore;
extern uint8_t TTP229_busy;

extern uint16_t NowKeyNum;	//�����ļ���ֵ��0#~15#
extern uint8_t  isHandUP;	//0��ʾ�����ڰ��£�1��ʾ���Ѿ�̧��

void TTP229_Initialize (void);
void TouchKey_Read (void *argument);

#endif	/* __TTP229_H */


