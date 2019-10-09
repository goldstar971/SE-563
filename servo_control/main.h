#ifndef __main_H
#define __main_H

#define INVALID 0x30
#define MOV  0x20
#define WAIT 0x40
#define LOOP 0x80
#define END_lOOP 0xA0
#define RECIPE_END 0x00
#define opcode_detect 0xE0
#define parameter_detect 0x1F
#define one_hundred_ms 9999
#define twenty_ms 1999
#define NUMBER_OF_RECIPES 6

//needed to differentiate 
#define no_error 0
#define loop_error 1
#define command_error 2
#define parameter_error 3


void switchRecipe(char);



#endif
