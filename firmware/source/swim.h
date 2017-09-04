#ifndef _swim_h
#define _swim_h

#include "pinport.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

extern uint8_t	swim_pin;
extern GPIO_TypeDef  *swim_base;
extern uint16_t swim_mask;

uint8_t swim_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata );
void swim_activate();
void swim_reset();
uint8_t swim_out(uint16_t stream, uint8_t len);
uint8_t swim_woft(uint16_t addr, uint8_t data);
uint16_t append_pairity(uint8_t n);

#endif
