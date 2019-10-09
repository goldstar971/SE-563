#ifndef __STM32L476G_DISCOVERY_TIMER_H
#define __STM32L476G_DISCOVERY_TIMER_H
#include "stm32l476xx.h"



void TIM_Init(TIM_TypeDef *timx,int Arr_value,char PWM_out);
void TIM_GPIO_Init(GPIO_TypeDef *CH1x);
void move_left(int motor_number);
void move_right(int motor_number);
void set_motor_position(int motor_position, int motor_number);



#endif /* __STM32L476G_DISCOVERY_TIMER_H */
