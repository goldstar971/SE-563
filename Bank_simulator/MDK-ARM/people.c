#include "FreeRTOS.h"
#include "people.h"
#include "tim.h"
#include "usart.h"
#include "main.h"
#include "task.h"

// Global Variables
extern bank bank_sim;
char data_buffer[100];

/***************************************************************************
* Purpose: Frees all the memory for the gloabl bank variable. 
* Input: void
* Output: void
***************************************************************************/
void deinit_bank(void){
		vPortFree((void*)bank_sim.tellers);
}

/***************************************************************************
* Purpose: Initializes and mallocs memory for the global bank variable.
*					 Allocates memory for the teller structs as well.
* Input: void
* Output: void
***************************************************************************/
void init_bank(void){
	bank_sim.customers_pulled_out_of_line=0;
	bank_sim.max_queue=0;
	bank_sim.queue=0;
	bank_sim.tellers=(teller*)pvPortMalloc(sizeof(teller)*NUMBER_OF_TELLERS);
	//teller init
	for(int i=0;i < NUMBER_OF_TELLERS;i++){
		 teller_init(bank_sim.tellers++);
	}
	bank_sim.tellers-=3;
	bank_sim.start_count=TIM2->CNT;
	bank_sim.end_count=bank_sim.start_count + BANK_OPERATION_TIME;
	bank_sim.bank_open=1;
	bank_sim.block=xSemaphoreCreateMutex();	
}

/***************************************************************************
* Purpose: Initializes a teller struct.
* Input: teller* Teller Pointer to a teller struct.
* Output: void
***************************************************************************/
void teller_init(teller* Teller){
	 Teller->status=0;
	 Teller->customers_served=0;
	 return;	
}

/***************************************************************************
* Purpose: Creates the current simulated time using TIM2 and initial 
*					 starting time for the global bank variable. Simulated time is
*					 offset by 9 hours to simulate opening at 9 AM.
* Input: void
* Output: current_time Current time of simulation.
***************************************************************************/
current_time get_current_time(){
		current_time curr_time={0};
		int elapsed_count=TIM2->CNT-bank_sim.start_count;
		int elapsed_seconds=convert_cnt_2_seconds(elapsed_count);
	  int hours=elapsed_seconds/3600;
		int minutes=(elapsed_seconds%3600)/60;
		int seconds=(elapsed_seconds%3600)%60;
	 
		curr_time.hours=9+hours;
		curr_time.minutes=minutes;
		curr_time.seconds=seconds;
		return curr_time;
}

/***************************************************************************
* Purpose: Retrieves the number of customers that a teller has served.
* Input: char teller_num Index of teller in the global bank variable.
* Output: int Number of customers served by the teller.
***************************************************************************/
int get_customers_served(char teller_num){
	return bank_sim.tellers[teller_num].customers_served;
}

/***************************************************************************
* Purpose: Converts the number of timer ticks into seconds of simulated
*					 time.
* Input: int timer_count Number of ticks from timer to convert to seconds.
* Output: int Equivalent number of seconds as per the number of ticks.
***************************************************************************/
int convert_cnt_2_seconds(int timer_count){
		return (int)((timer_count/TIM_CLK_FQ)*600.0);//get number of simulated seconds that have elapsed.
}

/***************************************************************************
* Purpose: Converts the number of seconds in simulated time into number of
*					 timer ticks.
* Input: int seconds Number of seconds in simulated time to convert to
*				 ticks.
* Output: int Equivalent number of timer ticks as per the number of
*					simulated seconds.
***************************************************************************/
int convert_seconds_2_cnt(int seconds) {
	return (seconds/600.0) * TIM_CLK_FQ;
}

/***************************************************************************
* Purpose: Calculates the average time that customers spent in a queue for
*					 the bank global variable.
* Input: void
* Output: current_time Average time customers spent in the queue.
***************************************************************************/
current_time average_time_in_queue(void){
	  int sum_times=0;
		int average_time_sec; 
		current_time average_time={0};
		for(int i=0;i<bank_sim.customers_pulled_out_of_line;i++){
			 sum_times+=bank_sim.queue_wait_times[i];
		}
		//get average queue wait in terms of simulated seconds
		average_time_sec=convert_cnt_2_seconds(sum_times/bank_sim.customers_pulled_out_of_line);
	  average_time.minutes=average_time_sec/60;
		average_time.seconds=average_time_sec%60;
		
		return average_time;
}

