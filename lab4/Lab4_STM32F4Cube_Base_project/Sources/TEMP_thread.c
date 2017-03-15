#include <TEMP_thread.h>
#include "stm32f4xx_hal.h"
#include <cmsis_os.h>
#include "adc.h"
#include "gpio.h"
void TEMP_thread(void const *args);
//Global 

osThreadId TEMP_thread_ID;
osThreadDef(TEMP_thread, osPriorityNormal, 1 , 0);
TIM_HandleTypeDef TIM2_handle;
float temp;

void TEMP_thread_periph_init(void) {
	
	__HAL_RCC_TIM2_CLK_ENABLE();
	//100hz
	__TIM4_CLK_ENABLE();
	
	//set timer as TIM 2
	TIM2_handle.Instance = TIM2;
	
	//Frequency wanted = frequency of TIM4 / (Prescaler + Period) = 2kHz
	TIM2_handle.Init.Prescaler = 19999;
	//counter
	TIM2_handle.Init.Period = 500;
	//count upward 
	TIM2_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM2_handle.Init.RepetitionCounter = 0;

	//Start timer 
	HAL_TIM_Base_Init(&TIM2_handle);
	HAL_TIM_Base_Start_IT(&TIM2_handle);
	
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_NVIC_SetPriority(TIM2_IRQn, 2 , 0);
	
}

void start_TEMP_thread(void *args) {
	TEMP_thread_ID = osThreadCreate(osThread(TEMP_thread), args);
}

void TEMP_thread(void const *args) {
	
	TEMP_thread_periph_init();
	ADC_Init();
	
	while(1){
	osSignalWait(0x00000001, osWaitForever);
	temp = ADC_getValue();
	}
}