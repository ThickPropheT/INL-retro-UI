
#include "usb.h"

#include "pinport.h"

//USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8]);
/* This function is called when the driver receives a SETUP transaction from
 * the host which is not answered by the driver itself (in practice: class and
 * vendor requests). All control transfers start with a SETUP transaction where
 * the host communicates the parameters of the following (optional) data
 * transfer. The SETUP data is available in the 'data' parameter which can
 * (and should) be casted to 'usbRequest_t *' for a more user-friendly access
 * to parameters.
 *
 * If the SETUP indicates a control-in transfer, you should provide the
 * requested data to the driver. There are two ways to transfer this data:
 * (1) Set the global pointer 'usbMsgPtr' to the base of the static RAM data
 * block and return the length of the data in 'usbFunctionSetup()'. The driver
 * will handle the rest. Or (2) return USB_NO_MSG in 'usbFunctionSetup()'. The
 * driver will then call 'usbFunctionRead()' when data is needed. See the
 * documentation for usbFunctionRead() for details.
 *
 * If the SETUP indicates a control-out transfer, the only way to receive the
 * data from the host is through the 'usbFunctionWrite()' call. If you
 * implement this function, you must return USB_NO_MSG in 'usbFunctionSetup()'
 * to indicate that 'usbFunctionWrite()' should be used. See the documentation
 * of this function for more information. If you just want to ignore the data
 * sent by the host, return 0 in 'usbFunctionSetup()'.
 *
 * Note that calls to the functions usbFunctionRead() and usbFunctionWrite()
 * are only done if enabled by the configuration in usbconfig.h.
 */

	//typedef struct usbRequest{
	//	uchar       bmRequestType;
	//	uchar       bRequest;
	//	usbWord_t   wValue;
	//	usbWord_t   wIndex;
	//	usbWord_t   wLength;
	//}usbRequest_t;

#define ENDPOINT_BIT   0x80    //Bit 7 of bmRequest type determines endpoint
#define ENDPOINT_IN    0x80    //In: device-to-host.
#define ENDPOINT_OUT   0x00    //Out: host-to-device. 


typedef struct setup_packet{
	uint8_t		bmRequestType;	//contains endpoint
	uint8_t		bRequest;	//designates dictionary of opcode
	uint8_t		opcode;		//wValueLSB (little endian)
	uint8_t		wValueMSB;	//expansion byte
	uint8_t		wIndexLSB;	//operand LSB
	uint8_t		wIndexMSB;	//operand MSB
	uint16_t	wLength;
}setup_packet;

USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8]) {

	//cast incoming data into the the usb setup packet it is
	setup_packet *spacket = (void *)data;

	//8 Byte buffer to be used for returning error code and return values
	//must be static so driver can still access after function return
	static uchar rv[8];
	//rv[0] contains opcode success/error code
	//rv[1-7] available for return data, start with index 1

	//number of bytes to return to host
	uint8_t rlen = 0;

	//determine endpoint IN/OUT
	if ( (spacket->bmRequestType & ENDPOINT_BIT) == ENDPOINT_IN ) {
		//read from device request
		//send error code and return value
		rlen = 1; //min value of error code
	} else {
		//write to device request 
		//host doesn't want to waste time with reading back error codes
		rlen = 0;
		//must also come here if opcode has payload coming in data packets to follow
		//in that case host would have to send follow up read request to get error code
	}


	switch(spacket->bRequest) {
		case PINPORT:
			if((spacket->opcode > PP_OPCODE_ONLY_MIN) 
			 & (spacket->opcode < PP_OPCODE_ONLY_MAX)) {
				rv[0] = pinport_opcode_only( spacket->opcode );	

			} else if ((spacket->opcode > PP_OPCODE_8BOP_MIN) 
				 & (spacket->opcode < PP_OPCODE_8BOP_MAX)) {
				rv[0] = pinport_opcode_8b_operand( spacket->opcode, spacket->wIndexLSB );	

			} else if ((spacket->opcode > PP_OPCODE_16BOP_MIN) 
				 & (spacket->opcode < PP_OPCODE_16BOP_MAX)) {
				rv[0] = pinport_opcode_16b_operand( spacket->opcode,
							spacket->wIndexMSB, spacket->wIndexLSB );	

			} else if ((spacket->opcode > PP_OPCODE_24BOP_MIN) 
				 & (spacket->opcode < PP_OPCODE_24BOP_MAX)) {
				rv[0] = pinport_opcode_24b_operand( spacket->opcode,
					spacket->wValueMSB, spacket->wIndexMSB, spacket->wIndexLSB );	

			} else if ((spacket->opcode > PP_OPCODE_8BRV_MIN) 
				 & (spacket->opcode < PP_OPCODE_8BRV_MAX)) {
				rv[0] = pinport_opcode_8b_return( spacket->opcode, &rv[1]);
				//return error code plus opcode return value
				rlen ++;
			}
			break; //end of PINPORT
		default:
			//request (aka dictionary) is unknown
			rv[0] = ERR_UNKN_DICTIONARY;
	}

	/* (1) Set the global pointer 'usbMsgPtr' to the base of the static RAM data
	 * block and return the length of the data in 'usbFunctionSetup()'. The driver
	 * will handle the rest. Or (2) return USB_NO_MSG in 'usbFunctionSetup()'. The
	 * driver will then call 'usbFunctionRead()' when data is needed. See the
	 */
	usbMsgPtr = rv;
	return rlen;

	//return USB_NO_MSG;
	//Don't have a use for usbFunctionRead yet..  Not expecting to anytime soon
	//probably easier and perhaps faster to send cart dump commands and store rom image
	//in a buffer to be returned here.
}


//USB_PUBLIC uchar usbFunctionRead(uchar *data, uchar len);
/* This function is called by the driver to ask the application for a control
 * transfer's payload data (control-in). It is called in chunks of up to 8
 * bytes each. You should copy the data to the location given by 'data' and
 * return the actual number of bytes copied. If you return less than requested,
 * the control-in transfer is terminated. If you return 0xff, the driver aborts
 * the transfer with a STALL token.
 * In order to get usbFunctionRead() called, define USB_CFG_IMPLEMENT_FN_READ
 * to 1 in usbconfig.h and return 0xff in usbFunctionSetup()..
 */
//USB_PUBLIC uchar usbFunctionRead(uchar *data, uchar len) {
//	//this function should only get called if usbFunctionSetup returns USB_NO_MSG
////	data[0] = 0xAA;
//	return len;
//}


//USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len);
/* This function is called by the driver to provide a control transfer's
 * payload data (control-out). It is called in chunks of up to 8 bytes. The
 * total count provided in the current control transfer can be obtained from
 * the 'length' property in the setup data. If an error occurred during
 * processing, return 0xff (== -1). The driver will answer the entire transfer
 * with a STALL token in this case. If you have received the entire payload
 * successfully, return 1. If you expect more data, return 0. If you don't
 * know whether the host will send more data (you should know, the total is
 * provided in the usbFunctionSetup() call!), return 1.
 * NOTE: If you return 0xff for STALL, 'usbFunctionWrite()' may still be called
 * for the remaining data. You must continue to return 0xff for STALL in these
 * calls.
 * In order to get usbFunctionWrite() called, define USB_CFG_IMPLEMENT_FN_WRITE
 * to 1 in usbconfig.h and return 0xff in usbFunctionSetup()..
 */
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) {
	return 1;	//"don't know how much data coming
}

