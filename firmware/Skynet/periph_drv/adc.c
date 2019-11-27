//  ***************************************************************************
/// @file    adc.c
/// @author  NeoProg
///
/// ADC channels map
/// ADC_CHER_CH7     (A0)
/// ADC_CHER_CH6     (A1)
/// ADC_CHER_CH5     (A2)
/// ADC_CHER_CH4     (A3)
/// ADC_CHER_CH3     (A4)
/// ADC_CHER_CH2     (A5)
/// ADC_CHER_CH1     (A6)
/// ADC_CHER_CH0     (A7)
/// ADC_CHER_CH10    (A8)
/// ADC_CHER_CH11    (A9)
/// ADC_CHER_CH12    (A10)
/// ADC_CHER_CH13    (A11)
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#define SUPPORT_ADC_CHANNELS_COUNT                (3)

static uint16_t adc_data[SUPPORT_ADC_CHANNELS_COUNT] = { 0 };
    

//  ***************************************************************************
/// @brief  Initialize ADC
/// @note    none
/// @return none
//  ***************************************************************************
void adc_init(void) {

    // Enable ADC and PDC clock
    REG_PMC_PCER1 = PMC_PCER1_PID37;
    while ( (REG_PMC_PCSR1 & PMC_PCER1_PID37) == 0 );
    
    // Configure PIO
    REG_PIOA_PDR = PIO_PA16 | PIO_PA24 | PIO_PA23 | PIO_PA22 | PIO_PA6;

    // Reset ADC
    REG_ADC_CR = ADC_CR_SWRST;
    REG_ADC_PTCR = ADC_PTCR_RXTDIS | ADC_PTCR_TXTDIS;

    // Setup ADC
    REG_ADC_MR = ADC_MR_PRESCAL(SystemCoreClock / (2 * 20000000) - 1) | ADC_MR_STARTUP_SUT768 | 
                 ADC_MR_SETTLING_AST17 | ADC_MR_TRGEN_DIS;
    
    // Disable all IRQ
    REG_ADC_IDR = 0xFFFFFFFF;
    
    // Disable all channels
    REG_ADC_IDR = 0xFFFFFFFF;
    
    // Enable all support channels
    REG_ADC_CHER = ADC_CHER_CH7;    // (A0)
    REG_ADC_CHER = ADC_CHER_CH6;    // (A1)
    REG_ADC_CHER = ADC_CHER_CH5;    // (A2)
}

//  ***************************************************************************
/// @brief  Start ADC conversion with PDC
/// @note    none
/// @return none
//  ***************************************************************************
void adc_start_conversion(void) {

    // Initialize PDC channel for ADC
    REG_ADC_PTCR = ADC_PTCR_RXTDIS;
    REG_ADC_RPR  = (uint32_t)adc_data;
    REG_ADC_RCR  = SUPPORT_ADC_CHANNELS_COUNT;
    REG_ADC_PTCR = ADC_PTCR_RXTEN;

    // Start ADC conversion
    REG_ADC_CR = ADC_CR_START;
}

//  ***************************************************************************
/// @brief  Check conversion status
/// @note    none
/// @return true - conversion complete, false - conversion on progress
//  ***************************************************************************
bool adc_is_conversion_complete(void) {
    
    return (REG_ADC_RCR == 0);
}

//  ***************************************************************************
/// @brief  Get channel voltage
/// @param  ch: number
/// @return ADC channel voltage
//  ***************************************************************************
float adc_get_voltage(uint32_t ch) {
    
    if (ch >= SUPPORT_ADC_CHANNELS_COUNT) return 0;
    
    return (adc_data[ch] & 0x0FFF) * 3.3f / 4095.0f;
}
