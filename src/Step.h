#ifndef __STEP_H
#define __STEP_H
#include <stdint.h>
#include "stm32f4xx.h"
#include "cmsis_os2.h"

#define Step_Enable_Pin					GPIO_PIN_7
#define Step_Enable_GPIO_Port 	GPIOE
#define Step_Orient_Pin 				GPIO_PIN_8
#define Step_Orient_GPIO_Port 	GPIOE
#define Pul_Pin 								GPIO_PIN_9
#define Pul_GPIO_Port 					GPIOE

#define On 	1
#define Off	0

#define CW	1
#define ACW	0
extern int curAngle;

void Step_Init(void);
void Step_Control(int angle);
void Step_Control_Thread(void *argument);

#endif  /* __STEP_H */
