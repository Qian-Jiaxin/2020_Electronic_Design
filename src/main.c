//#include <stdio.h>
#include "stm32f4xx.h"
#include "cmsis_os2.h"
#include "LED.h"
#include "Timer.h"
#include "GPIO_EXTI.h"
#include "TTP229.h"
#include "Usart.h"
#include "Beep_LED.h"
#include "OLED_SPI.h"
#include "OLED_SHOW.h"
#include "Step.h"



void app_main (void *argument);
void Communication_Tasks (void *argument);
uint32_t Laser_Detect(void);
void Light_Control(uint8_t Status);


uint32_t HAL_GetTick(void)
{
  return osKernelGetSysTimerCount();
}

void usSleep(uint32_t us)
{
//ע�⣺΢�뼶����ʱ��������SysTimer32λ��������0�����ʱ���ڵ���ʱ
//RTX5��ϵͳtick��uint64_t��ʱ��������Ϊ1msʱ��tick������0���������ʱ5.8����
//����osKernelGetSysTimerCount�ڼ���ʱǿ��ת��Ϊ(uint32_t)�����Դ���32λ����ֵ��������
	unsigned int SysTimerCount, SysTimerCount0, us_Count, temp;
	#define MaxSysTimerCount 0xFFFFFFFF
	us_Count = osKernelGetSysTimerFreq()/1000000 * us;//΢�����ֵ
	SysTimerCount0 = osKernelGetSysTimerCount();
	while(1){
		SysTimerCount  = osKernelGetSysTimerCount();//ȡ����ǰʱ�Ӽ���ֵ
		temp = (SysTimerCount>SysTimerCount0) ? 
				(SysTimerCount - SysTimerCount0) : 
				(MaxSysTimerCount-SysTimerCount0 + SysTimerCount);//������
		if( temp>=us_Count) break;
	}
}

void app_main (void *argument) 
{
	TIM2_Init(1);
//	Sound_Light_GPIO_Init();
//	OLED_Init();
//  Step_Init();
  MyUSART_Initialization();
  Laser_USART_Initialization();
	
//	Light_Control(0x01);
//  osDelay(500);
//  Light_Control(0x00);
//  osDelay(500);
//  Light_Control(0x01);
  
  osThreadNew(MyUSART_Test, NULL, NULL);
	
//	osThreadNew(TouchKey_Read, NULL, NULL);
//	osThreadNew(TouchKey_Handle, NULL, NULL);
//	osThreadNew(Communication_Tasks, NULL, NULL); 
}
void SystemClock_Config(void) 
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the
	 device is clocked below the maximum system frequency (see datasheet). */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;//25;//4;
	RCC_OscInitStruct.PLL.PLLN = 168;//l336;//168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
								RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

int main (void) 
{
//OLED��ʾ��RS--PC5 BLK--PB1
//PA0--KEY PA1--LED
// ʹ���ⲿ����ʱ��ע����HSE_VALUE��ֵ�Ƿ��뾧����ͬ������ʵʱ����ϵͳ��ticks��׼ȷ
	SystemClock_Config();                     /* Configure the System Clock     */
	SystemCoreClockUpdate();
	HAL_Init();                               /* Initialize the HAL Library     */

	osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	osThreadNew(app_main, NULL, NULL);    // Create application main thread

	osKernelStart();                      // Start thread execution
}
