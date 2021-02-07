#ifndef __OLED_SPI_DEFINE_H
#define __OLED_SPI_DEFINE_H
#include <stdint.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

typedef enum
{
  BackOn = 0,
  BackOff = 1
}Background;


void SPI_Initialize (void);
int32_t SPI_WriteBuf (const uint8_t *buf, uint32_t len);
int32_t SPI_ReadBuf (uint8_t *buf, uint32_t len);
int32_t SPI_TransferBuf (uint8_t *buf_out, uint8_t *buf_in, uint32_t len); 

extern osMutexId_t mutex_OLED;

void OLED_SPI_Test (void *argument);



//-----------------OLED端口定义----------------  					   
#define OLED_RST(a)	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,(GPIO_PinState)a)
#define OLED_RS(a)	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,(GPIO_PinState)a)

		     
//OLED控制用函数
void OLED_WR_Byte(uint8_t dat);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(int x,int y,uint8_t t);
void OLED_Fill(int x1,int y1,int x2,int y2,uint8_t dot);
void OLED_ShowChar(int x,int y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(int x,int y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(int x,int y,const uint8_t *p,uint8_t size,uint8_t mode);	
void OLED_DrawFont_GBK16(int x, int y, uint8_t *s, int mode);

#endif
