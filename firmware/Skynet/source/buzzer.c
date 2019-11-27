//  ***************************************************************************
/// @file    buzzer.c
/// @author  NeoProg
//  ***************************************************************************
#include "buzzer.h"

#include <sam.h>
#include <stdint.h>
#include "dac.h"
#include "systimer.h"
#include "error_handling.h"

#define LOW_BATTERY_VOLTAGE_BEEP_ENABLE_TIME        (150)    // ms
#define LOW_BATTERY_VOLTAGE_BEEP_DISABLE_TIME       (500)    // ms


typedef enum {
    BEEP_STATE_CHECK,
    BEEP_STATE_ENABLE,
    BEEP_STATE_ENABLE_DELAY,
    BEEP_STATE_DISABLE,
    BEEP_STATE_DISABLE_DELAY
} beep_state_t;



//  ***************************************************************************
/// @brief  Buzzer initialization
/// @param  none
/// @return none
//  ***************************************************************************
void buzzer_init(void) {
    
    dac_init();
    dac_set_output_value(0, 4090);
    delay_ms(LOW_BATTERY_VOLTAGE_BEEP_ENABLE_TIME);
    dac_set_output_value(0, 0);
}

//  ***************************************************************************
/// @brief  Buzzer process (for main loop)
/// @param  none
/// @return none
//  ***************************************************************************
void buzzer_process(void) {
    
    static beep_state_t beep_state = BEEP_STATE_CHECK;
    static uint32_t prev_time = 0;
    static uint32_t beep_count = 0;
    
    switch (beep_state) {
        
        case BEEP_STATE_CHECK:
            if (callback_is_voltage_error_set() == true) {
                beep_state = BEEP_STATE_ENABLE;
            }
            break;
        
        case BEEP_STATE_ENABLE:
            prev_time = get_time_ms();
            dac_set_output_value(0, 4090);
            beep_state = BEEP_STATE_ENABLE_DELAY;
            break;
        
        case BEEP_STATE_ENABLE_DELAY:
            if (get_time_ms() - prev_time > LOW_BATTERY_VOLTAGE_BEEP_ENABLE_TIME) {
                beep_state = BEEP_STATE_DISABLE;
            }
            break;
        
        case BEEP_STATE_DISABLE:
            prev_time = get_time_ms();
            dac_set_output_value(0, 0);
            beep_state = BEEP_STATE_DISABLE_DELAY;
            break;
        
        case BEEP_STATE_DISABLE_DELAY:
            if (get_time_ms() - prev_time > LOW_BATTERY_VOLTAGE_BEEP_DISABLE_TIME) {
                
                if (++beep_count >= 3) {
                    beep_count = 0;
                    beep_state = BEEP_STATE_CHECK;
                }
                else {
                    beep_state = BEEP_STATE_ENABLE;
                }
            }
            break;
    }
}