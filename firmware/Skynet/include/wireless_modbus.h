//  ***************************************************************************
/// @file    wireless_modbus.h
/// @author  NeoProg
/// @brief   Wireless modbus communication driver
//  ***************************************************************************
#ifndef WIRELESS_MODBUS_H_
#define WIRELESS_MODBUS_H_

#include <stdint.h>


#define WIRELESS_MODBUS_FRAME_SIZE						(sizeof(wireless_frame_t))
#define WIRELESS_MODBUS_FRAME_DATA_SIZE					(1017)
#define WIRELESS_MODBUS_FRAME_CRC_SIZE					(2)

#define WIRELESS_MODBUS_CMD_WRITE_RAM					(0x41)	// Function Code: Write RAM
#define WIRELESS_MODBUS_CMD_READ_RAM					(0x44)	// Function Code: Read RAM
#define WIRELESS_MODBUS_CMD_READ_MULTIMEDIA_DATA_SIZE	(0x60)	// Function Code: Read multimedia data size
#define WIRELESS_MODBUS_CMD_READ_MULTIMEDIA_DATA		(0x61)	// Function Code: Read multimedia data
#define WIRELESS_MODBUS_EXCEPTION						(0x80)	// Function Code: Exception


// Frame size - 1024 bytes
typedef struct __attribute__ ((packed)) {

	uint8_t  function_code;
	uint16_t address;
	uint16_t bytes_count;
	uint8_t  data[WIRELESS_MODBUS_FRAME_DATA_SIZE];
	uint16_t crc;
	
} wireless_frame_t;


extern void wireless_modbus_init(void);
extern void wireless_modbus_process(void);


#endif /* WIRELESS_MODBUS_H_ */