#ifndef _buffer_h
#define _buffer_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

//include prior to other file includes
//that way DEBUG can be turned on/off for this file alone
//uncomment to DEBUG this file alone
#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"

#include "shared_errors.h"
#include "shared_dictionaries.h"
#include "dictionary.h"
#include "memory.h"
//	 "cartridge.h" is above the scope of this module don't include

#include "pindef.h"

int reset_buffers( USBtransfer *transfer );
int allocate_buffers( USBtransfer *transfer, int num_buffers, int buff_size );

#endif
