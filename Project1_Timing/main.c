#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"

#include <string.h>
#include <stdio.h>

char RxComByte = 0;
uint8_t buffer[BufferSize];
char str[] = "Give Red LED control input (Y = On, N = off):\r\n";

int main(void){
	//char rxByte;
	int		a ;
	int		n ;
	int		i ;
	float b;
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
		
	while (1){
		n = sprintf((char *)buffer, "a = %d\t", a);
		n += sprintf((char *)buffer + n, "b = %f\r\n", b);
		USART_Write(USART2, buffer, n);		
		a = a + 1;
		b = (float)a/100;
		// now spin for a while to slow it down
		for (i = 0; i < 4000000; i++)
			;
		
//		USART_Write(USART2, (uint8_t *)str, strlen(str));	
//		rxByte = USART_Read(USART2);
//		if (rxByte == 'N' || rxByte == 'n'){
//			Red_LED_Off();
//			USART_Write(USART2, (uint8_t *)"LED is Off\r\n\r\n", 16);
//		}
//		else if (rxByte == 'Y' || rxByte == 'y'){
//			Red_LED_On();
//			USART_Write(USART2, (uint8_t *)"LED is on\r\n\r\n", 15);
//		}
	}
}

