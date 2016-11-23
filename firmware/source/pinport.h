#include <avr/io.h>
#include "logic.h"

//This file contains pinout translations from AVR names to "kazzo" names
//this file also works to make all kazzo versions compatible and "alike" 
//There are defines for kazzo version, turns out unique early versions 
//can be differentiated by solder mask color.
//Final version is default and doesn't need any defines
//#define PURPLE_KAZZO
//#define GREEN_KAZZO

//=======================================================
//History of PCB revsisions produced by InfiniteNesLives
//=======================================================
// 
// uncomment define if buiding for either of the first two versions
//#define PURPLE_KAZZO

// First printed circuit board version
// only handful made (less than 10?)
// Purple solder mask
// Labeled "Kazzo PCB rev 3.0"
// Dated 8/22/2011
// * Only contained NES and Famicom connectors
// * Had bug where USB data lines mixed up
// 	-manually fixed during component assembly
// * Naruko determined this should have still been v1 board
// 	-due to it's overall design and conflicting with someother v3 board
// * This was my first ever PCB design and one of the first
// 	times designing something for the NES/FC.  Looking
// 	back at the design there were some very noob decisions...
// * INPUT/OUTPUT differences:
// 	EXP9 - PD1/LED jumper
// 		when closed, PD1 controls NES EXP9 and LED
// 		when open, EXP9 is floating PD1 only controls LED
// 		NES EXP9 was connected to GND pin #16 on FC
// 		-I must have thought that GND pin could serve 
// 			different purpose..
// 		-Result is PD1 is shorted to GND when FC cart inserted
// 			and jumper closed..
// 		Believe I closed this jumper on units I shipped
// 			prob should have left it open..
//		Suggested fix: leave open and ground EXP9
//			-prevents issue with LED when FC cart inserted
//			-minor draw back no access to EXP9
// 	ALOG - EXP6 - DIGI jumper
// 		another noob jumper decision...
// 		ALOG is the MCU AREF pin which should be tied to VCC
// 			-I thought it was an analog in apparently...
// 		DIGI is EXP flip flop Q6 output
// 		Best position for this jumper is EXP6-DIGI
// 		ALOG pad should be tied to VCC with jumper..
// 		Doing that would make it similar to future designs.
// 		Don't think I shipped any with ALOG jumper closed
// 		NES EXP6 is tied to Famicom pin 46 (Sound to RF)
// 	Expansion port FlipFlop /OE - CLK
// 		Aside from lame jumper design above, biggest difference 
// 		between this PCB version and future ones.
// 		-EXP FF /OE controlled by MCU PD7
// 		-EXP FF CLK controlled by MCU PC3
// 		Future versions control both /OE and CLK with PD7
// 			-this frees PC3 for user use
// 	Both FlipFlops D i/p are driven by Data bus (PORT B)
//
//
// Second printed circuit board version
// only handful made (about a dozen?)
// Purple solder mask
// Labeled "Kazzo PCB rev 1.1"
// Dated 8/22/2011
// * Only contained NES and Famicom connectors
// * Identical to version above aside from the following
// * Corrected bug where USB data lines mixed up
// * Changed silkscreen to v1.1 as suggested by naruko
// * INPUT/OUTPUT differences:
//	-Same as version above as far as I know.
//
//
// uncomment define if buiding for this versions
//#define GREEN_KAZZO
//
// Third printed circuit board version
// only handful made (about ten?) used primarily as SNES prototype
// Green solder mask
// Labeled "Retro programmer/dumper v1.2"  "Kazzo 1.x"
// Dated Apr 2013
// * First version to add SNES connector along with NES & FC
// * Removed noob jumpers from purple versions above.
// 	-grouned FC pin16 as it should have been.  
// * INPUT/OUTPUT differences:
//	-EXP FF /OE still controlled by MCU PD7
//	-MCU PC7 controls both CLK on both FF's.
//	-EXP FF D inputs are PORTA 
//		all other versions are driven by PORTB
//		This means you always have to clock both flipflops
//		Place desired value on PORTA/B respectively and clock with PD7
//	-PC3 is free for user use.
//	-SNES /RESET pin not controlled by PD0 (EXP0)
//		instead it's controlled by A20 (EXPFF Q4)
//		prevents putting INL SNES boards in program mode unless A20 is also low
//		pretty much makes flashing SNES boards a royal PITA/impossible
//		Suggested fix is to have PD0 (EXP0) control SNES /RESET
//		would have to free /RESET and wire to EXP0/PD0 to permit flashing/reading  INL SNES board.
// 
// 
// Fourth printed circuit board version
// First volume PCB run ~300 copies
// Yellow solder mask
// Labeled "Retro programmer/dumper v1.2b"  "Kazzo 1.x"
// Dated May 2013
// * Includes NES, Famicom, & SNES connector
// * SNES board must be removed from case and slid all the way to right
// * Pitch offset on SNES connector makes it difficult to connect with original SNES boards.
// 	-Connector designed to only provide support for INL SNES Hi/Lo-ROM boards.
// * Care must be excersized not to insert SNES board backwards applying reverse power.
// * Effectively Final circuit design after lessons learned on small batches that preceeded.
// * INPUT/OUTPUT differences:
//	-EXP FF /OE & CLK controlled by MCU PD7
//	-MCU PC7 only controls CLK on ADDR HIGH FF.
//	-EXP & ADDRHI FF D inputs both on Data bus PORTB 
//	-PC3 is free for user use.
//	-SNES /RESET pin controlled by PD0 (EXP0)
//	-Retains prev ver fixes, nothing funny going on with jumpers FC GND pin #16
//	
//
// Fifth printed circuit board version
// Second volume PCB run ~500 copies
// Yellow solder mask
// Labeled "Retro programmer/dumper v1.2b"  "Kazzo 1.x"
// Dated OCT 2014
// * Includes NES, Famicom, & SNES connector
// * SNES connector setup/cautions just like the prev version.
// * No significant changes from previous version.
// 	-Changed MCU clock to Crystal instead of resonator in prev ver.
// 	-Added screw mount holes, although not very well placed.
// * INPUT/OUTPUT differences:
// 	-None from previous version
//
//
// Sixth printed circuit board version
// Third volume PCB run in production as of 1NOV2016
// Orange solder mask
// Labeled "Retro programmer/dumper v1.4"  "Kazzo 1.x"
// Dated OCT 2016
// Addition of fancy INL logo for first time
// * Includes NES, Famicom, & SNES connector
// * SNES connector improvement to correct pitch issue with prev ver.
// * Addition of PCT resettable fuse on incoming power.
// 	-Provides protection to SNES boards inserted backwards.
// * Rearrangement of BL/RUN switch and screw holes for better case design.
// * Cut out buzzer pads (PD6) from under MCU which was never developed.
// * INPUT/OUTPUT differences:
// 	-None from previous version
//



