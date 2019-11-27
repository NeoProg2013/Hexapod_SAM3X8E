//  ***************************************************************************
/// @file    servo_driver.h
/// @author  NeoProg
/// @brief   Servo driver
//  ***************************************************************************
#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

#include <stdint.h>


#define SUPPORT_SERVO_COUNT                        (18)

typedef enum {
    SERVO_DRIVER_UPDATE_DISABLE,
    SERVO_DRIVER_UPDATE_ENABLE
} servo_driver_update_state_t;


extern void servo_driver_init(void); 
extern void servo_driver_set_update_state(servo_driver_update_state_t state);
extern void servo_driver_move(uint32_t ch, float angle);
extern void servo_driver_process(void);


#endif // SERVO_DRIVER_H_