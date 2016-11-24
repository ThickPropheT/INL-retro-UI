#include <avr/io.h>
#include "logic.h"
#include "pinport.h"
#include "shared_pinport.h"

//This file was created based on pinport.h
//the close relationship between these two files must be kept in mind when making changes.
//This file is also very dependent on shared_pinport.h
//the shared_pinport.h was generated from this file, so any changes here must be forwarded.

/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated macro.
 * 	shared_pinport.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macro must be defined in firmware pinport.h
 * 	opcode must be defined in shared-pinport.h
 * Post:Macro call complete.
 * Rtn: SUCCESS if opcode found, ERROR_UNKNOWN_OPCODE if opcode not present.
 */
uint8_t pinport_opcode2macro( uint8_t opcode ) 
{
	//these should be simple macros only for now
	//ie only changes one pin/port, macro doesn't call other macros yet
	//made exception to this rule for EXP0 since doesn't vary on board versions
	switch (opcode) { 
		//============================
		//ADDR[7:0] PORTA
		//============================
		//DDR-PORT MACROS
		case ADDR_IP: _ADDR_IP();	break;
		case ADDR_OP: _ADDR_OP();	break;
		case ADDR_LO: _ADDR_LO();	break;
		case ADDR_HI: _ADDR_HI();	break;
			
			
		//============================
		//DATA[7:0] PORTB
		//============================
		//DDR-PORT MACROS
		case DATA_IP: _DATA_IP();	break;
		case DATA_OP: _DATA_OP();	break;
		case DATA_LO: _DATA_LO();	break;
		case DATA_HI: _DATA_HI();	break;
		
		
		//============================
		//CTL PORTC
		//============================
		//DDR-PORT MACROS
		case CTL_IP: _CTL_IP();		break;
		// No CTL_OP() macro as some of these are inputs or bidir, best to individually assert as output
		case CTL_LO: _CTL_LO();		break;
		case CTL_HI: _CTL_HI();		break;
		
		//PIN MACROS
		case M2_IP: _M2_IP();		break;
		case M2_OP: _M2_OP();		break;
		case M2_LO: _M2_LO();		break;
		case M2_HI: _M2_HI();		break;
		
		case ROMSEL_IP: _ROMSEL_IP(); 	break;
		case ROMSEL_OP: _ROMSEL_OP(); 	break;
		case ROMSEL_LO: _ROMSEL_LO(); 	break;
		case ROMSEL_HI: _ROMSEL_HI(); 	break;
		
		case PRGRW_IP: _PRGRW_IP();	break;
		case PRGRW_OP: _PRGRW_OP();	break;
		case PRGRW_WR: _PRGRW_WR();	break;	//LO for writes
		case PRGRW_RD: _PRGRW_RD();	break;	//Hi for reads
		
		//give each def different version numbers to detect errors
		//where command given to board which doesn't have that function
		#ifdef PURPLE_KAZZO //purple boards only
		case p_AXL_ip: _p_AXL_ip();	break;	//Don't use these, use software tied together versions instead.
		case p_AXL_op: _p_AXL_op();	break;	//Increases compatibility between versions
		case p_AXL_lo: _p_AXL_lo();	break;	//Don't recommend calling lo/hi, use CLK instead
		case p_AXL_hi: _p_AXL_hi();	break;
		#else	//Green and final design
		case FREE_IP: _FREE_IP();	break;
		case FREE_OP: _FREE_OP();	break;
		case FREE_LO: _FREE_LO();	break;
		case FREE_HI: _FREE_HI();	break;
		#endif
		
		case CSRD_IP: _CSRD_IP();	break;
		case CSRD_OP: _CSRD_OP();	break;
		case CSRD_LO: _CSRD_LO();	break;
		case CSRD_HI: _CSRD_HI();	break;
		
		case CSWR_IP: _CSWR_IP();	break;
		case CSWR_OP: _CSWR_OP();	break;
		case CSWR_LO: _CSWR_LO();	break;
		case CSWR_HI: _CSWR_HI();	break;
		
		case CICE_IP: _CICE_IP();	break;
		case CICE_OP: _CICE_OP();	break;
		case CICE_LO: _CICE_LO();	break;
		case CICE_HI: _CICE_HI();	break;
		
		#ifdef GREEN_KAZZO
		case g_AXHL_IP: _g_AXHL_IP(); 	break;
		case g_AXHL_OP: _g_AXHL_OP(); 	break;
		case g_AXHL_lo: _g_AXHL_lo(); 	break;	//Don't recommend calling these as AXHL should be left low
		case g_AXHL_hi: _g_AXHL_hi(); 	break;	//That way AXHL_CLK(); is always effective
		#endif
		//purple and final design, safe to pretend green is similar due to software AHL/AXL CLK
		case AHL_IP: _AHL_IP();		break;
		case AHL_OP: _AHL_OP();		break;
		case AHL_lo: _AHL_lo();		break;	//Don't recommend calling these as AHL should be left low
		case AHL_hi: _AHL_hi();		break;	//That way AHL_CLK(); is always effective.
			 			//also helps maintain validity of software AHL/AXL CLK
						
		//============================
		//AUX PORTD
		//============================
		//DDR-PORT MACROS
		case AUX_IP: _AUX_IP();		break;	//Don't touch USB pins!!!
		// No AUX_OP(); macro as many of these are inputs or bidir, best to individually assert as output
		case AUX_LO: _AUX_LO();		break;
		case AUX_HI: _AUX_HI();		break;
		
		//PIN MACROS
		//lower case aren't meant to be called unless certain pin is 5v tolerant
		case EXP0_ip: _EXP0_ip();	break;
		case EXP0_op: _EXP0_op();	break;
		case EXP0_lo: _EXP0_lo();	break;	//Don't call this assuming EXP0 DDR is set to o/p
		case EXP0_hi: _EXP0_hi();	break;	//Don't call this unless you're certain pin is 5v tolerant
		//User options pull up, force low, and float
		case EXP0_LO:  _EXP0_LO();	break;	//Sets low then DDR to o/p
		case EXP0_PU:  _EXP0_PU();	break;	//maybe add some NOP(); to allow time for pull up
		case EXP0_FLT: _EXP0_FLT();	break;	//Set to i/p w/o pullup
		
		case LED_IP:  _LED_IP();	break;
		case LED_OP:  _LED_OP();	break;
		case LED_OFF: _LED_OFF();	break;
		case LED_ON:  _LED_ON();	break;
		
		case IRQ_IP: _IRQ_IP();		break;
		case IRQ_OP: _IRQ_OP();		break;
		case IRQ_LO: _IRQ_LO();		break;
		case IRQ_HI: _IRQ_HI();		break;
		
		case CIA10_IP: _CIA10_IP();	break;
		case CIA10_OP: _CIA10_OP();	break;
		case CIA10_LO: _CIA10_LO();	break;
		case CIA10_HI: _CIA10_HI();	break;
		
		case BL_IP: _BL_IP();		break;
		case BL_OP: _BL_OP();		break;
		case BL_LO: _BL_LO();		break;
		case BL_HI: _BL_HI();		break;
		
		//#ifndef pg_XOE	//FINAL_DESIGN
		//purple and green have versions of these which tie two pins together in software
		case AXLOE_IP: _AXLOE_IP();	break;
		case AXLOE_OP: _AXLOE_OP();	break;
		//Caution AXL_CLK() relies on EXPFF_OP() to be called beforehand
		//	Think of it like you must enable the output before you can clock it.
		//	Floating EXPFF also happens to clock it.  Think of it like it looses it's value if disabled.
		#ifdef PURPLE_KAZZO or GREEN_KAZZO //purple and green versions
		case XOE_ip: _XOE_ip();		break;	//Don't call these, use AXLOE instead	
		case XOE_op: _XOE_op();		break;	
		case XOE_lo: _XOE_lo();		break;	
		case XOE_hi: _XOE_hi();		break;	
		#endif

		//Same definition on all board versions
		//Only need to be cognizant that AXL_CLK won't work if EXPFF_FLT was called beforehand
		//This is only an issue on final design, so an error here should only cause probs on final design
		//Net effect is it it works on final design should be fine on other versions which is the goal
		case EXPFF_OP:  _EXPFF_OP();	break;	//FF /OE pin low->enable o/p
		case EXPFF_FLT: _EXPFF_FLT(); 	break;	//FF /OE pin high->disable o/p

		//AXL_CLK this is similar between purple and green versions, just on a different pin.
		//green boards don't have an AXL_CLK nor a AHL_CLK, as the two are combined.
		//green boards must resolve this in software storing value of FF's so can have the effect
		//of only clocking one of them.	
		//#ifdef GREEN_KAZZO
		//case XX: AXHL_CLK();	break;	//don't want to call this as software AXL/AHL don't track
		//case 87: software_AXL_CLK();	break;
		//case 88: software_AHL_CLK();	break;
		//#else
		//these two cases covers all designs with macro calling sofware versions for green board.
		case AXL_CLK: _AXL_CLK();	break;
		case AHL_CLK: _AHL_CLK();	break;
		//#endif
		//these work fine in hardware for purple and final.
		//green had to separate these two with software.


		default:
			 //macro doesn't exist on this PCB version
			 return ERROR_UNKWN_PINP_OPCODE;
	}
	
	return SUCCESS;
}

