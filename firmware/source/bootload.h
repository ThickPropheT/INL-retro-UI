#ifndef _bootload_h
#define _bootload_h

#include "pinport.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

#ifdef STM_INL6

#endif	//end STM_INL6

uint8_t bootload_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata );

void jump2addr(uint32_t addr);
void jump_to_bootloader();


#endif
