//  ***************************************************************************
/// @file    flash.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include "flash.h"
#define FLASH_BANK0_START_ADDRESS       (IFLASH0_ADDR)
#define FLASH_BANK0_END_ADDRESS         (FLASH_BANK0_START_ADDRESS + IFLASH0_SIZE - 1)


uint8_t flash_ram_buffer[FLASH_PAGE_SIZE] = {0};


__attribute__((__noinline__))
__attribute__((section(".ramfunc")))
static bool flash_execute_cmd(uint32_t cmd, uint16_t arg);


//  ***************************************************************************
/// @brief  Flash driver initialize
/// @param  none
/// @return none
//  ***************************************************************************
void flash_init(void) {
    
    // Configure EFC:
    // * 128 access mode
    // * 4 wait states for flash writing (default)
    // * RDY interrupt disables
    REG_EFC0_FMR = EEFC_FMR_FWS(4);
    REG_EFC1_FMR = EEFC_FMR_FWS(4);
}

//  ***************************************************************************
/// @brief  Read byte from flash
/// @param  flash_address: flash address
/// @return Byte value
//  ***************************************************************************
uint8_t flash_read_8(uint32_t flash_address) {
    
    uint8_t data = 0;
    flash_read_bytes(flash_address, &data, 1);
    return data;
}

//  ***************************************************************************
/// @brief  Read word from flash
/// @param  flash_address: flash address
/// @return Word value
//  ***************************************************************************
uint16_t flash_read_16(uint32_t flash_address) {
    
    uint8_t data[2] = {0};
    flash_read_bytes(flash_address, data, 2);

    return (data[0] << 8) | (data[1] << 0);
}

//  ***************************************************************************
/// @brief  Read double word from flash
/// @param  flash_address: flash address
/// @return Double word value
//  ***************************************************************************
uint32_t flash_read_32(uint32_t flash_address) {
    
    uint8_t data[4] = {0};
    flash_read_bytes(flash_address, data, 4);
    
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3] << 0);
}

//  ***************************************************************************
/// @brief  Read bytes from flash
/// @param  flash_address: flash address
/// @param  buffer: buffer address
/// @param  size: bytes count for read
/// @return none
//  ***************************************************************************
void flash_read_bytes(uint32_t flash_address, uint8_t* buffer, uint32_t size) {
    
    if (flash_address < FLASH_BANK1_START_ADDRESS || flash_address + size - 1 > FLASH_BANK1_END_ADDRESS) {
        return;
    }
    
    
    uint8_t* memory = (uint8_t*)flash_address;
    for (uint32_t i = 0; i < size; ++i) {
        buffer[i] = memory[i];
    }
}

//  ***************************************************************************
/// @brief  Write byte to flash
/// @param  flash_address: flash address
/// @return true - write success, false - fail
//  ***************************************************************************
bool flash_write_8(uint32_t flash_address, uint8_t data) {
    
    return flash_write_bytes(flash_address, &data, 1);
}

//  ***************************************************************************
/// @brief  Write word to flash
/// @param  flash_address: flash address
/// @return true - write success, false - fail
//  ***************************************************************************
bool flash_write_16(uint32_t flash_address, uint16_t data) {
    
    uint8_t msb = (data & 0xFF00) >> 8;
    uint8_t lsb = (data & 0x00FF);
    data = (lsb << 8) | (msb << 0);
    
    return flash_write_bytes(flash_address, (uint8_t*)&data, 2);
}

//  ***************************************************************************
/// @brief  Write double word to flash
/// @param  flash_address: flash address
/// @return true - write success, false - fail
//  ***************************************************************************
bool flash_write_32(uint32_t flash_address, uint32_t data) {
    
    uint8_t xmsb = (data & 0xFF000000) >> 24;
    uint8_t msb =  (data & 0x00FF0000) >> 16;
    uint8_t lsb =  (data & 0x0000FF00) >> 8;
    uint8_t xlsb = (data & 0x000000FF) >> 0;
    data = (xlsb << 24) | (lsb << 16)  | (msb << 8)  | (xmsb << 0);
    
    return flash_write_bytes(flash_address, (uint8_t*)&data, 4);
}

