#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv.h"
#include "macro.h"

#define REQ_LED_ON  1
#define REQ_LED_OFF 2


//USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8]);
/* This function is called when the driver receives a SETUP transaction from
 * the host which is not answered by the driver itself (in practice: class and
 * vendor requests). All control transfers start with a SETUP transaction where
 * the host communicates the parameters of the following (optional) data
 * transfer. The SETUP data is available in the 'data' parameter which can
 * (and should) be casted to 'usbRequest_t *' for a more user-friendly access
 * to parameters.
 *
 * If the SETUP indicates a control-in transfer, you should provide the
 * requested data to the driver. There are two ways to transfer this data:
 * (1) Set the global pointer 'usbMsgPtr' to the base of the static RAM data
 * block and return the length of the data in 'usbFunctionSetup()'. The driver
 * will handle the rest. Or (2) return USB_NO_MSG in 'usbFunctionSetup()'. The
 * driver will then call 'usbFunctionRead()' when data is needed. See the
 * documentation for usbFunctionRead() for details.
 *
 * If the SETUP indicates a control-out transfer, the only way to receive the
 * data from the host is through the 'usbFunctionWrite()' call. If you
 * implement this function, you must return USB_NO_MSG in 'usbFunctionSetup()'
 * to indicate that 'usbFunctionWrite()' should be used. See the documentation
 * of this function for more information. If you just want to ignore the data
 * sent by the host, return 0 in 'usbFunctionSetup()'.
 *
 * Note that calls to the functions usbFunctionRead() and usbFunctionWrite()
 * are only done if enabled by the configuration in usbconfig.h.
 */
USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	//cast incoming data into the the usb setup packet it is
	usbRequest_t *spacket = (void *)data;
	//typedef struct usbRequest{
	//	uchar       bmRequestType;
	//	uchar       bRequest;
	//	usbWord_t   wValue;
	//	usbWord_t   wIndex;
	//	usbWord_t   wLength;
	//}usbRequest_t;

	switch(spacket->bRequest) {
		case REQ_LED_ON:
			LED_ON(); //from macros
			break;
		case REQ_LED_OFF:
			LED_OFF();
			break;
		default:
			return 1;
	}

	return 0; //ignore any data from the host for now
}


//USB_PUBLIC uchar usbFunctionRead(uchar *data, uchar len);
/* This function is called by the driver to ask the application for a control
 * transfer's payload data (control-in). It is called in chunks of up to 8
 * bytes each. You should copy the data to the location given by 'data' and
 * return the actual number of bytes copied. If you return less than requested,
 * the control-in transfer is terminated. If you return 0xff, the driver aborts
 * the transfer with a STALL token.
 * In order to get usbFunctionRead() called, define USB_CFG_IMPLEMENT_FN_READ
 * to 1 in usbconfig.h and return 0xff in usbFunctionSetup()..
 */
USB_PUBLIC uchar usbFunctionRead(uchar *data, uchar len) {
	return len;
}


//USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len);
/* This function is called by the driver to provide a control transfer's
 * payload data (control-out). It is called in chunks of up to 8 bytes. The
 * total count provided in the current control transfer can be obtained from
 * the 'length' property in the setup data. If an error occurred during
 * processing, return 0xff (== -1). The driver will answer the entire transfer
 * with a STALL token in this case. If you have received the entire payload
 * successfully, return 1. If you expect more data, return 0. If you don't
 * know whether the host will send more data (you should know, the total is
 * provided in the usbFunctionSetup() call!), return 1.
 * NOTE: If you return 0xff for STALL, 'usbFunctionWrite()' may still be called
 * for the remaining data. You must continue to return 0xff for STALL in these
 * calls.
 * In order to get usbFunctionWrite() called, define USB_CFG_IMPLEMENT_FN_WRITE
 * to 1 in usbconfig.h and return 0xff in usbFunctionSetup()..
 */
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) {
	return 1;	//"don't know how much data coming
}


int main()
{
        uint8_t i;
	
	//enable watchdog timer with 1 second timeout
	wdt_enable(WDTO_1S);

	//need USB pins input w/o pullup which is default at startup.
	//So further HW init is not needed/desired.

	//initialize V-USB driver
	usbInit();	

	//enforce re-enumeration
	//interupts must be disabled!
	usbDeviceDisconnect();

	i = 0;
	while (--i) {
		//fake USB disconnect for >250 ms
		//without WDT timing out
		wdt_reset();
		_delay_ms(1);
	}	

	usbDeviceConnect();

	//configure LED PORT/DDR
	SETUP_LED();
	//Always startup with LED ON
	LED_ON();
        
	//enable interrupts
	sei();

	//=================
	//MAIN LOOP
	//=================
	while(1) {

		//reset WDT
		wdt_reset();

		//V-USB driver poll
		usbPoll();

	}//End MAIN LOOP
        
	return 0;
}

