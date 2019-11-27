//  ***************************************************************************
/// @file    systimer.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include "systimer.h"


static volatile uint32_t systime_ms = 0;


//  ***************************************************************************
/// @brief  System timer initialize
/// @param  none
/// @return none
//  ***************************************************************************
void systimer_init(void) {
    
    if (SysTick_Config(SystemCoreClock / 1000)) { 
        while (1);
    }
}

//  ***************************************************************************
/// @brief  Get current time in milliseconds
/// @param  none
/// @return Milliseconds
//  ***************************************************************************
uint32_t get_time_ms(void) {
    return systime_ms;
}

//  ***************************************************************************
/// @brief  Get current time in microseconds
/// @param  none
/// @return Microseconds
//  ***************************************************************************
uint32_t get_time_us(void) {
    
     uint32_t ticks = 0;
     uint32_t count = 0;

     SysTick->CTRL;
     do {
         ticks = SysTick->VAL;
         count = systime_ms;
     } 
     while (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk);

     return count * 1000 + (SysTick->LOAD + 1 - ticks) / (SystemCoreClock / 1000000) ;
}

//  ***************************************************************************
/// @brief  Synchronous delay
/// @param  ms: time delay [ms]
/// @return none
//  ***************************************************************************
void delay_ms(uint32_t ms) {
    
    uint32_t start = systime_ms;
    while (systime_ms - start < ms);
}



//  ***************************************************************************
/// @brief  Systimer ISR (1kHz)
/// @return none
//  ***************************************************************************
void SysTick_Handler(void) {
    ++systime_ms;
}