#ifndef __PEOPLE__H
#define __PEOPLE__H 

#define MAX_QUEUE_SIZE 420

typedef struct{
char served_by_teller;
char time_in_queue

} customer;

typedef struct{
char teller_num;
short service_times[MAX_QUEUE_SIZE];

} teller;


#endif
