#ifndef __STEER_H
#define __STEER_H

#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include "Timer.h"

void TIM5_2_STEERS_PWM_Init(void);
//void TIM5_2_STEERS_SetDuty(int angle_1,int angle_2);
void Steer_Work(int Adjust_Angle);

#endif	/* __STEER_H */


