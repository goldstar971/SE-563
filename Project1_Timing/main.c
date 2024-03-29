#include "stm32l476xx.h"
#include "SysClock.h"
#include "UART.h"
#include "timer.h"
#include "POST.h"

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
	int temp_floor=0;
	char new_range[5]={0};
	int measurement_count;
	int bins[101];
	char overflow;
	int measurement=0;
	uint8_t temp;
	System_Clock_Init(); // Switch System Clock = 80 MHz
	UART2_Init();
	TIM_GPIO_Init(GPIOA);
	TIM_Init(TIM2);
	run_power_test();
	
	n=sprintf((char *)buffer,"Lower limit is %d.\r\nUpper limit is %d.\r\n \
	Do you wish to change these values (y/n)?",floor,floor+100);
	USART_Write(USART2,buffer,n);
	answer=USART_Read(USART2);
	//allow for rerun with same limits or changing limits
	while(1){
	//get new limits
		if(answer=='y'){
			for(int i=0;i<5;i++){
				if(i==0){
					n=sprintf((char *)buffer,"\r\nEnter new value for lower limit in the range 50-9950: ");
					USART_Write(USART2,buffer,n);
				}
				//if four digits have been entered or return has been hit
				else if(temp=='\r'||i==4){
					temp_floor=atoi(new_range);
					//if the given value is withn the limits given by the assignment
					if(temp_floor>49 && temp_floor<=9950){
						floor=temp_floor;
						n=sprintf((char *)buffer,"\r\nNew lower limit is: %d",floor);
						USART_Write(USART2,buffer,n);
						break;
					}
					//if not within the limits, restart input
					else{
						n=sprintf((char *)buffer,"\r\nInvalid input. Not within range!");
						USART_Write(USART2,buffer,n);
						i=-1;
						continue;
					}
				}
				temp=USART_Read(USART2);
				//if non-numeric, non enter character given, restart input process
				if((temp<0x30 || temp>0x39)&&(temp!='\r')){ 
					n=sprintf((char *)buffer,"Invalid input!");
					USART_Write(USART2,buffer,n);
					i=-1;
					continue;
				}
				//convert string of ASCII characters into an integer that represents the new lower limit
				
				else{
					new_range[i]=temp;
				}
			}
		}
		temp=0;
		//display prompt and then wait until enter is pushed
		n=sprintf((char *)buffer,"\r\nPress Enter to begin");
		USART_Write(USART2,buffer,n);
		while(temp!='\r'){
			temp=USART_Read(USART2);
		}
		measurement_count=0;
		overflow=0;
		//begin measurements
		TIM2->SR &= ~TIM_SR_UIF; //clear capture event flag
		while (measurement_count<=1000){//get 1000 measurements
			if(TIM2->SR&2){
				//need to have one rising edge to occur to actually time the pulses
				if(measurement_count==0){
					old_count=TIM2->CCR1;
				}
				else{
					//for the unlikely event the counter overflows mid measurement.
					if(old_count>max_timer_count){
						overflow=1;//increment overflow counter
					}
					new_count=TIM2->CCR1; //get value of counter at most recent rising edge 
					measurement=max_timer_count*overflow+new_count-old_count;
					if(measurement>=floor && measurement<=floor+100){
						bins[measurement-floor]++; //increment correct bin
					}
					old_count=new_count;
				}
				measurement_count++;	 
			} 
		}	
		//sort measurements into bins
		for(int i=0;i<101;i++){
			//if bin has non zero count
			if(bins[i]>0){
				n=sprintf((char *)buffer,"%d%s%d\r\n",floor+i,"  ",bins[i]);
				USART_Write(USART2,buffer,n);
			}
			bins[i]=0; //clear array in case we want to run again
		}
		n=sprintf((char *)buffer,"Run again(y/n)?");
		USART_Write(USART2,buffer,n);
		answer=USART_Read(USART2);
		if(answer=='y'){
			n=sprintf((char *)buffer,"\r\nChange limits?(y/n)\r\n");
			USART_Write(USART2,buffer,n);
			answer=USART_Read(USART2); //if answer is y then if clause on line 41   will handle it
		}
		else{
			break;
		}
	}
}

