#ifndef __STM32L476G_DISCOVERY_TIMER_H
#define __STM32L476G_DISCOVERY_TIMER_H
#include "stm32l476xx.h"

#define twenty_ms 1999


void TIM_Init(TIM_TypeDef *TIMx, int ARR_value);
void TIM_GPIO_Init(GPIO_TypeDef *GPIOx);


#endif /* __STM32L476G_DISCOVERY_TIMER_H */
