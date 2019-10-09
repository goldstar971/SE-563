#include "motor.h"
#include "timer.h"
#include "main.h"
#include "led.h"

extern motor_ctrl motor1;
extern motor_ctrl motor2;
int pos_values[6]={40,72,104,136,168,200};

//initalize motors
void motor_Init(char motor, char recipe_num, char motor_position) {
	char wait_time=11;
	if(motor == 1) {
		motor1.paused = 1;
		motor1.error_state = 0;
		motor1.in_loop = 0;
		motor1.loop_counter = 0;
		motor1.start_of_loop = 0;
		motor1.recipe_index = 0;
		motor1.end_recipe = 0;
		motor1.recipe_num = recipe_num;
		motor1.motor_position = motor_position;
		Green_LED_Off();
		Red_LED_Off();
		//allow motors time to reset.
		while(wait_time>0){
					if(TIM5->SR & TIM_SR_UIF){
						wait_time--;
					}
		}
	} else if (motor == 2) {
		motor2.paused = 1;
		motor2.error_state = 0;
		motor2.in_loop = 0;
		motor2.loop_counter = 0;
		motor2.start_of_loop = 0;
		motor2.recipe_index = 0;
		motor2.end_recipe = 0;
		motor2.recipe_num = recipe_num;
		motor2.motor_position = motor_position;

		//allow motors time to reset.
		while(wait_time>0){
					if(TIM5->SR & TIM_SR_UIF){
						wait_time--;
					}
		}
	}		
}