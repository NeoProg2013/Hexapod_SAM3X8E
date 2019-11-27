//  ***************************************************************************
/// @file    orientation.c
/// @author  NeoProg
//  ***************************************************************************
#include "orientation.h"

#include <sam.h>
#include "systimer.h"
#include "hc-sr04.h"
#include "error_handling.h"

#define OS_FRONT_SENSOR             (0x01)


orientation_t current_orientation;
uint8_t orientation_sensors_status = 0;


//  ***************************************************************************
/// @brief  Initialization orientation subsystem
/// @return none
//  ***************************************************************************
void orientation_init(void) {
    
    hcsr04_init();
    hcsr04_start();
    
    orientation_sensors_status = 0;
    orientation_sensors_status |= OS_FRONT_SENSOR;
}

//  ***************************************************************************
/// @brief  Process orientation subsystem
/// @return none
//  ***************************************************************************
void orientation_process(void) {
    
    hcsr04_process();
    current_orientation.front_distance = hcsr04_get_distance();
}

//  ***************************************************************************
/// @brief  Disable front distance sensor
/// @return none
//  ***************************************************************************
void orientation_set_front_distance_sensor_state(bool is_enable) {
    
    if (is_enable == true) {
        orientation_sensors_status |= OS_FRONT_SENSOR;
        hcsr04_start();
    }
    else {
        orientation_sensors_status &= ~OS_FRONT_SENSOR;
        hcsr04_stop();
    }
}