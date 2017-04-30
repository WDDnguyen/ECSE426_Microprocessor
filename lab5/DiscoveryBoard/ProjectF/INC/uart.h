#ifndef __UART
#define __UART

typedef enum{
	x_label,
	y_label,
	z_label
}Acc_Label;

/**
  * @brief  UART Initialization
  * @param  None
	* @retval None
  */
void UART_Initialize(void);

/**
  * @brief  Transmit acc axis data
  * @param  data:  pointer to the acc data
						label: indicate acc axis
	* @retval None
  */
void UART_Transmit(float* data, Acc_Label label);

/**
  * @brief  Receive pitch & roll angles
  * @param  None
	* @retval None
  */
void UART_Receive(void);

/**
  * @brief  Transmit acc array
  * @param  acc_x: pointer to the x axis data
					  acc_y: pointer to the y axis data
						acc_z: pointer to the z axis data
	* @retval None
  */
void UART_Transmit_Arr(float* acc_x, float* acc_y, float* acc_z);

/**
  * @brief  Getter for pitch angle
  * @param  index: indicate which angle to retrieve
	* @retval int pitch angle
  */
int data_pitch(int index);

/**
  * @brief  Getter for roll angle
  * @param  index: indicate which angle to retrieve
	* @retval int roll angle
  */
int data_roll(int index);

/**
  * @brief  Format data to unsigned 8 bits
  * @param  data:  pointer to the data
	          label: indicate acc axis
		* @retval None
  */
void dataFormat(float* data, Acc_Label label);

/**
  * @brief  Get lower 8 bits of the data
  * @param  data: unsigned 16 bits input data
	* @retval unsigned data with 8 bits
  */
uint8_t data16to8_L(uint16_t data);

/**
  * @brief  Get upper 8 bits of the data
  * @param  data: unsigned 16 bits input data
	* @retval unsigned data with 8 bits
  */
uint8_t data16to8_R(uint16_t data);

/**
  * @brief  Compute 2's complement of the data
  * @param  value: input accelerations
	* @retval 2's complement of input data value
  */
uint16_t complConvert(float value);

#endif