//  ***************************************************************************
/// @brief  Write bytes to flash
/// @param  flash_address: flash address
/// @param  data: data for write
/// @param  size: bytes count for write
/// @return true - write success, false - fail
//  ***************************************************************************
bool flash_write_bytes(uint32_t flash_address, const uint8_t* data, uint32_t size) {
    
    if (flash_address < FLASH_BANK1_START_ADDRESS || flash_address + size - 1 > FLASH_BANK1_END_ADDRESS) {
        return false;
    }
    
    
    // Get page number, inpage address and page begin address
    uint32_t page_number = (flash_address - FLASH_BANK0_START_ADDRESS) / FLASH_PAGE_SIZE;
    uint32_t page_start_address = FLASH_BANK0_START_ADDRESS + page_number * FLASH_PAGE_SIZE;
    uint32_t inpage_address = flash_address - FLASH_BANK0_START_ADDRESS - page_number * FLASH_PAGE_SIZE;
    
    // Read flash data to RAM
    flash_read_bytes(page_start_address, flash_ram_buffer, FLASH_PAGE_SIZE);
    
    // Write data to RAM buffer
    bool is_need_erase = false;
    for (uint32_t i = 0; i < size; ++i) {
        
        // Check need erase page. If cell contain not 0xFF value, then need erase page
        if (flash_ram_buffer[inpage_address + i] != 0xFF) {
            is_need_erase = true;
        }
        
        flash_ram_buffer[inpage_address + i] = data[i];
    }

    // Clear lock bit
    if (flash_execute_cmd(EEFC_FCR_FCMD_CLB, page_number) == false) {
        return false;
    }
    
    // Write data to flash memory
    uint32_t* memory = (uint32_t*)page_start_address;
    for (uint32_t i = 0; i < FLASH_PAGE_SIZE; i += 4) {
        
        uint32_t data = (flash_ram_buffer[i + 3] << 24) | (flash_ram_buffer[i + 2] << 16) | 
                        (flash_ram_buffer[i + 1] << 8 ) | (flash_ram_buffer[i + 0] << 0 );
        
        *memory = data;
        ++memory;
    }
    
    // Write page and lock page
    REG_EFC1_FMR = EEFC_FMR_FWS(6); // According errata
    __disable_irq();
    if (is_need_erase == true) {
        
        if (flash_execute_cmd(EEFC_FCR_FCMD_EWPL, page_number) == false) {
            return false;
        }
    }
    else {
        
        if (flash_execute_cmd(EEFC_FCR_FCMD_WPL, page_number) == false) {
            return false;
        }
    }
    __enable_irq();
    REG_EFC1_FMR = EEFC_FMR_FWS(4); // Restore to default
    
    // Check written data
    for (uint32_t i = 0; i < size; ++i) {
        
        uint8_t byte = flash_read_8(page_start_address + i);
        if (byte != flash_ram_buffer[i]) {
            return false;
        }
    }
    
    return true;
}





//  ***************************************************************************
/// @brief  Execute FLASH command
/// @note   This function should be execute from RAM
/// @param  cmd: flash command
/// @param  arg: command argument
/// @return true - execute success, false - fail
//  ***************************************************************************
__attribute__((__noinline__)) 
__attribute__((section(".ramfunc")))
static bool flash_execute_cmd(uint32_t cmd, uint16_t arg) {
    
    REG_EFC1_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FARG(arg) | cmd;
    
    volatile uint32_t ul_status = REG_EFC1_FSR;
    while ((ul_status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY) {
        ul_status = REG_EFC1_FSR;
    }
    
    // Check result
    return (ul_status & (EEFC_FSR_FCMDE | EEFC_FSR_FLOCKE)) == 0;
}