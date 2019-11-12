#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "motor.h"

#define MAX_CMD_LENGTH 11

/************************************************
Purpose: Switch recipe for a motor
Inputs: motor: the number of the motor whose recipie is to be switched
Outputs: None
************************************************/





//array of recipies
char *recipes[6]={recipe1,recipe2,recipe3,recipe4,recipe5,recipe6};


int main(void){
	
	// Initialize all peripherals
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	TIM_GPIO_Init(GPIOA);
	//init PWM timer
	TIM_Init(TIM2,twenty_ms,1);
	//init recipe timer
	TIM_Init(TIM5,one_hundred_ms,0);
	
	motor_Init(1, 0, 0);
	motor_Init(2, 0, 0);
	
	//print start message to terminal
	USART_Write(USART2, (uint8_t *) "Welcome to Servo Control!\r\n", 27);
	USART_Write(USART2, (uint8_t *) "=========================\r\n>", 28);
	char wait1=0,wait2=0;
	char recipe_command;
	// Execute Recipes
	while (1){
		TIM5->SR &= ~TIM_SR_UIF;
	
		// Motor 1
		//if not delaying
		if(wait1==0){
			//if not paused or read an end of recipe command or in an error state.
			if(motor1.error_state==no_error && motor1.paused==0 && motor1.end_recipe==0){
				//read recipe command
				recipe_command=recipes[motor1.recipe_num][motor1.recipe_index];
				//get opcode
				switch(recipe_command&opcode_detect){
					case MOV:					// Move the Motor to Corresponding Position
						set_motor_position(recipe_command&0xf,1,0);
						break;
					case WAIT:				// Delay the Next Recipe Command
						wait1=recipe_command&parameter_detect;
						// TODO
						break;
					case LOOP:				// Begining of Loop - Times to Itterate
						//if in loop already, throw error
						if(motor1.in_loop==1){
							motor1.error_state=loop_error;
							break;
						}
						//get number of iterations needed
						motor1.loop_counter = recipe_command&parameter_detect;
						motor1.in_loop = 1;
						motor1.start_of_loop = motor1.recipe_index;
						break;
					case END_lOOP:		// Marks End of Loop
						//if number of iterations is not zero
						if(motor1.loop_counter) {
							motor1.loop_counter--;
							motor1.recipe_index = motor1.start_of_loop;
						}
							//if loop counter is 0, then no more iterations 
						else {
							motor1.in_loop = 0;
							motor1.loop_counter = 0;
						}
						break;
					case RECIPE_END:	// End of Recipe
						motor1.end_recipe = 1;
						break;
					//if not a valid opcode, set error state
					default:
						motor1.error_state=command_error; //see comments in main.h
				}
				motor1.recipe_index++;
			}
		}
		else{
				--wait1;
		}
			
		//do LED stuff based on motor 1 status
		if(motor1.paused==0 && motor1.error_state==0) {															// Motor is Currently Running
			Green_LED_On();
		} else if (motor1.paused && motor1.error_state==0) {												// Motor is Currently Paused
			Green_LED_Off();
			Red_LED_Off();
		} else if (motor1.error_state == command_error) {	// Motor Experienced Command Error
			Green_LED_Off();
			Red_LED_On();
		} else if (motor1.error_state == loop_error) {		// Motor Experienced Loop Error
			Green_LED_On();
			Red_LED_On();
		}
		
		
		// Motor 2
		if(wait2==0){
			if(motor2.error_state==no_error && motor2.paused==0 && motor2.end_recipe==0){
				recipe_command=recipes[motor2.recipe_num][motor2.recipe_index];
				switch(recipe_command&opcode_detect){
					case MOV:					// Move the Motor to Corresponding Position
						set_motor_position(recipe_command&0xf,2,0);
						break;
					case WAIT:				// Delay the Next Recipe Command
						wait2=recipe_command&parameter_detect;
						// TODO
						break;
					case LOOP:				// Begining of Loop - Times to Itterate
						if(motor2.in_loop==1){
							motor2.error_state=loop_error;
							break;
						}
						motor2.loop_counter = recipe_command&parameter_detect;
						motor2.in_loop = 1;
						motor2.start_of_loop = motor2.recipe_index;
						break;
					case END_lOOP:		// Marks End of Loop
						if(motor2.loop_counter) {
							motor2.loop_counter--;
							motor2.recipe_index = motor2.start_of_loop;
						} else {
							motor2.in_loop = 0;
							motor2.loop_counter = 0;
						}
						break;
					case RECIPE_END:	// End of Recipe
						motor2.end_recipe = 1;
						motor2.paused = 0;
						break;
					default:
						motor2.error_state=command_error; //see comments in main.h
				}
				motor2.recipe_index++;
			}
		}
		else{
				--wait2;
		}
		while(!(TIM5->SR & TIM_SR_UIF)){
		}
	}
}
