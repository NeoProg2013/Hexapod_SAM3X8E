//  ***************************************************************************
/// @file    usart0_pdc.h
/// @author  NeoProg
/// @brief   USART0 driver
//  ***************************************************************************
#ifndef USART0_PDC_H_
#define USART0_PDC_H_

#include <stdbool.h>


#define USART0_USE_INTERNAL_BUFFER            (0x00000000)


void           usart0_init(uint32_t baud_rate);
void           usart0_set_baud_rate(uint32_t baud_rate);
void           usart0_reset(bool is_reset_transmitter, bool is_reset_receiver);
bool           usart0_is_error(void);

void           usart0_start_tx(uint32_t bytes_count);
bool           usart0_is_tx_complete(void);
uint8_t*       usart0_get_internal_tx_buffer_address(void);

void           usart0_start_rx(uint8_t* external_rx_buffer, uint32_t external_buffer_size);
bool           usart0_is_frame_received(void);
uint32_t       usart0_get_frame_size(void);
const uint8_t* usart0_get_internal_rx_buffer_address(void);


#endif // USART0_PDC_H_