#include <stdlib.h>
#include "timer.h"
#include "main.h"


/************************************************
Purpose: initalize timers to generate PWM or produce update events at specific time periods 
Inputs: TIMx:the hardware timer to configure, ARR_value: the value the timer is to count up to,
PWM_out: whether or not the timer is for PWM
Outputs: None
************************************************/
void TIM_Init(TIM_TypeDef * TIMx, int ARR_value,char PWM_out){
	

	if(PWM_out){
		RCC->APB1ENR1 |=RCC_APB1ENR1_TIM2EN; //enable timer for timer 2
	}
	else{
		RCC->APB1ENR1 |=RCC_APB1ENR1_TIM5EN; //enable timer 5
	}
	TIMx-> CR1=0x84; //disable timer and set it to count up
	TIMx->CCER &=~0x11;//disable channels 1 and 2
	TIMx-> PSC =799; //set prescale clock to 100khz
	TIMx-> ARR =ARR_value;  // resets after n miliseconds
	if (PWM_out){
		TIMx-> CCMR1=0x6868; // set ch1 and ch2 as outputs 
//		TIMx-> CCR1=pos_values[0]+offset; //	 servo 1 set to move to position zero (1 % duty cycle)
//		TIMx-> CCR2=pos_values[0]+offset; // servo 2 set to move to position zero (2% duty cycle)
		
		TIMx->CCER |=TIM_CCER_CC1E+TIM_CCER_CC2E; //enable timer channel 1 and 2
	}
	TIMx->EGR |=0x1;//create update event, loading prescaler
	TIMx->CR1|=TIM_CR1_CEN; //start timer 
	
}
/************************************************
Purpose: initalize GPIO pins A0, A1 to serve as CH1 and CH2 for TIM2   
Inputs: GPIOx: The PIN bank to initalize
Outputs: None
************************************************/
void TIM_GPIO_Init(GPIO_TypeDef *GPIOx){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ; //enable clock for GPIO port A
	GPIOx->MODER &=~0x0000000f; // clear mode register for PA1 and PA0
	GPIOx->MODER|=0x0000000A; //configure PA0 and PA1  as alternative function
	GPIOx->AFR[0]&=~0x0000011; //clear alternative function register for PA0 and PA1
	GPIOx->AFR[0]|=0x11; //configure PAO and PA1 with alt function  (CHx_timx)	
}
