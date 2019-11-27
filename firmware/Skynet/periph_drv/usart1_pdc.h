//  ***************************************************************************
/// @file    usart1_pdc.h
/// @author  NeoProg
/// @brief   USART1 driver
//  ***************************************************************************
#ifndef USART1_PDC_H_
#define USART1_PDC_H_

#include <stdbool.h>


#define USART1_USE_INTERNAL_BUFFER            (0x00000000)


void           usart1_init(uint32_t baud_rate);
void           usart1_set_baud_rate(uint32_t baud_rate);
void           usart1_reset(bool is_reset_transmitter, bool is_reset_receiver);
bool           usart1_is_error(void);

void           usart1_start_tx(uint32_t bytes_count);
bool           usart1_is_tx_complete(void);
uint8_t*       usart1_get_internal_tx_buffer_address(void);

void           usart1_start_rx(uint8_t* external_rx_buffer, uint32_t external_buffer_size);
bool           usart1_is_frame_received(void);
uint32_t       usart1_get_frame_size(void);
const uint8_t* usart1_get_internal_rx_buffer_address(void);


#endif // USART1_PDC_H_
