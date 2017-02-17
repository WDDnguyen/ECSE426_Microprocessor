/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */
ADC_HandleTypeDef ADC1_Handle;
void ADC_Init(void){
	
	ADC_InitTypeDef ADC1_Init; //ADC1 init struct	
	ADC_ChannelConfTypeDef ADC1_Channel; //ADC1 channel struct
	
	// initialize ADC init struct 
	//ADC1_Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // ADC Clock frequency 42Mz (168Mhz/4)
	ADC1_Init.ClockPrescaler = 49200;
	ADC1_Init.Resolution = ADC_RESOLUTION_12B; // 12 bit resolution 
	ADC1_Init.DataAlign = ADC_DATAALIGN_RIGHT; // align the 12 bits data at the right of the 32 bits words
	ADC1_Init.ScanConvMode = DISABLE; // Only use a single channel
	ADC1_Init.ContinuousConvMode = DISABLE;	// single mode convertion
	ADC1_Init.NbrOfConversion = 1; // one conversion
	
	// initialize ADC handle struct 
	ADC1_Handle.Instance = ADC1; //Make instance with ADC1
	ADC1_Handle.Init = ADC1_Init; 
	ADC1_Handle.NbrOfCurrentConversionRank = 1;
	
	// initialize ADC channel struct 
	ADC1_Channel.Channel = ADC_CHANNEL_16; // Channel for temperature sensor
	ADC1_Channel.Rank = 1; //determine the rank in which this channel is sampled
	ADC1_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;	// time for the internal capacitor to charge.
	ADC1_Channel.Offset = 0;
	
	
	// Initialise the sensor pin 
	__HAL_RCC_ADC1_CLK_ENABLE(); //Enable ADC1 Clock
	HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC1_Channel);	//Configurate channel for Handle
	
}

//Poll value and convert the value to celsius
float ADC_getValue(void){																			

	HAL_ADC_Start(&ADC1_Handle);
	float value = 0;
			//polling data of sensor
	if (HAL_ADC_PollForConversion(&ADC1_Handle, 10000) == HAL_OK){
			value = HAL_ADC_GetValue(&ADC1_Handle);
	}
	HAL_ADC_Stop(&ADC1_Handle);
	
	//acquired value of sensor to celsius 
	value *= 3;
	value /= 4096;
	value -= .76;
	value /= .0025;
	value += 25;
	
	return value;
}

float convertToFahrenheit(float celsius){
	float fahrenheit = (9 * celsius)/5 + 32 ;
	return fahrenheit;
}

/* USER CODE END 0 */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
