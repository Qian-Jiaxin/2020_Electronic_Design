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
//注意：微秒级的延时仅仅用于SysTimer32位计数器从0到溢出时间内的延时
//RTX5的系统tick是uint64_t，时间间隔设置为1ms时，tick计数从0到溢出将耗时5.8亿年
//但是osKernelGetSysTimerCount在计算时强制转换为(uint32_t)，所以存在32位计数值溢出情况。
	unsigned int SysTimerCount, SysTimerCount0, us_Count, temp;
	#define MaxSysTimerCount 0xFFFFFFFF
	us_Count = osKernelGetSysTimerFreq()/1000000 * us;//微秒计数值
	SysTimerCount0 = osKernelGetSysTimerCount();
	while(1){
		SysTimerCount  = osKernelGetSysTimerCount();//取出当前时钟计数值
		temp = (SysTimerCount>SysTimerCount0) ? 
				(SysTimerCount - SysTimerCount0) : 
				(MaxSysTimerCount-SysTimerCount0 + SysTimerCount);//溢出情况
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
//OLED显示屏RS--PC5 BLK--PB1
//PA0--KEY PA1--LED
// 使用外部晶振时，注意检查HSE_VALUE的值是否与晶振相同，否则实时操作系统的ticks不准确
	SystemClock_Config();                     /* Configure the System Clock     */
	SystemCoreClockUpdate();
	HAL_Init();                               /* Initialize the HAL Library     */

	osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	osThreadNew(app_main, NULL, NULL);    // Create application main thread

	osKernelStart();                      // Start thread execution
}
