#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "timer.h"

#include <string.h>
#include <stdio.h>

#define MAX_CMD_LENGTH 11

volatile int gflash;
volatile int rflash;
volatile int hunMS;

void SysTick_Handler(void) {
			if(hunMS >= 60) {
				if (rflash) {
					Red_LED_Toggle();
				}
				if (gflash) {
					Green_LED_Toggle();
				}
				hunMS = 0;
			} else {
				hunMS++;
			}
}

int main(void){
	char cmd[MAX_CMD_LENGTH] = ""; // Max Command Length Including End String
	
	hunMS = 0; // How Many 100 MS Intervals have Passed
	
	char input[2] = "" ; // Array to Read Input (Including End String)
	int idx = 0 ; // Current IDX for String
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	SysTick_Initialize(1000000);
	
	USART_Write(USART2, (uint8_t *) "Welcome to the UART Machine!\r\n", 32);
	USART_Write(USART2, (uint8_t *) "============================\r\n", 32);
	
	while (1) { // Execute Commands
		while (1) { // Get Commands - Time Loop
	
			// Read Input and Append to String
			input[0] = USART_Read(USART2);
			cmd[idx] = input[0];
			
			if(input[0] == '\0') { // No Input Given
				// Don't Do Anything
			}
			else if (input[0] == 0x7F && idx > 0) { // Backspace Pressed - Can Delete
				USART_Write(USART2, (uint8_t *)input, 1);
				idx--;
			} 
			else if (input[0] == 0x7F && idx <= 0) { // Backspace Pressed - Can't Delete
				idx = 0;
			} 
			else if (input[0] == '\r' ||
				idx >= sizeof(cmd) / sizeof(cmd[0])) { // Command Finished
				USART_Write(USART2, (uint8_t *)"\r\n", 2);
				break;
			} 
			else { // General Character Pressed
				USART_Write(USART2, (uint8_t *)input, 1);
				idx++;
			}
		}

		// Execute Finished Command
		if (!strncmp(cmd, "ROFF", 4) && idx == 4) {
			rflash = 0;
			Red_LED_Off();
			USART_Write(USART2, (uint8_t *) "Turned off red LED.", 19);
		}
		else if (!strncmp(cmd, "RON", 3) && idx == 3){
			rflash = 0;
			Red_LED_On();
			USART_Write(USART2, (uint8_t *) "Turned on red LED.", 18);
		}
		else if (!strncmp(cmd, "GOFF", 4) && idx == 4){
			gflash = 0;
			Green_LED_Off();
			USART_Write(USART2, (uint8_t *) "Turned off green LED.", 21);
		}
		else if (!strncmp(cmd, "GON", 3) && idx == 3){
			gflash = 0;
			Green_LED_On();
			USART_Write(USART2, (uint8_t *) "Turned on green LED.", 20);
		}
		else if (!strncmp(cmd, "RFLASH", 6) && idx == 6){
			rflash = 1;
			USART_Write(USART2, (uint8_t *) "Flashing red LED.", 17);
		}
		else if (!strncmp(cmd, "GFLASH", 6) && idx == 6){
			gflash = 1;
			USART_Write(USART2, (uint8_t *) "Flashing green LED.", 19);
		}
		else if (!strncmp(cmd, "FLASHOFF", 8) && idx == 8){
			rflash = 0;
			gflash = 0;
			USART_Write(USART2, (uint8_t *) "Stopped Flashing for both LEDs.", 31);
		}
		else {
			USART_Write(USART2, (uint8_t *) "Error: Invalid Command", 22);
		}
		USART_Write(USART2, (uint8_t *)"\r\n", 2);
		USART_Write(USART2, (uint8_t *)"\r\n", 2);
		
		// Prepare for Next Command
		input[0] = 0;
		idx = 0;
	}
}

