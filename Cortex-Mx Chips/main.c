//#include <lpc122x.h>				//we use lpc122x
#include "gpio.h"					//we use F_CPU + gpio functions
#include "delay.h"					//we use software delays
#include "systick.h"				//we use systick
#include "util.h"					//cpu utilization measurement

//hardware configuration
#define LED_DLY			100			//counts
#define PWM_STEP		50
//end hardware configuration

//global defines

//global variables

int main(void) {
	uint32_t time0, time1, tmp;
	int32_t index;

	mcu_init();									//reset the  mcu
	systick_init();								//reset systick

	IO_OUT(LED_PORT, LED);						//led3/4 as output
	ei();
	while (1) {
		IO_FLP(LED_PORT, LED);					//flip led

		time0=ticks();							//time stamp time0
		while (mydelay(24000ul)==0) continue;		//waste cycle time - SystemCoreClock = 24M
		time1=util_get();						//measure the number of ticks elapsed
		time1=T2UTIL(time1);					//convert it to utilization figures
		time0-=systick_get();					//time stamp and calculate elapsed time
	}
}

