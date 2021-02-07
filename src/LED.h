#ifndef __LED_H
#define __LED_H
#include <stdint.h>
#include "stm32f4xx.h"

#define LED_A   0
#define LED_B   1
#define LED_C   2
#define LED_D 	3

void LED_Initialize (void);
void LED_On (int num); 
void LED_Off (int num); 
void LED_Toggle (int num);
int LED_GetCount (void); 

#define Key0Press (HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)==0)
#define Key1Press (HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)==0)
#define Key2Press (HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)==0)

#endif  /* __LED_H */
