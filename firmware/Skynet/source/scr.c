//  ***************************************************************************
/// @file    src.c
/// @author  NeoProg
//  ***************************************************************************
#include "scr.h"

#include <sam.h>
#include "movement_engine.h"
#include "veeprom.h"
#include "orientation.h"

#define SCR_CMD_SELECT_SEQUENCE_UP                      (0x01)
#define SCR_CMD_SELECT_SEQUENCE_DOWN                    (0x02)
#define SCR_CMD_SELECT_SEQUENCE_RUN                     (0x03)
#define SCR_CMD_SELECT_SEQUENCE_DIRECT_MOVEMENT         (0x04)
#define SCR_CMD_SELECT_SEQUENCE_REVERSE_MOVEMENT        (0x05)
#define SCR_CMD_SELECT_SEQUENCE_ROTATE_LEFT             (0x06)
#define SCR_CMD_SELECT_SEQUENCE_ROTATE_RIGHT            (0x07)
#define SCR_CMD_SELECT_SEQUENCE_DIRECT_MOVEMENT_SLOW    (0x08)
#define SCR_CMD_SELECT_SEQUENCE_REVERSE_MOVEMENT_SLOW   (0x09)
#define SCR_CMD_SELECT_SEQUENCE_SHIFT_LEFT              (0x10)
#define SCR_CMD_SELECT_SEQUENCE_SHIFT_RIGHT             (0x11)

#define SCR_CMD_SELECT_SEQUENCE_ATTACK_LEFT             (0x20)
#define SCR_CMD_SELECT_SEQUENCE_ATTACK_RIGHT            (0x21)
#define SCR_CMD_SELECT_SEQUENCE_DANCE                   (0x30)
#define SCR_CMD_SELECT_SEQUENCE_ROTATE_X                (0x31)
//#define SCR_CMD_SELECT_SEQUENCE_ROTATE_Y                (0x32)
#define SCR_CMD_SELECT_SEQUENCE_ROTATE_Z                (0x33)

#define SCR_CMD_SELECT_SEQUENCE_NONE                    (0x90)

#define SCR_CMD_CALCULATE_CHECKSUM                      (0xB0)
#define SCR_CMD_ENABLE_FRONT_SENSOR                     (0xB1)
#define SCR_CMD_DISABLE_FRONT_SENSOR                    (0xB2)
#define SCR_CMD_RESET                                   (0xFE)


uint8_t scr = 0;
int32_t scr_argument = 0;


//  ***************************************************************************
/// @brief    Special command register process
/// @return    none
//  ***************************************************************************
void scr_process(void) {
    
    switch (scr) {
        
        case SCR_CMD_SELECT_SEQUENCE_UP:
            movement_engine_select_sequence(SEQUENCE_UP);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_DOWN:
            movement_engine_select_sequence(SEQUENCE_DOWN);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_RUN:
            movement_engine_select_sequence(SEQUENCE_RUN);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_DIRECT_MOVEMENT:
            movement_engine_select_sequence(SEQUENCE_DIRECT_MOVEMENT);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_REVERSE_MOVEMENT:
            movement_engine_select_sequence(SEQUENCE_REVERSE_MOVEMENT);
            break;
        
        case SCR_CMD_SELECT_SEQUENCE_ROTATE_LEFT:
            movement_engine_select_sequence(SEQUENCE_ROTATE_LEFT);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_ROTATE_RIGHT:
            movement_engine_select_sequence(SEQUENCE_ROTATE_RIGHT);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_DIRECT_MOVEMENT_SLOW:
            movement_engine_select_sequence(SEQUENCE_DIRECT_MOVEMENT_SLOW);
            break;
        
        case SCR_CMD_SELECT_SEQUENCE_REVERSE_MOVEMENT_SLOW:
            movement_engine_select_sequence(SEQUENCE_REVERSE_MOVEMENT_SLOW);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_SHIFT_LEFT:
            movement_engine_select_sequence(SEQUENCE_SHIFT_LEFT);
            break;
       
        case SCR_CMD_SELECT_SEQUENCE_SHIFT_RIGHT:
            movement_engine_select_sequence(SEQUENCE_SHIFT_RIGHT);
            break;
            
            
            
        case SCR_CMD_SELECT_SEQUENCE_ATTACK_LEFT:
            movement_engine_select_sequence(SEQUENCE_ATTACK_LEFT);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_ATTACK_RIGHT:
            movement_engine_select_sequence(SEQUENCE_ATTACK_RIGHT);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_DANCE:
            movement_engine_select_sequence(SEQUENCE_DANCE);
            break;
            
        case SCR_CMD_SELECT_SEQUENCE_ROTATE_X:
            movement_engine_select_sequence(SEQUENCE_ROTATE_X);
            break;
            
        /*case SCR_CMD_SELECT_SEQUENCE_ROTATE_Y:
            movement_engine_select_sequence(SEQUENCE_ROTATE_Y);
            break;*/
            
        case SCR_CMD_SELECT_SEQUENCE_ROTATE_Z:
            movement_engine_select_sequence(SEQUENCE_ROTATE_Z);
            break;
            
            

        case SCR_CMD_SELECT_SEQUENCE_NONE:
            movement_engine_select_sequence(SEQUENCE_NONE);
            break;
            
            
            
        case SCR_CMD_ENABLE_FRONT_SENSOR:
            orientation_set_front_distance_sensor_state(true);
            break;
            
        case SCR_CMD_DISABLE_FRONT_SENSOR:
            orientation_set_front_distance_sensor_state(false);
            break;
        
        /*case SCR_CMD_CALCULATE_CHECKSUM:
            veeprom_update_checksum();
            break;*/
            
        case SCR_CMD_RESET:
            REG_RSTC_CR = 0xA5000005;
            break;
    }
    
    scr = 0x00;
}