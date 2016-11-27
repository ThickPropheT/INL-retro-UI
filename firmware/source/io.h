#ifndef _io_h
#define _io_h

#include <avr/io.h>
#include "pinport.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

uint8_t io_opcode_only( uint8_t opcode );
void io_reset();
void nes_init();
void snes_init();

#endif
