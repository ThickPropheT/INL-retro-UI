#ifndef _swim_h
#define _swim_h

#include "pinport.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

extern uint8_t	swim_pin;

uint8_t swim_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata );
void swim_activate();
void swim_reset();

#endif
