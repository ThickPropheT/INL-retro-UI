#include <avr/io.h>

//This file contains pinout translations from AVR names to "kazzo" names
//this file also works to make all kazzo versions compatible and "alike" 

//=======================================================
//History of PCB revsisions produced by InfiniteNesLives
//=======================================================
// 
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
//		would have to free /RESET and wire to EXP0/PD0 to flash INL SNES board.
// 
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
// * Includes NES, Famicom, & SNES connector
// * SNES connector improvement to correct pitch issue with prev ver.
// * Addition of PCT resettable fuse on incoming power.
// 	-Provides protection to SNES boards inserted backwards.
// * Rearrangement of BL/RUN switch and screw holes for better case design.
// * Cut out buzzer pads (PD6) from under MCU which was never developed.
// * INPUT/OUTPUT differences:
// 	-None from previous version
//

#define LO 0x00
#define HI 0xFF
//#define TRUE 0x00
//FALSE is ANYTHING but TRUE, the value signifies the error number


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
#define ADDR_HI()	ADDR_OUT = HI
#define ADDR_LO()	ADDR_OUT = LO


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
#define DATA_HI()	DATA_OUT = HI
#define DATA_LO()	DATA_OUT = LO


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
#define CTL_HI()	CTL_OUT = HI
#define CTL_LO()	CTL_OUT = LO
//OLD CODE	//AHL, AXL, are always output and high, unless individually asserted.
//	#define CTL_IP()	CTL_DDR = 0b10001000// &=  ((1<<AHL) | (1<<AXL))
//	#define CTL_OP()	CTL_DDR = 0b10111111 //&= ~(1<<CICE); CTL_DDR  |=  ~((1<<AHL) | (1<<AXL) | (1<<CICE)) //CIRAM /CE is always input
//	#define CTL_HI()	CTL_OUT  |=  ~((1<<AHL) | (1<<AXL))
//	//maintain these high unless individually asserted
//	#define CTL_LO()	CTL_OUT  &=  ((1<<AHL) | (1<<AXL))

//PIN DEFN
#define M2	PC0	//NES, FC, & SNES (SYSCLK)
#define ROMSEL	PC1	//(aka PRG/CE) NES, FC, & SNES 
#define PRGRW	PC2	//PRG R/W on NES & FC 

#define P_AXL	PC3	//EXP FF CLK on purple boards
#define FREE	PC3	//Free pin on all other boards

#define C_RD	PC4	//NES & FC CHR /RD
#define S_RD	PC4	//SNES /RD

#define C_WR	PC5	//NES & FC CHR /WR
#define S_WR	PC5	//SNES /WR

#define CICE 	PC6	//NES & FC CIRAM /CE, most carts are 2screen tying this to CHR A13 making this an I/P

#define AHL	PC7	//ADDR MID FF CLK per orig kazzo design
#define G_AXL	PC7	//Also EXP/ADDRHI FF CLK on green prototype

//PIN MACROS
#define	M2_IP()		CTL_DDR	&= ~(1<<M2)
#define	M2_OP()		CTL_DDR	|=  (1<<M2)
#define M2_HI()		CTL_OUT |=  (1<<M2)
#define M2_LO()		CTL_OUT &= ~(1<<M2)
//TODO read M2 PIN as input

#define ROMSEL_IP()	CTL_DDR &= ~(1<<ROMSEL)
#define ROMSEL_OP()	CTL_DDR |=  (1<<ROMSEL)
#define ROMSEL_HI()	CTL_OUT |=  (1<<ROMSEL)
#define ROMSEL_LO()	CTL_OUT &= ~(1<<ROMSEL)

#define CICE_IP()	CTL_DDR &= ~(1<<CICE)
#define CICE_OP()	CTL_DDR |=  (1<<CICE)
#define CICE_HI()	CTL_OUT |=  (1<<CICE)
#define CICE_LO()	CTL_OUT &= ~(1<<CICE)
//#define PU_CICE()	CTL_DDR &= ~(1<<CICE); CTL_OUT |= (1<<CICE);

#define PRGRW_IP()	CTL_OUT &= ~(1<<PRGRW)
#define PRGRW_OP()	CTL_OUT |=  (1<<PRGRW)
#define PRGRW_RD()	CTL_OUT |=  (1<<PRGRW)	//HI for reads
#define PRGRW_WR()	CTL_OUT &= ~(1<<PRGRW)	//LO for writes

//	//SNES copy cart needs CIRAM A10 and CIRAM /CE to be outputs
//	#define CC_OP()		AUX_DDR |= (1<<CIA10); CTL_DDR |= (1<<CICE)
//	#define CC_IP()		AUX_DDR &= ~(1<<CIA10); CTL_DDR &= ~(1<<CICE)


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
#define AUX_HI()	AUX_OUT  |= ~((1<<USBP) | (1<<USBM))
#define AUX_LO()	AUX_OUT  &=  ((1<<USBP) | (1<<USBM))

