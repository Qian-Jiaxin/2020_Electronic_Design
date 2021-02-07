#include "Step.h"

osThreadId_t Semaphore_Step_Control;
osThreadId_t Semaphore_Step_Control_Done;

uint16_t freq = 3200;
static void Step_States(uint16_t States)
{
	if(States == On)
		HAL_GPIO_WritePin(Step_Enable_GPIO_Port,Step_Enable_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(Step_Enable_GPIO_Port,Step_Enable_Pin,GPIO_PIN_SET);
}

void Step_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOE_CLK_ENABLE();

//  HAL_GPIO_WritePin(GPIOE, Step_Enable_Pin|Step_Orient_Pin|Pul_Pin,GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = Step_Enable_Pin|Step_Orient_Pin|Pul_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	Step_States(Off);
	
	Semaphore_Step_Control = osSemaphoreNew(1, 0, NULL);
  Semaphore_Step_Control_Done = osSemaphoreNew(1,0,NULL);
}


extern void TTP229_usSleep(int us);
static uint32_t Step_Cal_Num(int angle)
{
	double temp = (360.00/freq)/3.71;
	uint32_t Num = angle/temp/100;
	return Num;
}
int curAngle=0;
void Step_Control(int angle)
{
	int Num,i;
	if(curAngle==angle)
    return;
	Step_States(On);
	
  Num = curAngle-angle;
	if(Num>0)//ÄæÊ±Õë×ª¶¯
		HAL_GPIO_WritePin(Step_Orient_GPIO_Port,Step_Orient_Pin,GPIO_PIN_SET);
	else
  {
		HAL_GPIO_WritePin(Step_Orient_GPIO_Port,Step_Orient_Pin,GPIO_PIN_RESET);
    Num = -Num;
  }
  curAngle = angle;
  Num = Step_Cal_Num(Num);
	
	for(i=0;i<Num;i++)
	{
		HAL_GPIO_WritePin(Pul_GPIO_Port,Pul_Pin,GPIO_PIN_SET);
		TTP229_usSleep(400);
		HAL_GPIO_WritePin(Pul_GPIO_Port,Pul_Pin,GPIO_PIN_RESET);
		TTP229_usSleep(400);
	}
	Step_States(Off);
}

void Step_Control_Thread(void *argument)
{
//	while(1)
//	{
//	osSemaphoreAcquire(Semaphore_Step_Control,osWaitForever);
//    
//	extern double angle;
//	
//	if(angle>0)
//		Step_Control(ACW,__fabs(angle));
//	else if(angle<0)
//		Step_Control(CW,__fabs(angle));
//  
//  osSemaphoreRelease(Semaphore_Step_Control_Done);
//}
}
