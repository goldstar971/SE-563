#include "FreeRTOS.h"
#include "people.h"
#include "tim.h"
#include "usart.h"
#include "main.h"
#include "task.h"

extern bank bank_sim;
char data_buffer[100];

//free all dynamic memory that was allocated.
void deinit_bank(void){
		vPortFree((void*)bank_sim.tellers);
}

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

void teller_init(teller* Teller){
	 Teller->status=0;
	 Teller->customers_served=0;
	 return;	
}


//returns current simulated time
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


int get_customers_served(char teller_num){
	return bank_sim.tellers[teller_num].customers_served;
}
	
//timer_count should be the difference between the current_counter_value and the start value
//returns number of seconds elapsed
int convert_cnt_2_seconds(int timer_count){
		return (int)((timer_count/TIM_CLK_FQ)*600.0);//get number of simulated seconds that have elapsed.
}
int convert_seconds_2_cnt(int seconds) {
	return (seconds/600.0) * TIM_CLK_FQ;
}
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
int get_max_queue_depth(void){
		return bank_sim.max_queue;
}
int get_total_customers_served(){
		return bank_sim.customers_pulled_out_of_line;
}





