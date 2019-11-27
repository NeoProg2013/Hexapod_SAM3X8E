//  ***************************************************************************
/// @file    dac.h
/// @author  NeoProg
/// @brief   DAC driver
//  ***************************************************************************
#ifndef DAC_H_
#define DAC_H_

#include <stdint.h>


extern void dac_init(void);
extern void dac_set_output_value(uint32_t ch, uint16_t value);


#endif // DAC_H_