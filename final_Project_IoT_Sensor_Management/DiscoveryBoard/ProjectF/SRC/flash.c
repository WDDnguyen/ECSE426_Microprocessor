#include "stm32f4xx_it.h"
#include "error_func.h"
#include "flash.h"

FLASH_EraseInitTypeDef flash_erase;
uint32_t sec_error;

// initialize flash erasing
void Flash_Erase_Init(uint32_t init_addr, uint32_t end_addr)
{
	uint32_t init_sec;
	uint32_t end_sec;
	
	HAL_FLASH_Unlock();
	
	init_sec = Sector_Map(init_addr);
	end_sec = Sector_Map(end_addr);
	
	flash_erase.TypeErase = FLASH_TYPEERASE_SECTORS;
	flash_erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	flash_erase.Sector = init_sec;
	flash_erase.NbSectors = end_sec - init_sec + 1;
}

// erase flash
void Flash_Erase(void)
{
	if (HAL_FLASHEx_Erase(&flash_erase, &sec_error) != HAL_OK)
	{
		Error_Handler(FLASH_ERASE_ERROR);
	}
}

// flash address mapping
uint32_t Sector_Map(uint32_t addr)
{
	if (addr >= FLASH_SECTOR0_ADDR && addr < FLASH_SECTOR1_ADDR)
	{
		return FLASH_SECTOR_0;
	}
	else if (addr >= FLASH_SECTOR1_ADDR && addr < FLASH_SECTOR2_ADDR)
	{
		return FLASH_SECTOR_1;
	}
	else if (addr >= FLASH_SECTOR2_ADDR && addr < FLASH_SECTOR3_ADDR)
	{
		return FLASH_SECTOR_2;
	}
	else if (addr >= FLASH_SECTOR3_ADDR && addr < FLASH_SECTOR4_ADDR)
	{
		return FLASH_SECTOR_3;
	}
	else if (addr >= FLASH_SECTOR4_ADDR && addr < FLASH_SECTOR5_ADDR)
	{
		return FLASH_SECTOR_4;
	}
	else if (addr >= FLASH_SECTOR5_ADDR && addr < FLASH_SECTOR6_ADDR)
	{
		return FLASH_SECTOR_5;
	}
	else if (addr >= FLASH_SECTOR6_ADDR && addr < FLASH_SECTOR7_ADDR)
	{
		return FLASH_SECTOR_6;
	}
	else if (addr >= FLASH_SECTOR7_ADDR && addr < FLASH_SECTOR8_ADDR)
	{
		return FLASH_SECTOR_7;
	}
	else if (addr >= FLASH_SECTOR8_ADDR && addr < FLASH_SECTOR9_ADDR)
	{
		return FLASH_SECTOR_8;
	}
	else if (addr >= FLASH_SECTOR9_ADDR && addr < FLASH_SECTOR10_ADDR)
	{
		return FLASH_SECTOR_9;
	}
	else if (addr >= FLASH_SECTOR10_ADDR && addr < FLASH_SECTOR11_ADDR)
	{
		return FLASH_SECTOR_10;
	}
	else if (addr >= FLASH_SECTOR11_ADDR && addr <= FLASH_SECTOR_END_ADDR)
	{
		return FLASH_SECTOR_11;
	}
	else
	{
		Error_Handler(FLASH_ADDR_ERROR);
		return 1;
	}
}

// flush data cache
void DCache_Flush(void)
{
	__HAL_FLASH_DATA_CACHE_DISABLE();
	__HAL_FLASH_DATA_CACHE_RESET();
	__HAL_FLASH_DATA_CACHE_ENABLE();
}

// flush instruction cache
void ICache_Flush(void)
{
	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
	__HAL_FLASH_INSTRUCTION_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
}
