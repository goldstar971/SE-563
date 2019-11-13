#include <string.h>
#include <stdlib.h>
#include "motor.h"
#include "led.h"
#include "recipe.h"
#include "FreeRTOS.h"
#include "task.h"

/************************************************
Purpose: Sets the possible positions for the motor. Each motor has specific
		 Duty Cylces, this helps define the possible Duty Cycles for each
		 motor.
Inputs: *motor: Pointer to the motor_ctrl struct to initialize.
		positions[]: Array of possible values.
Outputs: void.
************************************************/
void set_possible_positions(motor_ctrl *motor, int positions[]) {
	memcpy(motor->positions, positions, sizeof(motor->positions));
}

/************************************************
Purpose: Initializes a Motor struct by setting the default values and the motor
		 specific values. It is expected that the Motor struct has already had
		 the position values set.
Inputs: *motor: Pointer to the motor_ctrl struct to initialize.
		motor_num: char Number of the motor.
		motor_position: char Current position of the motor.
		recipe_num: char Index of the recipe for the motor to execute.
Outputs: void.
************************************************/
void motor_Init(motor_ctrl* motor, char motor_num, char motor_position, char recipe_num) {
	motor->paused = 1;
	motor->error_state = 0;
	motor->in_loop = 0;
	motor->loop_counter = 0;
	motor->start_of_loop = 0;
	motor->recipe_index = 0;
	motor->end_recipe = 0;
	motor->recipe_num = recipe_num;
	motor->motor_position = motor_position;
	motor->motor_num = motor_num;
	
	// Have the Motor go to the Position
	if(motor_num == 1) {
		TIM2->CCR1 = motor->positions[motor_position];	// servo 1 set to move to position zero (1 % duty cycle)
	}
	else if (motor_num == 2) {
		TIM2->CCR2 = motor->positions[motor_position];	// servo 2 set to move to position zero (2% duty cycle)
	}
}

/************************************************
Purpose: Switches the recipe for the motor. Simply itterates to
		 the next recipe in the array, starts at the begining after
		 reaching the end.
Inputs: *motor: Pointer to the motor_ctrl struct to be updated.
Outputs: void.
************************************************/
void switch_recipes(motor_ctrl *motor) {
	
	// Change Recipe
	if(motor->recipe_num >= NUMBER_OF_RECIPES - 1) {
		motor->recipe_num = 0;
	} else {
		motor->recipe_num++;
	}
	
	// Reset Motor Conditions
	motor->paused = 1;
	motor->error_state = 0;
	motor->in_loop = 0;
	motor->loop_counter = 0;
	motor->start_of_loop = 0;
	motor->recipe_index = 0;
	motor->end_recipe = 0;
}

/************************************************
Purpose: Sets position of motor to given position if position is legal.
Inputs: *motor: Pointer to motor_ctrl struct representing the motor to set.
		new_position: int Position to set the motor to (index for Duty Cycle).
		user_command: char Flag for whether or not the command was from the user.
Outputs: int Absolute value of the difference in positions. -1 if error.
************************************************/
int set_motor_position(motor_ctrl *motor, int new_position, char user_command){
	int old_position = motor->motor_position;
	
	// New Position Exists
	if(0 <= new_position && new_position < NUMBER_OF_POSITIONS) {
		motor->motor_position = new_position;
		
		if(motor->motor_num == 1) {
			TIM2->CCR1=motor->positions[new_position];
		}
		else if(motor->motor_num == 2) {
			TIM2->CCR2=motor->positions[new_position];
		}
	}
	else if(user_command == 0) {	// New Position DNE and was a Recipe Step
		motor->error_state = COMMAND_ERROR;
		return -1;
	}
	
	return abs(old_position - new_position);
}

/************************************************
Purpose: Updates the LEDs to represent the current state for a
		 specific motor. The Motor to display is decided in the
		 motor.h file. Will do nothing if not provided the proper
		 Motor.
Inputs: *motor: motor_ctrl Possible Motor to display LEDs for.
Outputs: void.
************************************************/
void update_led(motor_ctrl *motor) {
	
	// Not Correct Motor
	if(motor->motor_num != MOTOR_LED) {
		return;
	}
	
	// Update According to State
	if(motor->paused==0 && motor->error_state==NO_ERROR) {		// Motor is Currently Running
		Green_LED_On();
		Red_LED_Off();
	} else if (motor->paused && motor->error_state==NO_ERROR) {	// Motor is Currently Paused
		Green_LED_Off();
		Red_LED_Off();
	} else if (motor->error_state == COMMAND_ERROR) {			// Motor Experienced Command Error
		Green_LED_Off();
		Red_LED_On();
	} else if (motor->error_state == LOOP_ERROR) {				// Motor Experienced Loop Error
		Green_LED_On();
		Red_LED_On();
	}
}

/************************************************
Purpose: Task method responsible for operating the servo motors. Itterates through
		 recipes and exuctes the provided steps.
Inputs: *parameter: Pointer to a motor_ctrl struct.
Outputs: void.
************************************************/
void operate_motor(void *parameter) {
	motor_ctrl *motor = (motor_ctrl *) parameter;	// Motor to Control
	int change;										// Variable to Hold Change in Position
	char recipe_command;							// Code for the Command
	
	while(1) {
		
		// If (Not Paused) and (No Error) and (End Not Encountered)
		if(motor->error_state == NO_ERROR && motor->paused == 0 && motor->end_recipe == 0){
			recipe_command = recipes[motor->recipe_num][motor->recipe_index];
			switch(recipe_command&OPCODE_DETECT){
				case MOV:	// Move the Motor to Corresponding Position
					change = set_motor_position(motor, recipe_command&PARAMETER_DETECT, 0);
					if(change >= 0) {
						// Wait 150 MS Per Change in Position
						vTaskDelay( pdMS_TO_TICKS( 150 * change ) );
					}
					break;
				case WAIT:	// Wait X Deci Seconds before Executing the Next Command
					
					vTaskDelay( pdMS_TO_TICKS( (recipe_command&PARAMETER_DETECT) * 100 ) );

					break;
				case LOOP:	// Begining of Loop - Times to Itterate
					// Nested Loop - Set Error State
					if(motor->in_loop == 1){
						motor->error_state = LOOP_ERROR;
						break;
					}
					// Set Up Loop
					motor->loop_counter = recipe_command&PARAMETER_DETECT;
					motor->in_loop = 1;
					motor->start_of_loop = motor->recipe_index;
					break;
				case END_lOOP:	// Marks End of Loop
					// Prepare for Next Itteration
					if(motor->loop_counter) {
						motor->loop_counter--;
						motor->recipe_index = motor->start_of_loop;
					}
					else {	// Loop Command Complete - No More Itterations
						motor->in_loop = 0;
						motor->loop_counter = 0;
					}
					break;
				case RECIPE_END:	// End of Recipe
					motor->end_recipe = 1;
					break;
				default:	// Invalid OP Code - Set Error State
					motor->error_state = COMMAND_ERROR;
			}
			
			motor->recipe_index++;
		}
		
		update_led(motor);
		
		vTaskDelay( 100 );
	}
}
