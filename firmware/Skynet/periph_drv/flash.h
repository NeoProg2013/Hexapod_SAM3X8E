//  ***************************************************************************
/// @file    flash.h
/// @author  NeoProg
/// @brief   Internal flash driver (only BANK1 flash memory)
//  ***************************************************************************
#ifndef FLASH_H_
#define FLASH_H_

#include <stdbool.h>

// Use flash bank #1
#define FLASH_BANK1_START_ADDRESS           (IFLASH1_ADDR)
#define FLASH_BANK1_END_ADDRESS             (FLASH_BANK1_START_ADDRESS + IFLASH1_SIZE - 1)
#define FLASH_BANK1_PAGE_COUNT              (1024)
#define FLASH_PAGE_SIZE                     (IFLASH1_PAGE_SIZE)


void flash_init(void);

uint8_t flash_read_8(uint32_t flash_address);
uint16_t flash_read_16(uint32_t flash_address);
uint32_t flash_read_32(uint32_t flash_address);
void flash_read_bytes(uint32_t flash_address, uint8_t* buffer, uint32_t size);

bool flash_write_8(uint32_t flash_address, uint8_t data);
bool flash_write_16(uint32_t flash_address, uint16_t data);
bool flash_write_32(uint32_t flash_address, uint32_t data);
bool flash_write_bytes(uint32_t flash_address, const uint8_t* data, uint32_t size);


#endif /* FLASH_H_ */