//============================
//ADDR[7:0] PORTA
//============================
//PORT DEFN
#define ADDR_OUT	PORTA
#define ADDR_IN		PINA
#define ADDR_DDR	DDRA
//DDR-PORT MACROS
#define ADDR_IP()	ADDR_DDR = LO
#define ADDR_OP()	ADDR_DDR = HI
#define ADDR_LO()	ADDR_OUT = LO
#define ADDR_HI()	ADDR_OUT = HI


//============================
//DATA[7:0] PORTB
//============================
//PORT DEFN
#define DATA_OUT	PORTB
#define DATA_IN		PINB
#define DATA_DDR	DDRB
//DDR-PORT MACROS
#define DATA_IP()	DATA_DDR = LO
#define DATA_OP()	DATA_DDR = HI
#define DATA_LO()	DATA_OUT = LO
#define DATA_HI()	DATA_OUT = HI


//============================
//CTL PORTC
//============================
//PORT DEFN
#define CTL_OUT		PORTC
#define CTL_IN		PINC
#define CTL_DDR		DDRC
//DDR-PORT MACROS
#define CTL_IP()	CTL_DDR = LO
// No CTL_OP() macro as some of these are inputs or bidir, best to individually assert as output
#define CTL_LO()	CTL_OUT = LO
#define CTL_HI()	CTL_OUT = HI

//PIN DEFN
#define M2	PC0	//NES, FC, & SNES (SYSCLK)
#define ROMSEL	PC1	//(aka PRG/CE) NES, FC, & SNES 
#define PRGRW	PC2	//PRG R/W on NES & FC 

#ifdef PURPLE_KAZZO
#define p_AXL	PC3	//EXP FF CLK on purple boards
#else
#define FREE	PC3	//Free pin on all other boards
#endif

#define CSRD	PC4	//NES & FC CHR /RD, SNES /RD
#define CSWR	PC5	//NES & FC CHR /WR, SNES /WR
#define CICE 	PC6	//NES & FC CIRAM /CE, most carts are 2screen tying this to CHR /A13 making this an I/P

#ifdef GREEN_KAZZO
#define g_AXHL	PC7	//Both ADDR_MID & EXP/ADDRHI FF CLK on green prototype
#else
#define AHL	PC7	//ADDR MID FF CLK per orig kazzo design
#endif

//PIN MACROS
#define	M2_IP()		CTL_DDR	&= ~(1<<M2)
#define	M2_OP()		CTL_DDR	|=  (1<<M2)
#define M2_LO()		CTL_OUT &= ~(1<<M2)
#define M2_HI()		CTL_OUT |=  (1<<M2)
//TODO read M2 PIN as input

