#ifndef __motor_H
#define __motor_H
#include "timer.h"


#define offset 10  // for the specific motors we are demoing, this is the required offset

extern int pos_values[6];


struct {
	char paused; //whether recipe is executing. Initalize to zero at start of main
	char error_state; //whether we've entered an error state.  Initalize to zero at start of main/
	//Error state can be either 0 for no error, 1 if a nested loop error occured, or 2 if an invalid opcode or 
	//an illegal position for the servo motor is given.  These states determine LEDs for motor 1.
	
	char in_loop; //whether we are looping.  Initalize to zero at start of main
	char loop_counter; //how many times remaining to iterate.  Initalize to zero at start of main
	char start_of_loop; //where to go in the recipe for the beginning of the next recipe iteration.  Init to zero at start of main
	char recipe_index; //location in recipe
	char end_recipe; //recipe execution has ended i.e., end_of_recipe command encountered.  Init to zero at start of main
	char recipe_num; // Index in the Recipe List
	char motor_position; //index of motor position between 0 and 5, already initalized to zero during timer init.
} typedef motor_ctrl;


void motor_Init(char motor, char recipe_num, char motor_position);




#endif