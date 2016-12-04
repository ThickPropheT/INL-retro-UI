#include "dictionary.h"

/* Make dictionary calls simpler
 * provide USBtransfer pointer with handle set to retro programmer
 * provide dictionary as defined in shared_dictionaries.h (request)
 * provide opcode from the dictionary (wValueLSB)
 * provide 16bit addr used for usb transfer index (optional, can also send 8bits applied to wIndexLSB)
 * provide miscdata optional to be used for wValueMSB
 * provide data buffer pointer and length
 *
 * makes call to usb_transfer after determining:
 * endpoint direction
 * data length
 *
 * debug print of call and return values
 */

//default call dictionary without print option
int dictionary_call( USBtransfer *transfer, uint8_t dictionary, uint8_t opcode, uint16_t addr, 
			uint8_t miscdata, uint8_t endpoint, uint8_t *buffer, uint16_t length)
{
	return dictionary_call_print_option( FALSE, transfer, dictionary, opcode, addr, miscdata, endpoint, buffer, length);
}

//debug call dictionary without print option
int dictionary_call_debug( USBtransfer *transfer, uint8_t dictionary, uint8_t opcode, uint16_t addr, 
			uint8_t miscdata, uint8_t endpoint, uint8_t *buffer, uint16_t length)
{
	return dictionary_call_print_option( ~FALSE, transfer, dictionary, opcode, addr, miscdata, 
						endpoint, buffer, length);
}

int dictionary_call_print_option( int print_debug, USBtransfer *transfer, uint8_t dictionary, uint8_t opcode,
			 uint16_t addr, uint8_t miscdata, uint8_t endpoint, uint8_t *buffer, uint16_t length)
{
	transfer->request   = dictionary;	
	transfer->wValueMSB = miscdata;
	transfer->wValueLSB = opcode;
	transfer->wIndexMSB = (addr>>8);
	transfer->wIndexLSB = addr;

	//default IN for now reading back error codes from short commands
	transfer->endpoint = endpoint;
	transfer->wLength = length;
	//default length of zero
//	transfer->wLength = 0;

	//return data buffer big enough for one data packet
	uint8_t rbuf[8];

	if ( buffer == NULL ) {
		rbuf[0] = 0xee;
		rbuf[1] = 0xee;
		rbuf[2] = 0xee;
		rbuf[3] = 0xee;
		rbuf[4] = 0xee;
		rbuf[5] = 0xee;
		rbuf[6] = 0xee;
		rbuf[7] = 0xee;
		transfer->data = rbuf;
	} else {
		transfer->data = buffer;
	}

	
	if (print_debug) {
		printf("\ndictionary call dict:%d opcode:%dd/%xh addr:%x data:%x\n", 
			dictionary, opcode, opcode, addr, miscdata);
	}
	switch (dictionary) {
		case PINPORT: debug("dict: PINPORT");
			//transfer->wLength = 1;
			switch (opcode) {
				case PP_OPCODE_ONLY_MIN ... PP_OPCODE_ONLY_MAX:
					debug("PP_OPCODE_ONLY");
					break;
				case PP_OPCODE_8BOP_MIN ... PP_OPCODE_8BOP_MAX:
					debug("PP_OPCODE_8BOP");
					break;
				case PP_OPCODE_16BOP_MIN ... PP_OPCODE_16BOP_MAX:
					debug("PP_OPCODE_16BOP");
					break;
				case PP_OPCODE_24BOP_MIN ... PP_OPCODE_24BOP_MAX:
					debug("PP_OPCODE_24BOP");
					break;
				case PP_OPCODE_8BRV_MIN ... PP_OPCODE_8BRV_MAX:
					debug("PP_OPCODE_8BRV");
//					transfer->wLength = 2;
					break;
				default:	//pinport opcode min/max definition error 
					sentinel("bad PP opcode min/max err:%d",ERR_BAD_PP_OP_MINMAX);
			}
			break; //end of PINPORT

		case IO: debug("dict: IO");
//			transfer->wLength = 1;
			switch (opcode) {
				case IO_OPCODE_ONLY_MIN ... IO_OPCODE_ONLY_MAX:
					debug("IO_OPCODE_ONLY");
					break;
				case IO_OPCODE_RTN_MIN ... IO_OPCODE_RTN_MAX:
					debug("IO_OPCODE_RTN");
//					transfer->wLength = 8;
					break;
				default:	//io opcode min/max definition error 
					sentinel("bad IO opcode min/max err:%d",ERR_BAD_IO_OP_MINMAX);
			}
			break; //end of IO

		case NES: debug("dict: NES");
//			transfer->wLength = 1;
			switch (opcode) {
				case NES_OPCODE_24BOP_MIN ... NES_OPCODE_24BOP_MAX:
					debug("NES_OPCODE_24BOP");
					break;
				case NES_OPCODE_16BOP_8BRV_MIN ... NES_OPCODE_16BOP_8BRV_MAX:
					debug("NES_OPCODE_16BOP_8BRV");
//					transfer->wLength = 2;
					break;
				default:	//nes opcode min/max definition error 
					sentinel("bad NES opcode min/max err:%d",ERR_BAD_NES_OP_MINMAX);
			}
			break; //end of NES

		case SNES: debug("dict: SNES");
//			transfer->wLength = 1;
			switch (opcode) {
				case SNES_OPCODE_24BOP_MIN ... SNES_OPCODE_24BOP_MAX:
					debug("SNES_OPCODE_24BOP");
					break;
				case SNES_OPCODE_24BOP_8BRV_MIN ... SNES_OPCODE_24BOP_8BRV_MAX:
					debug("SNES_OPCODE_24BOP_8BRV");
//					transfer->wLength = 2;
					break;
				default:	//snes opcode min/max definition error 
					sentinel("bad SNES opcode min/max err:%d",ERR_BAD_SNES_OP_MINMAX);
			}
			break; //end of SNES

		case BUFFER: debug("dict: BUFFER");
//			transfer->wLength = length;
			if (buffer != NULL) {
				transfer->data = (unsigned char *)buffer;
			}
			break; //end of BUFF

		case USB: debug("dict: USB");
//			transfer->wLength = length;
			break;

		default:
			//request (aka dictionary) is unknown
			sentinel("unknown DICT err:%d",ERR_UNKN_DICTIONARY);
	}


	int xfr_cnt;

	xfr_cnt = usb_transfer( transfer );

	if (print_debug) {
		//print transfer details if small xfr
		if (xfr_cnt <= 8) {
		printf("	xf: %d   er: %d rv:",xfr_cnt, transfer->data[0]);
		int i ;
		for (i=1; i<xfr_cnt; i++){
			printf(" %x,", transfer->data[i]);
		}
		printf("\n"); 
		} else {
		//just print xfr cnt
		printf("						xf: %d\n",xfr_cnt); 

		}
	}

	if (xfr_cnt <= 8) {
		check(transfer->data[0] == SUCCESS, "retro programmer had error: %d, dict:%d, opcode:%d/%x, addr:%x, data:%x",transfer->data[0], dictionary, opcode, opcode, addr, miscdata)
	}

	return SUCCESS;

error:
	printf("dictionary call went to error\n");
	return -1;

}
