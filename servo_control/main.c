#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include "main.h"

#define MAX_CMD_LENGTH 11

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
char *recipes[6]={recipe1,recipe2,recipe3,recipe4,recipe5,recipe6};

void switchRecipe(char motor) {
	if(motor == 1) {
		if(motor1.recipe_num >= NUMBER_OF_RECIPES) {
			motor1.recipe_num = 0;
		} else {
			motor1.recipe_num++;
		}
		motor1.running = 0;
		motor1.paused = 0;
		motor1.error_state = 0;
		motor1.in_loop = 0;
		motor1.loop_counter = 0;
		motor1.start_of_loop = 0;
		motor1.recipe_index = 0;
		motor1.end_recipe = 0;
	} else if (motor == 2) {
		if(motor2.recipe_num >= NUMBER_OF_RECIPES) {
			motor2.recipe_num = 0;
		} else {
			motor2.recipe_num++;
		}
		motor2.running = 0;
		motor2.paused = 0;
		motor2.error_state = 0;
		motor2.in_loop = 0;
		motor2.loop_counter = 0;
		motor2.start_of_loop = 0;
		motor2.recipe_index = 0;
		motor2.end_recipe = 0;
	}
}

void motor_Init(char motor, char recipe_num, char motor_position) {
	char wait_time=11;
	if(motor == 1) {
		motor1.running = 0;
		motor1.paused = 0;
		motor1.error_state = 0;
		motor1.in_loop = 0;
		motor1.loop_counter = 0;
		motor1.start_of_loop = 0;
		motor1.recipe_index = 0;
		motor1.end_recipe = 0;
		motor1.recipe_num = recipe_num;
		motor1.motor_position = motor_position;
		Green_LED_Off();
		Red_LED_Off();
		//allow motors time to reset.
		while(wait_time>0){
					if(TIM5->SR & TIM_SR_UIF){
						wait_time--;
					}
		}
	} else if (motor == 2) {
		motor2.running = 0;
		motor2.paused = 0;
		motor2.error_state = 0;
		motor2.in_loop = 0;
		motor2.loop_counter = 0;
		motor2.start_of_loop = 0;
		motor2.recipe_index = 0;
		motor2.end_recipe = 0;
		motor2.recipe_num = recipe_num;
		motor2.motor_position = motor_position;

		//allow motors time to reset.
		while(wait_time>0){
					if(TIM5->SR & TIM_SR_UIF){
						wait_time--;
					}
		}
	}		
}

int main(void){
	
	// Initialize Configurations
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	TIM_GPIO_Init(GPIOA);
	TIM_Init(TIM2,twenty_ms,1);
	TIM_Init(TIM5,one_hundred_ms,0);
	
	motor_Init(1, 0, 0);
	motor_Init(2, 0, 0);
	
	USART_Write(USART2, (uint8_t *) "Welcome to Servo Control!\r\n", 29);
	USART_Write(USART2, (uint8_t *) "=========================\r\n", 29);
	char wait1=0,wait2=0;
	char recipe_command;
	// Execute Recipes
	while (1){
		TIM5->SR &= ~TIM_SR_UIF;
	
		// Motor 1
		if(wait1==0){
			if(motor1.error_state==no_error && motor1.paused==0 && motor1.end_recipe==0 && motor1.running==1){
				recipe_command=recipes[motor1.recipe_num][motor1.recipe_index];
				switch(recipe_command&opcode_detect){
					case MOV:					// Move the Motor to Corresponding Position
						set_motor_position(recipe_command&0xf,1);
						break;
					case WAIT:				// Delay the Next Recipe Command
						wait1=recipe_command&parameter_detect;
						// TODO
						break;
					case LOOP:				// Begining of Loop - Times to Itterate
						if(motor1.in_loop==1){
							motor1.error_state=loop_error;
							break;
						}
						motor1.loop_counter = recipe_command&parameter_detect;
						motor1.in_loop = 1;
						motor1.start_of_loop = motor1.recipe_index;
						break;
					case END_lOOP:		// Marks End of Loop
						if(motor1.loop_counter) {
							motor1.loop_counter--;
							motor1.recipe_index = motor1.start_of_loop;
						} else {
							motor1.in_loop = 0;
							motor1.loop_counter = 0;
						}
						break;
					case RECIPE_END:	// End of Recipe
						motor1.end_recipe = 1;
						motor1.paused = 0;
						motor1.running = 0;
						break;
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
		if(motor1.running && motor1.error_state==0) {															// Motor is Currently Running
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
			if(motor2.error_state==no_error && motor2.paused==0 && motor2.end_recipe==0 && motor2.running==1){
				recipe_command=recipes[motor2.recipe_num][motor2.recipe_index];
				switch(recipe_command&opcode_detect){
					case MOV:					// Move the Motor to Corresponding Position
						set_motor_position(recipe_command&0xf,2);
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
						motor2.running = 0;
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
