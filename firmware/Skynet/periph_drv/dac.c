//  ***************************************************************************
/// @file    dac.c
/// @author  NeoProg
//  ***************************************************************************
#include "dac.h"

#include <sam.h>
#include <stdbool.h>
    

//  ***************************************************************************
/// @brief  Initialize DAC
/// @note    none
/// @return none
//  ***************************************************************************
void dac_init(void) {

    // Enable DAC clock
    REG_PMC_PCER1 = PMC_PCER1_PID38;
    while ( (REG_PMC_PCSR1 & PMC_PCER1_PID38) == 0 );

    // Software reset
    REG_DACC_CR = DACC_CR_SWRST;            

    // External trigger disabled
    // Half word transfer mode (16 bit)
    // Tag disabled
    // Refresh        - 0x08 (1024 * 8 dacc clocks)
    // Max speed mode - 0 (disabled)
    // Startup time   - 0x10 (1024 DAC clocks)
    // Channel           - 0
    REG_DACC_MR = DACC_MR_TRGEN_DIS | DACC_MR_WORD_HALF | DACC_MR_REFRESH(0x08) | DACC_MR_STARTUP_1920 |
                  DACC_MR_USER_SEL_CHANNEL0 | DACC_MR_TAG_DIS;
    REG_DACC_ACR |= DACC_ACR_IBCTLCH0(0x02) | DACC_ACR_IBCTLCH1(0x02); // ?
}

//  ***************************************************************************
/// @brief  Start ADC conversion with PDC
/// @param    ch: DAC channel
/// @param    value: DAC output value (0x0000 - 0x0FFF)
/// @return none
//  ***************************************************************************
void dac_set_output_value(uint32_t ch, uint16_t value) {
    
    if (ch == 0) {
        
        // Check channel enabled
        if ((REG_DACC_CHSR & DACC_CHSR_CH0) == 0) {
            REG_PIOB_PDR = PIO_PDR_P15;        // Disable PIO controller
            REG_DACC_CHER = DACC_CHER_CH0;    // Enable DAC channel
        }
        
        uint32_t mode_reg = REG_DACC_MR & (~DACC_MR_USER_SEL_Msk);    // Get MR without USER_SEL field
        REG_DACC_MR = mode_reg | DACC_MR_USER_SEL_CHANNEL0;            // Select channel 0
        
        REG_DACC_CDR = value;    // Write channel value
    }
    else if (ch == 1) {
        
        // Check channel enabled
        if ((REG_DACC_CHSR & DACC_CHSR_CH1) == 0) {
            REG_PIOB_PDR = PIO_PDR_P16;        // Disable PIO controller
            REG_DACC_CHER = DACC_CHER_CH1;    // Enable DAC channel
        }
        
        uint32_t mode_reg = REG_DACC_MR & (~DACC_MR_USER_SEL_Msk);    // Get MR without USER_SEL field
        REG_DACC_MR = mode_reg | DACC_MR_USER_SEL_CHANNEL1;            // Select channel 0
        
        REG_DACC_CDR = value;    // Write channel value
    }
}