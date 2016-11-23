#include <avr/io.h>
#include "logic.h"
#include "pinport.h"

//This file was created based on pinport.h
//the close relationship between these two files must be kept in mind when making changes.
//This file is also very dependent on macro definitions in host app.
//the host app pinport.h was generated from this file, so any changes here must be forwarded.

/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated macro.
 * Pre: Macro must be defined in firmware pinport.h
 * 	opcode must align with host pinport.h otherwise who knows what you're calling
 * Post:Macro call complete.
 * Rtn: SUCCESS if opcode found, ERROR_UNKNOWN_OPCODE if opcode not present.
 */
uint8_t pinport_macro( uint8_t opcode ) 
{
	//these should be simple macros only for now
	//ie only changes one pin/port, macro doesn't call other macros yet
	//made exception to this rule for EXP0 since doesn't vary on board versions
	switch (opcode) { 
		//============================
		//ADDR[7:0] PORTA
		//============================
		//DDR-PORT MACROS
		case 0:	ADDR_IP();	break;
		case 1: ADDR_OP();	break;
		case 2: ADDR_LO();	break;
		case 3: ADDR_HI();	break;
			
			
		//============================
		//DATA[7:0] PORTB
		//============================
		//DDR-PORT MACROS
		case 4: DATA_IP();	break;
		case 5: DATA_OP();	break;
		case 6: DATA_LO();	break;
		case 7: DATA_HI();	break;
		
		
		//============================
		//CTL PORTC
		//============================
		//DDR-PORT MACROS
		case 8:	 CTL_IP();	break;
		// No CTL_OP() macro as some of these are inputs or bidir, best to individually assert as output
		case 9:	 CTL_LO();	break;
		case 10: CTL_HI();	break;
		
		//PIN MACROS
		case 11: M2_IP();	break;
		case 12: M2_OP();	break;
		case 13: M2_LO();	break;
		case 14: M2_HI();	break;
		
		case 15: ROMSEL_IP(); break;
		case 16: ROMSEL_OP(); break;
		case 17: ROMSEL_LO(); break;
		case 18: ROMSEL_HI(); break;
		
		case 19: CICE_IP();	break;
		case 20: CICE_OP();	break;
		case 21: CICE_LO();	break;
		case 22: CICE_HI();	break;
		
		case 23: PRGRW_IP();	break;
		case 24: PRGRW_OP();	break;
		case 25: PRGRW_WR();	break;	//LO for writes
		case 26: PRGRW_RD();	break;	//Hi for reads
		
		//give each def different version numbers to detect errors
		//where command given to board which doesn't have that function
		#ifdef PURPLE_KAZZO //purple boards only
		case 27: p_AXL_ip();	break;	//Don't use these, use software tied together versions instead.
		case 28: p_AXL_op();	break;	//Increases compatibility between versions
		case 29: p_AXL_lo();	break;	//Don't recommend calling lo/hi, use CLK instead
		case 30: p_AXL_hi();	break;
		#else	//Green and final design
		case 31: FREE_IP();	break;
		case 32: FREE_OP();	break;
		case 33: FREE_LO();	break;
		case 34: FREE_HI();	break;
		#endif
		
		case 35: CSRD_IP();	break;
		case 36: CSRD_OP();	break;
		case 37: CSRD_LO();	break;
		case 38: CSRD_HI();	break;
		
		case 39: CSWR_IP();	break;
		case 40: CSWR_OP();	break;
		case 41: CSWR_LO();	break;
		case 42: CSWR_HI();	break;
		
		case 43: CICE_IP();	break;
		case 44: CICE_OP();	break;
		case 45: CICE_LO();	break;
		case 46: CICE_HI();	break;
		
		#ifdef GREEN_KAZZO
		case 47: g_AXHL_IP(); break;
		case 48: g_AXHL_OP(); break;
		case 49: g_AXHL_lo(); break;	//Don't recommend calling these as AXHL should be left low
		case 50: g_AXHL_hi(); break;	//That way AXHL_CLK(); is always effective
		#endif
		//purple and final design, safe to pretend green is similar due to software AHL/AXL CLK
		case 51: AHL_IP();	break;
		case 52: AHL_OP();	break;
		case 53: AHL_lo();	break;	//Don't recommend calling these as AHL should be left low
		case 54: AHL_hi();	break;	//That way AHL_CLK(); is always effective.
			 			//also helps maintain validity of software AHL/AXL CLK
						
		//============================
		//AUX PORTD
		//============================
		//DDR-PORT MACROS
		case 55: AUX_IP();	break;	//Don't touch USB pins!!!
		// No AUX_OP(); macro as many of these are inputs or bidir, best to individually assert as output
		case 56: AUX_LO();	break;
		case 57: AUX_HI();	break;
		
		//PIN MACROS
		//lower case aren't meant to be called unless certain pin is 5v tolerant
		case 58: EXP0_ip();	break;
		case 59: EXP0_op();	break;
		case 60: EXP0_lo();	break;	//Don't call this assuming EXP0 DDR is set to o/p
		case 61: EXP0_hi();	break;	//Don't call this unless you're certain pin is 5v tolerant
		//User options pull up, force low, and float
		case 62: EXP0_LO();	break;	//Sets low then DDR to o/p
		case 63: EXP0_PU();	break;	//maybe add some NOP(); to allow time for pull up
		case 64: EXP0_FLT();	break;	//Set to i/p w/o pullup
		
		case 65: LED_IP();	break;
		case 66: LED_OP();	break;
		case 67: LED_OFF();	break;
		case 68: LED_ON();	break;
		
		case 69: IRQ_IP();	break;
		case 70: IRQ_OP();	break;
		case 71: IRQ_LO();	break;
		case 72: IRQ_HI();	break;
		
		case 73: CIA10_IP();	break;
		case 74: CIA10_OP();	break;
		case 75: CIA10_LO();	break;
		case 76: CIA10_HI();	break;
		
		case 77: BL_IP();	break;
		case 78: BL_OP();	break;
		case 79: BL_LO();	break;
		case 80: BL_HI();	break;
		
		//#ifndef pg_XOE	//FINAL_DESIGN
		//purple and green have versions of these which tie two pins together in software
		case 81: AXLOE_IP();	break;
		case 82: AXLOE_OP();	break;
		//Caution AXL_CLK() relies on EXPFF_OP() to be called beforehand
		//	Think of it like you must enable the output before you can clock it.
		//	Floating EXPFF also happens to clock it.  Think of it like it looses it's value if disabled.
		#ifdef PURPLE_KAZZO or GREEN_KAZZO //purple and green versions
		case 83: XOE_ip();	break;	//Don't call these, use AXLOE instead	
		case 84: XOE_op();	break;	
		case 85: XOE_lo();	break;	
		case 86: XOE_hi();	break;	
		#endif

		//Same definition on all board versions
		//Only need to be cognizant that AXL_CLK won't work if EXPFF_FLT was called beforehand
		//This is only an issue on final design, so an error here should only cause probs on final design
		//Net effect is it it works on final design should be fine on other versions which is the goal
		case 87: EXPFF_OP();	break;	//FF /OE pin low->enable o/p
		case 88: EXPFF_FLT(); 	break;	//FF /OE pin high->disable o/p

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
		case 89: AXL_CLK();	break;
		case 90: AHL_CLK();	break;
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
	ADDR_OP();	//should already be set, but in case not
	ADDR_OUT = curAXLaddr;

	//Clock both latches
	g_AXHL_OP();	//can't be sure "AHL" is OP as assumption is AXL will be used as latch
	g_AXHL_lo();	//can't be sure it's low either
	AXHL_CLK();	//clock values

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
	ADDR_OP();	//should already be set, but in case not
	ADDR_OUT = curAXLaddr;

	//Clock both latches
	//Can assume AHL is OP as other versions would require it to latch AHL
	//Can also assume it was left low, if not causes issues in all board versions
	AXHL_CLK();	//clock values

	//finally restore original DATA & ADDR values
	//never changed: DATA_OUT = curAHLaddr;
	ADDR_OUT = orig_addr;

}

#endif	//GREEN_KAZZO

