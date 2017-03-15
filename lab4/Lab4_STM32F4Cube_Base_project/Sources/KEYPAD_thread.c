#include <KEYPAD_thread.h>
#include <LIS3DSH.h>
#include <stm32f4xx_hal.h>
#include <cmsis_os.h>
#include <KeyPad.h>
#include <arm_math.h> 
#include "gpio.h"

//declare the function 

void KEYPAD_thread(void const *args);

//		Globals 		//
osThreadId KEYPAD_thread_ID;
osThreadDef(KEYPAD_thread, osPriorityNormal, 1,0);
TIM_HandleTypeDef HWTimer;
extern int state;

//variable for numpad
int pressedCounter = 0;
int pressedValue = -5;

// roll and pitch variable initialization
int multiplier  = 100 ;
int current = 0;

int roll = 0;
int pitch = 0;

int position = 1;
// state 0 -> roll Input , state 1 -> pitch input 
int state = 0;
int angle = 0;
 
int rollValue = 0;
int pitchValue = 0;
int keypadValuesSet = 0;

extern float32_t rollAngle;		
extern float32_t pitchAngle;
float32_t gravity = 9.81;

extern int mode;

//function to set up roll and pitch value  after input on KeyPad
void setUpRollValue(int roll){
rollValue = roll;
}

void setUpPitchValue(int pitch){
pitchValue = pitch;
}


void keyPadInput(int input){
if(input != -1 && pressedValue < 0){
			pressedValue = input;
		//State 0 for roll and State 1 for pitch			
	}
	
		if(input == pressedValue){
			if (pressedCounter == 500){
				
						
			if(mode == 0){
				mode = 1;
				}
						
				displayValue(pressedValue,position);
				angle = angle * 10 + pressedValue;
				position = position + 1;

				if (state == 1 && pressedValue != Key_hash && pressedValue != Key_star ){
					current = pitch;
					pitch = 10*pitch + pressedValue;
					if (pitch > 180){
					pitch =0;
					angle = 0;
					//displayValue(4,4);
					}
					
					if (position > 3){
					position = 1;
					}		
				}
				else if ( state == 1 && pressedValue == Key_hash){
					
					state = 0;
					setUpPitchValue(pitch);
					//displayValue(5,4);
					pitch = 0; 
					position = 1;
					mode = 0;
					angle = 0;
				}
				else if ( state == 1 && pressedValue == Key_star){
							pitch = current;
							position -= 2;
							//displayValue(6,4);
					if(position <=0){
					position = 1;
					current = 0;
					pitch = 0;
					state = 0;
					}
				}
				
				else if (state == 0 && pressedValue != Key_hash && pressedValue != Key_star){
					current = roll;
					roll = 10*roll + pressedValue;
					if (roll > 180){
						roll =0;
						angle = 0;
						displayValue(1,4);				
					}				
				}
				else if ( state == 0 && pressedValue == Key_hash ){
					state = 1;
					setUpRollValue(roll);
				//	displayValue(2,4);
					roll = 0; 
					position = 1;
					mode = 2;
					angle = 0;
				}
				
				else if ( state == 0 && pressedValue == Key_star){
					roll = current;
					position -= 2;
					displayValue(3,4);
					if(position <=0){
					position = 1;
					current = 0;
					roll = 0;
					}
				}
										
				pressedCounter = 0;
				pressedValue = -1;		
						
				
			}else{
				pressedCounter++;
			}
		}//RESET COUNTER
		else if (input != -1){ 
			pressedCounter = 0;
			pressedValue = -1;
			
		}
		

}

void controlRollLED(){
float32_t convertedRollValue = rollValue - 90;
			float32_t rollDifference = fabs(rollAngle - convertedRollValue);
			
			if (rollDifference < 5){
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_1, 0);
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_3, 0);
			
			}
			else {
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_1, rollDifference);
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_3, rollDifference);
			}

}

void controlPitchLED(){
	
			float32_t convertPitchValue = pitchValue - 90;
			float32_t pitchDifference = fabs(pitchAngle - convertPitchValue);
			
			 if(pitchDifference < 5){
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_2, 0);
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_4, 0);
			}
			else {
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_2, pitchDifference);
			__HAL_TIM_SetCompare(&HWTimer, TIM_CHANNEL_4, pitchDifference);
			
			}
}

void KEYPAD_thread_periph_init(void) {
	__HAL_RCC_TIM4_CLK_ENABLE();
	//2KHz
	__TIM4_CLK_ENABLE();
	
	//set timer as TIM 4
	HWTimer.Instance = TIM4;
	
	//Frequency wanted = frequency of TIM4 / (Prescaler + Period) = 2kHz
	HWTimer.Init.Prescaler = 420 - 1;
	//counter
	HWTimer.Init.Period = 100;
	//count upward 
	HWTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	HWTimer.Init.RepetitionCounter = 0;

//Setting up PWM	
	TIM_OC_InitTypeDef outputChannelInit;

	//Configure TIM mode to be PWM1  which is fastest 
	outputChannelInit.OCMode = TIM_OCMODE_PWM1;
	//Pulse value to be set in compare register 
	outputChannelInit.Pulse = 100;
	//set up fast mode when using PWM1
	outputChannelInit.OCFastMode = TIM_OCFAST_ENABLE;
	outputChannelInit.OCPolarity = TIM_OCPOLARITY_HIGH;

	//set configuration to each channel of TIM
	HAL_TIM_PWM_ConfigChannel(&HWTimer, &outputChannelInit, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&HWTimer, &outputChannelInit, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&HWTimer, &outputChannelInit, TIM_CHANNEL_3);
	HAL_TIM_PWM_ConfigChannel(&HWTimer, &outputChannelInit, TIM_CHANNEL_4);

	//Start timer 
	HAL_TIM_Base_Init(&HWTimer);
	HAL_TIM_Base_Start_IT(&HWTimer);
	
	//START PWM 
	HAL_TIM_PWM_Start(&HWTimer,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&HWTimer,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&HWTimer,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&HWTimer,TIM_CHANNEL_4);
	
	HAL_NVIC_SetPriority(TIM4_IRQn,0,0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);	
}

void start_KEYPAD_thread(void *args) {
	KEYPAD_thread_ID = osThreadCreate(osThread(KEYPAD_thread), args);
}

void KEYPAD_thread(void const *args) {
	//Initialize Keypad thread
	KEYPAD_thread_periph_init();
	
	//Configure Keypad
	initializeKeyPad();
	// MIGHT NEED TO PUT GPIO INIT HERE 
	while(1){
		osSignalWait(0x00000001, osWaitForever);
		int input = KeyPadGetValue();
		
		keyPadInput(input);
		//if roll/pitch value are set then start checking LED
		if(rollValue > 0) {
			controlRollLED();
		}
		 if (pitchValue > 0 ){
			controlPitchLED();
		} 
	}
}