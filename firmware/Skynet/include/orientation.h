//  ***************************************************************************
/// @file    orientation.h
/// @author  NeoProg
/// @brief   Orientation subsystem
//  ***************************************************************************
#ifndef ORIENTATION_H_
#define ORIENTATION_H_

#include <stdint.h>
#include <stdbool.h>


typedef struct {
    uint32_t front_distance;
} orientation_t;


extern orientation_t current_orientation;
extern uint8_t orientation_sensors_status;


extern void orientation_init(void);
extern void orientation_process(void);
extern void orientation_set_front_distance_sensor_state(bool is_enable);


#endif /* ORIENTATION_H_ */