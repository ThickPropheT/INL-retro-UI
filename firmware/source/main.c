#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv.h"
#include "io.h"
#include "pinport.h"

int __attribute__((noreturn)) main(void)
{

	//set watch dog timer with 1 second timer
	wdt_enable(WDTO_1S);
	/* Even if you don't use the watchdog, turn it off here. On newer devices,
	 * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
	 */
	/* RESET status: all port bits are inputs without pull-up.
	 * That's the way we need D+ and D-. Therefore we don't need any
	 * additional hardware initialization.
	 */

	//odDebugInit();	//intialize debuging printing via serial port
	//DBG1(0x00, 0, 0);	//debug serial op: main starts

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

	//intialize i/o to pullup state
	io_pullup();

	//configure LED PORT/DDR
	_LED_OP();
	//boot with LED on to differentiate bettwen BL/RUN
	_LED_ON();

	//enable interrupts
	sei();

	//=================
	//MAIN LOOP
	//=================
	while (1) {

		//pet the watch doggie to keep him happy
		wdt_reset();	

		//must call at regular intervals no longer than 50msec
		//checks for setup packets from what I understand
		usbPoll();	
	}
}
