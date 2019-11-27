//  ***************************************************************************
/// @file    i2c.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"
#include "systimer.h"
#define IS_BIT_SET(_status, _bit)           (((_status) & (_bit)) == (_bit))
#define SDA_PIN                             (PIO_PB12)
#define SCK_PIN                             (PIO_PB13)
#define MAX_BUFFER_SIZE                     (150)
#define I2C_TIMEOUT_MS                      (50)

typedef enum {
    I2C_DRIVER_NO_ERROR,
    I2C_DRIVER_BUSY,
    I2C_DRIVER_ERROR
} driver_status_t;


static uint8_t tx_buffer[MAX_BUFFER_SIZE] = { 0 };
static uint8_t rx_buffer[MAX_BUFFER_SIZE] = { 0 };

static uint32_t start_operation_time = 0;
static volatile driver_status_t driver_status = I2C_DRIVER_NO_ERROR;


static void stop_communication(void);


//  ***************************************************************************
/// @brief    Initialize GPIO and I2C registers in Master Mode
/// @param    clock_speed: I2C_SPEED_100KHZ or I2C_SPEED_400KHZ constants
/// @return    none
//  ***************************************************************************
void i2c_init(i2c_speed_t speed) {
    
    // Enable PDC clock
    REG_PMC_PCER1 = PMC_PCER1_PID39;
    while ((REG_PMC_PCSR1 & PMC_PCER1_PID39) == 0);

    // Enable I2C clock
    REG_PMC_PCER0 = PMC_PCER0_PID23;
    while ((REG_PMC_PCSR0 & PMC_PCER0_PID23) == 0);

    // Disable and reset I2C controller
    REG_TWI1_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS; // Disable PDC
    REG_TWI1_CR = TWI_CR_SWRST | TWI_CR_SVDIS | TWI_CR_MSDIS;
    
    // Send 9 pulses on SCL
    REG_PIOB_PER = SCK_PIN;
    REG_PIOB_OER = SCK_PIN;
    for (uint32_t i = 0; i < 9; ++i) {
        
        REG_PIOB_CODR = SCK_PIN;
        delay_ms(1);
        REG_PIOB_SODR = SCK_PIN;
        delay_ms(1);
    }

    // Configure SCK and SDA as A peripheral function
    REG_PIOB_PDR = SDA_PIN | SCK_PIN;

    // Configure I2C controller
    REG_TWI1_RHR; // Reset holding register
    REG_TWI1_CWGR = speed;
    REG_TWI1_CR = TWI_CR_MSEN | TWI_CR_SVDIS; 
    
    REG_TWI1_IDR = 0xFFFFFFFF;
    NVIC_EnableIRQ(TWI1_IRQn);
}

//  ***************************************************************************
/// @brief    Check asynchronous operation complete
/// @param    none
/// @return    true - operation complete, false - operation in progress
//  ***************************************************************************
bool i2c_is_async_operation_complete(void) {
    
    if (driver_status == I2C_DRIVER_BUSY) {
        
        if (get_time_ms() - start_operation_time > I2C_TIMEOUT_MS) {
            stop_communication();
            driver_status = I2C_DRIVER_ERROR;
        }
    }

    return driver_status != I2C_DRIVER_BUSY;
}

//  ***************************************************************************
/// @brief    Check asynchronous operation status
/// @param    none
/// @return    true - operation successfully complete, false - error
//  ***************************************************************************
bool i2c_get_async_operation_status(void) {
    
    return driver_status == I2C_DRIVER_NO_ERROR;
}



//  ***************************************************************************
/// @brief    Write bits to register
/// @param    dev_addr: I2C device address
/// @param    reg_addr: internal register address on I2C device
/// @param    mask: bits mask
/// @param    bits: bits for written
/// @retval    true - no error, false - error
//  ***************************************************************************
bool i2c_write_bits(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t mask, uint8_t bits) {

    uint8_t reg_data = 0;
    if (i2c_read_byte(dev_addr, internal_addr, internal_addr_length, &reg_data) == false) {
        return false;
    }

    reg_data &= ~mask;    // Clear bits
    reg_data |= bits;    // Write new bits

    return i2c_write_byte(dev_addr, internal_addr, internal_addr_length, reg_data);
}

