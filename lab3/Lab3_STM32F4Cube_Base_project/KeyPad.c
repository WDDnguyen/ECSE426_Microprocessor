#include "KeyPad.h"
#include "stm32f4xx_hal.h"
#include "KeyPad.h"


int counter = 0;

void KeyPadInit(){

	GPIO_InitTypeDef GPIO_InitDef; //create the configuration information for the GPIO
	GPIO_InitDef.Pin = KeyPad_Row1 | KeyPad_Row2 | KeyPad_Row3 | KeyPad_Row4 | KeyPad_Column1 | KeyPad_Column2 | KeyPad_Column3 | KeyPad_Column4; 	
	GPIO_InitDef.Mode = GPIO_MODE_OUTPUT_PP;   			
	GPIO_InitDef.Pull = GPIO_NOPULL;
	GPIO_InitDef.Speed = GPIO_SPEED_FREQ_MEDIUM;		

	HAL_GPIO_Init(KeyPad_Port, &GPIO_InitDef);

	//Reset pins when initiate
	HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row1 | KeyPad_Row2 | KeyPad_Row3 | KeyPad_Row4 | KeyPad_Column1 | KeyPad_Column2 | KeyPad_Column3 | KeyPad_Column4, GPIO_PIN_RESET);	
	
}

int KeyPadValue(){
	
	int valueReturned = -1;
	counter = 3;
	
	if (counter == 0){
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Column1 | KeyPad_Column3 | KeyPad_Column4, GPIO_PIN_RESET );
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Column1, GPIO_PIN_SET );
		
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row1) > 0){valueReturned = Key_1;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row2) > 0){valueReturned = Key_4;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row3) > 0){valueReturned = Key_7;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row4) > 0){valueReturned = Key_star;}
	}
	
	else if (counter == 1) {
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Column1 | KeyPad_Column3 | KeyPad_Column4, GPIO_PIN_RESET );
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Column2, GPIO_PIN_SET );
		
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row1) > 0){valueReturned = Key_2;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row2) > 0){valueReturned = Key_5;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row3) > 0){valueReturned = Key_8;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row4) > 0){valueReturned = Key_0;}
	
	}
	
	else if (counter == 2){
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Column1 | KeyPad_Column3 | KeyPad_Column4, GPIO_PIN_RESET );
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Column3, GPIO_PIN_SET );
		
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row1) > 0){valueReturned = Key_3;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row2) > 0){valueReturned = Key_6;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row3) > 0){valueReturned = Key_9;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Row4) > 0){valueReturned = Key_hash;}
	}
	
	return valueReturned;
	
}


