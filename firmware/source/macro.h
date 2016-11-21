#include <avr/io.h>

#define NOP() do { __asm__ __volatile__ ("nop"); } while (0)
// used for a very short delay 


#define LO 0x00
#define HI 0xFF

#define TRUE 0x00
//FALSE is ANYTHING but TRUE, the value signifies the error number

//asci table
#define char_A 	0x41
#define char_C 	0x43
#define char_N 	0x4e
#define char_E 	0x45
#define char_P 	0x50
#define char_R 	0x52
#define char_S 	0x53
#define char_Y 	0x59
#define char_EOF 0x1A

//file header types
#define ERASE	1
#define NES	2
#define SNES 	3
#define SPI 	4
#define JTAG	5 
#define COPY	6 

//file header feilds
#define PRG_BANKS_16 header[4]
#define CHR_BANKS_8 header[5]
#define MAP_LO header[6]
#define MAP_HI header[7]

//Mapper numbers
#define NROM	0
#define MMC1	1
#define UxROM	2
#define CNROM	3
#define MMC3	4
#define MMC2	9
#define MMC4	10
#define ACTION53	28
#define IREM	65
#define FME7	69
#define VRC4	21
#define VRC4alt	23
#define NSF	255

//board versions
#define EPROM0	10
#define EPROM1	11
#define EPROM2	12

#define FLASH0	20
#define FLASH1	21

#define DISCRETE0 30


#define CA13n	7

//PIN DEFINITIONS
//AUX PORTD
#define EXP0	PD0	//RESET_n on SNES
#define LED	PD1
#define EXP9	PD1	//dual purposed pin
#define USBP	PD2
#define IRQ	PD3
#define USBM	PD4
#define CIA10	PD5
#define BL	PD6
#define XOE	PD7	//Only X_OE on purple and green boards
//X_OE and X_CLK on yellow final boards

//CTL PORTC
#define M2	PC0
#define PCE	PC1	//SNES /ROMSEL
#define PRW	PC2
#define AXL	PC3	//Free on green and yellow boards
//Also AXL /OE on Yellow boards
#define CRD	PC4	//SNES /RD
#define CWR	PC5	//SNES /WR
#define CICE 	PC6
#define AHL	PC7	//Also AXL on green proto boards

//PORT DEFINITIONS
#define ADDR_OUT	PORTA
#define ADDR_IN		PINA
#define ADDR_DDR	DDRA

#define DATA_OUT	PORTB
#define DATA_IN		PINB
#define DATA_DDR	DDRB

//#define	UP_ADDR		PORTB
//second revision moves this to PORTA and combines AXL/AHL
//#define	X_ADDR		PORTB	

#define CTL_OUT		PORTC
#define CTL_IN		PINC
#define CTL_DDR		DDRC

#define	AUX_OUT		PORTD
#define AUX_IN		PIND
#define AUX_DDR		DDRD

#define DATA_IP()	DATA_DDR =  LO
#define DATA_OP()	DATA_DDR =  HI
#define DATA_HI()	DATA_OUT =  HI
#define DATA_LO()	DATA_OUT =  LO

#define ADDR_IP()	ADDR_DDR  =  LO
#define ADDR_OP()	ADDR_DDR  =  HI
#define ADDR_HI()	ADDR_OUT =  HI
#define ADDR_LO()	ADDR_OUT =  LO


//AHL, AXL, are always output and high, unless individually asserted.
#define CTL_IP()	CTL_DDR = 0b10001000// &=  ((1<<AHL) | (1<<AXL))
#define CTL_OP()	CTL_DDR = 0b10111111 //&= ~(1<<CICE); CTL_DDR  |=  ~((1<<AHL) | (1<<AXL) | (1<<CICE)) //CIRAM /CE is always input
#define CTL_HI()	CTL_OUT  |=  ~((1<<AHL) | (1<<AXL))
//maintain these high unless individually asserted
#define CTL_LO()	CTL_OUT  &=  ((1<<AHL) | (1<<AXL))

//LED and XOE are separately asserted due to always outputs
#define AUX_IP()	AUX_DDR  &=  ((1<<USBP) | (1<<USBM) | (1<<LED) | (1<<XOE))
//#define AUX_OP()	AUX_DDR  |=  ~((1<<USBP) | (1<<USBM) | (1<<LED) | (1<<XOE))
#define AUX_HI()	AUX_OUT  |=  ~((1<<USBP) | (1<<USBM) | (1<<LED) | (1<<XOE))
#define AUX_LO()	AUX_OUT  &=  ((1<<USBP) | (1<<USBM) | (1<<LED) | (1<<XOE))

//SNES copy cart needs CIRAM A10 and CIRAM /CE to be outputs
#define CC_OP()		AUX_DDR |= (1<<CIA10); CTL_DDR |= (1<<CICE)
#define CC_IP()		AUX_DDR &= ~(1<<CIA10); CTL_DDR &= ~(1<<CICE)

#define	SETUP_LED()	AUX_DDR	|=  (1<<LED)
#define LED_ON()	AUX_OUT |=  (1<<LED)
#define LED_OFF()	AUX_OUT &= ~(1<<LED)

