#ifndef __DAC
#define __DAC

#define FULL_SCALE_DAC 4095
#define FULL_SCALE_ANGLE 180

/**
  * @brief  DAC Initialization
  * @param  None
	* @retval None
  */
void DAC_Initialize(void);

/**
  * @brief  Update DAC
  * @param  value: input data of DAC
	* @retval None
  */
void DAC_Set(float value);

/**
  * @brief  DMA Initialization
  * @param  None
	* @retval None
  */
void DMA_Initialize(void);

#endif
