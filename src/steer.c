#include "steer.h"

TIM_HandleTypeDef htim5;

//配置TIM5PWM波发生配置，舵机驱动要求以20ms为周期有效高电平时间为0.5ms到2.5ms的PWM
void TIM5_2_STEERS_PWM_Init(void)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	
	htim5.Instance = TIM5;
	htim5.Init.Prescaler = 84-1;  	//1us
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.Period = 20000-1;		//20ms
	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_PWM_Init(&htim5);       //初始化PWM

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1415-1;		//1.5ms(舵机归中状态)
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1500-1;		//1.5ms(舵机归中状态)
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
}

////设置两路舵机角度，对应angle_1为180度舵机实际角度（TIM5），对应angle_2为270度舵机实际角度.
//static void TIM5_2_STEERS_SetDuty(int angle_1,int angle_2)
//{
//	TIM5->CCR1 = (int)(500+((double)angle_1/180)*2000);
//	TIM5->CCR2 = (int)(500+((double)angle_2/270)*2000);
//}

static void TIM5_270_STEER_SetAngle(int angle)
{//angle实际角度*10
	TIM5->CCR2 = 500+angle*2000/270/10;
}

void Steer_Work(int Adjust_Angle)
{
//	double Angle;
//	if(Adjust_Angle>0)
//	{
//		Angle = (uint32_t)Adjust_Angle;
//		for(int i=0;i<Angle;i=i+1)
//		{
//			TIM5_270_STEER_SetAngle(135+i);
//			osDelay(50);
//		}
//		TIM5_270_STEER_SetAngle(135+Angle);
//	}
//	else if(Adjust_Angle<0)
//	{
//		Angle = (uint32_t)Adjust_Angle;
//		for(int i=0;i<Angle;i=i+2)
//		{
//			TIM5_270_STEER_SetAngle(135-i);
//			osDelay(50);
//		}
//		TIM5_270_STEER_SetAngle(135-Angle);
//	}
	TIM5_270_STEER_SetAngle(1350+Adjust_Angle);
}

