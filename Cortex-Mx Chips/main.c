//#include <lpc122x.h>				//we use lpc122x
#include "gpio.h"					//we use F_CPU + gpio functions
#include "delay.h"					//we use software delays
#include "systick.h"				//we use systick
#include "dhry.h"					//we use dhrystone benchmark
#include "whetstone.h"				//we use whetstone benchmark
#include "tmr32b1_pwm.h"
#include "uart0.h"					//we use uart0
#include "uart1.h"					//we use uart1
#include "adc.h"
#include "tmr16b1_mr.h"
#include "rtc.h"					//we use rtc
#include "benchmark_ti.h"			//we use TI's benchmark routines
#include "util.h"					//cpu utilization measurement

//hardware configuration
#define LED_DLY			100			//counts
#define PWM_STEP		50
//end hardware configuration

//global defines
#define RTC_PR			1			//rtc advance period
//global variables
uint32_t tmp;

//flip led
void led_flp(void) {
	//static int count=0;							//count
	IO_FLP(LED_PORT, LED);					//flip the led
}

//set led
void led_set(void) {
	//static int count=0;							//count
	FIO_SET(LED_PORT, LED);					//flip the led
}

//clr led
void led_clr(void) {
	//static int count=0;							//count
	FIO_CLR(LED_PORT, LED);					//flip the led
}

//return cycle counts
int myticks(void) {
	static int cnt=0;

	return cnt+=1;
}

//non-blocking delay
#define _ticks()		ticks()			//map _ticks() to systicks()
int mydelay(int cnt) {
	static int _cnt=0;

	if (_ticks() - _cnt > cnt) return _cnt+=cnt;
	return 0;
}

//software pwm
char spwm_set(char dc) {
	static char _cnt=0;

	if (_cnt++<dc) return 1;
	return 0;
}

//pwm pulse train sweep
char spwm_swp(int spd) {
	static int i=0, j=0;
	static char state=0;

	if (i++ > spd + 0) {i-=spd + 0; state ^= 1;}
	if (j++ > spd + 1) {j-=spd + 1; state ^= 1;}
	return state;
}

//led_flp2
void led_flp2(void) {
	static char state=0;						//lsb of systick counter
	char tmp;

	tmp = SysTick->VAL & 0x01;					//read systick's lsb
	if (tmp ^ state) IO_FLP(LED_PORT, LED);
	state = tmp;

}


int main(void) {
	uint32_t time0, time1, tmp;
	int32_t index;

	mcu_init();									//reset the  mcu
	systick_init();								//reset systick
	//tmr32b1_pwminit(1, 1<<15);					//initialize pwm
	//tmr32b1_setdc1(LPC_CT32B1->MR0/40);			//set pwm duty cycle
	//tmr16b1_init(10);
	//tmr16b1_setpr1(10000); tmr16b1_act1(led_flp);
	//tmr16b1_setpr2(10000+100); tmr16b1_act2(led_flp);

	IO_OUT(LED_PORT, LED);						//led3/4 as output
	tmp=F_CPU;									//read f_cpu - for debugging
	//uart0_init(9600);
	//uart1_init(9600);
	//adc_init();
	rtc_init();									//reset the rtc
	//rtc_act(led_flp2);							//install the handler
	ei();
	while (1) {
		IO_FLP(LED_PORT, LED);					//flip led

		time0=ticks();							//time stamp time0
		//+1<<24: 560065
		//if (mydelay(100000ul)) IO_FLP(LED_PORT, LED);
		//if (spwm_swp(400)) IO_SET(LED_PORT, LED); else IO_CLR(LED_PORT, LED);
		while (mydelay(24000ul)==0) continue;		//waste cycle time - SystemCoreClock = 24M
		//if (spwm_set(254)) IO_SET(LED_PORT, LED); else IO_CLR(LED_PORT, LED);
		//time0=rtc_get(); while (rtc_get() == time0) continue; IO_FLP(LED_PORT, LED);
		time1=util_get();						//measure the number of ticks elapsed
		time1=T2UTIL(time1);					//convert it to utilization figures
#if 0
		time1=benchmark(dhrystone);							//dhrystone benchmark
		time1=benchmark(whetstone);							//whetseone benchmark
		time1=benchmark(main_8);
		time1=benchmark(main_8m);
		time1=benchmark(main_8sw);
		time1=benchmark(main_16);
		time1=benchmark(main_16m);
		time1=benchmark(main_16sw);
		time1=benchmark(main_32);
		time1=benchmark(main_f);
		time1=benchmark(main_mmlt);
		time1=benchmark(main_fir);
#endif
		//systick_delayms(LED_DLY);				//benchmark test
		//ticks();								//109 ticks
		//micros();								//204 ticks
		//millis();								//309 ticks
		//uart0_puts("12345678\n\r");				//202k/10, 151k/8, 53k/4, 2k/2
		//uart1_puts("23456789\n\r");
		//for (index=0; index<1000; index++) tmp=adc_read(ADC_CH4);
		time0-=systick_get();					//time stamp and calculate elapsed time
		//if (time1>10) NOP();					//break point

		//time1=LPC_RTC->DR;
		//LPC_RTC->MR = LPC_RTC->DR + RTC_PR;
	}
}

