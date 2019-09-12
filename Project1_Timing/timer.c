#include "timer.h"

void TIM_Init(TIM_TypeDef * TIMx){
	    TIMx-> CR1&=0xf400; //disable timer and set it to count up 
	    TIMx-> CR1|=0x0001; //disable update event  
	    TIMx-> CR2&=0xff7f; //TIMc_CH1 pin is connected to TI1
		TIMx->PSC |=0x4f; //set prescale clock to 1 MHZ
		TIMx->SMCR &=0xfffffff0; //disable slave mode
		TIMx->CCER &=0xfff3;//disable capture and compare, set as input and capture event on rising edge
		TIMx->CCMR1 |=TIM_CCMR1_CC1S_0; //enable capture on tim5 input1
		/*configure capture event generation*/
		TIMx->EGR &=0xffa0;
		TIMx->EGR |=1;
		TIMx->ARR |=0xFFFFFFFF; //counter will count from 0 up until it reaches 2^31-1
		TIMx->CCER |=1; //enable capture event
		
}
