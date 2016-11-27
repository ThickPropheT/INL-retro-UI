#ifndef _dictionary_h
#define _dictionary_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <libusb.h>

//include prior to other file includes
//that way DEBUG can be turned on/off for this file alone
//uncomment to DEBUG this file alone
#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"

#include "usb_operations.h"
#include "shared_errors.h"
#include "shared_dictionaries.h"

//uncomment to DEBUG this file alone
#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"

int dictionary_call( USBtransfer *transfer, uint8_t dictionary, uint8_t opcode, uint16_t addr, uint8_t miscdata);

#endif
