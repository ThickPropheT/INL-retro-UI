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
int set_mem_n_part( USBtransfer *transfer, int buff_num, int mem_type, int part_num );
int set_map_n_mapvar( USBtransfer *transfer, int buff_num, int mapper, int map_var );
int payload_in( USBtransfer *transfer, uint8_t *data, int length );
int payload_out( USBtransfer *transfer, uint8_t *data, int length );
int get_buff_elements( USBtransfer *transfer, int buff_num );

#endif
