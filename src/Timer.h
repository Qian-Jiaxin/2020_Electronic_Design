#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>

void TIM2_Init(int set_us_div2);
void TIM5_CH2_PWM_Init(int arr);
//void TIM5_CH2_SetDuty(int duty);
void TIM8_CH1_PWM_Init(int arr);	
void TIM8_CH1_SetDuty(int duty);

//void TIM5_2_STEERS_PWM_Init(void);
//void TIM5_2_STEERS_SetDuty(int angle_1,int angle_2);

#endif /* __TIMER_H */
