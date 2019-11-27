//  ***************************************************************************
/// @file    usart3_pdc.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include "usart3_pdc.h"
#define TX_PIN                              (PIO_PD4)
#define RX_PIN                              (PIO_PD5)
#define INTERNAL_TX_BUFFER_SIZE             (128)
#define INTERNAL_RX_BUFFER_SIZE             (128)


static uint8_t internal_tx_buffer[INTERNAL_TX_BUFFER_SIZE] = { 0 };
static uint8_t internal_rx_buffer[INTERNAL_RX_BUFFER_SIZE] = { 0 };


//  ***************************************************************************
/// @brief    Initialization USART
/// @note    Mode 8N1
/// @param  baud_rate: USART baud rate
//  ***************************************************************************
void usart3_init(uint32_t baud_rate) {

    // Enable clock
    REG_PMC_PCER0 = PMC_PCER0_PID20;
    while ((REG_PMC_PCSR0 & PMC_PCER0_PID20) == 0);
    
    // Configure TX as output (B peripheral function) without pull-up
    REG_PIOD_PER   = TX_PIN;
    REG_PIOD_OER   = TX_PIN;
    REG_PIOD_PUDR  = TX_PIN;
    REG_PIOD_PDR   = TX_PIN;
    REG_PIOD_ABSR |= TX_PIN;

    // Configure RX as input (B peripheral function) with pull-up
    REG_PIOD_PER   = RX_PIN;
    REG_PIOD_ODR   = RX_PIN;
    REG_PIOD_PUER  = RX_PIN;
    REG_PIOD_PDR   = RX_PIN;
    REG_PIOD_ABSR |= RX_PIN;

    // Disable PDC channels and reset TX and RX
    REG_USART3_PTCR = US_PTCR_TXTDIS | US_PTCR_RXTDIS;
    REG_USART3_CR = US_CR_RSTTX | US_CR_RSTRX | US_CR_RSTSTA;

    // Configure 8N1 mode
    REG_USART3_MR = US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT | US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK | US_MR_CHMODE_NORMAL;

    // Disable all interrupts
    REG_USART3_IDR = 0xFFFFFFFF;
    NVIC_EnableIRQ(USART3_IRQn);

    // Configure PDC channels
    REG_USART3_TCR = 0;
    REG_USART3_TPR = (uint32_t)internal_tx_buffer;
    REG_USART3_RCR = 0;
    REG_USART3_RPR = (uint32_t)internal_rx_buffer;

    // Configure baud rate
    usart3_set_baud_rate(baud_rate);
}

//  ***************************************************************************
/// @brief    Set USART baud rate
/// @param    baud_rate: USART baud rate
//  ***************************************************************************
void usart3_set_baud_rate(uint32_t baud_rate) {
    
    // Disable PDC channels and reset TX and RX
    REG_USART3_PTCR = US_PTCR_TXTDIS | US_PTCR_RXTDIS;
    REG_USART3_CR = US_CR_RSTTX | US_CR_RSTRX | US_CR_RSTSTA;
    
    //
    // Configure baud rate
    //
    uint32_t CD = (SystemCoreClock / baud_rate) / 16;
    uint8_t best_FP = 0;
    float best_error = 100;
    for (uint8_t FP = 0; FP < 8; ++FP) {
        
        float actual_baud_rate = SystemCoreClock / (16.0 * (CD + FP / 8.0));
        float error = (1.0 - baud_rate / actual_baud_rate) * 100.0;
        if (error < 0) {
            error *= -1;
        }
        
        if (error < best_error) {
            best_error = error;
            best_FP = FP;
        }
    }
    REG_USART3_BRGR = (best_FP << 16) | CD;
    
    // Enable TX and RX
    REG_USART3_CR = US_CR_TXEN | US_CR_RXEN;
}

//  ***************************************************************************
/// @brief    Check USART errors
/// @note    Check overrun error, framing error, parity error. Need reset USART
/// @return    true - error, false - no errors
//  ***************************************************************************
bool usart3_is_error(void) {
    uint32_t reg = REG_USART3_CSR;
    return (reg & (US_CSR_OVRE | US_CSR_FRAME | US_CSR_PARE));
}

