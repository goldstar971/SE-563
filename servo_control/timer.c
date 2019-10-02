#include "timer.h"

void TIM_Init(TIM_TypeDef * TIMx){
	
	RCC->APB1ENR1 |=RCC_APB1ENR1_TIM2EN; //enable timer for timer 2

	TIMx-> CR1=0x4; //disable timer 2 and set it to count up 
	TIMx->CCER &=0xfff4;//disable capture and compare, set as input and capture event on rising edge
	TIMx->CCMR1 |= TIM_CCMR1_CC1S_0 ; //set compare input on timer ch1
	TIMx->ARR|=0xFFFFFFFF; //counter will count from 0 up until it reaches 2^31-1
	TIMx->PSC |=0x4f; //set prescale clock to 1 MHZ
	TIMx->CCER |=TIM_CCER_CC1E; //enable timer channel 1
	TIMx->EGR |=0x1;//create update event, loading prescaler
	TIMx->CR1|=TIM_CR1_CEN; //start timer 
	
}

void TIM_GPIO_Init(GPIO_TypeDef *GPIOx){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ; //enable clock for GPIO port A
	GPIOx->MODER &=0xFFFFFFFC; // clear mode register for PA0
	GPIOx->MODER|=0x2; //configure PA0 (pin23) mode as alternative function
	GPIOx->AFR[0]&=0xFFFFFFF0; //clear alternative function register for PAO
	GPIOx->AFR[0]|=0x1; //configure PAO with alt function 1 (CH1_tim2)	
}
