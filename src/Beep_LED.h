#ifndef __Beep_LED_H
#define __Beep_LED_H

#include "stm32f4xx.h"
#include "cmsis_os2.h"

#define Beep_Pin GPIO_PIN_7
#define Beep_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_6
#define LED_GPIO_Port GPIOC

void Sound_Light_GPIO_Init(void);
void Sound_Light_Control(void);

#endif	/* __Beep_LED_H */

