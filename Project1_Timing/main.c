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
	long long old_count;
	long long new_count; //timer is 32-bits and max time for measurements if at most ~10 miliseconds so needs to be able to represent (2^31-1+10050). 
	int floor=950;
	char new_range[5];
	int measurement_count=0;
	int measurements[1000];
	int bins[101];
	char overflow_cnt=0;
	uint8_t temp;

	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	TIM_Init(TIM2);
	n=sprintf((char *)buffer,"Lower limit is %d.\r\nUpper limit is %d\r\n. \
	Do you wish to change these values (y/n)?",floor,floor+100);
	USART_Write(USART2,buffer,n);
	answer=USART_Read(USART2);
	//allow for rerun with same limits or changing limits
	while(1){
	//get new limits
		if(answer=='y'){
				n=sprintf((char *)buffer,"\r\nEnter new value for lower limit in the range 50-9950: ");
				USART_Write(USART2,buffer,n);
			for(int i=0;i<5;i++){
				temp=USART_Read(USART2);
				//if non-numeric, non enter character given, restart input process
				if((temp<0x30 || temp>0x39)&&(temp!=0xd)){  
					n=sprintf((char *)buffer,"Invalid input\r\n");
					USART_Write(USART2,buffer,n);
					i=0;
				}
				//convert string of ASCII characters into an integer that represents the new lower limit
				else if (temp=='\r\n'){
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
		n=sprintf((char *)buffer,"Press Enter to begin\r\n");
		USART_Write(USART2,buffer,n);
		while(temp!='\r\n'){
			temp=USART_Read(USART2);
		}
		measurement_count=0;
		overflow=0;
		//begin measurements
		TIM2->SR = ~TIM_SR_CC1IF //clear capture event flag
		while (measurement_count<1000){
			if(TIM2->SR&2){
				//need to have one rising edge to occur to actually time the pulses
				if(measurement_count==0){old_count=TIM2->CCR1;}
				else{
					//for the unlikely event the counter overflows mid measurement.
					if(old_count>(2^31-1)){
						overflow=1;//increment overflow counter
					}
					new_count=TIM1->CCR1; //get value of counter at most recent rising edge 
					measurements[measurement_count]=(2^31-1)*overflow+new_count-old_count+1; //store inter-pulse arrival time
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
			n=sprintf((char *)buffer,"\r\nChange limits?\r\n");
			USART_Write(USART2,buffer,n);
			answer=USART_Read(USART2); //if answer is y then if clause will handle it
			continue;
		}
		else{
			break;
		}
	}
}

