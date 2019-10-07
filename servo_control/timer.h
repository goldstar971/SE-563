#ifndef __STM32L476G_DISCOVERY_TIMER_H
#define __STM32L476G_DISCOVERY_TIMER_H
#include "stm32l476xx.h"






void TIM_Init(TIM_TypeDef *timx,int Arr_value,char PWM_out);
void TIM_GPIO_Init(GPIO_TypeDef *CH1x);
int motor_left(char motor_number);
int  motor_right(char motor_number);
void motor_set_position(int);


#endif /* __STM32L476G_DISCOVERY_TIMER_H */
