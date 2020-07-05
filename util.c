#include "util.h"							//we use util measurement module

//hardware configuration
//end hardware configuration

//global defines


//global variables
volatile uint32_t util_tick=0;				//tick counter, an up counter

#if 0										//user code will need to implement this
//tmr0 isr
void interrupt isr(void) {
	if (T0IF) {
		T0IF = 0;							//clera the flag
		util_tick+=0x100;					//increment the tick counter  - tmr0 is 8bit
	}
}
#endif

//reading the ticks
//can be replaced by target-specific routines
uint32_t ticks(void) {
	uint32_t t0, t1;
	
	//perform a double read to ensure atomicity
	do {
		t0 = util_tick;						//read the msb
		t1 = TMR0;							//read the lsb
	} while (util_tick ^ t0);				//see if _tick has advanced
	return (util_tick | t1);				//return the combined value
}		
	
//initialize cpu utilization meter
void util_init(void) {
	util_tick = 0;							//initialize the tick counter
	
	//to be replaced with target-specific routines
	//initialize the tmr0 / tick generator
	//if the target has a tick generator and that it is initialized else where,
	//this section can be ignored
	//tmr0 is an 8-bit up counter, interrupt enabled
	tmr0_init(TMR0_PS_4x);					//initialize the tmr0 -> each tick is 4us @ 1Mhz timer input
	T0IF = 0;								//clera the flag
	T0IE = 1;								//enable the clock
	//TMR0 is running
}

//read tick
uint32_t util_get(void) {
	static uint32_t cnt=0;					//ticks since last read	
	static uint32_t tmp_smoothed=0;			//smoothed measurement
	uint32_t t0, tmp;						//read the current tick
	
	t0 = ticks();							//read the current tick
	tmp = t0 - cnt;							//calculate ticks elapsed since last read
	cnt = t0;								//update _tick
	//option 1 - return the instantanous measurement
	return tmp;								//return the ticks elapsed
	//option 2 - return a smoothed measurement
	return tmp_smoothed=(tmp_smoothed + tmp) / 2;	//calculate a moving average
}	

		