//PIN DEFN
#define EXP0	PD0	//NES EXP0 
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

#define P_XOE	PD7	//EXP/ADDRHI FF /OE pin on purple and green boards
#define AXLOE	PD7	//EXP/ADDRHI FF CLK & /OE pin on final board versions

//PIN MACROS
//	//EXP0 to only be pulled high because XO boards don't level shift EXP0
//	//EXP0_LO(); replace with EXP0_LO(); EXP0_OP();
//	//EXP0_HI(); replace with EXP0_IP(); EXP0_HI();
//	#define	EXP0_IP()	AUX_DDR	&=  ~(1<<EXP0)
//	#define	EXP0_OP()	AUX_DDR	|=  (1<<EXP0)
//	#define EXP0_HIGH()	AUX_OUT |=  (1<<EXP0)
//	#define EXP0_LOW()	AUX_OUT &=  ~(1<<EXP0)
//	#define EXP0_HI()	EXP0_IP(); EXP0_HIGH(); NOP(); NOP(); NOP(); NOP();
//	#define EXP0_LO()	EXP0_LOW(); EXP0_OP();

#define	ENABLE_LED()	AUX_DDR	|=  (1<<LED)
#define	DISABLE_LED()	AUX_DDR	&= ~(1<<LED)
#define LED_ON()	AUX_OUT |=  (1<<LED)
#define LED_OFF()	AUX_OUT &= ~(1<<LED)

//#define SETUP_ADDR_X()		AUX_DDR |=  (1<<XOE)
//#define	DISABLE_ADDR_X()	AUX_OUT |=  (1<<XOE)
//#define	ENABLE_ADDR_X()		AUX_OUT &= ~(1<<XOE)
//
//#define SETUP_AHL()	CTL_DDR |=  (1<<AHL); CTL_OUT |= (1<<AHL)	//output high
//#define SETUP_AXL()	CTL_DDR |=  (1<<AXL); CTL_OUT |= (1<<AXL)	//output high
//#define LATCH_AHL()	CTL_OUT  &= ~(1<<AHL); CTL_OUT |= (1<<AHL)	//toggle low -> high
//#define LATCH_AXL()	CTL_OUT  &= ~(1<<AXL); CTL_OUT |= (1<<AXL)	//toggle low -> high



//	#define PRG_RD_PCE_HI()	CTL_OUT |=  (1<<PRW) | (1<<PCE)
//	#define PRG_WR_PCE_LO()	CTL_OUT &=  ~((1<<PRW) | (1<<PCE))
//	
//	//#define SNES_RD_SEL_HI()	CTL_OUT |=  (1<<CRD) | (1<<PCE)
//	#define SNES_WR_SEL_LO()	CTL_OUT &=  ~((1<<CWR) | (1<<PCE))
//	#define SNES_RD_SEL_LO()	CTL_OUT &=  ~((1<<CRD) | (1<<PCE))
//	#define SNES_CTL_HI()		CTL_OUT |=  (1<<CWR) | (1<<CRD) | (1<<PCE)
//	#define CC_CTL_HI()		CTL_OUT |=  (1<<PRW) | (1<<CICE); AUX_OUT |= (1<<CIA10)
//	//#define SNES_CTL_LO()		CTL_OUT &=  ~((1<<CWR) | (1<<CRD) | (1<<PCE))
//	
//	#define CHR_OEN()	CTL_OUT |=  (1<<CRD)
//	#define CHR_OE()	CTL_OUT &=  ~(1<<CRD)
//	#define CHR_WEN()	CTL_OUT |=  (1<<CWR)	//Not-able  (high)
//	#define CHR_WE()	CTL_OUT &=  ~(1<<CWR)	//Enabled   (low)
//	
//	#define CC_OEN()	AUX_OUT |=  (1<<CIA10)
//	#define CC_OE()		AUX_OUT &=  ~(1<<CIA10)
//	#define CC_WEN()	CTL_OUT |=  (1<<PRW)	//Not-able  (high)
//	#define CC_WE()		CTL_OUT &=  ~(1<<PRW)	//Enabled   (low)
//	
//	#define CHR_RD()	CHR_WEN(); CHR_OE(); 
//	#define CHR_WR()	CHR_OEN(); CHR_WE(); 	// /OE-hi /WE-lo
//	
//				// /WE-hi    /OE-lo
//				// PRG RW hi, CIRAMA10 lo
//	#define CC_RD()		CC_WEN(); CC_OE(); 
//	#define CC_WR()		CC_OEN(); CC_WE(); 	
//				//CIRAM A10 hi, PRGRW lo
//				// /OE-hi    /WE-lo



