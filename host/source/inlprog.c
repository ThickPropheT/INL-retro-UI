#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <libusb.h>

//uncomment to DEBUG this file alone
//#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"

#include "usb_operations.h"

//control transfer request types
//uint8_t libusb_control_setup::bmRequestType
//Request type.
//	Bits 0:4 determine recipient, see libusb_request_recipient. Bits 5:6 determine type, see libusb_request_type. Bit 7 determines data transfer direction, see libusb_endpoint_direction.
//
//libusb_request_types:
//LIBUSB_REQUEST_TYPE_STANDARD	Standard handled by driver during setup/etc
//LIBUSB_REQUEST_TYPE_CLASS 	Class for use with specific device classes like HID.
//LIBUSB_REQUEST_TYPE_VENDOR	Vendor application specific as we choose which is what we'll be utilizing for all transfers
//LIBUSB_REQUEST_TYPE_RESERVED	Reserved. 
//
//libusb_request_recipients:
//LIBUSB_RECIPIENT_DEVICE	Device.
//LIBUSB_RECIPIENT_INTERFACE	Interface.
//LIBUSB_RECIPIENT_ENDPOINT	Endpoint.
//LIBUSB_RECIPIENT_OTHER	Other. 
//
//LIBUSB_ENDPOINT_IN		In: device-to-host.
//LIBUSB_ENDPOINT_OUT		Out: host-to-device. 

// vendor requests also defined in firmware
// TODO put in combined .h file for both host and fw
#define REQ_LED_ON  1
#define REQ_LED_OFF 2

//USB timeout
#define SEC_5 5000

int main(int argc, char *argv[]) 
{	
	//context set to NULL since only acting as single user of libusb
	libusb_context *context = NULL;
	libusb_device_handle *rprog_handle = NULL;

	rprog_handle = open_usb_device( context );
	check(rprog_handle != NULL, "Unable to open INL retro-prog usb device handle.");


	int xfr_cnt = 0;
    	//uint8_t buffer8[8];		//8 is max payload for low speed devices' data packet
    	//uint8_t buffer128[128];	//128 largest power of 2 for non-LONG_TRANSFERS with V-USB
    	uint8_t buffer254[254];		//254 is max for non-LONG_TRANSFERS with V-USB
    	//uint8_t buffer16k[16384];	//16384 is max for LONG_TRANSFERS with V-USB

        if (argc < 2) {
		printf("USAGE: inlretro <action o/f>\n");
		exit(1);
        }
	char action = argv[1][0];

	switch (action) {
		case 'o': //ON send REQ_LED_ON
			xfr_cnt = usb_write_to_device( rprog_handle, 
				REQ_LED_ON, (unsigned char *)buffer254, sizeof(buffer254) );
			printf("total bytes xfrd: %d \n", xfr_cnt);
			break;

		case 'f':
			xfr_cnt = usb_write_to_device( rprog_handle, 
				REQ_LED_OFF, (unsigned char *)buffer254, sizeof(buffer254) );
			printf("total bytes xfrd: %d \n", xfr_cnt);
			
			break;

		default:
			printf("Invalid action o-on f-off\n");
			exit(1);
	}
	
	close_usb( context, rprog_handle);	
	
	return 0;

error:
	printf("main program went to error\n");

	return 1;

}
