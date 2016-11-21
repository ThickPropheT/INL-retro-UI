#ifndef _usb_operations_h
#define _usb_operations_h

#include "usb_operations.h"
#include "usb_commands.h"

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

//USB timeout
#define SEC_5 5000

libusb_device_handle * open_usb_device( libusb_context *context );

void close_usb(libusb_context *context, libusb_device_handle *handle);

int usb_write_to_device( libusb_device_handle *handle, int command, unsigned char *data, uint16_t len );

#endif
