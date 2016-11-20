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
	check_debug(rprog_handle != NULL, "Unable to open INL retro-prog usb device handle.");


	int xfr_cnt = 0;
	uint16_t wValue = 0;		//setup packet wValue field
	uint16_t wIndex = 0;		//setup packet wIndex field
    	//uint8_t buffer8[8];		//8 is max payload for low speed devices' data packet
    	//uint8_t buffer128[128];	//128 largest power of 2 for non-LONG_TRANSFERS with V-USB
    	uint8_t buffer254[254];		//254 is max for non-LONG_TRANSFERS with V-USB
    	//uint8_t buffer16k[16384];	//16384 is max for LONG_TRANSFERS with V-USB

        if (argc < 2) {
		printf("USAGE: inlretro <action o/f>\n");
		exit(1);
        }
	char action = argv[1][0];

	
	//int libusb_control_transfer (libusb_device_handle *dev_handle, uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout)
	//
	//SETUP PACKET FIELDS:
	//bmRequestType: ORing of req type (STD/VENDOR), recipient (think we only care about DEVICE), endpoint direction IN-dev->host OUT-host->dev
	//bRequest: single byte that can signify any 'command' or 'request' we setup.
	//The wValue and wIndex fields allow parameters to be passed with the request.  Think we can do whatever we want with these
	//wLength is used the specify the number of bytes to be transferred should there be a data phase. 
	//wLength the length field for the setup packet. The data buffer should be at least this size. 
	//	USB 1.1 low speed standard limits to 8 bytes
	//	V-USB seems to break this limit with max of 254 bytes (255 designates "USB_NO_MSG"
	//	V-USB allows "LONG TRANSFERS" utilizing full 16bit wLength for upto 16384 bytes = exactly 16KBytes
	//	although considering sram on AVR doesn't explode with long transfers and read/write functions are in 8byte chunks,
	//		I think it really is limited to 8bytes
	//	One idea to squeeze more data is have a request type defined that utilizes wValue and wIndex to gain 4bytes + 8buffer = 12bytes 50% gain
	//		Not sure how to gain access to wValue/wIndex with vusb drivers...
	//		answer: usbFunctionSetup will get called for every setup packet and pass all 8 bytes of setup packet
	//	Can ultimately answer this question by counting how many startup packets are recieved by usbFunciton setup for transfers >8 bytes
	//	If when sending >8 byte control transfers, a setup packet only comes once, then there is nothing to abuse
	//		however if the same setup packet gets sent for every 8 bytes, it would be better to only perform 8byte transfers and stuff
	//		4 more bytes in wValue and wIndex fields to increase throughput by ~50%!!!
	//	Testing shows that usbFunctionSetup only gets called once for transfers of 254 bytes
	//		So there is only one setup packet for multiple data packets of 8bytes each
	//
	//Still not sure increasing transfer length doesn't simply break up into bunch of small 8byte transfers although it doesn't sound like it.
	//245byte limit is kind of a pain..  but wValue/wIndex fields could be abused to send 256 bytes
	//Long transfers apparently max out speed @ 24KBps with 300 bytes: https://forums.obdev.at/viewtopic.php?t=3059
	//
	//PAYLOAD:
	//data: a suitably-sized data buffer for either input or output (depending on direction bits within bmRequestType) 
	//
	//TIMEOUT:
	//timeout: (in millseconds) that this function should wait before giving up due to no response being received. 
	//	For an unlimited timeout, use value 0
	//	USB nutshell: A compliant host requires control transfer response within 5sec
	//
	//RETURN:
	//	Returns on success, the number of bytes actually transferred 
	//	LIBUSB_ERROR_TIMEOUT if the transfer timed out 
	//	LIBUSB_ERROR_PIPE if the control request was not supported by the device 
	//	LIBUSB_ERROR_NO_DEVICE if the device has been disconnected 
	//	another LIBUSB_ERROR code on other failures 

	switch (action) {
		case 'o': //ON send REQ_LED_ON
			xfr_cnt = libusb_control_transfer(rprog_handle, 
			LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, 
			REQ_LED_ON, wValue, wIndex, (unsigned char *)buffer254, sizeof(buffer254), SEC_5);
	
			printf("total bytes xfrd: %d \n", xfr_cnt);
			
			break;

		case 'f':
			xfr_cnt = libusb_control_transfer(rprog_handle, 
			LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, 
			REQ_LED_OFF, wValue, wIndex, (unsigned char *)buffer254, sizeof(buffer254), SEC_5);
	
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
