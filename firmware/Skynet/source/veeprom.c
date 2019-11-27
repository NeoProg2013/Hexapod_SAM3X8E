//  ***************************************************************************
/// @file    veeprom.c
/// @author  NeoProg
//  ***************************************************************************
#include "veeprom.h"

#include <sam.h>
#include "flash.h"
#include "error_handling.h"

#define VEEPROM_BEGIN_ADDRESS           (0x0000)
#define VEEPROM_PAGE_SIZE               (FLASH_PAGE_SIZE)
#define VEEPROM_PAGE_COUNT              (10)
#define VEEPROM_SIZE                    (VEEPROM_PAGE_SIZE * VEEPROM_PAGE_COUNT)    // bytes
#define VEEPROM_END_ADDRESS             (VEEPROM_BEGIN_ADDRESS + VEEPROM_SIZE + 1)

#define VEEPROM_CHECKSUM_SIZE           (4)
#define VEEPROM_CHECKSUM_ADDRESS        (VEEPROM_END_ADDRESS - VEEPROM_CHECKSUM_SIZE - 1)
#define VEEPROM_DATA_END_ADDRESS        (VEEPROM_END_ADDRESS - VEEPROM_CHECKSUM_SIZE - 1)        

#define VEEPROM_FLASH_START_ADDRESS     (FLASH_BANK1_START_ADDRESS + (FLASH_BANK1_PAGE_COUNT - VEEPROM_PAGE_COUNT) * FLASH_PAGE_SIZE)


static uint32_t calculate_checksum(void);


//  ***************************************************************************
/// @brief  Virtual EEPROM initialize
/// @param  address: address to EEPROM memory cell
/// @param  data: buffer address
/// @param  size: count bytes for read
/// @return true - initialize success, false - fail
//  ***************************************************************************
void veeprom_init(void) {
    
    flash_init();
    
    /*uint32_t calc_checksum = calculate_checksum();
    uint32_t read_checksum = veeprom_read_32(VEEPROM_CHECKSUM_ADDRESS);
    
    if (calc_checksum != read_checksum) {
        callback_set_memory_error(ERROR_MODULE_VEEPROM_DRIVER);
    }*/
}

