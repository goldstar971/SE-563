#ifndef __TASKS__H
#define __TASKS__H 
#include "FreeRTOS.h"
#include "semphr.h"
#include "people.h"

#define END_TIME ( 16 )	// 4 PM

void operate_bank(void *parameter);
void teller_handle_customers(void *teller_number);
void create_tasks(void);

#endif
