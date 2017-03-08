/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "arm_math.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

extern volatile int rollValue;
extern volatile int pitchValue;
extern volatile int keypadValuesSet;

void GPIO_Init(void){
	
	GPIO_InitTypeDef GPIO_INIT; //create the configuration information for the GPIO
	
	__HAL_RCC_GPIOD_CLK_ENABLE();	//Enable clock for Port D pins (LEDs)					 																	
	__HAL_RCC_GPIOE_CLK_ENABLE(); //Enable clock for PORT E pins (7 Segment display) 
	
	
	// 7-segment display 
	
	//7-segment display
	GPIO_INIT.Pin = SEG7_A | SEG7_B | SEG7_C | SEG7_D | SEG7_E | SEG7_F | SEG7_G | SEG7_DP | SEG7_OUT1 | SEG7_OUT2 | SEG7_OUT3 | SEG7_OUT4 ; 	// initialize the 7-segment display for the LEDs pin
	GPIO_INIT.Mode = GPIO_MODE_OUTPUT_PP; //set pins to output push pull mode
	GPIO_INIT.Pull = GPIO_NOPULL; // no pull for 7-segment pins
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM; //Speed of pin
	
	HAL_GPIO_Init(GPIOE, &GPIO_INIT); // Initiate the pins with the setting from GPIO_INIT
	// when initialize, set all the pins to LOW
	HAL_GPIO_WritePin(GPIOE, SEG7_A | SEG7_B | SEG7_C | SEG7_D | SEG7_E | SEG7_F | SEG7_G | SEG7_DP | SEG7_OUT1 | SEG7_OUT2 | SEG7_OUT3 | SEG7_OUT4, GPIO_PIN_RESET); 

	// FOR LEDs
	GPIO_INIT.Pin = LED1 | LED2 | LED3 | LED4; // initialize the pins for the LEDs pin
	GPIO_INIT.Mode = GPIO_MODE_OUTPUT_PP; // Set pins to output push pull mode 
	GPIO_INIT.Pull = GPIO_NOPULL; // No pull for LEDs pins
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM;	//Speed of pin
	
	HAL_GPIO_Init(GPIOD, &GPIO_INIT);	//Initiate the pins with the settings from GPIO_INIT.
	HAL_GPIO_WritePin(GPIOD, LED1 | LED2 | LED3 | LED4 , GPIO_PIN_RESET); // when initialize, set all the pins to LOW

}

int rollLED(float32_t rollDifference){
	
	if (rollDifference < 5){
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_RESET);
		rollValue = 0;
		return 100;
	}	
	
	else if (rollDifference < 25){
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_SET);
		return 25;
	}
	
	else if (rollDifference < 45){
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_SET);
		return 50;
	}
	
	else if (rollDifference < 65){
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_SET);
		return 75;
	}
	
	else{
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_SET);		
	  return 100;
	}
	
	return 100;
}

int pitchLED(float32_t pitchDifference){
	if (pitchDifference < 5){
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_RESET);
		pitchValue = 0;
		keypadValuesSet = 0;
		return 100;
	}	
	
	else if (pitchDifference < 25){
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_SET);
		return 25;
	}
	
	else if (pitchDifference < 45){
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_SET);
		return 50;
	}
	
	else if (pitchDifference < 65){
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_SET);
		return 75;
	}
	
	else{
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_SET);		
	  return 100;
	}
	
	return 100;
}

void displayValue(int number, int position){
	// set a specific value to set all the segment pins to low 
	if (number == -1){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_RESET);
	}
	
	else if (number == 1){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_RESET);
	}
	
	else if (number == 2){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_SET);	
	}
	
	else if (number == 3){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_SET);		
	}
	
	else if (number == 4){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_SET);
	}
	
	else if (number == 5){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_SET);	
	}
	
	else if (number == 6){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_SET);		
	}
	
	else if (number == 7){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_RESET);		
	}
	
	else if (number == 8){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_SET);
	}
	
	else if (number == 9){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_SET);	
	}
	
	else if (number == 0){
			HAL_GPIO_WritePin(GPIOE, SEG7_A , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_B , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_C , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_D , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_E , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_F , GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, SEG7_G , GPIO_PIN_RESET);
			
	}
	
	// Set which display to display the number.
	if (position == -1){
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT1 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT2 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT3 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT4 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_DP , GPIO_PIN_RESET);
	}
	
	else if (position == 1){
	  HAL_GPIO_WritePin(GPIOE, SEG7_OUT1 , GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT2 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT3 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT4 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_DP , GPIO_PIN_RESET);
	}
	
	else if (position == 2){
	  HAL_GPIO_WritePin(GPIOE, SEG7_OUT1 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT2 , GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT3 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT4 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_DP , GPIO_PIN_RESET);
	}
	
	else if (position == 3){
	  HAL_GPIO_WritePin(GPIOE, SEG7_OUT1 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT2 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT3 , GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT4 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_DP , GPIO_PIN_RESET);
	}
	
	else if (position == 4){
	  HAL_GPIO_WritePin(GPIOE, SEG7_OUT1 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT2 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT3 , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, SEG7_OUT4 , GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, SEG7_DP , GPIO_PIN_RESET);
	}	
}

void resetDisplay(){
	HAL_GPIO_WritePin(GPIOE, SEG7_A | SEG7_B | SEG7_C | SEG7_D | SEG7_E | SEG7_F | SEG7_G | SEG7_DP | SEG7_OUT1 | SEG7_OUT2 | SEG7_OUT3 | SEG7_OUT4, GPIO_PIN_RESET); 
}
