//  ***************************************************************************
/// @file    led.c
/// @author  NeoProg
//  ***************************************************************************
#include "led.h"

#include <sam.h>
#include "error_handling.h"
#include "systimer.h"


typedef enum {
    GREEN_LED,
    YELLOW_LED,
    RED_LED
} led_id;


static uint32_t leds[] = {PIO_PC23, PIO_PC24, PIO_PC25};
    
static void led_enable(led_id id);


//  ***************************************************************************
/// @brief  Led module initialization
/// @param  none
//  ***************************************************************************
void led_init(void) {
    
    // Initialization GPIO
    REG_PIOC_PER  = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
    REG_PIOC_OER  = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
    REG_PIOC_CODR = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
}

//  ***************************************************************************
/// @brief  Led process
/// @param  none
//  ***************************************************************************
void led_process(void) {
    
    if (callback_is_emergency_mode_active() == true) {
        led_enable(RED_LED);
        return;
    }

    if (callback_is_any_error_set() == true) {
        led_enable(YELLOW_LED);
    }
    else {
        led_enable(GREEN_LED);;
    }
}





//  ***************************************************************************
/// @brief  Led enable
/// @param  none
//  ***************************************************************************
static void led_enable(led_id id) {
    
    if (id >= sizeof(leds) / sizeof(leds[0])) {
        return;
    }
    
    // LED already enabled
    if (REG_PIOC_ODSR & leds[id]) {
        return;
    }
    
    // Disable all LEDs and enable requested LED
    REG_PIOC_CODR = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
    REG_PIOC_SODR = leds[id];
}