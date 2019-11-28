#ifndef __DAC_H
#define __DAC_H
#include "stm32l476xx.h"

typedef struct{
	int freq;
	int sample;
	float vref;
	char wave_type;
} DAC_state;




void init_dac(void);

float sine(char intermediate,int sample);

void cus(void);

void ramp(void);

void triangle(void);

void init_dac_GPIO(void);





#endif
