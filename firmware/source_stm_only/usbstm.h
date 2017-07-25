#ifndef _usbstm_h
#define _usbstm_h


//include target chip port definition library files
#include <stm32f0xx.h>


//clear RX interrupt
//set tx field to keep from accidentally clearing //mask out toggle feilds making them zero //clear rx bit removing active interrupt
#define EP0R_CLR_CTR_RX()	 USB->EP0R = (((USB->EP0R | USB_EP_CTR_TX) 	& USB_EPREG_MASK ) 	& ~USB_EP_CTR_RX )

//clear TX interrupt
#define EP0R_CLR_CTR_TX()	 USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX) 	& USB_EPREG_MASK ) 	& ~USB_EP_CTR_TX )

//VALID need to get both status bits set
//XOR the current value of status bits with 1 to write back inverted value, gets all status bits set
//							OR in bits that need set,	AND in bits to keep or avail for XOR,  XOR toggles to invert
#define	USB_EP0R_RXTX_VALID() USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & ~(USB_EP_DTOG_RX | USB_EP_DTOG_TX)) ^ (USB_EPTX_STAT | USB_EPRX_STAT))	
#define	USB_EP0R_RX_VALID() USB->EP0R = ((((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX)) & (USB_EPREG_MASK | USB_EPRX_STAT)) ^ USB_EPRX_STAT )
#define	USB_EP0R_TX_VALID() USB->EP0R = ((((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX)) & (USB_EPREG_MASK | USB_EPTX_STAT)) ^ USB_EPTX_STAT )
#define	USB_EP0R_RX_VALID_STATUS_OUT() USB->EP0R = ((((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX | USB_EP_KIND)) & (USB_EPREG_MASK | USB_EPRX_STAT)) ^ USB_EPRX_STAT )
#define	USB_EP0R_EP_KIND_ANY() USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX)) & (USB_EPREG_MASK & ~USB_EP_KIND)) 

//DISABLE need to get both bits cleared
//write back current value of status bits to toggle all 1's to zeros
//							OR in bits that need set	  AND in bits to keep or toggle from 1 to 0
#define	USB_EP0R_RXTX_DIS() USB->EP0R = ((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & ~(USB_EP_DTOG_RX | USB_EP_DTOG_TX))
#define	USB_EP0R_RX_DIS() USB->EP0R = ((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & (USB_EPREG_MASK | USB_EPRX_STAT))
#define	USB_EP0R_TX_DIS() USB->EP0R = ((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & (USB_EPREG_MASK | USB_EPTX_STAT))

//NAK/STALL need to get one bit set, and the other cleared
//Easiest way would be to DISABLE, and then set desired bit, uses two accesses to EP0R
//			  DISABLE first			              OR in bits that need set		AND in bits to keep or toggle
#define USB_EP0R_RX_NAK() USB_EP0R_RX_DIS(); USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & (USB_EPREG_MASK)) | USB_EP_RX_NAK)
#define USB_EP0R_TX_NAK() USB_EP0R_TX_DIS(); USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & (USB_EPREG_MASK)) | USB_EP_TX_NAK)
#define USB_EP0R_RXTX_NAK() USB_EP0R_RXTX_DIS(); USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & (USB_EPREG_MASK)) | (USB_EP_RX_NAK | USB_EP_TX_NAK))

#define USB_EP0R_RX_STALL() USB_EP0R_RX_DIS(); USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & (USB_EPREG_MASK)) | USB_EP_RX_STALL)
#define USB_EP0R_TX_STALL() USB_EP0R_TX_DIS(); USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & (USB_EPREG_MASK)) | USB_EP_TX_STALL)
#define USB_EP0R_RXTX_STALL() USB_EP0R_RXTX_DIS(); USB->EP0R = (((USB->EP0R | USB_EP_CTR_RX | USB_EP_CTR_TX) & (USB_EPREG_MASK)) | (USB_EP_RX_STALL | USB_EP_TX_STALL))

//USB_COUNTn_RX table entries are a little annoying..
//create some macros to simplify setting RX COUNT
#define	BL_SIZE32	(uint16_t) 0x8000
#define	BL_SIZE2	(uint16_t) 0x0000
#define	NUM_BLOCKS	10U	//shift number of blocks by this amount
#define	RX_COUNT_MSK	(uint16_t) 0x03FF

