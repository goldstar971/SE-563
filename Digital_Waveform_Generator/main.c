#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "timer.h"
#include "main.h"
#include "dac.h"
extern DAC_state dac_state;

int main(void){
	
	// Initialize all peripherals
	System_Clock_Init(); // Switch System Clock = 80 MHz
	UART2_Init();
	TIM_Init(TIM2);
	init_dac();
	init_dac_GPIO();
	
	
  
	while(1) {
	}
}
