#include "people.h"
#include "tim.h"
#include "usart.h"
#include "main.h"
#include "FreeRTOS.h"
extern bank Bank;
//initalize bank variable, to be called at the beginning of the queue task
//before while loop.



void init_bank(void){
	Bank.customers_served=0;
	Bank.max_queue=0;
	Bank.queue=0;
	Bank.tellers=(teller*)pvPortMalloc(sizeof(teller)*3);
	//teller init
	for(int i=0;i<3;i++){
		Bank.tellers=teller_init();
		Bank.tellers++;
	}
	Bank.queue_wait_times=(short*)pvPortMalloc(sizeof(short)*MAX_CUSTOMERS);
	Bank.start_count=__HAL_TIM_GetCounter(&htim2);
	Bank.bank_open=1;
}

teller* teller_init(void){
	 teller* Teller=(teller*)pvPortMalloc(sizeof(teller));
	 Teller->status=idle;
	 Teller->number_of_customers_served=0;
	 Teller->teller_idle_times=(short*)pvPortMalloc(sizeof(short)*MAX_CUSTOMERS);
	 Teller->transaction_times=(short*)pvPortMalloc(sizeof(short)*MAX_CUSTOMERS);
	 return Teller;	
}


//returns current simulated time
current_time get_current_time(){
		int elapsed_count=__HAL_TIM_GetCounter(&htim2)-Bank.start_count;
		int elapsed_seconds=convert_cnt_2_seconds(elapsed_count);
	  int hours=elapsed_seconds/3600;
		int minutes=(elapsed_seconds%3600)/60;
		int seconds=(elapsed_seconds%3600)%60;
	  current_time curr_time;
		curr_time.hours=7+hours;
		curr_time.minutes=minutes;
		curr_time.seconds=seconds;
		return curr_time;
}

int get_teller_stats(char teller_num);
int get_customers_served(char teller_num);

//timer_count should be the difference between the current_counter_value and the start value
//returns number of seconds elapsed
int convert_cnt_2_seconds(int timer_count){
		return (timer_count/500000)*600;//get number of simulated seconds that have elapsed.
}
int average_time_in_queue(void);
int average_transaction_time(char teller_num);
int max_transaction_time(char teller_num);
int max_time_queue(void);
int max_teller_idle(char teller_num);
int avg_teller_idle(char teller_num);
int get_max_queue_depth(void);
int get_total_customers_served(char teller_num);
void report_metrics(void);