// Cannot setup for 0 bytes to be received, that's equivalent of STATUS OUT packet which isn't a true data reception
#define _ADDRX_SET(oper) _DATA_OP(); DATA_OUT = oper; _AXL_CLK(); _DATA_IP();
#define	USB_RX_2TO62_MUL2B(oper)	((uint16_t) ((BL_SIZE2) | ((oper/2)<<NUM_BLOCKS))) 
#define	USB_RX_32TO992_MUL32B(oper)	((uint16_t) ((BL_SIZE32) | ((oper/32-1)<<NUM_BLOCKS))) 

//#define	USB_RX_2BYTES	((uint16_t) ((BL_SIZE2) | ((2/2)<<NUM_BLOCKS))) 
//#define	USB_RX_4BYTES	((uint16_t) ((BL_SIZE2) | ((4/2)<<NUM_BLOCKS))) 
//#define	USB_RX_6BYTES	((uint16_t) ((BL_SIZE2) | ((6/2)<<NUM_BLOCKS))) 
//#define	USB_RX_8BYTES	((uint16_t) ((BL_SIZE2) | ((8/2)<<NUM_BLOCKS))) 
//#define	USB_RX_16BYTES	((uint16_t) ((BL_SIZE2) | ((16/2)<<NUM_BLOCKS))) 
//// Can set from 2-62B in increments of 2B, just haven't bothered defining them all here...
//#define	USB_RX_32BYTES	((uint16_t) ((BL_SIZE32) | ((32/32-1)<<NUM_BLOCKS))) 
//#define	USB_RX_64BYTES	((uint16_t) ((BL_SIZE32) | ((64/32-1)<<NUM_BLOCKS))) 
//#define	USB_RX_96BYTES	((uint16_t) ((BL_SIZE32) | ((96/32-1)<<NUM_BLOCKS))) 
//#define	USB_RX_128BYTES	((uint16_t) ((BL_SIZE32) | ((128/32-1)<<NUM_BLOCKS))) 
//#define	USB_RX_256BYTES	((uint16_t) ((BL_SIZE32) | ((256/32-1)<<NUM_BLOCKS))) 
//#define	USB_RX_512BYTES	((uint16_t) ((BL_SIZE32) | ((512/32-1)<<NUM_BLOCKS))) 
//// Can set from 32-992B in increments of 32B, just haven't bothered defining them all here...
//#define	USB_RX_992BYTES	((uint16_t) ((BL_SIZE32) | ((992/32-1)<<NUM_BLOCKS))) 
//	while 1024 is max size of packet for USB 2.0, this part's designed USB buffer memory 
//	creates the limit of 992B xfr buffer + 32B buffer table (also in buffer ram) = 1024B size of USB buffer ram



//buffer table itself is located in 1KB buffer above, but it's location is programmable
#define USB_BTABLE_BASE ((uint16_t) 0x0000)	//least 3 significant bits are forced to zero
#define USB_BTABLE_SIZE 64	//32x 16bit halfwords

//Endpoint 0: setup as 8Bytes TX & RX following buffer table
//	endpoint 0 size defined in usb_descriptors.h as it's needed there
//#define EP0_SIZE	0x08		//8Bytes same as usb 1.1 for now
#define EP0_TX_ADDR     (USB_BTABLE_BASE + USB_BTABLE_SIZE)
#define EP0_TX_BASE	(EP0_TX_ADDR / 2)
//NOTE!!!  control_xfr functions are hardcoded for EP0 size of 8Bytes currently, must update if changing

#define EP0_RX_ADDR     (EP0_TX_ADDR + EP0_SIZE)
#define EP0_RX_BASE	(EP0_RX_ADDR / 2)

//#define LOG0     	(EP0_RX_ADDR + EP0_SIZE)
#define LOG0     	(64+16) / 2
#define LOG4		LOG0 + 2
#define LOG8		LOG0 + 4
#define LOGC		LOG0 + 6
#define LOG10		LOG0 + 8


