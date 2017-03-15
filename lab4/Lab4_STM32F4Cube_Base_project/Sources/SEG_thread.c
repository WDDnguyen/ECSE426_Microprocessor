#include <SEG_thread.h>
#include <stm32f4xx_hal.h>
#include <cmsis_os.h>
#include <gpio.h>
#include <math.h>

void SEG_thread(void const *args);
//Global 

osThreadId SEG_thread_ID;
osThreadDef(SEG_thread, osPriorityNormal, 1 , 0);
TIM_HandleTypeDef TIM3_handle;
int alarm = 0;
extern float temp;
extern int rollValue; 
extern int pitchValue;
extern int angle;
int segmentPosition = 3;
int toggle = 0;
int mode = 0;


void SEG_thread_periph_init(void) {
	
	__HAL_RCC_TIM3_CLK_ENABLE();
	__TIM3_CLK_ENABLE();
	
	//set timer as TIM 3
	TIM3_handle.Instance = TIM3;
	
	//Frequency wanted = frequency of TIM4 / (Prescaler + Period) = 2kHz
	TIM3_handle.Init.Prescaler = (1750/6) -1;
	//counter
	TIM3_handle.Init.Period = 749;
	//count upward 
	TIM3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM3_handle.Init.RepetitionCounter = 0;

	//Start timer 
	HAL_TIM_Base_Init(&TIM3_handle);
	HAL_TIM_Base_Start_IT(&TIM3_handle);
	
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_NVIC_SetPriority(TIM3_IRQn, 3 , 0);
	
}

void start_SEG_thread(void *args) {
	SEG_thread_ID = osThreadCreate(osThread(SEG_thread), args);
}

void SEG_thread(void const *args) {
	
	SEG_thread_periph_init();
	GPIO_Init();

	while(1){
	osSignalWait(0x00000001, osWaitForever);

	int value;
		
			if (temp < 35 || alarm % 20 > 10){
		if (segmentPosition == 3)
		{
		
						
					
				if (mode == 0){
					
					value = temp * 10;
				}else{
					value = angle ;//* 10;
				}
		
	}
		
	if(segmentPosition > 0){
		int out = value % 10;
		value = value / 10;
		displayValue(out,segmentPosition);
		segmentPosition = segmentPosition -1;
		}
		else {
		segmentPosition = 3;
		}
		
			if (temp > 35){
				alarm++;
			}
			else {
			 alarm = 0;
			}
		
		
	}
			else {
				displayValue(-1);
				alarm++;
			}

}
	}