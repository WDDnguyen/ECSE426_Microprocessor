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
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

void GPIO_Init(void){
	
	GPIO_InitTypeDef GPIO_INIT; //create the configuration information for the GPIO
	
	__HAL_RCC_GPIOD_CLK_ENABLE();	//Enable clock for Port D pins (LEDs)					 																	
	__HAL_RCC_GPIOE_CLK_ENABLE(); //Enable clock for PORT E pins (7 Segment display) 
	__HAL_RCC_GPIOA_CLK_ENABLE(); //Enable clock for PORT A pins (blue button)
	
	// FOR LEDs
	GPIO_INIT.Pin = LED1 | LED2 | LED3 | LED4; // initialize the pins for the LEDs pin
	GPIO_INIT.Mode = GPIO_MODE_OUTPUT_PP; // Set pins to output push pull mode 
	GPIO_INIT.Pull = GPIO_NOPULL; // No pull for LEDs pins
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM;	//Speed of pin
	
	HAL_GPIO_Init(GPIOD, &GPIO_INIT);	//Initiate the pins with the settings from GPIO_INIT.
	HAL_GPIO_WritePin(GPIOD, LED1 | LED2 | LED3 | LED4 , GPIO_PIN_RESET); // when initialize, set all the pins to LOW
	
	//7-segment display
	GPIO_INIT.Pin = SEG7_A | SEG7_B | SEG7_C | SEG7_D | SEG7_E | SEG7_F | SEG7_G | SEG7_DP | SEG7_OUT1 | SEG7_OUT2 | SEG7_OUT3 | SEG7_OUT4 ; 	// initialize the 7-segment display for the LEDs pin
	GPIO_INIT.Mode = GPIO_MODE_OUTPUT_PP; //set pins to output push pull mode
	GPIO_INIT.Pull = GPIO_NOPULL; // no pull for 7-segment pins
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM; //Speed of pin
	
	HAL_GPIO_Init(GPIOE, &GPIO_INIT); // Initiate the pins with the setting from GPIO_INIT
	// when initialize, set all the pins to LOW
	HAL_GPIO_WritePin(GPIOE, SEG7_A | SEG7_B | SEG7_C | SEG7_D | SEG7_E | SEG7_F | SEG7_G | SEG7_DP | SEG7_OUT1 | SEG7_OUT2 | SEG7_OUT3 | SEG7_OUT4, GPIO_PIN_RESET); 

	//Initialize blue button
	GPIO_INIT.Pin = GPIO_PIN_0; //initialize the pin PINA.0 for blue button
	GPIO_INIT.Mode = GPIO_MODE_INPUT; // set pin to input 
	GPIO_INIT.Pull = GPIO_NOPULL; // no pull for blue button
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_MEDIUM; // speed of pin
	
	HAL_GPIO_Init(GPIOA, &GPIO_INIT); // initiate the pins with the setting from GPIO_INIT
	
}

//when blue button pin is pressed (Read is HIGH) then return 1
int blueButtonPressed(){
	if ( HAL_GPIO_ReadPin(GPIOA, blueButton ) == SET) {
		return 1;
	}
	else 
	{
	return 0;
	}
	
}

//Turn on the LED on when the temperature celsius is higher than the threshold. 
//parameter to set which led to be turn on and else turn off.
void overHeatAlarm(int x){
	
	if (x == 0){
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_RESET);
	}
	
	else if (x == 1){
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_RESET);
	}
	
	else if (x == 2){
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_RESET);
	}
	
	else if (x == 3){
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_SET);
	}
	
	else {
		HAL_GPIO_WritePin(GPIOD, LED1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LED4, GPIO_PIN_RESET);
	}
}

//allow to display the value on a segment display depending on the number and position
//parameter number provide which number going to be display on the 
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
		HAL_GPIO_WritePin(GPIOE, SEG7_DP , GPIO_PIN_SET);
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


/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * c`
        * EVENT_OUT
        * EXTI
     PXX   ------> YYYYYY
		 
		 Continue your GPIO configuration information as abow.

*/

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