//Transmission buffer address n (USB_ADDRn_TX)
//Address offset: [USB_BTABLE] + n*8
//Note: In case of double-buffered or isochronous endpoints in the IN direction, this address location is referred to as USB_ADDRn_TX_0.
//In case of double-buffered or isochronous endpoints in the OUT direction, this address
//location is used for USB_ADDRn_RX_0.
#define USB_ADDR0_TX (USB_BTABLE_BASE + 0*8)
#define USB_ADDR1_TX (USB_BTABLE_BASE + 1*8)
#define USB_ADDR2_TX (USB_BTABLE_BASE + 2*8)
#define USB_ADDR3_TX (USB_BTABLE_BASE + 3*8)
#define USB_ADDR4_TX (USB_BTABLE_BASE + 4*8)
#define USB_ADDR5_TX (USB_BTABLE_BASE + 5*8)
#define USB_ADDR6_TX (USB_BTABLE_BASE + 6*8)
#define USB_ADDR7_TX (USB_BTABLE_BASE + 7*8)
//Bits 15:1 ADDRn_TX[15:1]: Transmission buffer address
//These bits point to the starting address of the packet buffer containing data to be transmitted
//by the endpoint associated with the USB_EPnR register at the next IN token addressed to it.
//Bit 0 Must always be written as Î÷Îõ0 since packet memory is half-word wide and all packet buffers
//must be half-word aligned.

//Transmission byte count n (USB_COUNTn_TX)
//Address offset: [USB_BTABLE] + n*8 + 2
//Note: In case of double-buffered or isochronous endpoints in the IN direction, this address location is referred to as USB_COUNTn_TX_0.
//In case of double-buffered or isochronous endpoints in the OUT direction, this address
//location is used for USB_COUNTn_RX_0.
#define USB_COUNT0_TX (USB_BTABLE_BASE + 0*8 + 1)
#define USB_COUNT1_TX (USB_BTABLE_BASE + 1*8 + 1)
#define USB_COUNT2_TX (USB_BTABLE_BASE + 2*8 + 1)
#define USB_COUNT3_TX (USB_BTABLE_BASE + 3*8 + 1)
#define USB_COUNT4_TX (USB_BTABLE_BASE + 4*8 + 1)
#define USB_COUNT5_TX (USB_BTABLE_BASE + 5*8 + 1)
#define USB_COUNT6_TX (USB_BTABLE_BASE + 6*8 + 1)
#define USB_COUNT7_TX (USB_BTABLE_BASE + 7*8 + 1)
//Bits 15:10These bits are not used since packet size is limited by USB specifications to 1023 bytes. Their
//value is not considered by the USB peripheral.
//Bits 9:0 COUNTn_TX[9:0]: Transmission byte count
//These bits contain the number of bytes to be transmitted by the endpoint associated with the
//USB_EPnR register at the next IN token addressed to it.


//Reception buffer address n (USB_ADDRn_RX)
//Address offset: [USB_BTABLE] + n*8 + 4
//Note: In case of double-buffered or isochronous endpoints in the OUT direction, this address
//location is referred to as USB_ADDRn_RX_1.
//In case of double-buffered or isochronous endpoints in the IN direction, this address location is used for USB_ADDRn_TX_1.
#define USB_ADDR0_RX (USB_BTABLE_BASE + 0*8 + 2)
#define USB_ADDR1_RX (USB_BTABLE_BASE + 1*8 + 2)
#define USB_ADDR2_RX (USB_BTABLE_BASE + 2*8 + 2)
#define USB_ADDR3_RX (USB_BTABLE_BASE + 3*8 + 2)
#define USB_ADDR4_RX (USB_BTABLE_BASE + 4*8 + 2)
#define USB_ADDR5_RX (USB_BTABLE_BASE + 5*8 + 2)
#define USB_ADDR6_RX (USB_BTABLE_BASE + 6*8 + 2)
#define USB_ADDR7_RX (USB_BTABLE_BASE + 7*8 + 2)
//Bits 15:1 ADDRn_RX[15:1]: Reception buffer address
//These bits point to the starting address of the packet buffer, which will contain the data
//received by the endpoint associated with the USB_EPnR register at the next OUT/SETUP
//token addressed to it.
//Bit 0 This bit must always be written as Î÷Îõ0 since packet memory is half-word wide and all packet
//buffers must be half-word aligned.