//  ***************************************************************************
/// @brief  Calculate and write VEEPROM checksum
/// @note   none
/// @return true - write success, false - fail
//  ***************************************************************************
bool veeprom_update_checksum(void) {
    
    uint32_t checksum = calculate_checksum();
    
    if (flash_write_32(VEEPROM_FLASH_START_ADDRESS + VEEPROM_CHECKSUM_ADDRESS, checksum) == false) {
        callback_set_memory_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    return true;
}

//  ***************************************************************************
/// @brief  Read byte from VEEPROM
/// @param  veeprom_address: VEEPROM address
/// @return Byte value
//  ***************************************************************************
uint8_t veeprom_read_8(uint32_t veeprom_address) {
    
    if (veeprom_address + sizeof(uint8_t) - 1 > VEEPROM_END_ADDRESS) {
        callback_set_internal_error(ERROR_MODULE_VEEPROM);
        return 0;
    }
    
    return flash_read_8(VEEPROM_FLASH_START_ADDRESS + veeprom_address);
}

//  ***************************************************************************
/// @brief  Read word from flash
/// @param  flash_address: flash address
/// @return Word value
//  ***************************************************************************
uint16_t veeprom_read_16(uint32_t veeprom_address) {
    
    if (veeprom_address + sizeof(uint16_t) - 1 > VEEPROM_END_ADDRESS) {
        callback_set_internal_error(ERROR_MODULE_VEEPROM);
        return 0;
    }
    
    return flash_read_16(VEEPROM_FLASH_START_ADDRESS + veeprom_address);
}

//  ***************************************************************************
/// @brief  Read double word from flash
/// @param  flash_address: flash address
/// @return Double word value
//  ***************************************************************************
uint32_t veeprom_read_32(uint32_t veeprom_address) {
    
    if (veeprom_address + sizeof(uint32_t) - 1 > VEEPROM_END_ADDRESS) {
        callback_set_internal_error(ERROR_MODULE_VEEPROM);
        return 0;
    }
    
    return flash_read_32(VEEPROM_FLASH_START_ADDRESS + veeprom_address);
}

//  ***************************************************************************
/// @brief  Read data from virtual EEPROM
/// @param  address: address to EEPROM memory cell
/// @param  buffer: buffer address
/// @param  size: count bytes for read
/// @return true - read success, false - fail
//  ***************************************************************************
void veeprom_read_bytes(uint32_t veeprom_address, uint8_t* buffer, uint32_t size) {

    if (veeprom_address + size - 1 > VEEPROM_END_ADDRESS) {
        callback_set_internal_error(ERROR_MODULE_VEEPROM);
        return;
    }
    
    flash_read_bytes(VEEPROM_FLASH_START_ADDRESS + veeprom_address, buffer, size);
}

//  ***************************************************************************
/// @brief  Write byte to flash
/// @param  flash_address: flash address
/// @return true - write success, false - fail
//  ***************************************************************************
bool veeprom_write_8(uint32_t veeprom_address, uint8_t data) {
    
    if (veeprom_address + sizeof(uint8_t) - 1 > VEEPROM_END_ADDRESS) {
        callback_set_internal_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    
    if (flash_write_8(VEEPROM_FLASH_START_ADDRESS + veeprom_address, data) == false) {
        callback_set_memory_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    
    return true;
}

//  ***************************************************************************
/// @brief  Write word to flash
/// @param  flash_address: flash address
/// @return true - write success, false - fail
//  ***************************************************************************
bool veeprom_write_16(uint32_t veeprom_address, uint16_t data) {
    
    if (veeprom_address + sizeof(uint16_t) - 1 > VEEPROM_END_ADDRESS) {
        callback_set_internal_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    
    if (flash_write_16(VEEPROM_FLASH_START_ADDRESS + veeprom_address, data) == false) {
        callback_set_memory_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    
    return true;
}

//  ***************************************************************************
/// @brief  Write double word to flash
/// @param  flash_address: flash address
/// @return true - write success, false - fail
//  ***************************************************************************
bool veeprom_write_32(uint32_t veeprom_address, uint32_t data) {
    
    if (veeprom_address + sizeof(uint32_t) - 1 > VEEPROM_END_ADDRESS) {
        callback_set_internal_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    
    if (flash_write_32(VEEPROM_FLASH_START_ADDRESS + veeprom_address, data) == false) {
        callback_set_memory_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    
    return true;
}

//  ***************************************************************************
/// @brief  Write data to virtual EEPROM
/// @param  address: address to EEPROM memory cell
/// @param  data: buffer address
/// @param  size: count bytes for write
/// @return true - write success, false - fail
//  ***************************************************************************
bool veeprom_write_bytes(uint32_t veeprom_address, const uint8_t* data, uint32_t size) {

    if (veeprom_address + size - 1 > VEEPROM_END_ADDRESS) {
        callback_set_internal_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    
    if (flash_write_bytes(VEEPROM_FLASH_START_ADDRESS + veeprom_address, data, size) == false) {
        callback_set_memory_error(ERROR_MODULE_VEEPROM);
        return false;
    }
    
    return true;
}





//  ***************************************************************************
/// @brief  Calculate VEEPROM checksum
/// @note   none
/// @return Checksum value
//  ***************************************************************************
static uint32_t calculate_checksum(void) {
    
    uint32_t checksum = 0;
    for (uint32_t i = 0; i < VEEPROM_DATA_END_ADDRESS; i += 4) {
        checksum += flash_read_32(VEEPROM_FLASH_START_ADDRESS + i);
    }
    
    return checksum;
}