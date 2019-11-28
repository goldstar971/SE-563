#include <stdlib.h>
#include <math.h>
#include "dac.h"
#include "main.h"


float ramp_values[32]={5.0000000e+00, 4.8387097e+00, 4.6774194e+00, 4.5161290e+00, 4.3548387e+00, 4.1935484e+00, 4.0322581e+00, 3.8709677e+00, 3.7096774e+00, 3.5483871e+00, 
 3.3870968e+00, 3.2258065e+00, 3.0645161e+00, 2.9032258e+00, 2.7419355e+00, 2.5806452e+00, 2.4193548e+00, 2.2580645e+00, 2.0967742e+00, 1.9354839e+00, 
 1.7741935e+00, 1.6129032e+00, 1.4516129e+00, 1.2903226e+00, 1.1290323e+00, 9.6774194e-01, 8.0645161e-01, 6.4516129e-01, 4.8387097e-01, 3.2258065e-01, 
 1.6129032e-01, 0.0000000e+00};

float sine_values[128]={0.0000000e+00, 2.4504280e-01, 3.6682611e-01, 4.8772570e-01, 6.0745033e-01, 7.2571155e-01, 8.4222447e-01, 9.5670839e-01, 
 1.0688875e+00, 1.1784916e+00, 1.2852566e+00, 1.3889253e+00, 1.4892480e+00, 1.5859829e+00, 1.6788971e+00, 1.7677667e+00, 1.8523775e+00, 1.9325258e+00, 
 2.0080185e+00, 2.0786737e+00, 2.1443212e+00, 2.2048029e+00, 2.2599730e+00, 2.3096986e+00, 2.3538599e+00, 2.3923506e+00, 2.4250780e+00, 2.4519631e+00, 
 2.4729412e+00, 2.4879617e+00, 2.4969886e+00, 2.5000000e+00};


extern DAC_state dac_state;

void init_dac(){
	RCC->APB1ENR1	|=RCC_APB1ENR1_DAC1EN; //enable DAC clock
	DAC1->CR|=DAC_CR_EN1;//enable DAC channel 1
}

void init_dac_GPIO(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ; //enable clock for GPIO port A
	GPIOA->MODER &=~0x00000300; // clear mode register for PA4
	GPIOA->MODER|=0x00000300; //configure PA4  as analog mode
	GPIOA->PUPDR&=~0x00000300; //set PA4 as no pullup or pulldown
}
	
float sine(char intermediate,int sample){
  int normalized_phase=sample%128;
	float quadrant=normalized_phase/(NUM_OF_SAMPLES-1.0);
	int actual_index=normalized_phase%32;
	float function_value;
	float range=dac_state.vref*DIVS_PER_VOLT;
	//get value of sine function at that point depending on quadrant
	if(quadrant<=(float).25){
			function_value=(sine_values[actual_index]);
	}
	else if(quadrant<=(float).50){
		function_value=(sine_values[31-actual_index]);
	}
	else if(quadrant<=(float).75){
		function_value=(-sine_values[actual_index]);
	}
	else if(quadrant<=1){
		function_value=(-sine_values[31-actual_index]);		
	}
	//if being called from another math function
	if(intermediate==1){
		return function_value;
	}
	else{
		DAC1->DHR12R1=(uint32_t)((function_value+(float)2.5)*range+(float).5);//round up
		return 0;
	}
}

void cus(void){
	float range=dac_state.vref*DIVS_PER_VOLT;
		DAC1->DHR12R1=(uint32_t)((sine(1,dac_state.sample)+sine(1,dac_state.sample*3)/3+
	sine(1,dac_state.sample*5)/5+sine(1,dac_state.sample*7)/7+sine(1,dac_state.sample*9)/9
	+(float)2.5)*range+(float).5); //round up
}

void triangle(void){
	float range=dac_state.vref*DIVS_PER_VOLT;
	if(dac_state.sample%2==0 && dac_state.sample<64){
		DAC1->DHR12R1=ramp_values[dac_state.sample/2]*range;
	}
	else if(dac_state.sample%2==0 && dac_state.sample>=64){
		DAC1->DHR12R1=ramp_values[(126-dac_state.sample)/2]*range;
	}
}

void ramp(void){
		float range=dac_state.vref*DIVS_PER_VOLT;
	if(dac_state.sample%4==0){
		DAC1->DHR12R1=ramp_values[dac_state.sample/4]*range;
	}	
}
