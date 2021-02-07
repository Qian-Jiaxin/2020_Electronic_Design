#include "TTP229.h"
void usSleep(uint32_t us);

osSemaphoreId_t sid_TTP229_Semaphore;
uint8_t TTP229_busy = 0;
static GPIO_InitTypeDef GPIO_Initure;
void TTP229_WaitForPress (void) 
{
    __HAL_RCC_GPIOA_CLK_ENABLE();           //ʹ��GPIOBʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  	//����
    GPIO_Initure.Pull=GPIO_PULLDOWN;        
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����         
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   
	
    GPIO_Initure.Pin=GPIO_PIN_6;            //PA6
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLDOWN;        
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����         
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
}

SPI_HandleTypeDef SPI1_Handler;  //SPI1���
    uint16_t Rxdata[20];
uint16_t TTP229_SPI_Read (void) 
{
	uint8_t *p = (uint8_t *)Rxdata;
	int dd;
    
    //PA5,6
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_5;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
    GPIO_Initure.Pull=GPIO_PULLDOWN;               
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //����            
    GPIO_Initure.Alternate=GPIO_AF5_SPI1;           //����ΪSPI1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

	
	usSleep(2);
//HAL_SPI_Abort(&SPI1_Handler); 
    __HAL_SPI_ENABLE(&SPI1_Handler);                    //ʹ��SPI1
    dd = HAL_SPI_Receive(&SPI1_Handler,p,4, 100000);       
	osDelay(1);
//    HAL_SPI_Receive(&SPI1_Handler,p,2, 1);       
	__HAL_SPI_DISABLE(&SPI1_Handler);
//	HAL_SPI_DeInit(&SPI1_Handler);
	
 	return Rxdata[1];          		    //�����յ�������		
}

void TTP229_Initialize (void) 
{
    __HAL_RCC_GPIOA_CLK_ENABLE();       //ʹ��GPIOAʱ��
    __HAL_RCC_SPI1_CLK_ENABLE();        //ʹ��SPI1ʱ��
	TTP229_WaitForPress();
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);   
	
    SPI1_Handler.Instance=SPI1;                         //SPI1
    SPI1_Handler.Init.Mode=SPI_MODE_MASTER;             //����SPI����ģʽ������Ϊ��ģʽ
    SPI1_Handler.Init.Direction=SPI_DIRECTION_2LINES_RXONLY;   //����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
    SPI1_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //����SPI�����ݴ�С:SPI���ͽ���16λ֡�ṹ
    SPI1_Handler.Init.CLKPolarity=SPI_POLARITY_LOW;     //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI1_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI1_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
//SPI�ٶ�=fAPB1/��Ƶϵ��
//����SPI�ٶ�Ҫע��TTP229˫��ͨ�����SCLΪ500KHz
//84Mhz/256 = 328125
    SPI1_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI1_Handler.Init.FirstBit=SPI_FIRSTBIT_LSB;        //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����LSBλ��ʼ
    SPI1_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //�ر�TIģʽ
    SPI1_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
    SPI1_Handler.Init.CRCPolynomial=7;                  //CRCֵ����Ķ���ʽ
    HAL_SPI_Init(&SPI1_Handler);//��ʼ��
    
}

uint16_t NowKeyNum;
void TouchKey_Read (void *argument) 
{
	uint16_t key;
	int i;
	sid_TTP229_Semaphore = osSemaphoreNew(1, 0, NULL);
	while (1) 
	{
		osSemaphoreAcquire (sid_TTP229_Semaphore, osWaitForever);
		//�а���������SPIģʽȥ��2Byte
		TTP229_busy = 1;
		key = TTP229_SPI_Read();
		TTP229_WaitForPress();
		
		for(i=0;i<16;i++)
		{
			if(key&1) break;
			key >>= 1;
		}
		//i���ǰ������
		NowKeyNum = i;
		osDelay(100);//TTP229̧�����ַ���һ�����壬ͨ����ʱ����������
		TTP229_busy = 0;
	}
}
