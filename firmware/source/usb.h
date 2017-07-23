#ifndef _usb_h
#define _usb_h


//Define the target core, only define one of these
//these defines are made in Makefile
//#define AVR_CORE
//#define STM_CORE

//== AVR CORE == AVR CORE == AVR CORE == AVR CORE  == AVR CORE
#ifdef AVR_CORE
//	#include <avr/io.h>

	#include "usbdrv.h"
#endif
//end AVR CORE

//== STM CORE == STM CORE == STM CORE == STM CORE  == STM CORE
#ifdef STM_CORE
	#include "..\source_stm_only\usbstm.h"
#endif
//end STM CORE

#include "pinport_al.h"

/*
#include "shared_dictionaries.h"
#include "types.h"
#include "logic.h"
#include "shared_errors.h"
#include "pinport.h"
#include "io.h"
#include "nes.h"
#include "snes.h"
#include "buffer.h"
*/

/*
#define ENDPOINT_BIT   0x80    //Bit 7 of bmRequest type determines endpoint
#define ENDPOINT_IN    0x80    //In: device-to-host.
#define ENDPOINT_OUT   0x00    //Out: host-to-device. 

//usbFunctionWrite return values
//return 0xFF (-1) "STALL" if error
//return 1 if entire payload received successfully
//return 0 if more data expected to complete transfer
#define STALL 		0xFF
#define PAYLD_DONE	1
#define NOT_DONE	0
*/



#endif
