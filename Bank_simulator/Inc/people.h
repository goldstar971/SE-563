#ifndef __PEOPLE__H
#define __PEOPLE__H 
#include "FreeRTOS.h"
#include "semphr.h"

#define MAX_QUEUE_SIZE ( 270 )
#define MAX_CUSTOMERS ( 420 )
#define NUMBER_OF_TELLERS ( 3 )
#define BANK_OPERATION_TIME ( 500000*42)
#define TIM_CLK_FQ ( 500000.0 )

typedef volatile struct{
	int seconds;
	int minutes;
	int hours;

} current_time;



typedef volatile struct{
	int transaction_times[MAX_CUSTOMERS];
	int teller_idle_times[MAX_CUSTOMERS];
	char status;
	short customers_served;
} teller;


typedef volatile struct{
	teller *tellers;
	short queue;
	char bank_open;
	short max_queue;
	int customers_pulled_out_of_line;
	int queue_wait_times[MAX_CUSTOMERS];
	int start_count;
	int end_count;
	SemaphoreHandle_t block;
}bank;

void teller_init(teller *Teller);
void init_bank(void);
void deinit_bank(void);
current_time get_current_time(void);
int get_customers_served(char teller_num);
int convert_cnt_2_seconds(int timer_count);
int convert_seconds_2_cnt(int seconds);
current_time average_time_in_queue(void);
current_time average_transaction_time(char teller_num);
current_time max_transaction_time(char teller_num);
current_time max_time_queue(void);
current_time max_teller_idle(char teller_num);
current_time avg_teller_idle(char teller_num);
int get_max_queue_depth(void);
int get_total_customers_served(void);


#endif
