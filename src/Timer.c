#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "Timer.h"
#include "LED.h"

TIM_HandleTypeDef htim2;
//TIM_HandleTypeDef htim2, htim5, htim8;
//TIM_HandleTypeDef htim2, htim8;
//extern TIM_HandleTypeDef htim4;//������·ֱ��������ƣ����ô�����motor.c��
extern TIM_HandleTypeDef htim5;//������·������ƣ����ô�����steer.c��


void TIM2_IRQHandler(void)
{
//	HAL_TIM_IRQHandler(&htim2); 
// ����Hal�⺯�����жϴ������ʹ�ûص�����HAL_TIM_PeriodElapsedCallback�������Լ�������
// �������ַ�ʽЧ�ʲ���
	
	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
	
//��������û��Լ��Ĵ���	
	extern int TTP229_Timer_delay_us;
	void TTP229_Timer_us_Callback(void);
	if (TTP229_Timer_delay_us)	TTP229_Timer_us_Callback();
}

//t_us���Զ���װֵ��>=1��
//psc��ʱ��Ԥ��Ƶ������������Ϊ����ʱ�ӵ�һ�� 42
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
void TIM2_Init(int set_us)
{
//TIM2�Ǹ߼�ʱ�ӣ��ź�Ƶ��84M 
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 84/2-1;			//��ʱ����Ƶ��Ϊ0.5us������ʵ����С1us��ʱ���ж�
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 2*set_us-1;		//�Զ���װ��ֵarr
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;//��ҪKeil.STM32F4xx_DFP.2.14.0.pack
	HAL_TIM_Base_Init(&htim2);
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

    //ʹ��TIM�ж�
    HAL_TIM_Base_Start_IT(&htim2);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
	//ִ��HAL_TIM_Base_Init�����Ļص�
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
//    HAL_TIM_PWM_Init(&htim5);       //��ʼ��PWM

//	sConfigOC.OCMode = TIM_OCMODE_PWM1;
//	sConfigOC.Pulse = arr/2;		//ռ�ձ�Ϊ50%
//	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//	HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_2);

//	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2); 
////	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_2);
//}

//void TIM8_CH1_PWM_Init(int arr)
//{ 
////TIM1��TIM8�Ǹ߼�ʱ�ӣ��д������Ļ���������ܣ��ź�Ƶ��168M 
//	TIM_OC_InitTypeDef sConfigOC = {0};
//	
//    htim8.Instance=TIM8;          
//    htim8.Init.Prescaler=168-1;       			//��ʱ����Ƶ 1us
//    htim8.Init.CounterMode=TIM_COUNTERMODE_UP;	//���ϼ���ģʽ
//    htim8.Init.Period=arr-1;          			//�Զ���װ��ֵ
//    htim8.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
//    HAL_TIM_PWM_Init(&htim8);       			//��ʼ��PWM
//    
//    sConfigOC.OCMode=TIM_OCMODE_PWM1; 			//ģʽѡ��PWM1
//    sConfigOC.Pulse=arr/2;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
//    sConfigOC.OCPolarity=TIM_OCPOLARITY_HIGH; 	//����Ƚϼ���Ϊ��
//    HAL_TIM_PWM_ConfigChannel(&htim8,&sConfigOC,TIM_CHANNEL_1);//����TIM8ͨ��1
//	
//    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);	//����PWMͨ��
////δ�о� TIM1��TIM8�ȸ߼�ʱ���л���������������ܣ����������	HAL_TIMEx_PWMN_Start(&htim5, TIM_CHANNEL_2); 
//}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Handle; 

	if(htim->Instance == TIM5)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE(); 
		__HAL_RCC_TIM5_CLK_ENABLE(); 
	
		GPIO_Handle.Pin = GPIO_PIN_0|GPIO_PIN_1; 			// PA0,PA1
		GPIO_Handle.Mode = GPIO_MODE_AF_PP; 					// ���� ����ģʽ
		GPIO_Handle.Pull = GPIO_PULLUP; 							// ��������
		GPIO_Handle.Speed = GPIO_SPEED_HIGH; 					// ����
		GPIO_Handle.Alternate = GPIO_AF2_TIM5;				// PA0����ΪTIM5_CH1,PA1����ΪTIM5_CH2
		HAL_GPIO_Init(GPIOA, &GPIO_Handle); 	
	}
//	if(htim->Instance == TIM4)
//	{
//		__HAL_RCC_GPIOB_CLK_ENABLE(); 
//		__HAL_RCC_TIM4_CLK_ENABLE(); 
//	
//		GPIO_Handle.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9; 	// PB6~9
//		GPIO_Handle.Mode = GPIO_MODE_AF_PP;															// ���� ����ģʽ
//		GPIO_Handle.Pull = GPIO_PULLUP; 																// ��������
//		GPIO_Handle.Speed = GPIO_SPEED_HIGH; 														// ����
//		GPIO_Handle.Alternate = GPIO_AF2_TIM4;													// PB6����ΪTIM4_CH1,PB7����ΪTIM4_CH2,PB8����ΪTIM4_CH3,PB9����ΪTIM4_CH4
//		HAL_GPIO_Init(GPIOB, &GPIO_Handle); 	
//	}
//	if(htim->Instance == TIM8)
//	{
//		__HAL_RCC_TIM8_CLK_ENABLE();			//ʹ�ܶ�ʱ��8
//		__HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
//		
//		GPIO_Handle.Pin=GPIO_PIN_6;           	//PC6
//		GPIO_Handle.Mode=GPIO_MODE_AF_PP;  		//�����������
//		GPIO_Handle.Pull=GPIO_PULLUP;          	//����
//		GPIO_Handle.Speed=GPIO_SPEED_HIGH;     	//����
//		GPIO_Handle.Alternate= GPIO_AF3_TIM8;	//PC6����ΪTIM8_CH1
//		HAL_GPIO_Init(GPIOC,&GPIO_Handle);
//	}
}

//void TIM8_CH1_SetDuty(int duty)
//{
//	TIM8->CCR1=duty;
//}

