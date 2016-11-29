#ifndef _buffer_h
#define _buffer_h

#include <avr/io.h>
#include "types.h"
#include "logic.h"
#include "usb.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"


uint8_t	* buffer_usb_call( setup_packet *spacket, uint8_t *rv, uint16_t *rlen);
uint8_t buffer_opcode_no_return( uint8_t opcode, buffer *buff, uint8_t oper1, uint8_t oper2, uint8_t oper3 );
uint8_t buffer_opcode_buffnum_no_return( uint8_t opcode, buffer *buff, uint8_t oper1, uint8_t oper2, uint8_t oper3 );
uint8_t buffer_opcode_return( uint8_t opcode, buffer *buff, uint8_t operMSB, uint8_t operLSB, uint8_t miscdata, uint8_t *rvalue );
void raw_buffer_reset( );
uint8_t allocate_buffer( buffer *buff, uint8_t new_id, uint8_t base_bank, uint8_t num_banks );


//used to communicate to usbFunctionWrite which buffer object
//it should be filling
static buffer *cur_usb_load_buff;
//used to determine number of bytes left to finish current
//OUT transfer utilized by usbFunctionWrite
static uint16_t	incoming_bytes_remain;

#endif
