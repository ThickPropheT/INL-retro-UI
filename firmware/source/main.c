
#include "usb.h"
#include "io.h"
#include "buffer.h"

#ifdef AVR_CORE
	#include <avr/interrupt.h>
	#include <avr/wdt.h>
	#include <util/delay.h>
	#include "usbdrv.h"
#endif

#ifdef STM_CORE
	#include <stm32f0xx.h>
	#include "../source_stm_only/stm_init.h"
#endif



int main(void)
{

#ifdef AVR_CORE
	//set watch dog timer with 1 second timer
	wdt_enable(WDTO_1S);
	/* Even if you don't use the watchdog, turn it off here. On newer devices,
	 * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
	 */
	/* RESET status: all port bits are inputs without pull-up.
	 * That's the way we need D+ and D-. Therefore we don't need any
	 * additional hardware initialization.
	 */

	//initialize V-usb driver before interupts enabled and entering main loop
	usbInit();
	//disconnect from host enforce re-enumeration, interupts must be disabled during this.
	usbDeviceDisconnect();

	//fake USB disconnect for over 250ms
	uint8_t index = 0;
	while(--index){		//loop 256 times
		wdt_reset();	//keep wdt happy during this time
		_delay_ms(1);	//delay 256msec
	}

	//reconnect to host
	usbDeviceConnect();


	//enable interrupts
	sei();
#endif

#ifdef STM_CORE
	//System is running at reset defaults
	
	//Default clock is in operation
	//Change system clock as needed
	init_clock();
	
	//Initialize periphery clocks as needed
	init_usb_clock();
	
	//Initialize WDT, core features, etc
	
	//enable interrupts
	__enable_irq();		//clear's processor PRIMASK register bit to allow interrupts to be taken
	//I think this gets done automatically when enabling individual IRQs
	
	//Initialize io, periphery, etc
	//setup LED as outputs and turn them on
	//setup user switch as input

	init_usb();
	//Initialize board/system
#endif

	//intialize i/o and LED to pullup state
	io_reset();

	//=================
	//MAIN LOOP
	//=================
	while (1) {

#ifdef AVR_CORE
		//pet the watch doggie to keep him happy
		wdt_reset();	

		//must call at regular intervals no longer than 50msec
		//keeps 8Byte EP buffer moving from what I understand
		usbPoll();	
#endif

		//check buffer status' and instruct them to 
		//flash/dump as needed to keep data moving
		//currently assuming this operation doesn't take longer
		//than 50msec to meet usbPoll's req't
		//considering using a timer counter interupt to call
		//usbPoll more often but going to see how speed is 
		//impacted first..
		//256Bytes * 20usec Tbp = 5.12msec programming time 
		//+ cpu operations that can't be hid behind flash wait time
		//another thought would be to call usbPoll mid programming
		//a few times to prevent incoming data from being delayed too long
		update_buffers();
	}
}
