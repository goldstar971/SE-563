#ifndef __STM32L476G_DISCOVERY_TIMER_H
#define __STM32L476G_DISCOVERY_TIMER_H
#include "stm32l476xx.h"

void TIM_Init(TIM_TypeDef *timx);
void TIM_GPIO_Init(GPIO_TypeDef *CH1x);

#endif /* __STM32L476G_DISCOVERY_TIMER_H */
