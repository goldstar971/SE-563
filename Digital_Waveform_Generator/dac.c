#include <stdlib.h>
#include <math.h>
#include "dac.h"
#include "main.h"


float ramp_values[32]={5.0000000e+00, 4.8387097e+00, 4.6774194e+00, 4.5161290e+00, 4.3548387e+00, 4.1935484e+00, 4.0322581e+00, 3.8709677e+00, 3.7096774e+00, 3.5483871e+00, 
 3.3870968e+00, 3.2258065e+00, 3.0645161e+00, 2.9032258e+00, 2.7419355e+00, 2.5806452e+00, 2.4193548e+00, 2.2580645e+00, 2.0967742e+00, 1.9354839e+00, 
 1.7741935e+00, 1.6129032e+00, 1.4516129e+00, 1.2903226e+00, 1.1290323e+00, 9.6774194e-01, 8.0645161e-01, 6.4516129e-01, 4.8387097e-01, 3.2258065e-01, 
 1.6129032e-01, 0.0000000e+00};

float sine_values[128]={2.622669,2.745042,2.866826,2.987725,3.107450,3.225711,3.342224,3.456708,3.568887,3.678491,
3.785256,3.888925,3.989248,4.085983,4.178897,4.267766,4.352377,4.432526,4.508018,4.578674,
4.644321,4.704803,4.759973,4.809698,4.853860,4.892350,4.925078,4.951963,4.972941,4.987961,
4.996988,5,4.996988,4.987961,4.972941,4.951963,4.925078,4.892350,4.853860,4.809698,4.759973,4.704803,
4.644321,4.578674,4.508018,4.432526,4.352377,4.267766,4.178897,4.085983,3.989248,3.888925,
3.785256,3.678491,3.568887,3.456708,3.342224,3.225711,3.107450,2.987725,2.866826,2.745042,
2.622669,2.500000,2.377330,2.254957,2.133173,2.012274,1.892549,1.774288,1.657775,1.543291,
1.431112,1.321508,1.214743,1.111074,1.010751,0.914016,0.821102,0.732233,0.647622,0.567473,
0.491981,0.421325,0.355678,0.295196,0.240026,0.190301,0.146139,0.107649,0.074921,0.048036,
0.027058,0.012038,0.003011,0,0.003011,0.012038,0.027058,0.048036,0.074921,0.107649,0.146139,
0.190301,0.240026,0.295196,0.355678,0.421325,0.491981,0.567473,0.647622,0.732233,0.821102,
0.914016,1.010751,1.111074,1.214743,1.321508,1.431112,1.543291,1.657775,1.774288,1.892549,
2.012274,2.133173,2.254957,2.377330,2.500000};


extern DAC_state dac_state;

void init_dac(){
	RCC->APB1ENR1	|=RCC_APB1ENR1_DAC1EN; //enable DAC clock
	DAC1->CR|=DAC_CR_EN2;//enable DAC channel 1
}

void init_dac_GPIO(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ; //enable clock for GPIO port A
	GPIOA->MODER &=~0x00000C00; // clear mode register for PA5
	GPIOA->MODER|=0x00000C00; //configure PA5  as analog mode
	GPIOA->PUPDR&=~0x00000C00; //set PA5 as no pullup or pulldown
}
	
float sine(char intermediate,int sample){
  	int normalized_phase=sample%128;
	float quadrant=normalized_phase/(NUM_OF_SAMPLES-1.0f);
	int actual_index=normalized_phase%32;
	float function_value;
	float range=dac_state.vref*DIVS_PER_VOLT;
	//get value of sine function at that point depending on quadrant
	if(quadrant<=.25f){
		function_value=(sine_values[actual_index]);
	}
	else if(quadrant<=.50f){
		function_value=(sine_values[31-actual_index]);
	}
	else if(quadrant<=.75f){
		function_value=(-sine_values[actual_index]);
	}
	else if(quadrant<=1.0f){
		function_value=(-sine_values[31-actual_index]);		
	}
	//if being called from another math function
	if(intermediate==1){
		return function_value;
	}
	else{
		DAC1->DHR12R2=(uint32_t)((function_value+2.5f)*range+.5f);//round up
		return 0;
	}
}

void cus(void){
	float range=dac_state.vref*DIVS_PER_VOLT;
	DAC1->DHR12R2=(uint32_t)((sine(1,dac_state.sample)+sine(1,dac_state.sample*3)/3+
	sine(1,dac_state.sample*5)/5+sine(1,dac_state.sample*7)/7+sine(1,dac_state.sample*9)/9
	+2.5f)*range+.5f); //round up
}

void triangle(void){
	float range=dac_state.vref*DIVS_PER_VOLT;
	if(dac_state.sample%2==0 && dac_state.sample<64){
		DAC1->DHR12R2=ramp_values[dac_state.sample/2]*range;
	}
	else if(dac_state.sample%2==0 && dac_state.sample>=64){
		DAC1->DHR12R2=ramp_values[(127-dac_state.sample)/2]*range;
	}
}

void ramp(void){
	float range=dac_state.vref*DIVS_PER_VOLT;
	if(dac_state.sample%4==0){
		DAC1->DHR12R2=ramp_values[dac_state.sample/4]*range;
	}	
}
