#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include "main.h"
#define MAX_CMD_LENGTH 11

volatile int gflash;
volatile int rflash;
volatile int hunMS;

motor_ctrl motor2;
motor_ctrl motor1;

char recipe1[20]={MOV|0,MOV|5,MOV|0,MOV|3,LOOP|0,MOV|1,MOV|4,END_lOOP,MOV|0, \
MOV|2,WAIT|0,MOV|2,MOV|3,WAIT|31,WAIT|31,WAIT|31,MOV|4}; //demo recipe
char recipe2[26]={MOV|0,WAIT|2,MOV|1,WAIT|2,MOV|2,WAIT|2,MOV|3,WAIT|2,MOV|4, \
WAIT|2,MOV|5,WAIT|2,MOV|4,WAIT|2,MOV|3,WAIT|2,MOV|2,WAIT|2,MOV|1,WAIT|2,MOV|0, \
RECIPE_END}; //demonstrates movement between all six positions in both directions
char recipe3[3]={MOV|3,RECIPE_END,MOV|2}; //recipe 
char recipe4[6]={MOV|2,LOOP|3,WAIT|2,MOV|5,LOOP,RECIPE_END}; //recipe to demonstrate loop error
char recipe5[5]={MOV|2,LOOP|3,WAIT|2,MOV|6,RECIPE_END}; //recipe to demonstrate command error
char recipe6[4]={MOV|3,WAIT|3,MOV|1,INVALID}; //recipe to demonstare illegal opcode handling
	




int main(void){
	
	char cmd[MAX_CMD_LENGTH] = ""; // Max Command Length Including End String
	char input[2] = "" ; // Array to Read Input (Including End String)
	int idx = 0 ; // Current IDX for String
	
	
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	TIM_Init(TIM2,twenty_ms,1);
	TIM_Init(TIM5,one_hundred_ms,0);
	USART_Write(USART2, (uint8_t *) "Welcome to the UART Machine!\r\n", 32);
	USART_Write(USART2, (uint8_t *) "============================\r\n", 32);
	
	while (1){
		TIM5->SR &= ~TIM_SR_UIF;
		
		if(motor1.error_state==no_error && motor1.paused==0 && motor1.end_recipe==0 && motor1.running==1){
			switch(motor1.recipe[motor1.recipe_index]&opcode_detect){
				case MOV:
					break;
				case WAIT:
					break;
				case LOOP:
					break;
				case END_lOOP:
					break;
				case RECIPE_END:
					break;
				default:
					motor1.error_state=command_error; //see comments in main.h
			}
		}
		//do LED stuff based on motor 1 status
		
		
		if(motor2.error_state==no_error && motor2.paused==0 && motor2.end_recipe==0 && motor2.running==1){
				switch(motor2.recipe[motor2.recipe_index]&opcode_detect){
					case MOV:
						break;
					case WAIT:
						break;
					case LOOP:
						break;
					case END_lOOP:
						break;
					case RECIPE_END:
						break;
					default:
						motor2.error_state=command_error;
			}	
		}
		while(!(TIM5->SR & TIM_SR_UIF)){
			
		}
	}
}
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