//  ***************************************************************************
/// @brief    Synchronous write byte
/// @param    dev_addr: I2C device address
/// @param    internal_addr: internal register address on I2C device
/// @param    data: byte for write
/// @return    true - no error, false - error
//  ***************************************************************************
bool i2c_write_byte(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t data) {
    
    return i2c_write_bytes(dev_addr, internal_addr, internal_addr_length, &data, 1);
}

//  ***************************************************************************
/// @brief    Synchronous write data
/// @param    dev_addr: I2C device address
/// @param    internal_addr: internal address on I2C device
/// @param  internal_addr_length: internal address length
/// @param    data: pointer to data for write. If NULL - using internal TX buffer
/// @return    true - no error, false - error
//  ***************************************************************************
bool i2c_write_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* data, uint32_t bytes_count) {

    // Disable all current I2C communications and reset driver status
    stop_communication();
    driver_status = I2C_DRIVER_NO_ERROR;

    // Start transmit data and wait communication complete
    i2c_async_write_bytes(dev_addr, internal_addr, internal_addr_length, data, bytes_count);
    while (i2c_is_async_operation_complete() == false);

    // Return operation status
    return i2c_get_async_operation_status();
}

//  ***************************************************************************
/// @brief    Start asynchronous write data
/// @param    dev_addr: I2C device address
/// @param    internal_addr: internal register address on I2C device
/// @param    data: pointer to data for write (if == NULL, then using internal buffer)
/// @param    size: byte for write
/// @retval    true - operation start success, false - operation start fail
//  ***************************************************************************
bool i2c_async_write_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* data, uint32_t bytes_count) {

    // Check driver state
    if (i2c_is_async_operation_complete() == false) {
        return false;
    }

    // Disable all I2C interrupts
    REG_TWI1_IDR = 0xFFFFFFFF;

    // Configure TX PDC channel
    REG_TWI1_TPR = ((data == NULL) ? (uint32_t)tx_buffer : (uint32_t)data);
    REG_TWI1_TCR = bytes_count;

    // Configure Master mode (DADR | write mode | internal address length)
    REG_TWI1_MMR = (dev_addr << 16) | (0 << 12) | (internal_addr_length << 8);
    REG_TWI1_IADR = internal_addr;

    // Enable transmitter
    REG_TWI1_PTCR = TWI_PTCR_TXTEN | TWI_PTCR_RXTDIS;

    // Configure interrupts
    REG_TWI1_IER = TWI_IER_ENDTX | TWI_IER_NACK;

    driver_status = I2C_DRIVER_BUSY;
    start_operation_time = get_time_ms();
    return true;
}



//  ***************************************************************************
/// @brief    Synchronous read byte
/// @param    dev_addr: I2C device address
/// @param    internal_addr: internal register address on I2C device
/// @param    data: pointer to receive buffer
/// @retval    Byte value
//  ***************************************************************************
bool i2c_read_byte(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* buffer) {
    
    return i2c_read_bytes(dev_addr, internal_addr, internal_addr_length, buffer, 1);
}

//  ***************************************************************************
/// @brief    Synchronous read data
/// @note    This wrappers for asynchronous mode functions
/// @param    dev_addr: I2C device address
/// @param    internal_addr: internal register address on I2C device
/// @param    data: pointer to receive buffer
/// @size    data: bytes count for read
/// @retval    true - no error, false - error
//  ***************************************************************************
bool i2c_read_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* buffer, uint32_t bytes_count) {

    // Disable all current I2C communications
    stop_communication();
    driver_status = I2C_DRIVER_NO_ERROR;

    // Start receive data and wait communication complete
    i2c_async_read_bytes(dev_addr, internal_addr, internal_addr_length, buffer, bytes_count);
    while (i2c_is_async_operation_complete() == false);

    // Return operation status
    return i2c_get_async_operation_status();
}