//Reception byte count n (USB_COUNTn_RX)
//Address offset: [USB_BTABLE] + n*8 + 6
//Note: In case of double-buffered or isochronous endpoints in the OUT direction, this address
//location is referred to as USB_COUNTn_RX_1.
//In case of double-buffered or isochronous endpoints in the IN direction, this address location is used for USB_COUNTn_TX_1.
#define USB_COUNT0_RX (USB_BTABLE_BASE + 0*8 + 3)
#define USB_COUNT1_RX (USB_BTABLE_BASE + 1*8 + 3)
#define USB_COUNT2_RX (USB_BTABLE_BASE + 2*8 + 3)
#define USB_COUNT3_RX (USB_BTABLE_BASE + 3*8 + 3)
#define USB_COUNT4_RX (USB_BTABLE_BASE + 4*8 + 3)
#define USB_COUNT5_RX (USB_BTABLE_BASE + 5*8 + 3)
#define USB_COUNT6_RX (USB_BTABLE_BASE + 6*8 + 3)
#define USB_COUNT7_RX (USB_BTABLE_BASE + 7*8 + 3)
//This table location is used to store two different values, both required during packet
//reception. The most significant bits contains the definition of allocated buffer size, to allow
//buffer overflow detection, while the least significant part of this location is written back by the
//USB peripheral at the end of reception to give the actual number of received bytes. Due to the 
//restrictions on the number of available bits, buffer size is represented using the number of allocated 
//memory blocks, where block size can be selected to choose the trade-off between fine-granularity/small-buffer 
//and coarse-granularity/large-buffer. The size of allocated buffer is a part of the endpoint descriptor 
//and it is normally defined during the enumeration process according to its maxPacketSize parameter value 
//(See ÎéÎíUniversal Serial Bus SpecificationÎéÎí).
//
//Bit 15 BL_SIZE: Block size
//This bit selects the size of memory block used to define the allocated buffer area.
//	-If BL_SIZE=0, the memory block is 2-byte large, which is the minimum block
//	allowed in a half-word wide memory. With this block size the allocated buffer size
//	ranges from 2 to 62 bytes.
//	-If BL_SIZE=1, the memory block is 32-byte large, which allows to reach the
//	maximum packet length defined by USB specifications. With this block size the
//	allocated buffer size theoretically ranges from 32 to 1024 bytes, which is the longest
//	packet size allowed by USB standard specifications. However, the applicable size is limited by the available buffer memory.
//
//Bits 14:10 NUM_BLOCK[4:0]: Number of blocks
//These bits define the number of memory blocks allocated to this packet buffer. The actual amount of 
//allocated memory depends on the BL_SIZE value as illustrated in Table127.
//
//Bits 9:0 COUNTn_RX[9:0]: Reception byte count
//These bits contain the number of bytes received by the endpoint associated with the
//USB_EPnR register during the last OUT/SETUP transaction addressed to it.

//#define __packed __attribute((packed))__
typedef struct usbRequest_t{
	uint8_t		bmRequestType;
	uint8_t		bRequest;
	uint16_t	wValue;
	uint16_t	wIndex;
	uint16_t	wLength;
}usbRequest_t;
//}__attribute((__packed__))usbRequest_t;

//	bmRequestType
//D7 Data Phase Transfer Direction
//0 = Host to Device
//1 = Device to Host
#define REQ_DIR		0x80
#define REQ_DIR_OUT	0x00
#define REQ_DIR_IN	0x80
//D6..5 Type
//0 = Standard
//1 = Class
//2 = Vendor
//3 = Reserved
#define REQ_TYPE	0x60
#define REQ_TYPE_STD	0x00
#define REQ_TYPE_CLASS	0x20
#define REQ_TYPE_VEND	0x40
#define REQ_TYPE_RES	0x60
//D4..0 Recipient
//0 = Device
//1 = Interface
//2 = Endpoint
//3 = Other
#define REQ_RECIP	0x0F
#define REQ_RECIP_DEV	0x00
#define REQ_RECIP_INT	0x01
#define REQ_RECIP_EP	0x02
#define REQ_RECIP_OTH	0x03
//4..31 = Reserved


#define usbMsgPtr_t uint16_t *
extern  usbMsgPtr_t usbMsgPtr;

extern uint16_t usbFunctionSetup(uint8_t data[8]);
extern uint8_t usbFunctionWrite(uint8_t *data, uint8_t len);

void init_clock();
void init_usb_clock();
void usb_reset_recovery();
void init_usb();


#endif
