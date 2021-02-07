#include "Driver_USART.h"
#include "cmsis_os2.h" 
#include <stdio.h>
#include <string.h> 

#include "Usart.h"

/*
说明：
1，在RTE_Device.h里面设置Usart，LPC串口程序DMA部分还未研究好，关闭DMA，否则接收会异常。
2，在IDE的C编译环境中Define设置栏添加这些常量
	USART0_TRIG_LVL=USART_TRIG_LVL_14,USART1_TRIG_LVL=USART_TRIG_LVL_14,USART2_TRIG_LVL=USART_TRIG_LVL_14,USART3_TRIG_LVL=USART_TRIG_LVL_14
或者在RTE_Device.h里面增加下面几行定义
	#define USART0_TRIG_LVL           USART_TRIG_LVL_14
	#define USART1_TRIG_LVL           USART_TRIG_LVL_14
	#define USART2_TRIG_LVL           USART_TRIG_LVL_14
	#define USART3_TRIG_LVL           USART_TRIG_LVL_14
*/


//void  MyUSART_Initialization (void); 

uint32_t MyUSART_Boudrate=115200;

extern ARM_DRIVER_USART Driver_USART1;
static ARM_DRIVER_USART *MyUSART_drv = &Driver_USART1;

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      200
/* Size of Reception buffer */
#define RXBUFFERSIZE                      32

uint8_t MyUSART_RxBuffer  [RXBUFFERSIZE];

char bSending_USART=0;
char bReceiving_USART=0;

osSemaphoreId_t sem_Usart_Send;
osSemaphoreId_t sem_Usart_Receive;

void MyUSART_callback(uint32_t event)
{
	uint32_t mask;
	mask = 	ARM_USART_EVENT_RX_OVERFLOW  		|
			ARM_USART_EVENT_TX_UNDERFLOW 		|
			ARM_USART_EVENT_RX_BREAK 			|
			ARM_USART_EVENT_RX_FRAMING_ERROR 	|
			ARM_USART_EVENT_RX_PARITY_ERROR  	;
	if (event & mask) {
// 发现故障，复位串口收发信号
		MyUSART_drv->Control(ARM_USART_ABORT_RECEIVE, 0);
		osSemaphoreRelease(sem_Usart_Send);
		osSemaphoreRelease(sem_Usart_Receive);
//		MyUSART_drv->Uninitialize();
//		MyUSART_Initialization();
		return ;
	}
	if (event & (ARM_USART_EVENT_TX_COMPLETE|ARM_USART_EVENT_SEND_COMPLETE)) {
// 发送结束
		osSemaphoreRelease(sem_Usart_Send);
		return ;
	}
	if (event & ARM_USART_EVENT_RX_TIMEOUT) {
// 收到一个新的命令
		osSemaphoreRelease(sem_Usart_Receive);
		return ;
	}
}

void MyUSART_Send (uint8_t *out, uint32_t cnt) 
{
	bSending_USART = 1;
	MyUSART_drv->Send (out, cnt);
    
	osSemaphoreAcquire (sem_Usart_Send, osWaitForever);
	bSending_USART = 0;
}

int MyUSART_Receive (uint8_t *in) 
{
//采用IDLE Line总线空闲检测功能，实现一个数据帧的接收
	bReceiving_USART = 1;
//	LPC_USART0->FCR = 0x07 | 3<<6;//使能FIFO; 14个字节FIFO //C语言环境常量设置后，这行不需要了	
	MyUSART_drv->Receive (in, RXBUFFERSIZE);
	
	osSemaphoreAcquire (sem_Usart_Receive, osWaitForever);
	bReceiving_USART = 0;
	MyUSART_drv->Control(ARM_USART_ABORT_RECEIVE, 0);
	return MyUSART_drv->GetRxCount();
}

void  MyUSART_Initialization (void) 
{  
	sem_Usart_Send = osSemaphoreNew(1, 0, NULL);
	sem_Usart_Receive = osSemaphoreNew(1, 0, NULL);
	
    /*Initialize the USART driver */
    MyUSART_drv->Initialize(MyUSART_callback);
    /*Power up the USART peripheral */
    MyUSART_drv->PowerControl(ARM_POWER_FULL);
    /*Configure the USART */
    MyUSART_drv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, MyUSART_Boudrate);
     
    /* Enable Receiver and Transmitter lines */
    MyUSART_drv->Control (ARM_USART_CONTROL_TX, 1);
    MyUSART_drv->Control (ARM_USART_CONTROL_RX, 1);
}

