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
	{	//���ִ�������ʱ��TTP229�ᷢ��һ�����壬֪ͨ���հ�����Ϣ
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
//ע�⣺����usSleep��ʱ���ӳ���Խ�׼����Ȼ��Ҫ����ÿ��ָ�����ķѵ�ʱ��
//		�����ź�����ʱ���ж������ӳ٣�CPU�������С��Ч����ߣ��������������л��в���ʱ���ӳ٣������нϴ���ʱ���
//	void usSleep(uint32_t us);
//	usSleep(us);
	TTP229_Timer_delay_us = us;	//Timer��1usһ���ж�
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
			if(key<0) // �����в���ԭ�򣬵��³��ֶ��ν����ж����������ֻȡ��һ���ж�ʱ�ļ�ֵ
				key = i;
  }						    
	if (key==15) TTP229_Read_Bit(); //�����15#��������ն�һ��CLK���������ݿڻ᳤ʱ�䱣�ָߵ�ƽ
 	return key;
}

void TTP229_Initialize (void) 
{
    __HAL_RCC_GPIOA_CLK_ENABLE();       //ʹ��GPIOAʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //CLK���
    GPIO_Initure.Pull=GPIO_PULLDOWN;        
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����         
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   
	
    GPIO_Initure.Pin=GPIO_PIN_6;            //PA6
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING; //�����ش���
    GPIO_Initure.Pull=GPIO_NOPULL;        
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����         
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
  //MyUSART_Send(str1,sizeof(str1)-1);//�ַ���������0���÷���
	while (1) 
	{
    //MyUSART_Send(str1,sizeof(str1)-1);//�ַ���������0���÷���
		osSemaphoreAcquire (sid_TTP229_Semaphore, osWaitForever);
		//�а���������SPIģʽȥ��2Byte
		isHandUP = 0;
		key = TTP229_Read();//key���ǰ������
		
		if(key>=0){	
			NowKeyNum = key+1;
//      sprintf(str1,"key = %2d\n",NowKeyNum);
//      MyUSART_Send(str1,strlen(str1));//�ַ���������0���÷���
    }
		else{
			isHandUP = 1;//TTP229����̧�������ַ���һ�����壬�����ޣ���ʱ��key=-1����ʾ���Ѿ�̧��
//      sprintf(str1,"key = %u\n",NowKeyNum);
//      MyUSART_Send(str1,sizeof(str1)-1);//�ַ���������0���÷���
    }
		TTP229_busy = 0;
    osDelay(50);
	}
}



