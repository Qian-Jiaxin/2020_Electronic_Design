#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "Timer.h"
#include "LED.h"

TIM_HandleTypeDef htim2;
//TIM_HandleTypeDef htim2, htim5, htim8;
//TIM_HandleTypeDef htim2, htim8;
//extern TIM_HandleTypeDef htim4;//用于四路直流电机控制，配置代码在motor.c中
extern TIM_HandleTypeDef htim5;//用于两路舵机控制，配置代码在steer.c中


void TIM2_IRQHandler(void)
{
//	HAL_TIM_IRQHandler(&htim2); 
// 调用Hal库函数的中断处理可以使用回调函数HAL_TIM_PeriodElapsedCallback来处理自己的任务，
// 但是这种方式效率不高
	
	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
	
//下面添加用户自己的代码	
	extern int TTP229_Timer_delay_us;
	void TTP229_Timer_us_Callback(void);
	if (TTP229_Timer_delay_us)	TTP229_Timer_us_Callback();
}

//t_us：自动重装值，>=1。
//psc：时钟预分频数，这里设置为外设时钟的一半 42
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
void TIM2_Init(int set_us)
{
//TIM2是高级时钟，信号频率84M 
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 84/2-1;			//定时器分频设为0.5us，方能实现最小1us的时钟中断
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 2*set_us-1;		//自动重装载值arr
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;//需要Keil.STM32F4xx_DFP.2.14.0.pack
	HAL_TIM_Base_Init(&htim2);
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

    //使能TIM中断
    HAL_TIM_Base_Start_IT(&htim2);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
	//执行HAL_TIM_Base_Init函数的回调
  if(tim_baseHandle->Instance==TIM2)
  {
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  }
}

//void TIM5_CH2_PWM_Init(int arr)
//{
//	TIM_OC_InitTypeDef sConfigOC = {0};
//	
//	htim5.Instance = TIM5;
//	htim5.Init.Prescaler = 84-1;  	//1us
//	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
//	htim5.Init.Period = arr-1;
//	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//    HAL_TIM_PWM_Init(&htim5);       //初始化PWM

//	sConfigOC.OCMode = TIM_OCMODE_PWM1;
//	sConfigOC.Pulse = arr/2;		//占空比为50%
//	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//	HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_2);

//	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2); 
////	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_2);
//}

//void TIM8_CH1_PWM_Init(int arr)
//{ 
////TIM1、TIM8是高级时钟，有带死区的互补输出功能，信号频率168M 
//	TIM_OC_InitTypeDef sConfigOC = {0};
//	
//    htim8.Instance=TIM8;          
//    htim8.Init.Prescaler=168-1;       			//定时器分频 1us
//    htim8.Init.CounterMode=TIM_COUNTERMODE_UP;	//向上计数模式
//    htim8.Init.Period=arr-1;          			//自动重装载值
//    htim8.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
//    HAL_TIM_PWM_Init(&htim8);       			//初始化PWM
//    
//    sConfigOC.OCMode=TIM_OCMODE_PWM1; 			//模式选择PWM1
//    sConfigOC.Pulse=arr/2;            //设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
//    sConfigOC.OCPolarity=TIM_OCPOLARITY_HIGH; 	//输出比较极性为高
//    HAL_TIM_PWM_ConfigChannel(&htim8,&sConfigOC,TIM_CHANNEL_1);//配置TIM8通道1
//	
//    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);	//开启PWM通道
////未研究 TIM1、TIM8等高级时钟有互补输出及死区功能，启动用这个	HAL_TIMEx_PWMN_Start(&htim5, TIM_CHANNEL_2); 
//}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Handle; 

	if(htim->Instance == TIM5)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE(); 
		__HAL_RCC_TIM5_CLK_ENABLE(); 
	
		GPIO_Handle.Pin = GPIO_PIN_0|GPIO_PIN_1; 			// PA0,PA1
		GPIO_Handle.Mode = GPIO_MODE_AF_PP; 					// 复用 推挽模式
		GPIO_Handle.Pull = GPIO_PULLUP; 							// 上拉电阻
		GPIO_Handle.Speed = GPIO_SPEED_HIGH; 					// 高速
		GPIO_Handle.Alternate = GPIO_AF2_TIM5;				// PA0复用为TIM5_CH1,PA1复用为TIM5_CH2
		HAL_GPIO_Init(GPIOA, &GPIO_Handle); 	
	}
//	if(htim->Instance == TIM4)
//	{
//		__HAL_RCC_GPIOB_CLK_ENABLE(); 
//		__HAL_RCC_TIM4_CLK_ENABLE(); 
//	
//		GPIO_Handle.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9; 	// PB6~9
//		GPIO_Handle.Mode = GPIO_MODE_AF_PP;															// 复用 推挽模式
//		GPIO_Handle.Pull = GPIO_PULLUP; 																// 上拉电阻
//		GPIO_Handle.Speed = GPIO_SPEED_HIGH; 														// 高速
//		GPIO_Handle.Alternate = GPIO_AF2_TIM4;													// PB6复用为TIM4_CH1,PB7复用为TIM4_CH2,PB8复用为TIM4_CH3,PB9复用为TIM4_CH4
//		HAL_GPIO_Init(GPIOB, &GPIO_Handle); 	
//	}
//	if(htim->Instance == TIM8)
//	{
//		__HAL_RCC_TIM8_CLK_ENABLE();			//使能定时器8
//		__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
//		
//		GPIO_Handle.Pin=GPIO_PIN_6;           	//PC6
//		GPIO_Handle.Mode=GPIO_MODE_AF_PP;  		//复用推完输出
//		GPIO_Handle.Pull=GPIO_PULLUP;          	//上拉
//		GPIO_Handle.Speed=GPIO_SPEED_HIGH;     	//高速
//		GPIO_Handle.Alternate= GPIO_AF3_TIM8;	//PC6复用为TIM8_CH1
//		HAL_GPIO_Init(GPIOC,&GPIO_Handle);
//	}
}

//void TIM8_CH1_SetDuty(int duty)
//{
//	TIM8->CCR1=duty;
//}

