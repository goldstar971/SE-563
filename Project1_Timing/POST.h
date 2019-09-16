#include "stm32l476xx.h"
#include "LED.h"

#define MAX_TIME 100
#define TEST_DURATION 500 // Hundred MS
#define max_timer_count 2147483647

void run_power_test(void);
void pulse_test(void);
void successful_test(void);
void failed_test(void);
