#include "job.h"
#include "main.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "people.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include "stm32l4xx_hal_rng.h"
#include "stm32l4xx_hal_uart.h"

extern bank bank_sim;
extern RNG_HandleTypeDef hrng;
extern UART_HandleTypeDef huart2;

TaskHandle_t task_handles[4];

// Generates a random number of seconds in simulated time before the next customer. Between 60 and 240 seconds.
int generate_customer_time(void) {
	int *number;
	
	HAL_RNG_GenerateRandomNumber(&hrng, (uint32_t *) number);
	
	return convert_seconds_2_cnt(((*number)%180)+61);
}

// Generates a random number of seconds in simulated time for teller to handle the customer. Between 30 and 480 seconds.
int generate_teller_time(void){
	int *number;
	
	HAL_RNG_GenerateRandomNumber(&hrng, (uint32_t *) number);
	
	return convert_seconds_2_cnt(((*number)%450)+31);
}

// Checks the Status of the Tellers. If all Tellers are idle return 0,
// else return 1. Tellers are either busy or idle.
char check_teller_status(void) {
	for(int i = 0; i <= (NUMBER_OF_TELLERS - 1); i++) {
		if(bank_sim.tellers->status == busy){
			return 1;
		}
	}
	return 0;
}

void print_teller_statistics(int teller) {
}

void print_all_statistics(void) {
}

// Task responsible for generating customers and placing them on the queue. Main Task for program.
// Creates and ends the teller_handle_customers tasks.
void operate_bank(void *parameters) {
	init_bank();
	
	int bufferSize;
	char *buffer;
	
	bufferSize = sprintf((char *)buffer,"\r\nEnter new value for lower limit in the range 50-9950: ");
	HAL_UART_Transmit_IT(&huart2, (uint8_t *) buffer, bufferSize);
	
	int time_for_next_customer = 0;
	int customers_generated=0;
	while(1) {	
		
		// Generate Customers Until Closed
		if(bank_sim.max_queue == 0 || (bank_sim.bank_open && (time_for_next_customer <= __HAL_TIM_GetCounter(&htim2)))) {
			
			xSemaphoreTake(bank_sim.block, portMAX_DELAY );
    
			// Create New Customer
			bank_sim.queue++;
			customers_generated++;
			// Update Max Queue
			if(bank_sim.queue > bank_sim.max_queue) {
				bank_sim.max_queue = bank_sim.queue;
			}
			bank_sim.queue_wait_times[customers_generated]=__HAL_TIM_GetCounter(&htim2);
			xSemaphoreGive(bank_sim.block);
			// Setup Next Customer
			time_for_next_customer = __HAL_TIM_GetCounter(&htim2)+generate_customer_time();
		
		}
		
		// Stop Generating Customers
		if(__HAL_TIM_GetCounter(&htim2) >= bank_sim.end_count) {
			bank_sim.bank_open = 0;
		}
		
		// Is the Program Done
		if( ! (check_teller_status() || bank_sim.bank_open || bank_sim.queue) ) {
			// End Teller Tasks
			for(int i=0;i<3;i++){
				vTaskDelete(task_handles[i]);
			}
			print_all_statistics();
			deinit_bank();
			vTaskDelete(task_handles[3]);
			
			HAL_UART_MspDeInit(&huart2);
			HAL_RNG_MspDeInit(&hrng);
		}
	}
	
	
}

/*queue task
init bank structure (starts timer and opens)
start teller tasks
while(1){
	if(timer>time to generate customer and bank is not closed){
		generate customer (increment queue)
		generate random time for next customer
	}
	print diagnostics for tellers
	if (time to close<time){
		close bank
	}
	if all tellers idle and bank is closed and queue is empty{
		end teller threads
		print out bank statistics
	}
}
*/

// Task responsible for pulling customers off the queue and handling transaction times.
void teller_handle_customers(void *teller_number) {
	teller *worker = &bank_sim.tellers[*((int *) teller_number)];
	
	int transaction_start_time = 0;	// TIM Time - Start of Transaction
	int transaction_end_time;				// value of TIM timer count at end of transaction
	int idle_start_time = __HAL_TIM_GetCounter(&htim2);	// TIM Time - Start of Teller Idle
	
	int idle_idx = 0;
	
	while(1) {
		// Elapsed Time is Current Time Minus The Last Time
		
		// Customer Available and Teller Open
		xSemaphoreTake(bank_sim.block, portMAX_DELAY );
		if(bank_sim.queue && ((worker->status == idle) || __HAL_TIM_GetCounter(&htim2)>= transaction_end_time)) {
			xSemaphoreGive(bank_sim.block);
			// Set New Times
			transaction_start_time = __HAL_TIM_GetCounter(&htim2);
			transaction_end_time = generate_teller_time()+transaction_start_time;
			
			// Calculate Customer Wait Time
			
			// Store Teller Wait Time
			if(worker->status == idle) {
				worker->teller_idle_times[idle_idx] = __HAL_TIM_GetCounter(&htim2) - idle_start_time;
				idle_idx++;
				
				// Overflow - Should Not Get Here
				if(idle_idx >= MAX_CUSTOMERS) {
					idle_idx = 0;
				}
			}
			
			// Update Bank and Teller
			xSemaphoreTake(bank_sim.block, portMAX_DELAY );
			bank_sim.queue--;
			bank_sim.queue_wait_times[bank_sim.customers_pulled_out_of_line]=__HAL_TIM_GetCounter(&htim2)-bank_sim.queue_wait_times[bank_sim.customers_pulled_out_of_line];
			bank_sim.customers_pulled_out_of_line++;
			xSemaphoreGive(bank_sim.block);
			worker->customers_served++;
			worker->status = busy;
		}
		// Worker Finished and Can't Pull from Queue - Idle
		else if( __HAL_TIM_GetCounter(&htim2)>= transaction_end_time && (worker->status == busy) && !bank_sim.queue) {
			xSemaphoreGive(bank_sim.block);
			worker->status = idle;
			idle_start_time = __HAL_TIM_GetCounter(&htim2);
		}
		
	}
}

void create_tasks(void) {
	HAL_RNG_MspDeInit (&hrng);
	HAL_UART_MspInit (&huart2);
	
	
	xTaskCreate(operate_bank, "Bank Simulation", 1000, NULL, 1, task_handles[3]);
	
	for(int i = 0; i <= (NUMBER_OF_TELLERS - 1); i++) {
		xTaskCreate(teller_handle_customers, "Teller", 1000, (void*) i, 1, task_handles[i]);
	}
}

/*
teller task
if (queue is greater than 0 and (teller is idle or time>transaction time)) 
	generate random transaction time
	take customer from queue (decrement it)
	calculate time customer waited in queue and store in array using customers serviced
	increment number of customers serviced
else if time>transcition time and teller is not idle and queue is empty
	record time (for recording lengths of being idle
)
*/
