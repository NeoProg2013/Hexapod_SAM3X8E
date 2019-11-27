//  ***************************************************************************
/// @file    usart3_pdc.h
/// @author  NeoProg
/// @brief   USART3 driver
//  ***************************************************************************
#ifndef USART3_PDC_H_
#define USART3_PDC_H_

#include <stdbool.h>


#define USART3_USE_INTERNAL_BUFFER            (0x00000000)


void           usart3_init(uint32_t baud_rate);
void           usart3_set_baud_rate(uint32_t baud_rate);
void           usart3_reset(bool is_reset_transmitter, bool is_reset_receiver);
bool           usart3_is_error(void);

void           usart3_start_tx(uint32_t bytes_count);
bool           usart3_is_tx_complete(void);
uint8_t*       usart3_get_internal_tx_buffer_address(void);

void           usart3_start_rx(uint8_t* external_rx_buffer, uint32_t external_buffer_size);
bool           usart3_is_frame_received(void);
uint32_t       usart3_get_frame_size(void);
const uint8_t* usart3_get_internal_rx_buffer_address(void);


#endif // USART3_PDC_H_