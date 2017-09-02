#include "swim.h"

//=================================================================================================
//
//	SWIM operations
//	This file includes all the swim functions possible to be called from the swim dictionary.
//
//	See description of the commands contained here in shared/shared_dictionaries.h
//
//=================================================================================================

/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated function.
 * 	shared_dict_swim.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macros must be defined in firmware pinport.h
 * 	opcode must be defined in shared_dict_swim.h
 * Post:function call complete.
 * Rtn: SUCCESS if opcode found, error if opcode not present or other problem.
 */
uint8_t swim_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata )
{
#define	RD_LEN	0
#define	RD0	1
#define	RD1	2

#define	BYTE_LEN 1
#define	HWORD_LEN 2
	switch (opcode) { 
		case SWIM_ACTIVATE:	swim_activate();	break;
		case SWIM_RESET:	swim_reset();		break;
//		case SWIM_SRST:		swim_srst();		break;
//		case WOTF:		swim_woft(operand);	break;
//		case ROTF:	
//				rdata[RD_LEN] = BYTE_LEN;
//				rdata[RD0] = swim_roft();	break;
		default:
			 //opcode doesn't exist
			 return ERR_UNKN_SWIM_OPCODE;
	}
	
	return SUCCESS;

}

uint8_t swim_pin;

void delay( uint16_t delay )
{
	uint16_t i = 0;

	for( i=0; i<delay; i++) { 
		NOP(); 
		NOP(); 
	}	 //16->11.8 on stmad

}

/* Desc:Initiate SWIM activate sequence
 * Pre: swim_pin must be set and initialized via io.h
 * Post:STM8 mcu SWIM active
 * Rtn: SUCCESS if able to enter sucessfully.
 */
void swim_activate()
{
	uint16_t i;
	
//	pinport_call( CTL_OP_, 0, swim_pin, 0);

	//toggles in this manner take 4.55-4.6usec on AVR
	//toggles in this manner take 4.9-4.95usec on STM adapter
	
	//pulse low for 16usec  spec says 16usec
	//but looking at working programmers they do twice the delays below
	pinport_call( CTL_SET_LO_, 0, swim_pin, 0);
	delay(16);
	pinport_call( CTL_SET_HI_, 0, swim_pin, 0);

	//toggle high->low T=1msec 4x
	for( i = 0; i < 4; i++) {
	//STM adapter 720 = 500.9usec
#ifdef STM_INL6
	delay(719);
#endif
#ifdef STM_ADAPTER
	delay(720);
#endif
	pinport_call( CTL_SET_LO_, 0, swim_pin, 0);
	delay(718);
	pinport_call( CTL_SET_HI_, 0, swim_pin, 0);
	}

	//toggle high->low T=0.5msec 4x
	for( i = 0; i < 4; i++) {
	//STM adapter 358 = 256usec
	delay(356);
	pinport_call( CTL_SET_LO_, 0, swim_pin, 0);
	delay(355);
	pinport_call( CTL_SET_HI_, 0, swim_pin, 0);
	}

	//pinport_call( CTL_IP_PU_, 0, swim_pin, 0);


	//wait for device to take swim_pin low for ~16usec
	//it's low for 128 SWIM clock sync pulse
	//Best way to do this would be to wait for an interrupt
	//on the swim pin going low, then have the isr count
	//low time.  If anything takes too long timeout.
	
	//TODO
	//return SUCCESS/FAIL depending on wether that ~16usec pulse was obtained
//	return SUCCESS;

}

/* Desc:Hold swim pin low for >128 SWIM clocks (16usec)
 * Pre: swim must be activated by
 * Post:STM8 mcu SWIM comms are reset
 * Rtn: SUCCESS if device responds with sync window.
 */
void swim_reset()
{
	pinport_call( CTL_OP_, 0, swim_pin, 0);

	//pulse low for 16usec  spec says 16usec
	//but looking at working programmers they do twice the delays below
	pinport_call( CTL_SET_LO_, 0, swim_pin, 0);
	delay(16);
	pinport_call( CTL_IP_PU_, 0, swim_pin, 0);

}
