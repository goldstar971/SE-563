#ifndef __PEOPLE__H
#define __PEOPLE__H 
#include "FreeRTOS.h"
#include "semphr.h"

#define MAX_QUEUE_SIZE ( 270 )
#define MAX_CUSTOMERS ( 420 )
#define NUMBER_OF_TELLERS ( 3 )
#define BANK_OPERATION_TIME ( 500000 * 60 * 7 )
#define TIM_CLK_FQ ( 500000 )

typedef struct{
	int seconds;
	int minutes;
	int hours;

} current_time;

typedef enum {idle,busy} teller_state;

typedef struct{
short *transaction_times;
short *teller_idle_times;
teller_state status;
short customers_served;
} teller;


typedef struct{
	teller *tellers;
	short queue;
	char bank_open;
	short max_queue;
	short customers_pulled_out_of_line;
	short *queue_wait_times;
	int start_count;
	int end_count;
	SemaphoreHandle_t block;
}bank;

teller* teller_init(void);
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
int get_total_customers_served(char teller_num);


#endif