#ifdef GREEN_KAZZO

/* Desc:
 * 	other board versions have PORTB "DATA" feed into both FF's
 * 	this board feeds EXP FF with PORTA "ADDR" instead
 * 	goal is to make board versions 'identical'
 * 	to do this we assume higher level functions will have already
 * 	placed desired latch value on PORTB "DATA_OUT"
 * 	we need to juggle this data around and not stomp on anything 
 * Pre: DATA_OP() set 	
 * 	curAHLaddr set by software_AHL_CLK
 * 	DATA_OUT contains desired value to be latched by EXP FF
 * 	AXHL might not be set as O/P
 * 	AXHL might not be low ready for AXHL_CLK
 * Post:Both FF's have desired value latched
 * 	ADDR_OP() left set
 * 	curAXLaddr updated for use by software_AHL_CLK
 * 	DATA_OUT and ADDR_OUT replaced with original values
 * 	AXHL left as O/P and ready for subsequent CLK
 */

//these variables are updated each time the FF's are clocked
//that way we can retain the value of other FF as both must be clocked at once
static uint8_t curAHLaddr;
static uint8_t curAXLaddr;

void software_AXL_CLK()
{
	//first store current DATA & ADDR values
	uint8_t	curAXLaddr = DATA_OUT;	//This is desired AXL value
	uint8_t orig_addr = ADDR_OUT;	//PORTA
	
	//Put current AHL latched value on DATA as that's where it'll be relatched
	//software_AHL_CLK function is one to maintain this value
	DATA_OUT = curAHLaddr;

	//set ADDR as O/P and place desired value on bus
	_ADDR_OP();	//should already be set, but in case not
	ADDR_OUT = curAXLaddr;

	//Clock both latches
	_g_AXHL_OP();	//can't be sure "AHL" is OP as assumption is AXL will be used as latch
	_g_AXHL_lo();	//can't be sure it's low either
	_AXHL_CLK();	//clock values

	//finally restore original DATA & ADDR values
	DATA_OUT = curAXLaddr;
	ADDR_OUT = orig_addr;

}

