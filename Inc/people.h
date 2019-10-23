#ifndef __PEOPLE__H
#define __PEOPLE__H 

#define MAX_QUEUE_SIZE ( 270 )
#define MAX_CUSTOMERS ( 420 )

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
	short customers_served;
	short *queue_wait_times;
	int start_count;
	int end_count;

}bank;

teller* teller_init(void);
void init_bank();
current_time get_current_time(void);
int get_customers_served(char teller_num);
int convert_cnt_2_seconds(int timer_count);
current_time average_time_in_queue(void);
current_time average_transaction_time(char teller_num);
current_time max_transaction_time(char teller_num);
current_time max_time_queue(void);
current_time max_teller_idle(char teller_num);
current_time avg_teller_idle(char teller_num);
int get_max_queue_depth(void);
int get_total_customers_served(char teller_num);
int get_teller_stats(char teller_num);
void report_metrics(void);

#endif