/***************************************************************************
* Purpose: Calculates the average transaction time for a specific teller
*					 required to complete a customer's transaction.
* Input: char teller_num Index of the desired teller in the bank global
*				 variable.
* Output: current_time Average time that it took the teller to complete a
*					customer's transaction.
***************************************************************************/
current_time average_transaction_time(char teller_num){
		int sum_times=0;
		int average_time_sec=0; 
		current_time average_time={0};
		for(int i=0;i<bank_sim.tellers[teller_num].customers_served;i++){
			 sum_times+=bank_sim.tellers[teller_num].transaction_times[i];
		}
		//get average transaction time in terms of simulated seconds
		average_time_sec=convert_cnt_2_seconds(sum_times/bank_sim.tellers[teller_num].customers_served);
		average_time.minutes=average_time_sec/60;
		average_time.seconds=average_time_sec%60;
		
		return average_time;
}

/***************************************************************************
* Purpose: Calculates the longest transaction time for a specific teller. 
* Input: char teller_num Index of the desired teller in the bank global
*				 variable.
* Output: current_time Longest transaction time that the teller has
*					completed.
***************************************************************************/
current_time max_transaction_time(char teller_num){
		int max_time=0;
		int max_time_sec=0;
		current_time time={0};
		for(int i=0;i<bank_sim.tellers[teller_num].customers_served;i++){ 
			if(max_time<bank_sim.tellers[teller_num].transaction_times[i]){
				max_time=bank_sim.tellers[teller_num].transaction_times[i];
			}
		}
		//get maximum transaction time in terms of simulated seconds
		max_time_sec=convert_cnt_2_seconds(max_time);
		time.minutes=max_time_sec/60;
		time.seconds=max_time_sec%60;
		
		return time;
}

/***************************************************************************
* Purpose: Calculates the longest wait time that a customer had to spend in
*					 the queue.
* Input: void
* Output: current_time Longest wait time that a customer had to spend in the
*					queue.
***************************************************************************/
current_time max_time_queue(void){
		int max_time=0;
		int max_time_sec=0;
		current_time time={0};
		for(int i=0;i<bank_sim.customers_pulled_out_of_line;i++){
			if(max_time<bank_sim.queue_wait_times[i]){
				max_time=bank_sim.queue_wait_times[i];
			}
		}
		//get max queue time in terms of simulated seconds
		max_time_sec=convert_cnt_2_seconds(max_time);
		time.minutes=max_time_sec/60;
		time.seconds=max_time_sec%60;
		
		return time;
}

/***************************************************************************
* Purpose: Calculates the longest idle time that a specific teller endured. 
* Input: char teller_num Index of the desired teller in the bank global
*				 variable.
* Output: current_time Longest idle time the teller had to endure.
***************************************************************************/
current_time max_teller_idle(char teller_num){
	int max_time=0;
		int max_time_sec=0;
		current_time time={0};
		for(int i=0;i<bank_sim.tellers[teller_num].customers_served;i++){ 
			if(max_time<bank_sim.tellers[teller_num].teller_idle_times[i]){
				max_time=bank_sim.tellers[teller_num].teller_idle_times[i];
			}
		}
		//get max teller idle time in terms of simulated seconds
		max_time_sec=convert_cnt_2_seconds(max_time);
		time.minutes=max_time_sec/60;
		time.seconds=max_time_sec%60;
		
		return time;
}

/***************************************************************************
* Purpose: Calculates the average idle time that a specific teller endured. 
* Input: char teller_num Index of the desired teller in the bank global
*				 variable.
* Output: current_time Average idle time the teller had to endure.
***************************************************************************/
current_time avg_teller_idle(char teller_num){
		int sum_times=0;
		int average_time_sec=0; 
		current_time average_time={0};
		for(int i=0;i<bank_sim.tellers[teller_num].customers_served;i++){
			 sum_times+=bank_sim.tellers[teller_num].teller_idle_times[i];
		}
		//get average teller idle time in terms of simulated seconds
		average_time_sec=convert_cnt_2_seconds(sum_times/bank_sim.tellers[teller_num].customers_served);
	  average_time.minutes=average_time_sec/60;
		average_time.seconds=average_time_sec%60;
		
		return average_time;
}

/***************************************************************************
* Purpose: Retrieves the maximum size of the queue at any point in the day.
* Input: void
* Output: int Largest queue size of the global bank varaible.
***************************************************************************/
int get_max_queue_depth(void){
		return bank_sim.max_queue;
}

/***************************************************************************
* Purpose: Retrieves the total number of people served by the global bank
					 variable.
* Input: void
* Output: int Number of people served by the bank global variable.
***************************************************************************/
int get_total_customers_served(){
		return bank_sim.customers_pulled_out_of_line;
}
