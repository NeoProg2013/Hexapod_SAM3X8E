//  ***************************************************************************
/// @file    systimer.h
/// @author  NeoProg
/// @brief   System timer (1 kHz)
//  ***************************************************************************
#ifndef SYSTIMER_H_
#define SYSTIMER_H_


extern void systimer_init(void);
extern uint32_t get_time_ms(void);
extern uint32_t get_time_us(void);
extern void delay_ms(uint32_t ms);


#endif /* SYSTIMER_H_ */