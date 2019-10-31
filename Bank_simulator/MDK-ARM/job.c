#include "job.h"
#include "main.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "people.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "stm32l4xx_hal_rng.h"
int tellers_assigned;
extern bank bank_sim;
static RNG_HandleTypeDef rng;
TaskHandle_t task_handles[4];
uint8_t buffer[100];

void RNG_INIT() {
	HAL_RNG_Init(&rng);
}

// Generates a random number of seconds in simulated time before the next customer. Between 60 and 240 seconds.
int generate_customer_time(void) {
	return convert_seconds_2_cnt(((RNG->DR)%180)+61);
}

// Generates a random number of seconds in simulated time for teller to handle the customer. Between 30 and 480 seconds.
int generate_teller_time(void){
	
	return convert_seconds_2_cnt(((RNG->DR)%450)+31);
}

// Checks the Status of the Tellers. If all Tellers are idle return 0,
// else return 1. Tellers are either busy or idle.
char check_teller_status(void) {
	for(int i = 0; i <= (NUMBER_OF_TELLERS-1); i++) {
		if(bank_sim.tellers->status == 1){
			return 1;
		}
	}
	return 0;
}

void print_teller_statistics() {
	vTaskSuspendAll();	
	current_time Current=get_current_time();
	int n;
    n=sprintf((char*)&buffer,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",0x1b,0x5b,'H',0x1b,0x5b,'1',
	'J',0x1b,0x5b,'3','J',0x1b,0x5b,'2','J');
    HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	n=sprintf((char*)&buffer,"Time is %02d:%02d:%02d\r\n",Current.hours,Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	n=sprintf((char*)&buffer, "Number of Customers waiting in the queue is: %d\r\n",bank_sim.queue);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	for(int i=0;i<NUMBER_OF_TELLERS;i++){
		if(bank_sim.tellers[i].status==1){
			n=sprintf((char*)&buffer,"Teller %d has served %d customers and is currently busy\r\n",i,get_customers_served(i));
		}
		else{
			n=sprintf((char*)&buffer,"Teller %d has served %d customers and is currently idle\r\n",i,get_customers_served(i));
		}
		HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	}
	xTaskResumeAll();
}

void print_all_statistics(void) {
	int n;
	current_time Current;

	//report customers served
	n=sprintf((char*)&buffer,"Total number of customers served: %d\r\n",get_total_customers_served());
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	n=sprintf((char*)&buffer,"Customers served by teller 1 is: %d\r\n",get_customers_served(0));
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	n=sprintf((char*)&buffer,"Customers served by teller 2 is: %d\r\n",get_customers_served(1));
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	n=sprintf((char*)&buffer,"Customers served by teller 3 is: %d\r\n",get_customers_served(2));
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=average_time_in_queue();
	
	//report average time waited in the queue
	n=sprintf((char*)&buffer,"Average time customers waited in queue is: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	
	//report average transaction times
	Current=average_transaction_time(0);
	n=sprintf((char*)&buffer,"Average transaction time for teller 1 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=average_transaction_time(1);
	n=sprintf((char*)&buffer,"Average transaction time for teller 2 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=average_transaction_time(2);
	n=sprintf((char*)&buffer,"Average transaction time for teller 3 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	
	//report teller idle times
	Current=avg_teller_idle(0);
	n=sprintf((char*)&buffer,"Average idle time for teller 1 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=avg_teller_idle(1);
	n=sprintf((char*)&buffer,"Average idle time for teller 2 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=avg_teller_idle(2);
	n=sprintf((char*)&buffer,"Average idle time for teller 3 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	
	//report maximum wait time in queue
	Current=max_time_queue();
	n=sprintf((char*)&buffer,"Maximum customer wait time in queue was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	
	//report maximum idle time for tellers
	Current=max_teller_idle(0);
	n=sprintf((char*)&buffer,"Maximum idle time for teller 1 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=max_teller_idle(1);
	n=sprintf((char*)&buffer,"Maximum idle time for teller 2 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=max_teller_idle(2);
	n=sprintf((char*)&buffer,"Maximum idle time for teller 3 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	
	//report maximum transaction times for tellers
	Current=max_transaction_time(0);
	n=sprintf((char*)&buffer,"Maximum transaction time for teller 1 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=max_transaction_time(1);
	n=sprintf((char*)&buffer,"Maximum transaction time for teller 2 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	Current=max_transaction_time(2);
	n=sprintf((char*)&buffer,"Maximum transaction time for teller 3 was: %d minutes and %d seconds\
		\r\n",Current.minutes,Current.seconds);
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	
	//report maximum queue depth
	
	n=sprintf((char*)&buffer,"Maximum number of customers in the queue was: %d\
		\r\n",get_max_queue_depth());
	HAL_UART_Transmit(&huart2,buffer,n,HAL_MAX_DELAY);
	

	
	
	
}

// Task responsible for generating customers and placing them on the queue. Main Task for program.
// Creates and ends the teller_handle_customers tasks.
void operate_bank(void *parameters) {
	 int time_for_next_customer = 0;
	int customers_generated=0;
	
	
	while(1) {	
		
		// Generate Customers Until Closed
		
		
		if(bank_sim.max_queue == 0 || (bank_sim.bank_open && (time_for_next_customer <= TIM2->CNT))) {
			
			// Update Max Queue
			xSemaphoreTake(bank_sim.block, portMAX_DELAY ); // Added
			if(bank_sim.queue > bank_sim.max_queue) {
				bank_sim.max_queue = bank_sim.queue;
			}
			bank_sim.queue_wait_times[customers_generated]=TIM2->CNT;
			// Create New Customer
			bank_sim.queue++;
			xSemaphoreGive(bank_sim.block); 
			// Setup Next Customer
			time_for_next_customer =TIM2->CNT + generate_customer_time();
			customers_generated++;
		}
		// Stop Generating Customers
		if(TIM2->CNT >= bank_sim.end_count) {
			bank_sim.bank_open = 0;
		}
		
		// Is the Program Done
		if(check_teller_status()==0 && bank_sim.bank_open==0 && bank_sim.queue==0){
			// End Teller Tasks
			print_all_statistics();
			for(int i=0;i<3;i++){
				vTaskDelete(task_handles[i]);
			}
			
			deinit_bank();
			vTaskDelete(task_handles[3]);	
		}
			print_teller_statistics();
	}
	
	
}



// Task responsible for pulling customers off the queue and handling transaction times.
void teller_handle_customers(void *teller_number) {

	xSemaphoreTake(bank_sim.block, portMAX_DELAY );
	 int teller_assigned=tellers_assigned++;
	xSemaphoreGive(bank_sim.block);
	
	int transaction_start_time = 0;	// TIM Time - Start of Transaction
	int transaction_end_time;				// value of TIM timer count at end of transaction
	int transaction_duration;
	int idle_start_time = TIM2->CNT;	// TIM Time - Start of Teller Idle
	int idle_idx = 0;
	

	while(1) {
		
		// Customer Available and Teller Open
		xSemaphoreTake(bank_sim.block, portMAX_DELAY );
		if(bank_sim.queue && (bank_sim.tellers[teller_assigned].status == 0 || TIM2->CNT>= transaction_end_time)) {
			
			// Set New Times
			transaction_start_time = TIM2->CNT;
			transaction_duration=generate_teller_time();
			transaction_end_time = transaction_duration+transaction_start_time;
			bank_sim.tellers[teller_assigned].transaction_times[bank_sim.tellers[teller_assigned].customers_served]=transaction_duration;
			// Calculate Customer Wait Time
			// Store Teller Wait Time
			if(bank_sim.tellers[teller_assigned].status == 0) {
				bank_sim.tellers[teller_assigned].teller_idle_times[idle_idx] = TIM2->CNT - idle_start_time;
				idle_idx++;
			}
			// Update Bank and Teller
			if(bank_sim.tellers[teller_assigned].status){
				bank_sim.queue--;
				bank_sim.queue_wait_times[bank_sim.customers_pulled_out_of_line]=TIM2->CNT-bank_sim.queue_wait_times[bank_sim.customers_pulled_out_of_line];
				bank_sim.customers_pulled_out_of_line++;
				bank_sim.tellers[teller_assigned].customers_served++;
			}
			bank_sim.tellers[teller_assigned].status = 1;
		}
		// Worker Finished and Can't Pull from Queue - Idle
		else if( TIM2->CNT>= transaction_end_time && (bank_sim.tellers[teller_assigned].status == 1) && !bank_sim.queue) {
			bank_sim.tellers[teller_assigned].status = 0;
			idle_start_time = TIM2->CNT;
		}
		xSemaphoreGive(bank_sim.block);
		
	}
}

void create_tasks(void) {
	xTaskCreate(operate_bank, "Bank Simulation", 2000, NULL, 1, task_handles[3]);
	xTaskCreate(teller_handle_customers, "Teller", 2000, NULL, 1, task_handles[0]);
	xTaskCreate(teller_handle_customers, "Teller1", 3000, NULL, 1, task_handles[1]);
	xTaskCreate(teller_handle_customers, "Teller2", 3000, NULL, 1, task_handles[2]);	
	
}

