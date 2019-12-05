#ifndef __DAC_H
#define __DAC_H
#include "stm32l476xx.h"

#define function(array,value,reference) ((uint32_t) array[value]*20475.0/reference)


typedef struct{
	int freq;
	int sample;
	float vref;
	char wave_type;
} DAC_state;




void init_dac(void);

void sine(void);

void cus(void);

void ramp(void);

void triangle(void);

void init_dac_GPIO(void);





#endif