#define ROMSEL_IP()	CTL_DDR &= ~(1<<ROMSEL)
#define ROMSEL_OP()	CTL_DDR |=  (1<<ROMSEL)
#define ROMSEL_LO()	CTL_OUT &= ~(1<<ROMSEL)
#define ROMSEL_HI()	CTL_OUT |=  (1<<ROMSEL)

#define CICE_IP()	CTL_DDR &= ~(1<<CICE)
#define CICE_OP()	CTL_DDR |=  (1<<CICE)
#define CICE_LO()	CTL_OUT &= ~(1<<CICE)
#define CICE_HI()	CTL_OUT |=  (1<<CICE)

#define PRGRW_IP()	CTL_DDR &= ~(1<<PRGRW)
#define PRGRW_OP()	CTL_DDR |=  (1<<PRGRW)
#define PRGRW_WR()	CTL_OUT &= ~(1<<PRGRW)	//LO for writes
#define PRGRW_RD()	CTL_OUT |=  (1<<PRGRW)	//HI for reads

#ifdef p_AXL
#define p_AXL_IP()	CTL_DDR &= ~(1<<p_AXL)
#define p_AXL_OP()	CTL_DDR |=  (1<<p_AXL)
#define p_AXL_lo()	CTL_OUT &= ~(1<<p_AXL)	//Don't recommend calling lo/hi, use CLK instead
#define p_AXL_hi()	CTL_OUT |=  (1<<p_AXL)
//AXL_CLK assumes AXL was previously left in default low state
#define AXL_CLK()	p_AXL_hi(); p_AXL_lo();	//same name and convention on final design
#else	//Green and final design
#define FREE_IP()	CTL_DDR &= ~(1<<FREE)
#define FREE_OP()	CTL_DDR |=  (1<<FREE)
#define FREE_LO()	CTL_OUT &= ~(1<<FREE)
#define FREE_HI()	CTL_OUT |=  (1<<FREE)
#endif

#define CSRD_IP()	CTL_DDR &= ~(1<<CSRD)
#define CSRD_OP()	CTL_DDR |=  (1<<CSRD)
#define CSRD_LO()	CTL_OUT &= ~(1<<CSRD)
#define CSRD_HI()	CTL_OUT |=  (1<<CSRD)

#define CSWR_IP()	CTL_DDR &= ~(1<<CSWR)
#define CSWR_OP()	CTL_DDR |=  (1<<CSWR)
#define CSWR_LO()	CTL_OUT &= ~(1<<CSWR)
#define CSWR_HI()	CTL_OUT |=  (1<<CSWR)

#define CICE_IP()	CTL_DDR &= ~(1<<CICE)
#define CICE_OP()	CTL_DDR |=  (1<<CICE)
#define CICE_LO()	CTL_OUT &= ~(1<<CICE)
#define CICE_HI()	CTL_OUT |=  (1<<CICE)

#ifdef g_AXHL
#define g_AXHL_IP()	CTL_DDR &= ~(1<<g_AXHL)
#define g_AXHL_OP()	CTL_DDR |=  (1<<g_AXHL)
#define g_AXHL_lo()	CTL_OUT &= ~(1<<g_AXHL)	//Don't recommend calling these as AXHL should be left low
#define g_AXHL_hi()	CTL_OUT |=  (1<<g_AXHL)	//That way AXHL_CLK(); is always effective
#define AXHL_CLK()	g_AXHL_hi(); g_AXHL_lo();	
#else	//purple and final design
#define AHL_IP()	CTL_DDR &= ~(1<<AHL)
#define AHL_OP()	CTL_DDR |=  (1<<AHL)
#define AHL_lo()	CTL_OUT &= ~(1<<AHL)	//Don't recommend calling these as AHL should be left low
#define AHL_hi()	CTL_OUT |=  (1<<AHL)	//That way AHL_CLK(); is always effective
#define AHL_CLK()	AHL_hi(); AHL_lo();
#endif

//============================
//AUX PORTD
//============================
//PORT DEFN
#define	AUX_OUT		PORTD
#define AUX_IN		PIND
#define AUX_DDR		DDRD
//DDR-PORT MACROS
#define AUX_IP()	AUX_DDR  &=  ((1<<USBP) | (1<<USBM))	//Don't touch USB pins!!!
// No AUX_OP() macro as many of these are inputs or bidir, best to individually assert as output
#define AUX_LO()	AUX_OUT  &=  ((1<<USBP) | (1<<USBM))
#define AUX_HI()	AUX_OUT  |= ~((1<<USBP) | (1<<USBM))

