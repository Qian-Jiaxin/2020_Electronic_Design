#include "Beep_LED.h"

void Sound_Light_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
	
  GPIO_InitStruct.Pin = Beep_Pin|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

void Sound_Light_Control(void)
{
	HAL_GPIO_WritePin(GPIOC,Beep_Pin|LED_Pin,GPIO_PIN_SET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOC,Beep_Pin|LED_Pin,GPIO_PIN_RESET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOC,Beep_Pin|LED_Pin,GPIO_PIN_SET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOC,Beep_Pin|LED_Pin,GPIO_PIN_RESET);
}