void MyUSART_SendChar (uint8_t ch) 
{
	MyUSART_Send(&ch,1);
}


	uint8_t str[] = "Hello World!\n";
void  MyUSART_Test (void *argument) 
{
	int rxCount=0;
	
//	MyUSART_Initialization();
	
	MyUSART_Send(str,sizeof(str)-1);//字符串结束符0不用发送
	while(1){
		rxCount = MyUSART_Receive(MyUSART_RxBuffer);
		MyUSART_Send(MyUSART_RxBuffer,rxCount);
	}
}



/*-----------------------------------------------------------------------*/
uint32_t Laser_USART_Boudrate=38400;

extern ARM_DRIVER_USART Driver_USART2;
static ARM_DRIVER_USART *Laser_USART_drv = &Driver_USART2;

uint8_t Laser_USART_RxBuffer  [16];

char Laser_bSending_USART=0;
char Laser_bReceiving_USART=0;

osSemaphoreId_t sem_Usart_Send_1;
osSemaphoreId_t sem_Usart_Receive_1;

void Laser_USART_callback(uint32_t event)
{
	uint32_t mask;
	mask = 	ARM_USART_EVENT_RX_OVERFLOW  		|
			ARM_USART_EVENT_TX_UNDERFLOW 		|
			ARM_USART_EVENT_RX_BREAK 			|
			ARM_USART_EVENT_RX_FRAMING_ERROR 	|
			ARM_USART_EVENT_RX_PARITY_ERROR  	;
	if (event & mask) {
// 发现故障，复位串口收发信号
		Laser_USART_drv->Control(ARM_USART_ABORT_RECEIVE, 0);
		osSemaphoreRelease(sem_Usart_Send_1);
		osSemaphoreRelease(sem_Usart_Receive_1);
//		Laser_USART_drv->Uninitialize();
//		MyUSART_Initialization();
		return ;
	}
	if (event & (ARM_USART_EVENT_TX_COMPLETE|ARM_USART_EVENT_SEND_COMPLETE)) {
// 发送结束
		osSemaphoreRelease(sem_Usart_Send_1);
		return ;
	}
	if (event & ARM_USART_EVENT_RX_TIMEOUT) {
// 收到一个新的命令
		osSemaphoreRelease(sem_Usart_Receive_1);
		return ;
	}
}

void Laser_USART_Send (uint8_t *out, uint32_t cnt) 
{
	Laser_bSending_USART = 1;
	Laser_USART_drv->Send (out, cnt);
    
	osSemaphoreAcquire (sem_Usart_Send_1, osWaitForever);
	Laser_bSending_USART = 0;
}

int Laser_USART_Receive (uint8_t *in) 
{
//采用IDLE Line总线空闲检测功能，实现一个数据帧的接收
	Laser_bReceiving_USART = 1;
//	LPC_USART0->FCR = 0x07 | 3<<6;//使能FIFO; 14个字节FIFO //C语言环境常量设置后，这行不需要了	
	Laser_USART_drv->Receive (in, RXBUFFERSIZE);
	
	osSemaphoreAcquire (sem_Usart_Receive_1, osWaitForever);
	Laser_bReceiving_USART = 0;
	Laser_USART_drv->Control(ARM_USART_ABORT_RECEIVE, 0);
	return Laser_USART_drv->GetRxCount();
}

void  Laser_USART_Initialization (void) 
{  
	sem_Usart_Send_1 = osSemaphoreNew(1, 0, NULL);
	sem_Usart_Receive_1 = osSemaphoreNew(1, 0, NULL);
	
    /*Initialize the USART driver */
    Laser_USART_drv->Initialize(Laser_USART_callback);
    /*Power up the USART peripheral */
    Laser_USART_drv->PowerControl(ARM_POWER_FULL);
    /*Configure the USART */
    Laser_USART_drv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, Laser_USART_Boudrate);
     
    /* Enable Receiver and Transmitter lines */
    Laser_USART_drv->Control (ARM_USART_CONTROL_TX, 1);
    Laser_USART_drv->Control (ARM_USART_CONTROL_RX, 1);
}

void Laser_USART_SendChar (uint8_t ch) 
{
	Laser_USART_Send(&ch,1);
}
