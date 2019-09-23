#include "POST.h"
#include "UART.h"
#include <stdio.h>

// ----------
// Attributes
// ----------

// SysTick Sensitive
volatile int failed;
volatile int hunMS;

// UART
uint8_t buff[BufferSize];
int message;

int measurement = 0;
long long old_count;
long long new_count;
char overflow;

// -------
// Methods
// -------

/**
 * Sets up the device for SysTick. Does not start SysTick.
 */
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

/**
 * POST failed, requests user input to run test again. Prevents the program
 * from continuing without passing. Locks down progam.
 */
void failed_test() {
	char response;
	
	SysTick->CTRL = 0; // Disable SysTick
	Red_LED_On();
	message = sprintf((char *)buff, "    Test Failed!                   \r\n");
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "    Retry POST? (Y/N)              \r\n");
	USART_Write(USART2, buff, message);
	
	while(1) {
		
		response = USART_Read(USART2);
		//if user asks to repeat POST test, turn off red LED and conduct pulse test again
		if(response == 'y' || response == 'Y') {
			message = sprintf((char *)buff, "    Testing again...               \r\n");
			USART_Write(USART2, buff, message);
			message = sprintf((char *)buff, "-----------------------------------\r\n");
			USART_Write(USART2, buff, message);
			Red_LED_Off();
			pulse_test();
			return;
		}
		//if user says they do not wish to repeat test, lock down and await restart
		else if (response == 'n' || response == 'N') {
			message = sprintf((char *)buff, "    Locking down...                \r\n");
			USART_Write(USART2, buff, message);
			message = sprintf((char *)buff, "    Restart device to unlock.      \r\n");
			USART_Write(USART2, buff, message);
			message = sprintf((char *)buff, "-----------------------------------\r\n");
			USART_Write(USART2, buff, message);
			
			while(1) { // Lock Down the Program
			}
		}
		else {
			message = sprintf((char *)buff, "    Invalid character              \r\n");
			USART_Write(USART2, buff, message);
			message = sprintf((char *)buff, "    Please try again.              \r\n\n");
			USART_Write(USART2, buff, message);
		}
	}
}

/**
 * POST was successful, prints results.
 */
void successful_test() {
	SysTick->CTRL = 0; // Disable SysTick
	Green_LED_On(); 
	message = sprintf((char *)buff, "    Test Successful!               \r\n");
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "    Continuing with main program...\r\n");
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "-----------------------------------\r\n\n");
	USART_Write(USART2, buff, message);
}

/**
 * Simulates time for the test. If the time is past the test duration, the test failed.
 * Other methods handle the startup/setup of SysTick.
 */ 
void SysTick_Handler(void) {
	if(hunMS >= TEST_DURATION) {
		failed = 1;
	}
	else {
		hunMS++;
	}
}

/**
 * Tests for a pulse specified in header file. Identical to how measurements are taken in
 * main program.
 */
void pulse_test() {
	hunMS = 0;
	failed = 0;
	SysTick_Initialize(1000000);
	
	
	old_count=TIM2->CCR1; //need to have one rising edge to occur to actually time the pulses
	//if the SYSTIC interrupt has not fired
	while(!failed) {
		if(TIM2->SR&2){
			new_count=TIM2->CCR1; //get value of counter at most recent rising edge 
			measurement=new_count-old_count;
			if(measurement <= MAX_TIME){
				successful_test();
				return;
			}
			old_count=new_count;
		} 
	}
	failed_test();
}

/**
 * Starts a POST. Once started, the test must pass in order to continue with
 * the rest of the program.
 */
void run_power_test() {

	LED_Init();
	
	message = sprintf((char *)buff, "-----------------------------------\r\n");
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "    Power On Self Test             \r\n");
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "-----------------------------------\r\n\n");
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "    Testing Now...                 \r\n");
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "    Looking for a pulse under:     \r\n");
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "    %dMS                           \r\n", MAX_TIME);
	USART_Write(USART2, buff, message);
	message = sprintf((char *)buff, "-----------------------------------\r\n");
	USART_Write(USART2, buff, message);
	
	pulse_test();
}
