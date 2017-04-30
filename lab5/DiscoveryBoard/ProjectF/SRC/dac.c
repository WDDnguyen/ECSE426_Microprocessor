#include "stm32f4xx_it.h"
#include "error_func.h"
#include "dac.h"

DAC_HandleTypeDef dac_handle;
DMA_HandleTypeDef dma_handle;

uint32_t angle;

volatile Flag_Sig dac_flag;

void DAC_Initialize(void)
{
	DAC_ChannelConfTypeDef sConfig;
	
  __HAL_RCC_DAC_CLK_ENABLE();
	
	// initialization
	dac_handle.Instance = DAC;
  if(HAL_DAC_Init(&dac_handle) != HAL_OK)
  {
    Error_Handler(DAC_INIT_ERROR);
  }
  
  // config channel
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;  

  if(HAL_DAC_ConfigChannel(&dac_handle, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler(DAC_CONFIG_ERROR);
  }
}

void DMA_Initialize(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
	
	// dma initialization
  dma_handle.Instance = DMA1_Stream5;  
  dma_handle.Init.Channel  = DMA_CHANNEL_7;
  dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
  dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
  dma_handle.Init.MemInc = DMA_MINC_ENABLE;
  dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  dma_handle.Init.Mode = DMA_CIRCULAR;
  dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
  dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;         
  dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
  dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE; 
	
  if (HAL_DMA_Init(&dma_handle) != HAL_OK)
	{
		Error_Handler(DMA_INIT_ERROR);
	}
    
  // link dac & dma
  __HAL_LINKDMA(&dac_handle, DMA_Handle1, dma_handle);

  // enable IT
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

void DAC_Set(float value)
{
	// data formatting
	if (value < 0) value = -value;
	angle = (uint32_t)(value/FULL_SCALE_ANGLE*FULL_SCALE_DAC);
	
	// send data to dac
	if(HAL_DAC_Start_DMA(&dac_handle, DAC_CHANNEL_1, &angle, 1, DAC_ALIGN_12B_R) != HAL_OK)
  {
    Error_Handler(DMA_START_ERROR);
  }
	while(dac_flag == OFF);
	dac_flag = OFF;
	printf("DAC reading: %f\n", 180.0f/4095*HAL_DAC_GetValue(&dac_handle, DAC_CHANNEL_1));
	HAL_DAC_Stop_DMA(&dac_handle, DAC_CHANNEL_1);
}
