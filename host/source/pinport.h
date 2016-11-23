#ifndef _pinport_h
#define _pinport_h

//This file was created based on firmware version of pinport.h and pinport.c
//the close relationship between these two files must be kept in mind when making changes.
//This file is also very dependent on macro definitions in firmware.
//Any changes to this file must be applied to firmware.
//Don't recommend changing opcodes or anything here, change them in fw first then apply here.

	//these should be simple macros only for now
	//ie only changes one pin/port, macro doesn't call other macros yet
	//made exception to this rule for EXP0 since doesn't vary on board versions
	//switch (opcode) { 
//============================
//ADDR[7:0] PORTA
//============================
//DDR-PORT MACROS
#define ADDR_IP	0
#define ADDR_OP	1
#define ADDR_LO	2
#define ADDR_HI 3
			
			
//============================
//DATA[7:0] PORTB
//============================
//DDR-PORT MACROS
#define DATA_IP	4
#define DATA_OP	5
#define DATA_LO	6
#define DATA_HI	7
		
		
//============================
//CTL PORTC
//============================
//DDR-PORT MACROS
#define 	CTL_IP 	8
// No CTL_OP() macro as some of these are inputs or bidir, best to individually assert as output
#define 	CTL_LO 	9
#define 	CTL_HI 	10
		
//PIN MACROS
#define 	M2_IP 	11
#define 	M2_OP 	12
#define 	M2_LO 	13
#define 	M2_HI 	14
		
#define 	ROMSEL_IP 15
#define 	ROMSEL_OP 16
#define 	ROMSEL_LO 17
#define 	ROMSEL_HI 18

#define 	CICE_IP	19
#define 	CICE_OP	20
#define 	CICE_LO	21
#define 	CICE_HI	22
		
#define 	PRGRW_IP 23
#define 	PRGRW_OP 24
#define 	PRGRW_WR 25	//LO for writes
#define 	PRGRW_RD 26	//HI for reads
		
//give each def different version numbers to detect errors
//where command given to board which doesn't have that function
//#ifdef PURPLE_KAZZO //purple boards only
#define 	p_AXL_ip 27	//Don't use these, use software tied together versions instead.
#define 	p_AXL_op 28	//Increases compatibility between versions
#define 	p_AXL_lo 29	//Don't recommend calling lo/hi, use CLK instead
#define 	p_AXL_hi 30
//#else	//Green and final design
#define 	FREE_IP	31
#define 	FREE_OP	32
#define 	FREE_LO	33
#define 	FREE_HI	34
//#endif
		
#define 	CSRD_IP	35
#define 	CSRD_OP	36
#define 	CSRD_LO	37
#define 	CSRD_HI	38
		
#define 	CSWR_IP	39
#define 	CSWR_OP	40
#define 	CSWR_LO	41
#define 	CSWR_HI	42
			
#define 	CICE_IP	43
#define 	CICE_OP	44
#define 	CICE_LO	45
#define 	CICE_HI	46
			
//#ifdef GREEN_KAZZO
#define 	g_AXHL_IP 47
#define 	g_AXHL_OP 48
#define 	g_AXHL_lo 49	//Don't recommend calling these as AXHL should be left low
#define 	g_AXHL_hi 50	//That way AXHL_CLK(); is always effective
//#endif
//purple and final design, safe to pretend green is similar due to software AHL/AXL CLK
#define 	AHL_IP	51
#define 	AHL_OP	52
#define 	AHL_lo	53	//Don't recommend calling these as AHL should be left low
#define 	AHL_hi	54	//That way AHL_CLK(); is always effective.
			 	//also helps maintain validity of software AHL/AXL CLK
						
//============================
//AUX PORTD
//============================
//DDR-PORT MACROS
#define 	AUX_IP	55	//Don't touch USB pins!!!
// No AUX_OP(); macro as many of these are inputs or bidir, best to individually assert as output
#define 	AUX_LO	56
#define 	AUX_HI	57
		
//PIN MACROS
//lower case aren't meant to be called unless certain pin is 5v tolerant
#define 	EXP0_ip	58
#define 	EXP0_op	59
#define 	EXP0_lo	60	//Don't call this assuming EXP0 DDR is set to o/p
#define 	EXP0_hi	61	//Don't call this unless you're certain pin is 5v tolerant
//User options pull up, force low, and float
#define 	EXP0_LO	62	//Sets low then DDR to o/p
#define 	EXP0_PU	63	//maybe add some NOP(); to allow time for pull up
#define 	EXP0_FLT 64	//Set to i/p w/o pullup
		
#define		LED_IP	65
#define		LED_OP	66
#define		LED_OFF	67
#define		LED_ON	68
		
#define		IRQ_IP	69
#define		IRQ_OP	70
#define		IRQ_LO	71
#define		IRQ_HI	72
		
#define		CIA10_IP 73
#define		CIA10_OP 74
#define		CIA10_LO 75
#define		CIA10_HI 76
		
#define		BL_IP	77
#define		BL_OP	78
#define		BL_LO	79
#define		BL_HI	80
		
//#ifndef pg_XOE	//FINAL_DESIGN
//purple and green have versions of these which tie two pins together in software
#define		AXLOE_IP 81
#define		AXLOE_OP 82
//Caution AXL_CLK() relies on EXPFF_OP() to be called beforehand
//	Think of it like you must enable the output before you can clock it.
//	Floating EXPFF also happens to clock it.  Think of it like it looses it's value if disabled.
//#ifdef PURPLE_KAZZO or GREEN_KAZZO //purple and green versions
#define		XOE_ip 83	//Don't call these, use AXLOE instead	
#define		XOE_op 84
#define		XOE_lo 85
#define		XOE_hi 86
//#endif

//Same definition on all board versions
//Only need to be cognizant that AXL_CLK won't work if EXPFF_FLT was called beforehand
//This is only an issue on final design, so an error here should only cause probs on final design
//Net effect is it it works on final design should be fine on other versions which is the goal
#define		EXPFF_OP  87	//FF /OE pin low->enable o/p
#define		EXPFF_FLT 88	//FF /OE pin high->disable o/p

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
#define		AXL_CLK 89
#define		AHL_CLK	90
//#endif
//these work fine in hardware for purple and final.
//green had to separate these two with software.


/*		default:
			 //macro doesn't exist on this PCB version
			 return ERROR_UNKWN_PINP_OPCODE;
	}
	
	return SUCCESS;
}*/

#endif
