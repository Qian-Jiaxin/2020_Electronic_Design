#include "TTP229.h"
#include "OLED_SPI.h"
#include "Timer.h"
#include "usart.h"
#include "stdio.h"
//
extern void usSleep(uint32_t us);
extern void TouchKey_Handle(void);

osSemaphoreId_t sid_TTP229_Semaphore;
osSemaphoreId_t sid_TTP229_delay_Semaphore;
uint8_t TTP229_busy = 0;
static GPIO_InitTypeDef GPIO_Initure;
uint8_t str1[10];

#define TTP229_CLK(level)	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (GPIO_PinState)level);
int PA6=0;

int TTP229_Timer_delay_us=0;
void TTP229_Timer_us_Callback(void)
{
	if(TTP229_Timer_delay_us)
	{
		TTP229_Timer_delay_us--;
		if(TTP229_Timer_delay_us==0)
			osSemaphoreRelease (sid_TTP229_delay_Semaphore);
	}
}
void TTP229_EXTI_Callback(void)
{
	if (TTP229_busy == 0 )
	{	//当手触碰按键时，TTP229会发送一个脉冲，通知接收按键信息
		TTP229_busy = 1;
		osSemaphoreRelease (sid_TTP229_Semaphore);
	}
	else
	{
		PA6 = 1;
	}
}

void TTP229_usSleep(int us)
{
//注意：采用usSleep的时间延迟相对较准，当然还要考虑每条指令所耗费的时间
//		采用信号量加时钟中断来做延迟，CPU的损耗最小，效率最高，但是由于任务切换有不定时间延迟，所以有较大延时误差
//	void usSleep(uint32_t us);
//	usSleep(us);
	TTP229_Timer_delay_us = us;	//Timer是1us一次中断
	if(TTP229_Timer_delay_us<1) TTP229_Timer_delay_us = 1;
	osSemaphoreAcquire (sid_TTP229_delay_Semaphore, 2);//osWaitForever);
}


int TTP229_Read_Bit(void) 
{
	PA6 = 0;
	TTP229_usSleep(10);
	TTP229_CLK(1); 
	TTP229_usSleep(10);
	TTP229_CLK(0); 

	return PA6;
}
int TTP229_Read (void) 
{
	int i,key=-1;
	
	TTP229_usSleep(100);
	TTP229_usSleep(12);
	for (i=0;i<16;i++) 
	{
     if(TTP229_Read_Bit())
			if(key<0) // 发现有不明原因，导致出现二次进入中断情况，所以只取第一次中断时的键值
				key = i;
  }						    
	if (key==15) TTP229_Read_Bit(); //如果是15#按键，则空读一个CLK，否则数据口会长时间保持高电平
 	return key;
}

void TTP229_Initialize (void) 
{
    __HAL_RCC_GPIOA_CLK_ENABLE();       //使能GPIOA时钟
    
    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //CLK输出
    GPIO_Initure.Pull=GPIO_PULLDOWN;        
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //快速         
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   
	
    GPIO_Initure.Pin=GPIO_PIN_6;            //PA6
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING; //上升沿触发
    GPIO_Initure.Pull=GPIO_NOPULL;        
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //快速         
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   
	
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);   
	
}

uint16_t NowKeyNum;
uint8_t  isHandUP;

void TouchKey_Read (void *argument) 
{
	int key;
	sid_TTP229_Semaphore = osSemaphoreNew(1, 0, NULL);
  sid_TTP229_delay_Semaphore = osSemaphoreNew(1, 0, NULL);
  TTP229_Initialize();
  //MyUSART_Send(str1,sizeof(str1)-1);//字符串结束符0不用发送
	while (1) 
	{
    //MyUSART_Send(str1,sizeof(str1)-1);//字符串结束符0不用发送
		osSemaphoreAcquire (sid_TTP229_Semaphore, osWaitForever);
		//有按键，开启SPI模式去读2Byte
		isHandUP = 0;
		key = TTP229_Read();//key就是按键编号
		
		if(key>=0){	
			NowKeyNum = key+1;
//      sprintf(str1,"key = %2d\n",NowKeyNum);
//      MyUSART_Send(str1,strlen(str1));//字符串结束符0不用发送
    }
		else{
			isHandUP = 1;//TTP229在手抬起动作后又发了一个脉冲，读数无，此时的key=-1，表示手已经抬起
//      sprintf(str1,"key = %u\n",NowKeyNum);
//      MyUSART_Send(str1,sizeof(str1)-1);//字符串结束符0不用发送
    }
		TTP229_busy = 0;
    osDelay(50);
	}
}



