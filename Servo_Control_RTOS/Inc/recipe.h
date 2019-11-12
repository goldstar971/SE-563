#ifndef __recipe_H
#define __recipe_H


// Mnemonic - Opcode
#define INVALID 0x30
#define MOV  0x20
#define WAIT 0x40
#define LOOP 0x80
#define END_lOOP 0xA0
#define RECIPE_END 0x00

// Byte Checker
#define OPCODE_DETECT 0xE0
#define PARAMETER_DETECT 0x1F

#define NUMBER_OF_RECIPES 6


// Recipes
static char recipe1[20]={MOV|0,MOV|5,MOV|0,MOV|3,LOOP|0,MOV|1,MOV|4,END_lOOP,MOV|0, \
MOV|2,WAIT|0,MOV|2,MOV|3,WAIT|31,WAIT|31,WAIT|31,MOV|4}; //demo recipe
static char recipe2[26]={MOV|0,WAIT|2,MOV|1,WAIT|2,MOV|2,WAIT|2,MOV|3,WAIT|2,MOV|4, \
WAIT|2,MOV|5,WAIT|2,MOV|4,WAIT|2,MOV|3,WAIT|2,MOV|2,WAIT|2,MOV|1,WAIT|2,MOV|0, \
RECIPE_END}; //demonstrates movement between all six positions in both directions
static char recipe3[3]={MOV|3,RECIPE_END,MOV|2}; //recipe 
static char recipe4[6]={MOV|2,LOOP|3,WAIT|2,MOV|5,LOOP,RECIPE_END}; //recipe to demonstrate loop error
static char recipe5[5]={MOV|2,LOOP|3,WAIT|2,MOV|6,RECIPE_END}; //recipe to demonstrate command error
static char recipe6[4]={MOV|3,WAIT|3,MOV|1,INVALID}; //recipe to demonstare illegal opcode handling
static char *recipes[NUMBER_OF_RECIPES]={recipe1,recipe2,recipe3,recipe4,recipe5,recipe6}; //array of recipes


#endif