#define SETUP_ADDR_X()		AUX_DDR |=  (1<<XOE)
#define	DISABLE_ADDR_X()	AUX_OUT |=  (1<<XOE)
#define	ENABLE_ADDR_X()		AUX_OUT &= ~(1<<XOE)

#define SETUP_AHL()	CTL_DDR |=  (1<<AHL); CTL_OUT |= (1<<AHL)	//output high
#define SETUP_AXL()	CTL_DDR |=  (1<<AXL); CTL_OUT |= (1<<AXL)	//output high
#define LATCH_AHL()	CTL_OUT  &= ~(1<<AHL); CTL_OUT |= (1<<AHL)	//toggle low -> high
#define LATCH_AXL()	CTL_OUT  &= ~(1<<AXL); CTL_OUT |= (1<<AXL)	//toggle low -> high

#define	M2_IP()		CTL_DDR	&=  ~(1<<M2)
#define	M2_OP()		CTL_DDR	|=  (1<<M2)
#define M2_HI()		CTL_OUT |=  (1<<M2)
#define M2_LO()		CTL_OUT &=  ~(1<<M2)
#define CYCLE()		CTL_OUT |= (1<<M2); NOP(); NOP(); NOP();    CTL_OUT &=  ~(1<<M2); NOP(); NOP(); NOP();    CTL_OUT |= (1<<PCE)	//toggle M2 high -> low for CPU access cycle, and takes PRG /CE high if it was low

#define PCE_HI()		CTL_OUT |=  (1<<PCE)
#define PCE_LO()		CTL_OUT &=  ~(1<<PCE)

#define CICE_HI()		CTL_OUT |=  (1<<CICE)
#define CICE_LO()		CTL_OUT &=  ~(1<<CICE)

#define PRG_RD()	CTL_OUT |=  (1<<PRW)
#define PRG_WR()	CTL_OUT &=  ~(1<<PRW)

#define PRG_RD_PCE_HI()	CTL_OUT |=  (1<<PRW) | (1<<PCE)
#define PRG_WR_PCE_LO()	CTL_OUT &=  ~((1<<PRW) | (1<<PCE))

//#define SNES_RD_SEL_HI()	CTL_OUT |=  (1<<CRD) | (1<<PCE)
#define SNES_WR_SEL_LO()	CTL_OUT &=  ~((1<<CWR) | (1<<PCE))
#define SNES_RD_SEL_LO()	CTL_OUT &=  ~((1<<CRD) | (1<<PCE))
#define SNES_CTL_HI()		CTL_OUT |=  (1<<CWR) | (1<<CRD) | (1<<PCE)
#define CC_CTL_HI()		CTL_OUT |=  (1<<PRW) | (1<<CICE); AUX_OUT |= (1<<CIA10)
//#define SNES_CTL_LO()		CTL_OUT &=  ~((1<<CWR) | (1<<CRD) | (1<<PCE))

#define CHR_OEN()	CTL_OUT |=  (1<<CRD)
#define CHR_OE()	CTL_OUT &=  ~(1<<CRD)
#define CHR_WEN()	CTL_OUT |=  (1<<CWR)	//Not-able  (high)
#define CHR_WE()	CTL_OUT &=  ~(1<<CWR)	//Enabled   (low)

#define CC_OEN()	AUX_OUT |=  (1<<CIA10)
#define CC_OE()		AUX_OUT &=  ~(1<<CIA10)
#define CC_WEN()	CTL_OUT |=  (1<<PRW)	//Not-able  (high)
#define CC_WE()		CTL_OUT &=  ~(1<<PRW)	//Enabled   (low)

#define CHR_RD()	CHR_WEN(); CHR_OE(); 
#define CHR_WR()	CHR_OEN(); CHR_WE(); 	// /OE-hi /WE-lo

			// /WE-hi    /OE-lo
			// PRG RW hi, CIRAMA10 lo
#define CC_RD()		CC_WEN(); CC_OE(); 
#define CC_WR()		CC_OEN(); CC_WE(); 	
			//CIRAM A10 hi, PRGRW lo
			// /OE-hi    /WE-lo

//EXP0 to only be pulled high because XO boards don't level shift EXP0
//EXP0_LO(); replace with EXP0_LO(); EXP0_OP();
//EXP0_HI(); replace with EXP0_IP(); EXP0_HI();
#define	EXP0_IP()	AUX_DDR	&=  ~(1<<EXP0)
#define	EXP0_OP()	AUX_DDR	|=  (1<<EXP0)
#define EXP0_HIGH()	AUX_OUT |=  (1<<EXP0)
#define EXP0_LOW()	AUX_OUT &=  ~(1<<EXP0)
#define EXP0_HI()	EXP0_IP(); EXP0_HIGH(); NOP(); NOP(); NOP(); NOP();
#define EXP0_LO()	EXP0_LOW(); EXP0_OP();

#define PU_CICE()	CTL_DDR &= ~(1<<CICE); CTL_OUT |= (1<<CICE);
//Masks for where the signals are on the bus
/*		   
//	       	    0b76543210
#define AHL	    0b10000000
#define ciramA10    0b01000000
#define chr_WR      0b00100000
#define chr_RD      0b00010000
#define AXL         0b00001000
#define prgR_W      0b00000100
#define prg_CE      0b00000010
#define M2          0b00000001

#define _WR			0b00100100	//write for both busses
*/

