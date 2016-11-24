#ifndef _shared_pinport_h
#define _shared_pinport_h

//This file was created based on firmware version of pinport.h and pinport.c
//the close relationship between these two files must be kept in mind when making changes.
//This file is also very dependent on macro definitions in firmware.
//Any changes to this file must be applied to firmware.
//Don't recommend changing opcodes or anything here, change them in fw first then apply here.
//making this a shared file helps cut room for error as changing opcode numbers here will
//inherently get forwarded to both firmware and app at same time.



//=============================================================================================
//	OPCODES with no operand and no return value besides SUCCESS/ERROR_CODE
//=============================================================================================
//	0x00-0x7F
//	0-90:	currently defined
//	19-22:	unused due to accidentaly double defining CICE opcodes
//	91-127:	not yet in use
//
//	Current limit for these types of opcodes is 0-127
//	This allows for the MSB to be used for decoding pinport opcode to this type
//
//=============================================================================================
//=============================================================================================

//============================
//ADDR[7:0] PORTA
//============================
//DDR-PORT MACROS
#define 	ADDR_IP	0
#define 	ADDR_OP	1
#define 	ADDR_LO	2
#define 	ADDR_HI 3
			
			
//============================
//DATA[7:0] PORTB
//============================
//DDR-PORT MACROS
#define 	DATA_IP	4
#define 	DATA_OP	5
#define 	DATA_LO	6
#define 	DATA_HI	7
		
		
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

//accidentally doubly defined...
//having this shared .h file helped as the compiler points out these issues...
//#define 	CICE_IP	19
//#define 	CICE_OP	20
//#define 	CICE_LO	21
//#define 	CICE_HI	22
		
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
#define 	EXP0_ip  58
#define 	EXP0_op  59
#define 	EXP0_lo  60	//Don't call this assuming EXP0 DDR is set to o/p
#define 	EXP0_hi  61	//Don't call this unless you're certain pin is 5v tolerant
//User options pull up, force low, and float
#define 	EXP0_LO  62	//Sets low then DDR to o/p
#define 	EXP0_PU  63	//maybe add some NOP(); to allow time for pull up
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




//=============================================================================================
//	OPCODES WITH OPERAND and no return value besides SUCCESS/ERROR_CODE
//=============================================================================================
//	0x80-0x9F: opcodes with 8bit operand
//		0x80-83 are only ones currently in use
//	0xA0-0xAF: opcodes with 16bit operand
//		0xA0-A4 are only ones currently in use
//	0xB0-0xBF: opcodes with 24bit operand
//		0xA0 is currently only one in use
//
//
//	Current limit for these types of opcodes is 128-191 (0x80-0xBF)
//	This allows for the MSBs' to be used for decoding pinport opcode to this type
//
//
//=============================================================================================
//=============================================================================================


//=================================
//8bit operand
//=================================

//ADDR[7:0] PORTA
#define ADDR_SET	0x80

//DATA[7:0] PORTB
#define DATA_SET	0x81

//ADDR[15:8] FLIPFLOP
//NES CPU: ADDRH[6:0] -> CPU A[14:8]
//	   ADDRH[7]   -> NC on CPU side
//NES PPU: ADDRH[5:0] -> PPU A[13:8]
//	   ADDRH[6]   -> NC on PPU side
//	   ADDRH[7]   -> PPU /A13 (which drives CIRAM /CE on most carts "2-screen mirroring")
//SNES:    ADDRH[7:0] -> CPU A[15:8]
#define ADDRH_SET	0x82

//EXPANSION FLIPFLOP
//NES:  ADDRX[7:0] -> EXP PORT [8:1]
//SNES: ADDRX[7:0] -> CPU A[23:16]
#define ADDRX_SET	0x83

//Set ADDR/DATA bus DDR registers with bit granularity
//	OP() IP() macros affect entire 8bit port's direction
//	Each pin can be controlled individually though
//	This could be useful for advanced feature that doesn't treat DATA/ADDR as byte wide port.
#define ADDR_DDR	0x84
#define DATA_DDR	0x84
//Perhaps it will be useful to have this function on other ports as well
//But probably wouldn't be very useful if standard carts are plugged in..

//TODO consider listing AVR internal registers here..?
//could be useful when utilizing SPI/I2C communications etc


//=================================
//16bit operand
//=================================

//ADDR[15:0]	(ADDRH:ADDR) 
//Doesn't affect control signals
//bits[13:0] are applied to NES CPU, NES PPU, and SNES address bus
//bit[14] is only applied to CPU A14 on NES
//bit[15] is only applied to PPU /A13 on NES
//bit[15:14] are applied to SNES A[15:14]
#define	ADDR16_SET	0xA0

//Set NES CPU ADDRESS BUS SET with /ROMSEL
//bit 15 is decoded to enable /ROMSEL properly (aka PRG /CE)
//bit15 is actually inverted then applied to /ROMSEL since /ROMSEL is low when NES CPU A15 is high
//NOTE! This does NOT affect M2 (aka phi2), so carts using M2 to decode things like WRAM is dependent on last value of M2
//This will also stop current value of PPU /A13 with bit15
#define NCPU_ADDR_ROMSEL 0xA1

