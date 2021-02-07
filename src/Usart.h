#ifndef __USART_DEFINE_H
#define __USART_DEFINE_H
#include <stdint.h>

extern uint8_t USART_RxBuffer[];

void  MyUSART_Initialization (void);
void  MyUSART_SendChar (uint8_t ch);
void  MyUSART_Send (uint8_t *out, uint32_t cnt);
int   MyUSART_Receive (uint8_t *in);


void Laser_USART_Send (uint8_t *out, uint32_t cnt);
int Laser_USART_Receive (uint8_t *in);
void  Laser_USART_Initialization (void);
void Laser_USART_SendChar (uint8_t ch);
void MyUSART_Test(void *argument);

//void  MyUSART_Check (void);
//void Model_1_Commun(void);
//void Model_2_Commun(void);

#endif
