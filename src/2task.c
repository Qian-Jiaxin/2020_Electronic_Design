#include "cmsis_os2.h"
#include "LED.h"

osThreadId_t tid_Thread1, tid_Thread2;
 
void Thread1 (void *argument) 
{
	while (1) {
		LED_On(0);
		osDelay(500);
		LED_Off(0);
		osDelay(500);
	}
}
void Thread2 (void *argument) 
{
	while (1) {
		LED_On(1);
		osDelay(300);
		LED_Off(1);
		osDelay(300);
	}
}

void app_main_2task (void *argument) 
{
	tid_Thread1 = osThreadNew (Thread1, NULL, NULL);
	tid_Thread2 = osThreadNew (Thread2, NULL, NULL);
}
