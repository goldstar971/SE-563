#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include "main.h"


int main(void){
	
	// Initialize all peripherals
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	TIM_GPIO_Init(GPIOA);
	//init PWM timer
	TIM_Init(TIM2,TWENTY_MS,1);
	//init recipe timer
	TIM_Init(TIM5,ONE_HUNDRED_MS,0);
	
	
	//print start message to terminal
	USART_Write(USART2, (uint8_t *) "Welcome to Servo Control!\r\n", 27);
	USART_Write(USART2, (uint8_t *) "=========================\r\n>", 28);
  
	while(1) {
	}
}