/* Desc: Same premise as software_AXL_CLK above.
 * 	this is a little simpler as data has already been feed with AHL value.
 * 	just need to make sure AXL latch doesn't get corrupted.
 * Pre: DATA_OP() set 	
 * 	curAXLaddr set by software_AXL_CLK
 * 	DATA_OUT contains desired value to be latched by ADDRMID FF
 * 	AXHL is already set to O/P
 * 	AXHL already low ready for AXHL_CLK
 * Post:Both FF's have desired value latched
 * 	curAHLaddr updated for use by software_AXL_CLK
 * 	DATA_OUT and ADDR_OUT replaced with original values
 * 	AXHL left as O/P and ready for subsequent CLK
 */
void software_AHL_CLK()
{
	//first store current DATA & ADDR values
	uint8_t	curAHLaddr = DATA_OUT;	//This is desired AHL value (store it for other function's use)
	uint8_t orig_addr = ADDR_OUT;	//PORTA
	
	//Desired AHL latch value should have already been placed on DATA_OUT.

	//set ADDR as O/P and place curAXLaddr on bus other function should have updated it last latch
	_ADDR_OP();	//should already be set, but in case not
	ADDR_OUT = curAXLaddr;

	//Clock both latches
	//Can assume AHL is OP as other versions would require it to latch AHL
	//Can also assume it was left low, if not causes issues in all board versions
	_AXHL_CLK();	//clock values

	//finally restore original DATA & ADDR values
	//never changed: DATA_OUT = curAHLaddr;
	ADDR_OUT = orig_addr;

}

#endif	//GREEN_KAZZO

