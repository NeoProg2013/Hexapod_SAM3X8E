//  ***************************************************************************
/// @file    main.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdlib.h>
#include <stdbool.h>
#include "movement_engine.h"
#include "limbs_driver.h"
#include "servo_driver.h"
#include "monitoring.h"
#include "orientation.h"
#include "veeprom.h"
#include "modbus.h"
#include "scr.h"
#include "led.h"
#include "i2c.h"
#include "gui.h"
#include "buzzer.h"
#include "systimer.h"
#include "error_handling.h"


static void enter_to_emergency_loop(void);


//  ***************************************************************************
/// @brief  Normal mode loop
/// @param  none
/// @return none
//  ***************************************************************************
int main(void) {
    
    // Initialize SAM system
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS;
    
    // Enable PIOA, PIOB, PIOC, PIOD
    REG_PMC_PCER0 |= (PMC_PCER0_PID11 | PMC_PCER0_PID12 | PMC_PCER0_PID13 | PMC_PCER0_PID14);

    /*REG_PIOC_PER = PIO_PC22 | PIO_PC21 | PIO_PC29;
    REG_PIOC_OER = PIO_PC22 | PIO_PC21 | PIO_PC29;
    REG_PIOC_CODR = PIO_PC22 | PIO_PC21 | PIO_PC29;*/
    

    // Initialize FW
    systimer_init();
    led_init();
    i2c_init(I2C_SPEED_400KHZ);
    gui_init();
    veeprom_init();
    modbus_init();
    monitoring_init();
    orientation_init();
    
    servo_driver_init();
    limbs_driver_init();
    movement_engine_init();
    
    buzzer_init();
    
    while (1)  {
        
        //
        // CHECK SYSTEM STATUS
        //
        if (callback_is_emergency_mode_active() == true) {
            enter_to_emergency_loop();
        }
        if (callback_is_voltage_error_set() == true) {
            movement_engine_select_sequence(SEQUENCE_DOWN);
        }
        
        
        //
        // NORMAL MODE PROCESS
        //
        gui_process();
        led_process();
        buzzer_process();
        
        servo_driver_process();
        limbs_driver_process();
        movement_engine_process();
        
        modbus_process();
        scr_process();
        
        monitoring_process();
        orientation_process();
    }
}

//  ***************************************************************************
/// @brief  Emergency mode loop
/// @param  none
/// @return none
//  ***************************************************************************
static void enter_to_emergency_loop(void) {
    
    while (1)  {
        
        gui_process();
        led_process();
        
        modbus_process();
        scr_process();
        
        monitoring_process();
    }
}

void HardFault_Handler(void) {
    
    while (true);
}