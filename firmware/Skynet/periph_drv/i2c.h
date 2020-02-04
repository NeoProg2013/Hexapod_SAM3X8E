//  ***************************************************************************
/// @file    i2c.h
/// @author  NeoProg
/// @brief   I2C bus driver
//  ***************************************************************************
#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stdbool.h>


typedef enum {
    I2C_SPEED_100KHZ = 0x1D0D0,
    I2C_SPEED_400KHZ = 0x6565
} i2c_speed_t;


extern void i2c_init(i2c_speed_t speed);

extern bool i2c_is_async_operation_complete(void);
extern bool i2c_get_async_operation_status(void);

extern bool i2c_write_bits(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t mask, uint8_t bits);
extern bool i2c_write_byte(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t data);
extern bool i2c_write_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* data, uint32_t bytes_count);
extern bool i2c_async_write_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* data, uint32_t bytes_count);

extern bool i2c_read_byte(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* buffer);
extern bool i2c_read_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* buffer, uint32_t bytes_count);
extern bool i2c_async_read_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t internal_addr_length, uint8_t* buffer, uint32_t bytes_count);

extern uint8_t* i2c_get_internal_tx_buffer_address(void);
extern uint8_t* i2c_get_internal_rx_buffer_address(void);


#endif // I2C_H_