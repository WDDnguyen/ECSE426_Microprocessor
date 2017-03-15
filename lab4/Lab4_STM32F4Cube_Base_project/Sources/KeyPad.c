#include "KeyPad.h"
#include "stm32f4xx_hal.h"
int counter = 0;
//int pressedValue = -1;

//debouncing Flag
int debouncingFlag = 10;
int debouncingValue = -1;

void initializeKeyPad(){
	
	GPIO_InitTypeDef GPIO_InitDef; //create the configuration information for the GPIO
	GPIO_InitDef.Pin = KeyPad_Row1 | KeyPad_Row2 | KeyPad_Row3 | KeyPad_Row4 | KeyPad_Column1 | KeyPad_Column2 | KeyPad_Column3; 	
	GPIO_InitDef.Mode = GPIO_MODE_OUTPUT_PP;   			
	GPIO_InitDef.Pull = GPIO_NOPULL;
	GPIO_InitDef.Speed = GPIO_SPEED_FREQ_MEDIUM;		

	KeyPad_GPIO_PORT_CLK_EN;
	
	HAL_GPIO_Init(KeyPad_Port, &GPIO_InitDef);	
	//Reset pins when initiate
	
	HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row1 | KeyPad_Row2 | KeyPad_Row3 | KeyPad_Row4 | KeyPad_Column1 | KeyPad_Column2 | KeyPad_Column3, GPIO_PIN_RESET);	
	
}

int KeyPadGetValue(){
	
	int valueReturned = -1;
	
	counter = 0;

	while(counter != 4){
	if (counter == 0){
		
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row2 | KeyPad_Row3 | KeyPad_Row4, GPIO_PIN_RESET );
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row1, GPIO_PIN_SET );
		
		
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column1) > 0){valueReturned = Key_1;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column2) > 0){valueReturned = Key_2;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column3) > 0){valueReturned = Key_3;}
		
		}
	
	else if (counter == 1) {
		
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row1 | KeyPad_Row3 | KeyPad_Row4, GPIO_PIN_RESET );
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row2, GPIO_PIN_SET );
		
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column1) > 0){valueReturned = Key_4;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column2) > 0){valueReturned = Key_5;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column3) > 0){valueReturned = Key_6;}		
	}
	
		else if (counter == 2){
		
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row1 | KeyPad_Row2| KeyPad_Row4, GPIO_PIN_RESET );
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row3, GPIO_PIN_SET );
		
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column1) > 0){valueReturned = Key_7;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column2) > 0){valueReturned = Key_8;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column3) > 0){valueReturned = Key_9;}
	}
		
		else if (counter == 3){
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row1 | KeyPad_Row2 | KeyPad_Row3, GPIO_PIN_RESET );
		HAL_GPIO_WritePin(KeyPad_Port, KeyPad_Row4, GPIO_PIN_SET );
		
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column1) > 0){valueReturned = Key_star;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column2) > 0){valueReturned = Key_0;}
		if (HAL_GPIO_ReadPin(KeyPad_Port, KeyPad_Column3) > 0){valueReturned = Key_hash;}
	}
		counter++;
	  
}
	
return valueReturned;
//printf("INPUT : %d, %d\n",valueReturned,debouncingFlag );
/*
if (valueReturned == -1){
	debouncingFlag -=	1;
	if (debouncingFlag < 0)
	debouncingFlag = 0;
}

else if (debouncingFlag <= 0){
	debouncingFlag = 10;
	printf("RECORDED : %d\n",valueReturned);
	return valueReturned;
}
else{
	debouncingFlag+= 1;
	if (debouncingFlag > 10)
		debouncingFlag = 10;
}
 
return -1;
*/	
	
}


