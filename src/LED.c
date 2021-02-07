#include "LED.h"

/* GPIO Pin identifier */
typedef struct _GPIO_PIN {
  GPIO_TypeDef *port;
  uint16_t      pin;
} GPIO_PIN;

/* LED GPIO definitions */
const GPIO_PIN Pin_LED[] = {
	{ GPIOA, GPIO_PIN_1 },		//LED1
//Ì½Ë÷Õß	
	{ GPIOE, GPIO_PIN_13 },		//LED1
	{ GPIOE, GPIO_PIN_14 },		//LED2
	{ GPIOE, GPIO_PIN_15 },		//LED3
};

#define LED_COUNT (sizeof(Pin_LED)/sizeof(GPIO_PIN))


void LED_On (int num) 
{
	HAL_GPIO_WritePin(Pin_LED[num].port, Pin_LED[num].pin, GPIO_PIN_RESET);
}

void LED_Off (int num) 
{
	HAL_GPIO_WritePin(Pin_LED[num].port, Pin_LED[num].pin, GPIO_PIN_SET);
}
void LED_Toggle (int num) 
{
	HAL_GPIO_TogglePin(Pin_LED[num].port, Pin_LED[num].pin);
//	if(GPIO_PinRead(Pin_LED[num].port, Pin_LED[num].num))
//		LEDOn(num);
//	else
//		LEDOff(num);
}

void LED_SetOut (int val) {
	int n;

	for (n = 0; n < LED_COUNT; n++) {
		if (val & (1 << n)) 
			LED_On(n);
		else                
			LED_Off(n);
	}
}

int LED_GetCount (void) 
{
	return LED_COUNT;
}
void LED_Uninitialize (void) 
{
	int n;

	for (n = 0; n < LED_COUNT; n++) 
		HAL_GPIO_DeInit(Pin_LED[n].port, Pin_LED[n].pin);
}

void LED_Initialize (void) 
{
	int n;
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__GPIOA_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	for (n = 0; n < LED_COUNT; n++) 
	{
		GPIO_InitStruct.Pin   = Pin_LED[n].pin ;
		HAL_GPIO_Init(Pin_LED[n].port, &GPIO_InitStruct);	
		LED_Off(n);
	}
}

