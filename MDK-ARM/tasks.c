/*queue task
init bank structure (starts timer and opens)
start teller tasks
while(1){
	if(timer>time to generate customer and bank is not closed){
		generate customer (increment queue)
		generate random time for next customer
	}
	print diagnostics for tellers
	if (time to close<time){
		close bank
	}
	if all tellers idle and bank is closed and queue is empty{
		end teller threads
		print out bank statistics
	}
}
*/

/*
teller task
if (queue is greater than 0 and (teller is idle or time>transaction time)) 
	generate random transaction time
	take customer from queue (decrement it)
	calculate time customer waited in queue and store in array using customers serviced
	increment number of customers serviced
else if time>transcition time and teller is not idle and queue is empty
	record time (for recording lengths of being idle
	

)
	








*/