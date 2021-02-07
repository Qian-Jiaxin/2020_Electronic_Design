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
	{ GPIOC, GPIO_PIN_8 },			// ���ڼ��������ź�
	{ GPIOA, GPIO_PIN_15 },			// ����֡���տ����ź�	
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

    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //�����ش���
	GPIO_Initure.Pull  = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	for (n = 0; n < PinInput_COUNT; n++) 
	{
		port = Pin_Input[n].port;
		pin	 = Pin_Input[n].pin;
		GPIO_Initure.Pin = pin ;
		HAL_GPIO_Init(port, &GPIO_Initure);	
	}

    //�ж���0-PA0
    HAL_NVIC_SetPriority(EXTI0_IRQn,2,0);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);             //ʹ���ж���0
    
    //�ж���2-PE2
    HAL_NVIC_SetPriority(EXTI2_IRQn,2,1);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ1
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);             //ʹ���ж���2
    
    //�ж���3-PE3
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,2);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);    
    
    //�ж���4-PE4
    HAL_NVIC_SetPriority(EXTI4_IRQn,2,3); 
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);  

    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);         	
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         	
}

//�жϷ�����
void EXTI0_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);	//����жϱ�־

// ����û�����
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
    {	//PA6��TTP229���ź�����ˣ����жϱ�ʾ�а�������	
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
		
		void TTP229_EXTI_Callback(void);
		TTP229_EXTI_Callback();//�����ж��ź�ʱ����TTP229�Ļص����� 

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

