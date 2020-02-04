//  ***************************************************************************
/// @file    adc.h
/// @author  NeoProg
/// @brief   ADC driver
//  ***************************************************************************
#ifndef ADC_H_
#define ADC_H_


extern void adc_init(void);
extern void adc_start_conversion(void);
extern bool adc_is_conversion_complete(void);
extern float adc_get_voltage(uint32_t ch);


#endif // ADC_H_