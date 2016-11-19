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

	debug("Initalizing libusb");
	//initialize libusb must be called prior to any other libusb function
	//returns 0 on success LIBUSB_ERROR code on failure
	//int libusb_init ( libusb_context **  context) 
	int usb_init = libusb_init(&context);
	check( usb_init == LIBUSB_SUCCESS, "Failed to initialize libusb: %s", libusb_strerror(usb_init));

	//void libusb_set_debug ( libusb_context *  ctx, int  level ) 
	libusb_set_debug(context, LIBUSB_LOG_LEVEL_ERROR);
		//LIBUSB_LOG_LEVEL_NONE (0) : no messages ever printed by the library (default)
		//LIBUSB_LOG_LEVEL_ERROR (1) : error messages are printed to stderr
		//LIBUSB_LOG_LEVEL_WARNING (2) : warning and error messages are printed to stderr
		//LIBUSB_LOG_LEVEL_INFO (3) : informational messages are printed to stdout, 
		//				warning and error messages are printed to stderr
		//LIBUSB_LOG_LEVEL_DEBUG (4) : debug and informational messages are printed to stdout, 
		//				warnings and errors to stderr

	//discover all usb devices
	libusb_device **device_list = NULL;
	//ssize_t libusb_get_device_list (libusb_context *ctx, libusb_device ***list)
	// Returns a list of USB devices currently attached to the system.
	// return value is number of devices plus one as list is null terminated, or LIBUSB_ERROR if negative.
	// Must free device list after done with it
	ssize_t dev_count = libusb_get_device_list( context, &device_list);
	check( dev_count >= 0, "libusb unable to find any devices: %s", libusb_strerror(dev_count));

	int rv = 0;
	ssize_t i = 0;

	libusb_device *retroprog = NULL;
	libusb_device *device = NULL;
	struct libusb_device_descriptor desc;
	libusb_device_handle *handle = NULL;
	unsigned char str[256];


	debug("searching %d total devices", dev_count-1);
	for( i=0; i<dev_count; i++) {
		device = device_list[i];
		debug("getting dev desc %d", i);
		rv = libusb_get_device_descriptor( device, &desc);
		check( rv == LIBUSB_SUCCESS, "Unable to get device #%d descriptor: %s", i, libusb_strerror(rv));
				
		debug("checking %x vendor", desc.idVendor);
		debug("checking %x product", desc.idProduct);
		if ((desc.idVendor == 0x16C0) && (desc.idProduct == 0x05DC)) {
			retroprog = device;
			debug("found vend %x prod %x\n", desc.idVendor, desc.idProduct);
			debug("manf: %d prod: %d\n", desc.iManufacturer, desc.iProduct);

			//opening device allows performing I/O via USB with device
			rv = libusb_open( retroprog, &handle );
			check( rv == LIBUSB_SUCCESS, "Unable to open USB device: %s", libusb_strerror(rv));

			if (desc.iManufacturer) {
				if ( libusb_get_string_descriptor_ascii( handle, desc.iManufacturer, str, sizeof(str) ) > 0) {
					printf("manf_ascii: %s\n",str);
				}
			}
			if (desc.iProduct) {
				if ( libusb_get_string_descriptor_ascii( handle, desc.iProduct, str, sizeof(str) ) > 0) {
					printf("prod_ascii: %s\n",str);
				}
			}
			if (desc.bcdDevice) {
				printf("bcd Device: %d\n",desc.bcdDevice);
				//old firmware returns 256, new returns 512
				//USB_CFG_DEVICE_VERSION 0x00, 0x01 for v1.0, 0x00, 0x02 for v2.0 (minor then major)
				//v2.0 released late 2016 early 2017
			}

			//TODO verify it's INL Retro Prog by InfiniteNesLives.com, then break
			//else close handle and keep searching as there might be other devices with matching prod/devIDs
			//also verify firmware version is compatible
			//
			//TODO verify have permission to interface with device, else give notice to user
				
			break;
		}
	}
	check( retroprog != NULL, "Could not find INL retro-prog USB device");

	//free device list now that INL retro-prog was found and opened
	//void libusb_free_device_list ( libusb_device **  list, int  unref_devices ) 
	libusb_free_device_list( device_list, 1);	//don't completely understand the unref_devices = 1...
	device_list = NULL; //denote that device list is free if end up in error
	//Guess this is what you're supposed to do..
	// the process of opening a device can be viewed as follows:
	//
	//     Discover devices using libusb_get_device_list().
	//     Choose the device that you want to operate, and call libusb_open().
	//     Unref all devices in the discovered device list.
	//     Free the discovered device list.
	//
	//     The order is important - you must not unreference the device before attempting to open it, because unreferencing it may destroy the device.
	//
	//     For convenience, the libusb_free_device_list() function includes a parameter to optionally unreference all the devices in the list before freeing the list itself. This combines steps 3 and 4 above.
	//
	//     As an implementation detail, libusb_open() actually adds a reference to the device in question. This is because the device remains available through the handle via libusb_get_device(). The reference is deleted during libusb_close(). 


	//code and life to axe
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
			xfr_cnt = libusb_control_transfer(handle, 
			LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, 
			REQ_LED_ON, wValue, wIndex, (unsigned char *)buffer254, sizeof(buffer254), SEC_5);
	
			printf("total bytes xfrd: %d \n", xfr_cnt);
			
			break;

		case 'f':
			xfr_cnt = libusb_control_transfer(handle, 
			LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, 
			REQ_LED_OFF, wValue, wIndex, (unsigned char *)buffer254, sizeof(buffer254), SEC_5);
	
			printf("total bytes xfrd: %d \n", xfr_cnt);
			
			break;

		default:
			printf("Invalid action o-on f-off\n");
			exit(1);
	}
	
	//free device list if it was left open
	if (device_list) {
		printf("Whoops! freeing device list\n");
		libusb_free_device_list( device_list, 1);
	}

	//must close device before exiting
	libusb_close(handle);
	handle = NULL;	//delete handle reference so error won't retry closing

	//deinitialize libusb to be called after closing all devices and before teminating application
	libusb_exit(context);

	
	return 0;
error:
	printf("program went to error\n");

		//must close device before exiting
	//	libusb_close(handle);

	if (device_list) {
		printf("freeing device list\n");
		libusb_free_device_list( device_list, 1);
	}

	if (handle) {
		printf("closing usb device\n");
		libusb_close(handle);
	}

	if (usb_init == LIBUSB_SUCCESS) {
	//deinitialize libusb to be called after closing all devices and before teminating application
		printf("exiting libusb\n");
		libusb_exit(context);
	}


	return 1;

}
