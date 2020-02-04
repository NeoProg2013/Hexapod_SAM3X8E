//  ***************************************************************************
/// @file    scr.h
/// @author  NeoProg
/// @brief   Special command register
//  ***************************************************************************
#ifndef SCR_H_
#define SCR_H_

#include <stdint.h>


extern uint8_t scr;
extern int32_t scr_argument;


extern void scr_process(void);


#endif /* SCR_H_ */