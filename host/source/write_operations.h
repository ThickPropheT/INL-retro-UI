#ifndef _write_operations_h
#define _write_operations_h

#include "write_operations.h"

int write_file( libusb_device_handle *usbhandle, char *filename, char *ines_mapper, char *board_config );

#endif
