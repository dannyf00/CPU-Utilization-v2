//cpu utilization measurement, on Arduino
//reference: https://dannyelectronics.wordpress.com/2020/07/05/measuring-cpu-mcu-utilization-again/

//hardware configuration
#define LED 			13			//led on pin13, for debugging
#define LED_DLY			(100)		//for testing

//#define ticks()			millie()	//map tick generator to millie()

//global defines
#define UTIL_TICKS01	(128)				//2's compliments, number of ticks at 0.1% cpu load -> 1ms loop cycle time. 100% @ UTIL_TICKS01*1000

//no error checking implemented below
#define T2UTIL(ticks)	((ticks) / UTIL_TICKS01)	//convert ticks to 0..100.0%

//macros for Arduino compatability
#define IO_FLP(port, pin)	digitalWrite(pin, !digitalRead(pin))
#define IO_OUT(ddr, pin)	pinMode(pin, OUTPUT)
#define _ticks()			micros()		//remap ticks() to micros()

//global variables
volatile uint32_t util_tick=0;				//tick counter

//initialize cpu utilization meter
void util_init(void);

//read tick
uint32_t util_get(void);

//fast convertion
uint16_t t2util(uint32_t ticks);

//reading the ticks
//can be replaced by target-specific routines
//use arduino ticks()

//initialize cpu utilization meter
void util_init(void) {
	util_tick = 0;							//initialize the tick counter
	
	//to be replaced with target-specific routines
	//initialize the tmr0 / tick generator
	//if the target has a tick generator and that it is initialized else where,
	//this section can be ignored
	//tmr0 is an 8-bit up counter, interrupt enabled
	//tmr0_init(TMR0_PS_4x);					//initialize the tmr0 -> each tick is 4us @ 1Mhz timer input
	//T0IF = 0;								//clera the flag
	//T0IE = 1;								//enable the clock
	//TMR0 is running
	//use arduino setup
}

//read tick
uint32_t util_get(void) {
	static uint32_t cnt=0;					//ticks since last read	
	static uint32_t tmp_smoothed=0;			//smoothed measurement
	uint32_t t0, tmp;						//read the current tick
	
	t0 = _ticks();							//read the current tick
	tmp = t0 - cnt;							//calculate ticks elapsed since last read
	cnt = t0;								//update _tick
	//option 1 - return the instantanous measurement
	return tmp;								//return the ticks elapsed
	//option 2 - return a smoothed measurement
	return tmp_smoothed=(tmp_smoothed + tmp) / 2;	//calculate a moving average
}	

//simulate mcu load
//non-blocking implementation
uint32_t task(uint32_t cnt) {
	static uint32_t _cnt=0;					//tick counter since the last invocation
	uint32_t tmp;							//temporary variable
	
	tmp = _ticks();							//save the value temporarily
	if (tmp - _cnt > cnt) return _cnt+= cnt;//test for overflow
	return 0;								//returns 0 if no overflow
}

//arduino void setup()
void setup(void) {
	//mcu_init();							//initialize the mcu
	util_init();							//reset cpu utilization meter
	IO_OUT(LED_DDR, LED);					//led as output
	//ei();									//enable by arduino by default

	//for debug only 
	Serial.begin(9600);						//initialize serial
}

//arduino loop
void loop(void) {
	volatile uint32_t t0, t1;
	
	while (1) {
		//IO_FLP(LED_PORT, LED);			//flip led
		
		//read mcu ticks elapsed
		t0 = util_get();					//t0 has the number of ticks elapsed since last read
		t1 = T2UTIL(t0);					//convert it to percentage
		//for debug purposes only
		Serial.print("t0 = "); Serial.println(t0);
		Serial.print("t1 = "); Serial.println(t1);
		
		//simulate mcu load
		//non-blocking implementation
		if (task(LED_DLY)) IO_FLP(LED_PORT, LED);			//1000 ticks task
		//blocking implementation
		//while (task(LED_DLY)==0) continue; IO_FLP(LED_PORT, LED);	//prescaler = 4:1
	}
}
