//  ***************************************************************************
/// @file    monitoring.h
/// @author  NeoProg
/// @brief   Monitoring subsystem
//  ***************************************************************************
#ifndef MONITORING_H_
#define MONITORING_H_

#include <stdint.h>
#include <stdbool.h>


extern uint8_t wireless_voltage;
extern uint8_t sensors_voltage;
extern uint8_t battery_voltage;


extern void monitoring_init(void);
extern void monitoring_process(void);
extern bool monitoring_is_low_battery_voltage(void);


#endif /* MONITORING_H_ */