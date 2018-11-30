#include "bootload.h"

//=================================================================================================
//
//	BOOTLOAD operations
//	This file includes all the bootload functions possible to be called from the bootload dictionary.
//
//	See description of the commands contained here in shared/shared_dictionaries.h
//
//=================================================================================================


//const uint32_t fixed_const  __attribute__((at(0x0800F000)));
//fixed_const= 0xDEADBEEF;

//int gSquared __attribute__((at(0x5000)));  // Place at 0x5000


/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated function.
 * 	shared_dict_bootload.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macros must be defined in firmware pinport.h & bootload.h
 * 	opcode must be defined in shared_dict_bootload.h
 * Post:function call complete.
 * Rtn: SUCCESS if opcode found, error if opcode not present or other problem.
 */

uint32_t addrh;


uint8_t bootload_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata )
{
	switch (opcode) { 
#ifdef STM_CORE
		case JUMP_BL:		jump_to_bootloader();		break;
					//device won't respond after this point so actually expect an error to result

		case LOAD_ADDRH:	addrh = operand;		break;
		case JUMP_ADDR:		jump2addr((addrh<<16) | (operand));	break;
					//device may not respond depending on the address/function being jumped to
		
		case PREP_FWUPDATE:	//while we are directly jumping to fwupdate section
					//it should be okay if it's in a fixed location
					return fwupdate_forever();	break;	
					//this function hijacked the stack frame to steal execution
					//after returing from the current USB ISR
					//it returns SUCCESS/ERR depending on if it found and modified
					//the stack frame successfully
					//leaves main application code for good
					//will respond to usb interrupts, but are directed to fwupdater
		default:
			 //opcode doesn't exist
			 return ERR_UNKN_BOOTLOAD_OPCODE;
#endif
	}
	
	return SUCCESS;

}

void jump_to_bootloader()
{

#ifdef STM_INL6
// 070C6 jump to:	0x1FFFC519
// 070RB jump to:	0x1FFFCBC1


	asm(
			//"ldr     pc, boot_addr\n\t"
//			"ldr    r0, =0x12345678\n\t"
//			"mov     r0, r0\n\t"
//			"mov     r0, r0\n\t"
			//".p2align 2\n\t"
			//"boot_addr:\n\t"
			//".word	0x1FFFC519"
			"ldr    r0, [pc, #8]\n\t"//    @ remember pc is 8 bytes ahead
			"mov	r13, r0\n\t"	//load main stack pointer
			"ldr    r0, [pc, #8]\n\t"//    @ remember pc is 8 bytes ahead
			//"mov	lr, r0\n\t"
			//"bx	lr    	\n\t"//               @ function return
			//"bkpt\n\t"
			"mov	r0, r0\n\t"
			//"mov	pc, r0\n\t"
			"bx	r0\n\t"
			//"bootaddr\n\t"//
			//".word	0x1FFFC519"	//070C6
			".p2align 2\n\t"
			".word	0x20001278\n\t"	//C6 MSP @ reset
//			".word	0x00010002\n\t"
			//".word	0x00030004\n\t"
			//".word	0x1FFFC519\n\t"	//C6 AN2606
			".word	0x1FFFCAC5\n\t"	//C6 BL reset vector
			//".word	0x1FFFCBC1\n\t"		//stlink PC when connecting with BL grounded
			//".word	0x1FFFC919\n\t"	//best guess based on C6
			//".word	0x1FFFCBC1\n\t"		//bootloader reset vector
			".word	0x00050006\n\t"
			".word	0x00070008\n\t"
//			".word	0x1FFFC919\n\t"
//			".word	0x1FFFC919\n\t"
//			".word	0x1FFFC919\n\t"
//			".word	0x1FFFC919\n\t"
//			".word	0x1FFFC919\n\t"
			".word	0x1FFFC919"
			//".word	0x1FFFCBC1"
	   );
bootaddr:
	asm(
			".word	0xDEADBEEF"
	   );

#endif


}