//  ***************************************************************************
/// @brief    Reset USART
/// @note    Reset status register, reset transmitter and receiver
/// @param    is_reset_transmitter: true - reset transmitter
/// @param    is_reset_receiver: true - reset receiver
//  ***************************************************************************
void usart3_reset(bool is_reset_transmitter, bool is_reset_receiver) {

    if (is_reset_transmitter == true) {

        // Disable PDC channel and reset TX
        REG_USART3_PTCR = US_PTCR_TXTDIS;
        REG_USART3_CR = US_CR_RSTTX | US_CR_RSTSTA;

        // Reset PDC channel
        REG_USART3_TCR = 0;
        REG_USART3_TPR = (uint32_t)internal_tx_buffer;

        // Enable TX
        REG_USART3_CR = US_CR_TXEN;
    }

    if (is_reset_receiver == true) {

        // Disable PDC channel and reset RX
        REG_USART3_PTCR = US_PTCR_RXTDIS;
        REG_USART3_CR = US_CR_RSTRX | US_CR_RSTSTA;

        // Disable all interrupts
        REG_USART3_IDR = 0xFFFFFFFF;

        // Reset PDC channel
        REG_USART3_RCR = 0;
        REG_USART3_RPR = (uint32_t)internal_rx_buffer;

        // Enable RX
        REG_USART3_CR = US_CR_RXEN;
    }
}

//  ***************************************************************************
/// @brief    Start asynchronous transmit
/// @param    bytes_count: bytes count for transmit
//  ***************************************************************************
void usart3_start_tx(uint32_t bytes_count) {
    
    if (bytes_count > INTERNAL_TX_BUFFER_SIZE) {
        bytes_count = INTERNAL_TX_BUFFER_SIZE;
    }

    // Initialize DMA for transfer 
    REG_USART3_PTCR = US_PTCR_TXTDIS;
    REG_USART3_TPR = (uint32_t)internal_tx_buffer;
    REG_USART3_TCR = bytes_count;
    REG_USART3_PTCR = US_PTCR_TXTEN;
}

//  ***************************************************************************
/// @brief    Check transmit complete
/// @return None
//  ***************************************************************************
bool usart3_is_tx_complete(void) {
    uint32_t reg = REG_USART3_CSR;
    return (reg & US_CSR_TXEMPTY);
}

//  ***************************************************************************
/// @brief    Get internal TX buffer address
/// @return Buffer address
//  ***************************************************************************
uint8_t* usart3_get_internal_tx_buffer_address(void) {
    return internal_tx_buffer;
}



//  ***************************************************************************
/// @brief    Start asynchronous receive
/// @param  external_rx_buffer: pointer to external buffer
/// @param  external_buffer_size: external buffer size
/// @param    none
//  ***************************************************************************
void usart3_start_rx(uint8_t* external_rx_buffer, uint32_t external_buffer_size) {

    // Disable DMA
    REG_USART3_PTCR = US_PTCR_RXTDIS;

    // Initialize frame timeout
    REG_USART3_RTOR = 35 * 8;
    REG_USART3_CR |= US_CR_STTTO;
    REG_USART3_IER = US_IER_TIMEOUT;

    // Initialize DMA for receive
    if (external_rx_buffer == USART3_USE_INTERNAL_BUFFER) {
        REG_USART3_RPR = (uint32_t)internal_rx_buffer;
        REG_USART3_RCR = INTERNAL_RX_BUFFER_SIZE;
    }
    else {
        REG_USART3_RPR = (uint32_t)external_rx_buffer;
        REG_USART3_RCR = external_buffer_size;
    }

    // Enable DMA
    REG_USART3_PTCR = US_PTCR_RXTEN;
}

//  ***************************************************************************
/// @brief    Check frame receive complete
/// @return    true - frame received, false - no
//  ***************************************************************************
bool usart3_is_frame_received(void) {
    uint32_t reg = REG_USART3_CSR;
    return reg & US_CSR_TIMEOUT;
}

//  ***************************************************************************
/// @brief    Get received frame size
/// @return    Frame size
//  ***************************************************************************
uint32_t usart3_get_frame_size(void) {
    return INTERNAL_RX_BUFFER_SIZE - REG_USART3_RCR;
}

//  ***************************************************************************
/// @brief    Get internal RX buffer address
/// @return Buffer address
//  ***************************************************************************
const uint8_t* usart3_get_internal_rx_buffer_address(void) {
    return internal_rx_buffer;
}



//  ***************************************************************************
/// @brief    USART3 ISR
/// @note    This for only frame timeout IRQ
//  ***************************************************************************
void USART3_Handler(void) {
    
    // Frame received - disable DMA and frame timeout IRQ
    REG_USART3_IDR = US_IDR_TIMEOUT;
    REG_USART3_PTCR = US_PTCR_RXTDIS;
}