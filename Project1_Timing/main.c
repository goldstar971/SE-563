#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


char RxComByte = 0;
uint8_t buffer[BufferSize];

int main(void){
	//char rxByte;
	int n;
	char answer;
	int old_count;
	int new_count;
	int floor=950;
	char new_range[5];
	int measurement_count=0;
	int measurements[1000];
	int bins[101];
	uint8_t temp;

	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	TIM_Init(TIM2);
	n=sprintf((char *)buffer,"Lower limit is %d.\nUpper limit is %d\n. \
	Do you wish to change these values (y/n)?",floor,floor+100);
	USART_Write(USART2,buffer,n);
	answer=USART_Read(USART2);
	//allow for rerun with same limits or changing limits
	while(1){
	//get new limits
		if(answer=='y'){
				n=sprintf((char *)buffer,"\nEnter new value for lower limit in the range 50-9950: ");
				USART_Write(USART2,buffer,n);
			for(int i=0;i<5;i++){
				temp=USART_Read(USART2);
				//if non-numeric, non enter character given, restart input process
				if((temp<0x30 || temp>0x39)&&(temp!=0xd)){  
					n=sprintf((char *)buffer,"Invalid input\n");
					USART_Write(USART2,buffer,n);
					i=0;
				}
				//convert string of ASCII characters into an integer that represents the new lower limit
				else if (temp=='\n'){
					floor=atoi(new_range);
					break;
				}
				else{
					new_range[i]=temp;
				}
			}
		}
		temp=0;
		//display prompt and then wait until enter is pushed
		n=sprintf((char *)buffer,"Press Enter to begin\n");
		USART_Write(USART2,buffer,n);
		while(temp!='\n'){
			temp=USART_Read(USART2);
		}
		measurement_count=0;
		old_count=0;
		//begin measurements
		while (measurement_count<1000){
				if((TIM2->SR&1)==1){
					//first value will almost certainly be garbage so discard
					if(measurement_count==0){;}
					else{
						new_count=TIM1->CCR1; //get value of counter at most recent rising edge 
						measurements[measurement_count]=new_count-old_count; //store inter-pulse arrival time
						old_count=new_count;
					}
					measurement_count++;	 
				}
		}
		//sort measurements into bins
		for(int i=0;i<1000;i++){
			//only tally measurements that are within the limits
			if(measurements[i]>=floor && measurements[i]<=floor+100){
				 bins[measurements[i]-floor]++; //increment correct bin
			}
			measurements[i]=0; //clear array in case we want to run again 
		}
		for(int i=0;i<101;i++){
			//if bin has non zero count
			if(bins[i]>0){
				n=sprintf((char *)buffer,"%d  %d",floor+i,bins[i]);
				USART_Write(USART2,buffer,n);
			}
			bins[i]=0; //clear array in case we want to run again
		}
		n=sprintf((char *)buffer,"Run again(y/n)?");
		USART_Write(USART2,buffer,n);
		answer=USART_Read(USART2);
		if(answer=='y'){
			n=sprintf((char *)buffer,"\nChange limits?\n");
			USART_Write(USART2,buffer,n);
			answer=USART_Read(USART2); //if answer is y then if clause will handle it
			continue;
		}
		else{
			break;
		}
	}
}

