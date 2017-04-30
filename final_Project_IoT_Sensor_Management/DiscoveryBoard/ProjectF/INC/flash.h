#ifndef __FLASH
#define __FLASH

#define FLASH_SECTOR0_ADDR 		0x08000000	// 16 KB
#define FLASH_SECTOR1_ADDR 		0x08004000	// 16 KB
#define FLASH_SECTOR2_ADDR 		0x08008000	// 16 KB
#define FLASH_SECTOR3_ADDR 		0x0800C000	// 16 KB
#define FLASH_SECTOR4_ADDR 		0x08010000	// 64 KB
#define FLASH_SECTOR5_ADDR 		0x08020000	// 128 KB
#define FLASH_SECTOR6_ADDR 		0x08040000	// 128 KB
#define FLASH_SECTOR7_ADDR 		0x08060000	// 128 KB
#define FLASH_SECTOR8_ADDR 		0x08080000	// 128 KB
#define FLASH_SECTOR9_ADDR 		0x080A0000	// 128 KB
#define FLASH_SECTOR10_ADDR 	0x080C0000	// 128 KB
#define FLASH_SECTOR11_ADDR 	0x080E0000	// 128 KB
#define FLASH_SECTOR_END_ADDR 0X080FFFFF	// 128 KB

/**
  * @brief  Initialization for earsing flash memory
  * @param  init_addr: initial address to be earsed
						end_addr:	 end address to be earsed
	* @retval None
  */
void Flash_Erase_Init(uint32_t init_addr, uint32_t end_addr);

/**
  * @brief  Earse flash memory
  * @param  None
	* @retval None
  */
void Flash_Erase(void);

/**
  * @brief  Mapping from flash address to sectors
  * @param  addr: address of flash memory
	* @retval sector of flash memory
  */
uint32_t Sector_Map(uint32_t addr);

/**
  * @brief  Flush data cache
  * @param  None
	* @retval None
  */
void DCache_Flush(void);

/**
  * @brief  Flush instruction cache
  * @param  None
	* @retval None
  */
void ICache_Flush(void);

#endif
