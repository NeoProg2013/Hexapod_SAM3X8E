//  ***************************************************************************
/// @file    servo_driver.c
/// @author  NeoProg
//  ***************************************************************************
#include "servo_driver.h"

#include <sam.h>
#include <stdbool.h>
#include "pwm.h"
#include "veeprom.h"
#include "veeprom_map.h"
#include "error_handling.h"

#define SERVO_DIRECTION_CW                      (0x00)
#define SERVO_DIRECTION_CCW                     (0x01)
#define SERVO_BIDIRECTIONAL_MODE_DISABLE        (0x00)
#define SERVO_BIDIRECTIONAL_MODE_ENABLE         (0x02)

#define CALIBRATION_TABLE_MAX_SIZE              (28)
#define CALIBRATION_TABLE_STEP_SIZE             (10)


// Servo information
typedef struct {
    float    physic_angle;                                    // Current servo physic angle, [degree] 
    uint32_t config;                                        // Servo configuration
    uint32_t angle_currection;                                // Servo angle correction, [degree]
    uint32_t max_physic_angle;                                // Servo max physic angle, [degree]
    uint16_t calibration_table[CALIBRATION_TABLE_MAX_SIZE]; // Calibration table
} servo_info_t;


static servo_info_t   servo_channels[SUPPORT_SERVO_COUNT] = { 0 };


static bool read_configuration(void);
static uint32_t convert_angle_to_pulse_width(const servo_info_t* servo_info);


//  ***************************************************************************
/// @brief  Servo driver initialization
/// @return none
//  ***************************************************************************
void servo_driver_init(void) {
    
    if (read_configuration() == false) {
        callback_set_config_error(ERROR_MODULE_SERVO_DRIVER);
        return;
    }

    pwm_init();
    pwm_enable();
    
    // Move servos to start position
    for (uint32_t i = 0; i < SUPPORT_SERVO_COUNT; ++i) {
        servo_driver_move(i, 0);
    }
}

//  ***************************************************************************
/// @brief  Set servo angles update state
/// @param  state: update state
/// @return none
//  ***************************************************************************
void servo_driver_set_update_state(servo_driver_update_state_t state) {
    
    if (state == SERVO_DRIVER_UPDATE_ENABLE) {
        pwm_set_update_state(PWM_UPDATE_ENABLE);
    }
    else {
        pwm_set_update_state(PWM_UPDATE_DISABLE);
    }
}

//  ***************************************************************************
/// @brief  Start move servo to new angle
/// @param  ch:    servo channel
/// @param  angle: new angle
/// @return none
//  ***************************************************************************
void servo_driver_move(uint32_t ch, float angle) {
    
    if (callback_is_servo_driver_error_set() == true) return;
        
        
    if (ch >= SUPPORT_SERVO_COUNT) {
        callback_set_internal_error(ERROR_MODULE_SERVO_DRIVER);
        return;
    }
    
    servo_info_t* servo_info = &servo_channels[ch];
    
    // Calculate servo logical zero
    uint32_t logical_zero = 0;
    if ((servo_info->config & SERVO_CONFIG_BIDIRECTIONAL_MODE_MASK) == SERVO_BIDIRECTIONAL_MODE_ENABLE) {
        logical_zero = servo_info->max_physic_angle / 2;
    }
    
    // Calculate physic servo angle
    servo_info->physic_angle = logical_zero + angle;
    
    // Constrain physic servo angle
    if (servo_info->physic_angle < 0) {
        servo_info->physic_angle = 0;
    }
    
    // Apply angle correction
    servo_info->physic_angle += servo_info->angle_currection;
    
    // Constrain physic servo angle
    if (servo_info->physic_angle > servo_info->max_physic_angle) {
        servo_info->physic_angle = servo_info->max_physic_angle;
    }
    
    // Calculate and load pulse width
    uint32_t pulse_width = convert_angle_to_pulse_width(servo_info);
    pwm_set_width(ch, pulse_width);
}

//  ***************************************************************************
/// @brief  Servo driver process
/// @note   Call from main loop
//  ***************************************************************************
void servo_driver_process(void) {
    
    if (callback_is_servo_driver_error_set() == true) {
        pwm_disable();
    }
}





//  ***************************************************************************
/// @brief  Read configuration
/// @param  none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
    
    for (uint32_t servo_index = 0; servo_index < SUPPORT_SERVO_COUNT; ++servo_index) {
        
        uint32_t base_address = SERVO_CONFIGURATION_BASE_EE_ADDRESS + servo_index * SERVO_CONFIGURATION_SIZE;
        
        // Read servo configuration
        uint8_t config = veeprom_read_8(base_address + SERVO_CONFIG_OFFSET);
        if (config == 0xFF) {
            return false;
        }
        
        // Read angle correction
        uint32_t angle_correction = veeprom_read_8(base_address + SERVO_ANGLE_CORRECTION_OFFSET);
        if (angle_correction == 0xFF) {
            return false;
        }
        
        // Read max physic angle
        uint32_t max_physic_angle = veeprom_read_16(base_address + SERVO_MAX_PHYSIC_ANGLE_OFFSET);
        if (max_physic_angle == 0xFFFF || angle_correction > max_physic_angle) {
            return false;
        }
        
        // Fill main information
        servo_channels[servo_index].physic_angle = 0;
        servo_channels[servo_index].config = config;
        servo_channels[servo_index].angle_currection = angle_correction;
        servo_channels[servo_index].max_physic_angle = max_physic_angle;

        // Read calibration table
        uint32_t max_table_point = max_physic_angle / CALIBRATION_TABLE_STEP_SIZE;
        for (uint32_t i = 0; i <= max_table_point; ++i) {
            
            servo_channels[servo_index].calibration_table[i] = veeprom_read_16(base_address + SERVO_CALIBRATION_TABLE_OFFSET + i * 2);
            
            if (servo_channels[servo_index].calibration_table[i] == 0xFFFF) {
                return false;
            }
        }
    }

    return true;
}

//  ***************************************************************************
/// @brief  Convert servo angle to PWM pulse width
/// @param  servo_info: servo info @ref servo_info_t
/// @return PWM pulse width
//  ***************************************************************************
static uint32_t convert_angle_to_pulse_width(const servo_info_t* servo_info) {
    
    float angle = servo_info->physic_angle;
    if ((servo_info->config & SERVO_CONFIG_ROTATE_DIRECTION_MASK) == SERVO_DIRECTION_CCW) {
        angle = servo_info->max_physic_angle - angle;
    }
    
    uint32_t table_index = angle / CALIBRATION_TABLE_STEP_SIZE;

    if (angle < servo_info->max_physic_angle) {
        
        // Read pulse width for linear interpolate
        float first_value = servo_info->calibration_table[table_index];
        float second_value = servo_info->calibration_table[table_index + 1];
        
        // Linear interpolate
        float step = (second_value - first_value) / CALIBRATION_TABLE_STEP_SIZE;
        return first_value + step * ((uint32_t)angle % 10);
    }
    
    // Servo has max angle - set max pulse width
    return servo_info->calibration_table[table_index];
}