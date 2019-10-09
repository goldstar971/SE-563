#include "timer.h"
#include "main.h"
#include <stdlib.h>

extern motor_ctrl motor1;
extern motor_ctrl motor2;
int pos_values[6]={800,1440,2080,2720,3360,4000};
void TIM_Init(TIM_TypeDef * TIMx, int ARR_value,char PWM_out){
	

	if(PWM_out){
		RCC->APB1ENR1 |=RCC_APB1ENR1_TIM2EN; //enable timer for timer 2
	}
	else{
		RCC->APB1ENR1 |=RCC_APB1ENR1_TIM5EN; //enable timer 5
	}
	TIMx-> CR1=0x84; //disable timer and set it to count up
	TIMx->CCER &=~0x11;//disable channels 1 and 2
	TIMx-> PSC =0x27; //set prescale clock to 2mhz
	TIMx-> ARR =ARR_value;  // resets after n miliseconds
	if (PWM_out){
		TIMx-> CCMR1=0x6868; // set ch1 and ch2 as outputs 
		TIMx-> CCR1=pos_values[0]; //	 servo 1 set to move to position zero (1 % duty cycle)
		TIMx-> CCR2=pos_values[0]; // servo 2 set to move to position zero (2% duty cycle)
		
		TIMx->CCER |=TIM_CCER_CC1E+TIM_CCER_CC2E; //enable timer channel 1 and 2
	}
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

void move_left(int motor_number){
	char wait_time=0;
	if (motor_number==1){
		if (motor1.motor_position==0 || motor1.paused==0){;
		}
		else{
			TIM2->CCR1=pos_values[--motor1.motor_position];
			while(wait_time<3){
				if(TIM5->SR & TIM_SR_UIF){
					wait_time++;
				}
			}
		}
	}
	else{
		if (motor2.motor_position==0 || motor2.paused==0){;
		}
		else{
			TIM2->CCR2=pos_values[--motor2.motor_position];

			while(wait_time<3){
				if(TIM5->SR & TIM_SR_UIF){
					wait_time++;
				}
			}

		}
	}
}

void move_right(int motor_number){
	char wait_time=0;
	if (motor_number==1){
		if (motor1.motor_position==5 || motor1.paused==0){;
		}
		else{
			TIM2->CCR1=pos_values[++motor1.motor_position];
			while(wait_time<3){
				if(TIM5->SR & TIM_SR_UIF){
					wait_time++;
				}
			}	
		}
	}
	else{
		if (motor2.motor_position==5 || motor2.paused==0){;
		}
		else{
			TIM2->CCR2=pos_values[++motor2.motor_position];
			while(wait_time<3){
				if(TIM5->SR & TIM_SR_UIF){
					wait_time++;
				}
			}
		}
	}
}

void set_motor_position(int pos, int motor_number){
	char wait_time=0;
	char last_pos=motor1.motor_position;
	if(motor_number==1&& 0<=pos && pos<=5){
		TIM2->CCR1=pos_values[pos];
		motor1.motor_position=pos;
		while(wait_time<(2*abs(pos-last_pos)+1)){
			if(TIM5->SR & TIM_SR_UIF){
				wait_time++;
			}
		}
		
	}
	else if(motor_number==2 && 0<=pos && pos<=5){
		TIM2->CCR2=pos_values[pos];
		motor2.motor_position=pos;
		while(wait_time<(2*abs(pos-last_pos)+1)){
			if(TIM5->SR & TIM_SR_UIF){
				wait_time++;
			}
		}
	}
	else if(motor_number==1){
		motor1.error_state=command_error;
		return;
	}
	else if(motor_number==2){
		motor2.error_state=command_error;
		return;
	}
}
