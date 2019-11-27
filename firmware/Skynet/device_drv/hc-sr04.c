//  ***************************************************************************
/// @file    hc-sr04.c
/// @author  NeoProg
//  ***************************************************************************
#include "hc-sr04.h"

#include <sam.h>
#include <stdbool.h>
#include "systimer.h"

#define HCSR04_TRIG_PIN                             (PIO_PB17)
#define HCSR04_ECHO_PIN                             (PIO_PA2)
#define HCSR04_TRIG_PULSE_WIDTH                     (1)         // ms
#define HCSR04_TIME_BETWEEN_MEAS                    (30)        // ms
#define HCSR04_MEAS_TIMEOUT                         (200)       // ms
#define HCSR04_DISTANCE_DEFAULT_VALUE               (0xFFFFFFFF)
#define HCSR04_ACCUMULATION_COUNT                   (5)


typedef enum {
    STATE_IDLE,
    STATE_TRIG_RISE,
    STATE_TRIG_FALL,
    STATE_MEAS,
    STATE_CALCULATE,
    STATE_DELAY
} driver_state_t;


static driver_state_t driver_state = STATE_IDLE;
static uint32_t distance = HCSR04_DISTANCE_DEFAULT_VALUE;


//  ***************************************************************************
/// @brief  HC-SR04 initialization
/// @return none
//  ***************************************************************************
void hcsr04_init(void) {

    // Initialize ECHO pin
    REG_PIOA_PDR   = HCSR04_ECHO_PIN;
    REG_PIOA_PUDR  = HCSR04_ECHO_PIN;
    REG_PIOA_ABSR &= ~(HCSR04_ECHO_PIN);

    // Initialize TRIG pin
    REG_PIOB_PER  = HCSR04_TRIG_PIN;
    REG_PIOB_PUDR = HCSR04_TRIG_PIN;
    REG_PIOB_OER  = HCSR04_TRIG_PIN;
    REG_PIOB_CODR = HCSR04_TRIG_PIN;
    
    // Initialize timer in capture mode
    REG_PMC_PCER0 |= PMC_PCER0_PID28;
    REG_TC0_CMR1 = TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_LDBSTOP | TC_CMR_LDRA_RISING | TC_CMR_LDRB_FALLING;

    driver_state = STATE_IDLE;
}

//  ***************************************************************************
/// @brief  HC-SR04 start continuous measurements
/// @return none
//  ***************************************************************************
void hcsr04_start(void) {
    
    if (driver_state == STATE_IDLE) {
        driver_state = STATE_TRIG_RISE;
    }
}

//  ***************************************************************************
/// @brief  HC-SR04 stop measurements
/// @return none
//  ***************************************************************************
void hcsr04_stop(void) {

    driver_state = STATE_IDLE;
}


//  ***************************************************************************
/// @brief  HC-SR04 driver process
/// @return none
//  ***************************************************************************
void hcsr04_process(void) {
    
    static uint32_t start_time = 0;
    static float meas_acc = 0;
    static uint32_t acc_counter = 0;
    
    switch (driver_state) {
        
        case STATE_IDLE:
            start_time = 0;
            meas_acc = 0;
            acc_counter = 0;
            distance = HCSR04_DISTANCE_DEFAULT_VALUE;
            break;
        
        case STATE_TRIG_RISE:
            REG_PIOB_SODR = HCSR04_TRIG_PIN;
            start_time = get_time_ms();
            driver_state = STATE_TRIG_FALL;
            break;
            
        case STATE_TRIG_FALL:
            if (get_time_ms() - start_time >= HCSR04_TRIG_PULSE_WIDTH) {
                
                // Reset timer and fall ECHO pulse
                REG_TC0_SR1;
                REG_TC0_CCR1 = TC_CCR_SWTRG | TC_CCR_CLKEN;
                REG_PIOB_CODR = HCSR04_TRIG_PIN;
                
                start_time = get_time_ms();
                driver_state = STATE_MEAS;
            }
            break;
            
        case STATE_MEAS:
            if (REG_TC0_SR1 & TC_SR_LDRBS) {
                
                uint32_t rise_time = REG_TC0_RA1;
                uint32_t fall_time = REG_TC0_RB1;
                uint32_t pulse_time_us = (fall_time - rise_time) / 42;
                meas_acc += (pulse_time_us / 58.0f) * 10;
                ++acc_counter;
                
                start_time = get_time_ms();
                driver_state = (acc_counter >= HCSR04_ACCUMULATION_COUNT) ? STATE_CALCULATE : STATE_DELAY;
            }
            else if (get_time_ms() - start_time >= HCSR04_MEAS_TIMEOUT) {
                
                distance = HCSR04_DISTANCE_DEFAULT_VALUE;
                meas_acc = 0;
                acc_counter = 0;
                start_time = get_time_ms();
                driver_state = STATE_DELAY;
            }
            break;
        
        case STATE_CALCULATE:
            distance = meas_acc / acc_counter;
            acc_counter = 0;
            meas_acc = 0;
            driver_state = STATE_DELAY;
            break;

        case STATE_DELAY:
            if (get_time_ms() - start_time >= HCSR04_TIME_BETWEEN_MEAS) {
                driver_state = STATE_TRIG_RISE;
            }
            break;
    }
}

//  ***************************************************************************
/// @brief  Calculate distance
/// @return Distance [mm]
//  ***************************************************************************
uint32_t hcsr04_get_distance(void) {
    
    return distance;
}
