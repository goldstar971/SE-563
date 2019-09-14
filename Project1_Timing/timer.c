#include "timer.h"

void TIM_Init(TIM_TypeDef * TIMx){
	
	RCC->APB1ENR1 |=RCC_APB1ENR1_TIM2EN; //enable timer for timer 2

	TIMx-> CR1=0x4; //disable timer 2 and set it to count up 
	TIMx->CCER &=0xfff4;//disable capture and compare, set as input and capture event on rising edge
	TIMx->ARR|=0xFFFFFFFF; //counter will count from 0 up until it reaches 2^31-1
	TIMx-> CR2&=0xff7f; //only TIMc_CH1 pin is connected to TI1
	TIMx-> PSC |=0x4f; //set prescale clock to 1 MHZ
	TIMx->CCER |=TIM_CCER_CC1E; //enable timer channel 1
	TIMx->EGR |=0x1;//create update event, loading prescaler
	TIMx->CR1|=(TIM_CR1_CEN; //start timer 
	
}
