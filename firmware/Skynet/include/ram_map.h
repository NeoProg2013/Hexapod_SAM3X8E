//  ***************************************************************************
/// @file    ram_map.h
/// @author  NeoProg
/// @brief   RAM mapping functions
//  ***************************************************************************
#ifndef RAM_MAP_H_
#define RAM_MAP_H_

#include <stdint.h>
#include <stdbool.h>

#define RAM_MAP_SIZE                    (512)
#define RAM_MAP_BEGIN_ADDRESS           (0x0000)
#define RAM_MAP_END_ADDRESS             (RAM_MAP_BEGIN_ADDRESS + RAM_MAP_SIZE)


extern bool ram_map_read(uint32_t ram_address, uint8_t* buffer, uint32_t bytes_count);
extern bool ram_map_write(uint32_t ram_address, const uint8_t* buffer, uint32_t bytes_count);


#endif /* RAM_MAP_H_ */