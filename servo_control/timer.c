#include "timer.h"
int pos_values[6]={800,1440,2080,2720,3360,4000};
void TIM_Init(TIM_TypeDef * TIMx){
	
	RCC->APB1ENR1 |=RCC_APB1ENR1_TIM2EN; //enable timer for timer 2

	TIMx-> CR1=0x84; //disable timer 2 and set it to count up 
	TIMx->CCER &=~0x11;//disable channels 1 and 2
	TIMx-> PSC =0x27; //set prescale clock to 2mhz
	TIMx-> ARR =0x9c3f;  // resets after 20 miliseconds
  TIMx-> CCMR1=0x6868; // set ch1 and ch2 as outputs 
  TIMx-> CCR1=pos_values[0]; //	 servo 1 set to move to position zero (1 % duty cycle)
  TIMx-> CCR2=pos_values[0]; // servo 2 set to move to position zero (2% duty cycle)
	TIMx->CCER |=TIM_CCER_CC1E+TIM_CCER_CC2E; //enable timer channel 1 and 2
	TIMx->EGR |=0x1;//create update event, loading prescaler
	TIMx->CR1|=TIM_CR1_CEN; //start timer 
	
}

void TIM_GPIO_Init(GPIO_TypeDef *GPIOx){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ; //enable clock for GPIO port A
	GPIOx->MODER &=~0x0000000f; // clear mode register for PA1 and PA0
	GPIOx->MODER|=0x0000000A; //configure PA0 and PA1  as alternative function
	GPIOx->AFR[0]&=~0x0000011; //clear alternative function register for PA0 and PA1
	GPIOx->AFR[0]|=0x11; //configure PAO and PA1 with alt function  (CHx_timx)	
}

void SysTick_Initialize(uint32_t ticks) {
 SysTick->CTRL = 0; // Disable SysTick
 SysTick->LOAD = ticks - 1; // Set reload register
 // Set interrupt priority of SysTick to least urgency (i.e., largest priority value)
 NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
 SysTick->VAL = 0; // Reset the SysTick counter value
 // Select processor clock: 1 = processor clock; 0 = external clock
 SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
 // Enables SysTick interrupt, 1 = Enable, 0 = Disable
 SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
 // Enable SysTick
 SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}