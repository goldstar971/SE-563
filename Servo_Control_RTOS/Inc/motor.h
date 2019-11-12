#ifndef __motor_H
#define __motor_H


// Servo Motor Error Codes
#define NO_ERROR ( 0 )
#define LOOP_ERROR ( 1 )
#define COMMAND_ERROR ( 2 )  


// CCRx Values - Duty Cycle
#define NUMBER_OF_POSITIONS ( 6 )


// Motor to Display LED
#define MOTOR_LED ( 1 )


struct {
	// Motor Specific
	char motor_num; 					// Number of the Motor
	int positions[NUMBER_OF_POSITIONS]; // The Positions for the Motor - Contains Duty Cycles for PWM
	
	// States
	volatile char paused; 				// Boolean - Recipe is not Running
	volatile char error_state; 			// Error Code - Value of 0 Means no Error
	volatile char recipe_index; 		// Idx for the Current Recipe
	volatile char end_recipe; 			// Boolean - Recipe Completed
	volatile char recipe_num; 			// Idx of the Recipe for the Recipe Array
	volatile char motor_position; 		// Idx for the Motor Position
	
	// Loop Related
	volatile char in_loop; 				// Boolean - Motor Currently Executing Loop
	volatile char loop_counter; 		// Number of Itterations for the Current Loop
	volatile char start_of_loop; 		// Idx for the Begining of the Current Loop
} typedef motor_ctrl;


// Methods
void operate_motor(void *parameter);
void motor_Init(motor_ctrl *motor, char motor_num, char motor_position, char recipe_num);
void set_possible_positions(motor_ctrl *motor, int posistions[]);
void switch_recipes(motor_ctrl *motor);
int set_motor_position(motor_ctrl *motor, int new_position, char user_command);
void update_led(motor_ctrl *motor);

// Position Values
static int motor1_positions[NUMBER_OF_POSITIONS]={55,90,125,160,195,230};
static int motor2_positions[NUMBER_OF_POSITIONS]={50,72,94,116,138,160};

#endif
