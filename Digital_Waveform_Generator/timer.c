#include <stdlib.h>
#include "timer.h"
#include "main.h"
#include "dac.h"

DAC_state dac_state={.freq=100, .sample=0, .vref=5, .wave_type='S'};

/************************************************
Purpose: initalize timers to generate PWM or produce update events at specific time periods 
Inputs: TIMx:the hardware timer to configure, ARR_value: the value the timer is to count up to,
PWM_out: whether or not the timer is for PWM
Outputs: None
************************************************/
void TIM_Init(TIM_TypeDef * TIMx){
	RCC->APB1ENR1 |=RCC_APB1ENR1_TIM2EN; //enable timer for timer 2
	TIMx-> CR1=0x84; //disable timer and set it to count up
	TIMx->CCER &=~0x11;//disable channels 1 and 2
	TIMx->CCER |=TIM_CCER_CC1E; //enable timer channel 1
	TIM2->ARR=8e7/(100*128);
	TIMx->DIER |=1; //update interrupts enabled
	TIMx->EGR |=0x1;//create update event
	TIMx->CR1|=TIM_CR1_CEN; //start timer 
}

void TIM2_IRQHandler(void){
		TIM2->SR&=~TIM_SR_UIF;  //clear update interrupt flag
	//update wave depending on wave type
		switch(dac_state.wave_type){
			case 'S': 
				sine(0,dac_state.sample);
				break;
			case 'T': 
				triangle();
				break;
			case 'R': 
				ramp();
				break;
			case 'A': 
				cus();
				break;
		}
		//increment sample number, roll over at max samples
		dac_state.sample++;
		if(dac_state.sample==NUM_OF_SAMPLES){
			dac_state.sample=0;
		}
}
