#include "config.h"							//configuration words
#include "gpio.h"                           //we use gpio functions
#include "delay.h"                          //we use software delays
#include "tmr0.h"							//we use tmr0 as tick generator
#include "util.h"							//we use utilization measurement module

//hardware configuration
#define LED_PORT		GPIO
#define LED_DDR			TRISIO
#define LED				(1<<0)
//end hardware configuration

//global defines

//global variables

//tmr0 isr
void interrupt isr(void) {
	if (T0IF) {
		T0IF = 0;							//clear the flag
		util_tick+=0x100;					//increment the tick counter  - tmr0 is 8bit. each tick = 4us
	}
}


//simulate mcu load
void task(uint32_t cnt) {
	uint32_t t0;
	
	t0=ticks();
	while (ticks() - t0 < cnt) continue;	//wait for enough ticks to elapse
}

volatile uint32_t t0, t1;
int main(void) {

	mcu_init();							    //initialize the mcu
	util_init();							//reset cpu utilization meter
	IO_OUT(LED_DDR, LED);					//led as output
	ei();
	while (1) {
		IO_FLP(LED_PORT, LED);				//flip led
		
		//read mcu ticks elapsed
		t0 = util_get();					//t0 has the number of ticks elapsed since last read
		t0 = T2UTIL(t0);					//convert it to percentage
		
		//simulate mcu load
		task(1000);							//1000 ticks task
	}
}

