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

libusb_device_handle * open_usb_device( libusb_context *context )
{	
	int rv = 0;

	//context set to NULL since only acting as single user of libusb
	//libusb_context *context = NULL;
	//passed in from main

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

	ssize_t i = 0;

	libusb_device *retroprog = NULL;
	libusb_device *device = NULL;
	struct libusb_device_descriptor desc;
	libusb_device_handle *handle = NULL;
	const char manf[256];	//used to hold manf/prod strings
	const char prod[256];	//used to hold manf/prod strings
		//Original kazzo
		// manf_ascii: obdev.at prod_ascii: kazzo bcd Device: 100
		//INL Retro-Prog v1.0
		// manf_ascii: InfiniteNesLives.com prod_ascii: INL Retro-Prog bcd Device: 100
		//INL Retro-Prog v2.0 v2.0 released late 2016 (only ver supported by this app
		// manf_ascii: InfiniteNesLives.com prod_ascii: INL Retro-Prog bcd Device: 200
	const char *kazzo_manf = "obdev.at";
	const char *kazzo_prod = "kazzo";
	const char *inl_manf = "InfiniteNesLives.com";
	const char *rprog_prod = "INL Retro-Prog";
	uint16_t min_fw_ver = 0x200;

	debug("searching %d total devices", dev_count-1);
	for( i=0; i<dev_count; i++) {
		device = device_list[i];
		debug("getting dev desc %d", i);
		rv = libusb_get_device_descriptor( device, &desc);
		check( rv == LIBUSB_SUCCESS, "Unable to get device #%d descriptor: %s", i, libusb_strerror(rv));
				
		debug("checking %x vendor", desc.idVendor);
		debug("checking %x product", desc.idProduct);
		if ((desc.idVendor == 0x16C0) && (desc.idProduct == 0x05DC)) {
			//Found a V-USB device with default VID/PID now see if it's actually a kazzo
			debug("found vend %x prod %x\n", desc.idVendor, desc.idProduct);
			debug("manf: %d prod: %d\n", desc.iManufacturer, desc.iProduct);

			//opening device allows performing I/O via USB with device
			rv = libusb_open( device, &handle );
			check( rv == LIBUSB_SUCCESS, "Unable to open USB device: %s", libusb_strerror(rv));

			if (desc.iManufacturer) {
				if ( libusb_get_string_descriptor_ascii( handle, 
					desc.iManufacturer, (unsigned char *)manf, sizeof(manf) ) > LIBUSB_SUCCESS) {
					debug("manf_ascii: %s\n",manf);
				}
			}
			if (desc.iProduct) {
				if ( libusb_get_string_descriptor_ascii( handle, 
					desc.iProduct, (unsigned char *)prod, sizeof(prod) ) > LIBUSB_SUCCESS) {
					debug("prod_ascii: %s\n",prod);
				}
			}

			//Now that manf and prod ID's have been retrieved determine if it's a kazzo/retroprog
			if ( strcmp( manf, kazzo_manf) == 0) {
				if ( strcmp( prod, kazzo_prod) == 0) {
					log_warn("Original kazzo found, firmware needs updated.");
				}
			}

			if ( strcmp( manf, inl_manf) == 0) {
				debug("INL manufactured device found.");
				if ( strcmp( prod, rprog_prod) == 0) {
					debug("INL Retro-Prog found.");
					debug("bcd Device fw version: %x required: %x",
						desc.bcdDevice, min_fw_ver);
					if (desc.bcdDevice < min_fw_ver) { 
						//close device since can't use it
						log_warn("INL Retro-Prog found, but firmware is too old, see Readme for instructions to update firmware.");
					} else {
						//Finally found the supported device!!!
						retroprog = device;
						break;
					}
				}
			}
			//Getting here means the device was opened because it matched V-USB
			//VID/PID, but it wasn't a compatible device.	
			//Can't use this device, so close it
			libusb_close(handle);
			handle = NULL;	//Don't want to try and reclose
		}
	}
	//looped through all devices retroprog will be assigned if it was found.
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


	//free device list if it was left open
	if (device_list) {
		log_err("USB Device list wasn't freed, freeing device list\n");
		libusb_free_device_list( device_list, 1);
	}

	return handle;

error:
	printf("open_usb_device went to error\n");

	if (device_list) {
		printf("freeing device list\n");
		libusb_free_device_list( device_list, 1);
	}

	if (handle) {
		printf("closing usb device\n");
		libusb_close(handle);
	}

	if (usb_init == LIBUSB_SUCCESS) {
		printf("deinitializing libusb\n");
		libusb_exit(context);
	}

	if (rv == LIBUSB_ERROR_ACCESS) {
		printf("-------------------------------------------------------\n");
		printf("Denied Permission is expected for initial use on Linux.\n");
		printf("See udev-rule-help/Readme.txt in host dir for help gaining permission.\n");
		printf("Try command with sudo for a cheap temporary solution.\n");
	}

	return NULL;	//Return NULL pointer if couldn't find INL Retro-Prog
}

void close_usb(libusb_context *context, libusb_device_handle *handle) 
{
	//must close device before exiting
	libusb_close(handle);
	handle = NULL;	//delete handle reference so error won't retry closing

	//deinitialize libusb to be called after closing all devices and before teminating application
	libusb_exit(context);
	
	return;
}

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
int usb_write_to_device( libusb_device_handle *handle, int command, unsigned char *data, uint16_t len ) 
{
	//TODO translate command into request, value, index, etc
	uint8_t request = command;
	uint16_t wValue = 0;		//setup packet wValue field
	uint16_t wIndex = 0;		//setup packet wIndex field

	int xfr_cnt = libusb_control_transfer( handle, 
			//Request type: vendor (as we define),  recip: device, out: host->device
			LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT, 
			request, wValue, wIndex, data, len, SEC_5);

	check( xfr_cnt >=0, "Write xfr failed with libusb error: %s", libusb_strerror(xfr_cnt));
	check( xfr_cnt == len, "Write transfer failed only %dB sent when expecting %dB", xfr_cnt, len);

	return xfr_cnt;

error:
	return -1;
}