//  ***************************************************************************
/// @brief    Start asynchronous read data
/// @note    This functions using PDC
/// @param    dev_addr: I2C device address
/// @param    internal_addr: internal register address on I2C device
/// @param    buffer: pointer to receive buffer (if == NULL, then using internal buffer)
/// @size    data: bytes count for read
/// @retval    true - operation start success, false - operation start fail
//  ***************************************************************************
bool i2c_async_read_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* buffer, uint32_t bytes_count) {

    // Check driver state
    if (driver_status == I2C_DRIVER_BUSY) {
        return false;
    }

    // Disable all I2C interrupts
    REG_TWI1_IDR = 0xFFFFFFFF;
    
    // Configure RX PDC channel
    REG_TWI1_RPR = ((buffer == NULL) ? (uint32_t)tx_buffer : (uint32_t)buffer);
    REG_TWI1_RCR = bytes_count - 1; // Without last byte (for send STOP)

    // Configure Master mode (DADR | read mode | internal address length)
    REG_TWI1_MMR = (dev_addr << 16) | TWI_MMR_MREAD | (internal_addr_length << 8);
    REG_TWI1_IADR = internal_addr;

    // Enable receiver
    REG_TWI1_PTCR = TWI_PTCR_TXTDIS | TWI_PTCR_RXTEN;

    // Send START
    REG_TWI1_CR = TWI_CR_START;

    // Configure interrupts
    REG_TWI1_IER = TWI_IER_ENDRX | TWI_IER_NACK;

    driver_status = I2C_DRIVER_BUSY;
    start_operation_time = get_time_ms();
    return true;
}



//  ***************************************************************************
/// @brief  Get internal TX buffer address
/// @return    Buffer address
//  ***************************************************************************
uint8_t* i2c_get_internal_tx_buffer_address(void) {
    
    return tx_buffer;
}

//  ***************************************************************************
/// @brief  Get internal RX buffer address
/// @return    Buffer address
//  ***************************************************************************
uint8_t* i2c_get_internal_rx_buffer_address(void) {
    
    return rx_buffer;
}



//  ***************************************************************************
/// @brief  Stop communication
/// @param    None
/// @return    None
//  ***************************************************************************
static void stop_communication(void) {

    // Disable PDC
    REG_TWI1_PTCR = TWI_PTCR_TXTDIS | TWI_PTCR_RXTDIS;
    REG_TWI1_RCR = 0;
    REG_TWI1_TCR = 0;

    // Disable all I2C interrupts
    REG_TWI1_IDR = 0xFFFFFFFF;

    // Reset status register
    REG_TWI1_SR;
}


//  ***************************************************************************
/// @brief  I2C bus ISR
/// @param    None
/// @return    None
//  ***************************************************************************
void TWI1_Handler(void) {

    uint32_t status = REG_TWI1_SR;
    uint32_t irq_mask = REG_TWI1_IMR;

    // Errors
    if (IS_BIT_SET(status, TWI_SR_NACK) && IS_BIT_SET(irq_mask, TWI_IMR_NACK)) {
        stop_communication();
        driver_status = I2C_DRIVER_ERROR;
    }
    
    // Send STOP complete
    else if (IS_BIT_SET(status, TWI_SR_TXCOMP) && IS_BIT_SET(irq_mask, TWI_IMR_TXCOMP)) {
        stop_communication();
        driver_status = I2C_DRIVER_NO_ERROR;
    }

    // TX handler
    else if (IS_BIT_SET(status, TWI_SR_ENDTX) && IS_BIT_SET(irq_mask, TWI_IMR_ENDTX)) {

        // Disable TX PDC channel
        REG_TWI1_PTCR = TWI_PTCR_TXTDIS;

        // Send STOP
        REG_TWI1_CR = TWI_CR_STOP;

        // Configure interrupts
        REG_TWI1_IDR = 0xFFFFFFFF;
        REG_TWI1_IER = TWI_IER_TXCOMP;
    }

    // RX handler
    else if (IS_BIT_SET(status, TWI_SR_ENDRX) && IS_BIT_SET(irq_mask, TWI_IMR_ENDRX)) {

        // Disable RX PDC channel
        REG_TWI1_PTCR = TWI_PTCR_RXTDIS;

        // Send STOP
        REG_TWI1_CR = TWI_CR_STOP;

        // Configure interrupts
        REG_TWI1_IDR = 0xFFFFFFFF;
        REG_TWI1_IER = TWI_IER_RXRDY;
    }

    // Receive last byte
    else if (IS_BIT_SET(status, TWI_SR_RXRDY) && IS_BIT_SET(irq_mask, TWI_IMR_RXRDY)) {

        // Read last byte
        uint8_t* buffer_addr = (uint8_t*)REG_TWI1_RPR;
        *buffer_addr = REG_TWI1_RHR;

        // Configure interrupts
        REG_TWI1_IDR = 0xFFFFFFFF;
        REG_TWI1_IER = TWI_IER_TXCOMP;
    }
}
