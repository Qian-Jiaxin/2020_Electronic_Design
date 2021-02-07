#include "TTP229.h"
void usSleep(uint32_t us);

osSemaphoreId_t sid_TTP229_Semaphore;
uint8_t TTP229_busy = 0;
static GPIO_InitTypeDef GPIO_Initure;
void TTP229_WaitForPress (void) 
{
    __HAL_RCC_GPIOA_CLK_ENABLE();           //使能GPIOB时钟
    
    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  	//输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速         
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   
	
    GPIO_Initure.Pin=GPIO_PIN_6;            //PA6
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLDOWN;        
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速         
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
}

SPI_HandleTypeDef SPI1_Handler;  //SPI1句柄
    uint16_t Rxdata[20];
uint16_t TTP229_SPI_Read (void) 
{
	uint8_t *p = (uint8_t *)Rxdata;
	int dd;
    
    //PA5,6
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_5;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLDOWN;               
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速            
    GPIO_Initure.Alternate=GPIO_AF5_SPI1;           //复用为SPI1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

	
	usSleep(2);
//HAL_SPI_Abort(&SPI1_Handler); 
    __HAL_SPI_ENABLE(&SPI1_Handler);                    //使能SPI1
    dd = HAL_SPI_Receive(&SPI1_Handler,p,4, 100000);       
	osDelay(1);
//    HAL_SPI_Receive(&SPI1_Handler,p,2, 1);       
	__HAL_SPI_DISABLE(&SPI1_Handler);
//	HAL_SPI_DeInit(&SPI1_Handler);
	
 	return Rxdata[1];          		    //返回收到的数据		
}

void TTP229_Initialize (void) 
{
    __HAL_RCC_GPIOA_CLK_ENABLE();       //使能GPIOA时钟
    __HAL_RCC_SPI1_CLK_ENABLE();        //使能SPI1时钟
	TTP229_WaitForPress();
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,3);   	
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);   
	
    SPI1_Handler.Instance=SPI1;                         //SPI1
    SPI1_Handler.Init.Mode=SPI_MODE_MASTER;             //设置SPI工作模式，设置为主模式
    SPI1_Handler.Init.Direction=SPI_DIRECTION_2LINES_RXONLY;   //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI1_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //设置SPI的数据大小:SPI发送接收16位帧结构
    SPI1_Handler.Init.CLKPolarity=SPI_POLARITY_LOW;     //串行同步时钟的空闲状态为高电平
    SPI1_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI1_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
//SPI速度=fAPB1/分频系数
//设置SPI速度要注意TTP229双线通信最大SCL为500KHz
//84Mhz/256 = 328125
    SPI1_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//定义波特率预分频的值:波特率预分频值为256
    SPI1_Handler.Init.FirstBit=SPI_FIRSTBIT_LSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从LSB位开始
    SPI1_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI1_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI1_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式
    HAL_SPI_Init(&SPI1_Handler);//初始化
    
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
		//有按键，开启SPI模式去读2Byte
		TTP229_busy = 1;
		key = TTP229_SPI_Read();
		TTP229_WaitForPress();
		
		for(i=0;i<16;i++)
		{
			if(key&1) break;
			key >>= 1;
		}
		//i就是按键编号
		NowKeyNum = i;
		osDelay(100);//TTP229抬起动作又发了一个脉冲，通过延时把它废弃掉
		TTP229_busy = 0;
	}
}