//PIN DEFN
#define EXP0	PD0	//NES EXP0 controls a number of varying flash cart features...
#define FC_APU	PD0	//FC Audio in cart from 2A03 APU
#define TDO	PD0	//CPLD JTAG on INL-ROM NES/FC boards released after ~Oct2016
#define S_RST	PD0	//SNES /RESET pin used for CPLD prgm/play mode and SRAM CE

#define LED	PD1	//LED on INL retro prog-dumper
#define EXP9	PD1	//NES dual purposed pin

#define USBP	PD2	//USB D+ don't touch this pin!
#define IRQ	PD3	//Connected to NES, FC, & SNES
#define USBM	PD4	//USB D- don't touch this pin!
#define CIA10	PD5	//NES & FC CIRAM A10 (aka VRAM A10)
#define BL	PD6	//Bootloader switch BL->GND, RUN->float

#ifdef PURPLE_KAZZO
#define pg_XOE	PD7	//EXP/ADDRHI FF /OE pin on purple and green boards
#endif
#ifdef GREEN_KAZZO
#define pg_XOE	PD7	//EXP/ADDRHI FF /OE pin on purple and green boards
#endif
#ifndef pg_XOE	//FINAL_DESIGN
#define AXLOE	PD7	//EXP/ADDRHI FF CLK & /OE pin on final board versions
#endif

//PIN MACROS
//lower case aren't meant to be called unless certain pin is 5v tolerant
#define	EXP0_ip()	AUX_DDR	&=  ~(1<<EXP0)
#define	EXP0_op()	AUX_DDR	|=  (1<<EXP0)
#define EXP0_lo()	AUX_OUT &=  ~(1<<EXP0)	//Don't call this assuming EXP0 DDR is set to o/p
#define EXP0_hi()	AUX_OUT |=  (1<<EXP0)	//Don't call this unless you're certain pin is 5v tolerant
//User options pull up, force low, and float
#define EXP0_LO()	EXP0_lo(); EXP0_op();	//Sets low then DDR to o/p
#define EXP0_PU()	EXP0_ip(); EXP0_hi();	//maybe add some NOP() to allow time for pull up
#define EXP0_FLT()	EXP0_ip(); EXP0_lo();	//Set to i/p w/o pullup


#define	LED_IP()	AUX_DDR	&= ~(1<<LED)
#define	LED_OP()	AUX_DDR	|=  (1<<LED)
#define LED_OFF()	AUX_OUT &= ~(1<<LED)
#define LED_ON()	AUX_OUT |=  (1<<LED)

#define	IRQ_IP()	AUX_DDR	&= ~(1<<IRQ)
#define	IRQ_OP()	AUX_DDR	|=  (1<<IRQ)
#define IRQ_LO()	AUX_OUT &= ~(1<<IRQ)
#define IRQ_HI()	AUX_OUT |=  (1<<IRQ)

#define	CIA10_IP()	AUX_DDR	&= ~(1<<CIA10)
#define	CIA10_OP()	AUX_DDR	|=  (1<<CIA10)
#define CIA10_LO()	AUX_OUT &= ~(1<<CIA10)
#define CIA10_HI()	AUX_OUT |=  (1<<CIA10)

#define	BL_IP()		AUX_DDR	&= ~(1<<BL)
#define	BL_OP()		AUX_DDR	|=  (1<<BL)
#define BL_LO()		AUX_OUT &= ~(1<<BL)
#define BL_HI()		AUX_OUT |=  (1<<BL)

#ifndef pg_XOE	//FINAL_DESIGN
#define	AXLOE_IP()	AUX_DDR	&= ~(1<<AXLOE)
#define	AXLOE_OP()	AUX_DDR	|=  (1<<AXLOE)
#define EXPFF_OP()	AUX_OUT &= ~(1<<AXLOE)	//FF /OE pin low->enable o/p
#define EXPFF_FLT()	AUX_OUT |=  (1<<AXLOE)	//FF /OE pin high->disable o/p
//Caution AXL_CLK() relies on EXPFF_OP() to be called beforehand
//	Think of it like you must enable the output before you can clock it.
//	Floating EXPFF also happens to clock it.  Think of it like it looses it's value if disabled.
#define AXL_CLK()	EXPFF_FLT(); EXPFF_OP(); //same name and convention as purple
#else	//purple and green versions
#define	XOE_IP()	AUX_DDR	&= ~(1<<pg_XOE)
#define	XOE_OP()	AUX_DDR	|=  (1<<pg_XOE)
#define EXPFF_OP()	AUX_OUT &= ~(1<<pg_XOE)	//FF /OE pin low->enable o/p
#define EXPFF_FLT()	AUX_OUT |=  (1<<pg_XOE)	//FF /OE pin high->disable o/p
#endif
