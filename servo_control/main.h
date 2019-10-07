#ifndef __main_H
#define __main_H

#define INVALID 0x30
#define MOV  0x20
#define WAIT 0x40
#define LOOP 0x80
#define END_lOOP 0xA0
#define RECIPE_END 0x00
#define opcode_detect 0xE0
#define one_hundred_ms 199999
#define twenty_ms 39999


//needed to differentiate 
#define no_error 0
#define loop_error 1
#define command_error 2




struct {
	char running; //whether executing recipe or not.  Initalize to zero at start of main.
	char paused; //whether recipie execution is paused. Initalize to zero at start of main
	char error_state; //whether we've entered an error state.  Initalize to zero at start of main/
	//Error state can be either 0 for no error, 1 if a nested loop error occured, or 2 if an invalid opcode or 
	//an illegal position for the servo motor is given.  These states determine LEDs for motor 1.
	
	char in_loop; //whether we are looping.  Initalize to zero at start of main
	char loop_counter; //how many times remaining to iterate.  Initalize to zero at start of main
	char start_of_loop; //where to go in the recipe for the beginning of the next recipe iteration.  Init to zero at start of main
	char recipe_index; //location in recipe
	char end_recipe; //recipe execution has ended i.e., end_of_recipe command encountered.  Init to zero at start of main
	char *recipe;  //pointer to the char array containing a particular recipe command.  Initialize it to the demo snipit (recipe 1)
	char motor_position; //index of motor position between 0 and 5, already initalized to zero during timer init.
} typedef motor_ctrl;



#endif
