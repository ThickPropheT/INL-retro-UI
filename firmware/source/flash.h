#ifndef _flash_h
#define _flash_h

#include <avr/io.h>
#include "types.h"
#include "logic.h"
#include "usb.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

uint8_t flash_page( buffer *buff ) ;

#endif
