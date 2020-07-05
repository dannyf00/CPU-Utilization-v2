#ifndef _UTIL_H
#define _UTIL_H

#include "tmr0.h"							//we use tmr0 as tick generator

//hardware configuration
//end hardware configuration

//global defines
#define UTIL_TICKS01	(16)				//2's compliments, number of ticks at 0.1% cpu load -> 1ms loop cycle time. 100% @ UTIL_TICKS01*1000

//no error checking implemented below
#define T2UTIL(ticks)	((ticks) / UTIL_TICKS01)	//convert ticks to 0..100.0%

//global variables
extern volatile uint32_t util_tick;			//tick counter, an up counter

#if 0										//user to implement
//tmr0 isr
void interrupt isr(void) {
	if (T0IF) {
		T0IF = 0;							//clera the flag
		util_tick+=0x100;					//increment the tick counter  - tmr0 is 8bit. each tick = 4us
	}
}
#endif

//reading the ticks
uint32_t ticks(void);
	
//initialize cpu utilization meter
void util_init(void);

//read tick
uint32_t util_get(void);

//fast convertion
uint16_t t2util(uint32_t ticks);

#endif	//_UTIL_H