//Set NES CPU ADDRESS BUS SET with M2
//Identical to NCPU_ADDR_ROMSEL above, but M2 (aka phi2) affected instead of /ROMSEL 
//bit 15 is decoded to assert M2 properly
//bit15 is actually applied directly to M2 since carts use M2 being high as part of A15=1 detection
//NOTE! This does NOT affect /ROMSEL, so /ROMSEL is whatever value it was previously
//This will also stop current value of PPU /A13 with bit15
#define NCPU_ADDR_M2 	 0xA2

//Set NES CPU ADDRESS BUS SET with M2 & /ROMSEL 
//Combination of opcodes above, but M2 and /ROMSEL will be asserted 
//bit 15 is decoded to assert M2 & /ROMSEL properly
//bit15 is actually applied directly to M2 since carts use M2 being high as part of A15=1 detection
//NOTE! This does NOT affect /ROMSEL, so /ROMSEL is whatever value it was previously
//This will also stop current value of PPU /A13 with bit15
#define NCPU_ADDR_M2ROMSEL 0xA3

//TODO consider opcode that preserves PPU /A13 instead of stomping it like the opcodes above.  
//Can't think of why this would be useful so ignoring for now
//One reason might be to keep VRAM silent on a NES board with 4screen mirroring..
//	But should be able to do this with CHR /RD in same manner CHR-ROM is kept silent..

//Set NES PPU ADDRESS BUS with /A13
//PPU address bus is 14bits wide A[13:0] so operand bits [15:14] are ignored.
//bit 13 is inverted and applied to PPU /A13
//PPU control signals CHR /RD and CHR /WR are unaffected
//Note: since PPU /A13 is tied to ADDRH[7] could perform this faster by using ADDR16_SET
//	but this opcode is convienent and ensures PPU /A13 is always inverse of PPU A13
//	This is important for NES carts with on board CHR-ROM and VRAM for 4screen mirroring.
#define NPPU_ADDR_SET	0xA4

//TODO consider opcode that sets PPU A[12:0] and maintains previous value of A13 & /A13
//might be useful if trying to latch/clock CHR memory with it's /CE pin instead of /OE /WE

//=================================
//24bit operand
//=================================

//ADDR[23:0]	(ADDRX:ADDRH:ADDR) SNES full address bus
//Sets SNES 24 bit address but to value of 24bit operand
//No control signals are modified
#define	ADDR24_SET	0xB0




//=============================================================================================
//	OPCODES with NO OPERAND but have RETURN VALUE plus SUCCESS/ERROR_CODE
//=============================================================================================
//	0xC0-0xCF: opcodes with 8bit operand
//		0x80-83 are only ones currently in use
//	0xA0-0xAF: opcodes with 16bit operand
//		0xA0-A4 are only ones currently in use
//	0xB0-0xBF: opcodes with 24bit operand
//		0xA0 is currently only one in use
//
//
//	Current limit for these types of opcodes is 192-255 (0xC0-0xFF)
//	This allows for the MSBs' to be used for decoding pinport opcode to this type
//
//
//=============================================================================================
//=============================================================================================


//READ MCU I/O PORT INPUT 'PIN' REGISTERS
//This is what's used to read bus after setting DDR register to input with IP() command/macro
//Current value of PORT Determines if pullups are activated or not, pull up with HI() macro, and float with LO() macro
//ADDR[7:0] PINA
#define ADDR_PIN	0xC1
//DATA[7:0] PINB
#define DATA_PIN	0xC0
//CTL PINC
//Should set pin of interest to input with IP with macros prior to reading 
//you're still allowed to read value even if some/all pins are output though
#define CTL_PIN		0xC2
//AUX PIND
//Should set pin of interest to input with IP with macros prior to reading 
//you're still allowed to read value even if some/all pins are output though
#define AUX_PIN		0xC3


//READ MCU I/O PORT OUTPUT 'PORT' REGISTERS
//Gives means to see what pins are currently being driven (or pulled up) to.
//ADDR[7:0] PORTA
#define ADDR_PORT	0xC4
//DATA[7:0] PORTB
#define DATA_PORT	0xC5
//CTL PORTC
#define CTL_PORT	0xC6
//AUX PORTD
#define AUX_PORT	0xC7


//READ MCU I/O PORT DIRECTION 'DDR' REGISTERS
//Gives means to see what pins are currently set to I/P or O/P.
//ADDR[7:0] DDRA
#define ADDR_DDR	0xC8
//DATA[7:0] DDRB
#define DATA_DDR	0xC9
//CTL DDRC
#define CTL_DDR		0xCA
//AUX DDRD
#define AUX_DDR		0xCB



//TODO consider listing AVR internal registers here..?
//could be useful when utilizing SPI/I2C communications etc


#endif
