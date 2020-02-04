//  ***************************************************************************
/// @file    hc-sr04.h
/// @author  NeoProg
/// @brief   HC-SR04 driver
//  ***************************************************************************
#ifndef HC_SR04_H_
#define HC_SR04_H_

#include <stdint.h>
#include <stdbool.h>


extern void hcsr04_init(void);
extern void hcsr04_start(void);
extern void hcsr04_stop(void);
extern void hcsr04_process(void);
extern uint32_t hcsr04_get_distance(void);


#endif /* HC_SR04_H_ */