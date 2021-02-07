#include "cmsis_os2.h"
#include "stm32f4xx.h"
#include "GPIO_EXTI.h"
#include "TTP229.h"

typedef struct _GPIO_PIN {
  GPIO_TypeDef *port;
  uint16_t      pin;
} GPIO_PIN;
const GPIO_PIN Pin_Output[] = {
	{ GPIOA, GPIO_PIN_9 },					
};
const GPIO_PIN Pin_Input[] = {
	{ GPIOC, GPIO_PIN_8 },			// 用于检测编码器信号
	{ GPIOA, GPIO_PIN_15 },			// 用于帧拍照开启信号	
};

#define PinOutput_COUNT (sizeof(Pin_Output)/sizeof(GPIO_PIN))
#define PinInput_COUNT  (sizeof(Pin_Input) /sizeof(GPIO_PIN))
void GPIO_EXTI_Init(void)
{
	int n;
    GPIO_InitTypeDef GPIO_Initure;
	GPIO_TypeDef *port;
	uint16_t      pin;
   
    __HAL_RCC_GPIOA_CLK_ENABLE();               
    __HAL_RCC_GPIOC_CLK_ENABLE();               
    __HAL_RCC_GPIOE_CLK_ENABLE();              

	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull  = GPIO_PULLDOWN;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	for (n = 0; n < PinOutput_COUNT; n++) 
	{
		port = Pin_Output[n].port;
		pin	 = Pin_Output[n].pin;
		GPIO_Initure.Pin = pin ;
		HAL_GPIO_Init(port, &GPIO_Initure);	
		
		HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	}

    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //上升沿触发
	GPIO_Initure.Pull  = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	for (n = 0; n < PinInput_COUNT; n++) 
	{
		port = Pin_Input[n].port;
		pin	 = Pin_Input[n].pin;
		GPIO_Initure.Pin = pin ;
		HAL_GPIO_Init(port, &GPIO_Initure);	
	}

    //中断线0-PA0
    HAL_NVIC_SetPriority(EXTI0_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);             //使能中断线0
    
    //中断线2-PE2
    HAL_NVIC_SetPriority(EXTI2_IRQn,2,1);       //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);             //使能中断线2
    
    //中断线3-PE3
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,2);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);    
    
    //中断线4-PE4
    HAL_NVIC_SetPriority(EXTI4_IRQn,2,3); 
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);  

    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);         	
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         	
}

//中断服务函数
void EXTI0_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);	//清除中断标志

// 添加用户代码
}

void EXTI2_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET)
    {		
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
		

    }
	else
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != RESET)
    {	//PA6接TTP229的信号输出端，有中断表示有按键动作	
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
		
		void TTP229_EXTI_Callback(void);
		TTP229_EXTI_Callback();//当有中断信号时调用TTP229的回掉函数 

    }
}

void EXTI15_10_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET)
    {		
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
		

    }
}


/******************************************************************************/

