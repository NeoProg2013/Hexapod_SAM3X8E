//  ***************************************************************************
/// @file    wireless_modbus.c
/// @author  NeoProg
//  ***************************************************************************
#include "wireless_modbus.h"

#include <sam.h>
#include <stdbool.h>
#include <string.h>
#include "ram_map.h"
#include "systimer.h"
#include "usart3_pdc.h"
#include "error_handling.h"

#define USART_BAUD_RATE                         (500000)
#define CRC16_POLYNOM							(0xA001)


static wireless_frame_t wireless_frame = {0};


static bool is_wireless_modbus_frame_detected(const uint8_t* data, uint32_t data_size);
static void read_ram_command_handler(wireless_frame_t* frame);
static void write_ram_command_handler(wireless_frame_t* frame);
static uint16_t calculate_crc16(const uint8_t* frame, uint32_t size);


//  ***************************************************************************
/// @brief	Wireless modbus driver initialization
/// @return	none
//  ***************************************************************************
void wireless_modbus_init(void) {
	
	usart3_init(USART_BAUD_RATE);
	usart3_start_rx(USART3_USE_INTERNAL_BUFFER, 0);
}

//  ***************************************************************************
/// @brief	Wireless modbus driver process
/// @note	Call from main loop
//  ***************************************************************************
void wireless_modbus_process(void) {

	// Check USART errors
	if (usart3_is_error() == true) {
		usart3_reset(true, true);
		usart3_start_rx(USART3_USE_INTERNAL_BUFFER, 0);
		return;
	}
	
	// Check frame received
	if (usart3_is_frame_received() == false) {
		return;
	}
	
	// Check complete transmit response
	if (usart3_is_tx_complete() == false) {
		usart3_start_rx(USART3_USE_INTERNAL_BUFFER, 0);
		return;
	}
	

	const uint8_t* recv_data = usart3_get_internal_rx_buffer_address();
	uint32_t data_size = usart3_get_frame_size();
	
	// Verify frame
	if (is_wireless_modbus_frame_detected(recv_data, data_size) == false) {
		usart3_start_rx(USART3_USE_INTERNAL_BUFFER, 0);
		return;
	}

	// Process frame
	memcpy(&wireless_frame, recv_data, WIRELESS_MODBUS_FRAME_SIZE);
	switch (wireless_frame.function_code) {
		
		case WIRELESS_MODBUS_CMD_WRITE_RAM:
			write_ram_command_handler(&wireless_frame);
			break;
		
		case WIRELESS_MODBUS_CMD_READ_RAM:
			read_ram_command_handler(&wireless_frame);
			break;
		
		default:
			usart3_reset(true, true);
			usart3_start_rx(USART3_USE_INTERNAL_BUFFER, 0);
			return;
	}

	// Prepare response
	wireless_frame.crc = calculate_crc16((uint8_t*)&wireless_frame, WIRELESS_MODBUS_FRAME_SIZE - WIRELESS_MODBUS_FRAME_CRC_SIZE);
	
	// Start transmit response
	uint8_t* tx_buffer = usart3_get_internal_tx_buffer_address();
	memcpy(tx_buffer, &wireless_frame, WIRELESS_MODBUS_FRAME_SIZE);
	memset(&wireless_frame, 0x00, WIRELESS_MODBUS_FRAME_SIZE);
	usart3_start_tx(WIRELESS_MODBUS_FRAME_SIZE);

	// Start receive next frame
	usart3_start_rx(USART3_USE_INTERNAL_BUFFER, 0);
}





//  ***************************************************************************
/// @brief	Check received data
/// @return	true - wireless frame detected, false - any data
//  ***************************************************************************
static bool is_wireless_modbus_frame_detected(const uint8_t* raw_frame, uint32_t frame_size) {
	
	// Check frame size
	if (frame_size != WIRELESS_MODBUS_FRAME_SIZE) {
		return false;
	}
	
	// Check CRC
	uint32_t crc = calculate_crc16(raw_frame, frame_size);
	if (crc != 0) {
		return false;
	}
	
	// Check frame function code
    const wireless_frame_t* wireless_frame = (const wireless_frame_t*)raw_frame;
	if (wireless_frame->function_code != WIRELESS_MODBUS_CMD_WRITE_RAM &&
		wireless_frame->function_code != WIRELESS_MODBUS_CMD_READ_RAM &&
		wireless_frame->function_code != WIRELESS_MODBUS_CMD_READ_MULTIMEDIA_DATA_SIZE &&
		wireless_frame->function_code != WIRELESS_MODBUS_CMD_READ_MULTIMEDIA_DATA) {
			
		return false;
	}

	return true;
}

//  ***************************************************************************
/// @brief  Function for processing ModBus read RAM command
/// @param  frame: pointer to wireless frame
/// @retval frame
//  ***************************************************************************
static void read_ram_command_handler(wireless_frame_t* frame) {

	// Check request parameters
	if (frame->bytes_count == 0 || frame->bytes_count > WIRELESS_MODBUS_FRAME_DATA_SIZE) {
		frame->function_code |= WIRELESS_MODBUS_EXCEPTION;
		return;
	}
	
	// Process command
	if (ram_map_read(frame->address, frame->data, frame->bytes_count) == false) {
		frame->function_code |= WIRELESS_MODBUS_EXCEPTION;
		return;
	}
}

//  ***************************************************************************
/// @brief  Function for processing ModBus write RAM command
/// @param  frame: pointer to wireless frame
/// @retval frame
//  ***************************************************************************
static void write_ram_command_handler(wireless_frame_t* frame) {
	
	// Check request parameters
	if (frame->bytes_count == 0 || frame->bytes_count > WIRELESS_MODBUS_FRAME_DATA_SIZE) {
		frame->function_code |= WIRELESS_MODBUS_EXCEPTION;
		return;
	}
	
	// Process command
	if (ram_map_write(frame->address, frame->data, frame->bytes_count) == false) {
		frame->function_code |= WIRELESS_MODBUS_EXCEPTION;
		return;
	}
	
	// Clear written data
	memset(frame->data, 0x00, WIRELESS_MODBUS_FRAME_SIZE);
}



//  ***************************************************************************
/// @brief  Calculate frame CRC16
/// @param  wireless_frame: pointer to wireless frame
/// @return CRC16 value
//  ***************************************************************************
static uint16_t calculate_crc16(const uint8_t* frame, uint32_t size) {
	
	uint16_t crc16 = 0xFFFF;
	uint16_t data = 0;
	uint16_t k = 0;
	
	while (size--) {
		crc16 ^= *frame++;
		k = 8;
		while (k--) {
			data = crc16;
			crc16 >>= 1;
			if (data & 0x0001) {
				crc16 ^= CRC16_POLYNOM;
			}
		}
	}
	return crc16;
}