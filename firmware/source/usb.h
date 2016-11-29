#ifndef _usb_h
#define _usb_h

#include <avr/io.h>

#include "usbdrv.h"
#include "logic.h"
#include "types.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"
#include "pinport.h"
#include "io.h"
#include "nes.h"
#include "snes.h"
#include "buffer.h"

#define ENDPOINT_BIT   0x80    //Bit 7 of bmRequest type determines endpoint
#define ENDPOINT_IN    0x80    //In: device-to-host.
#define ENDPOINT_OUT   0x00    //Out: host-to-device. 


//TODO these should probably be in shared so host code and utilize them on the other side
#define RETURN_BUFF_SIZE	8	//number of bytes in generic return buffer
#define RV_ERR_IDX		0	//(first) index of buffer that contains SUCCESS/ERROR#
#define RV_DATA0_IDX		RV_ERR_IDX + 1	//first index of return data
#define RV_DATA_MAX_IDX		RETURN_BUFF_SIZE - 1	//last index available for return data

#endif
