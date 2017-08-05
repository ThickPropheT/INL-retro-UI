#ifndef _shared_errors_h
#define _shared_errors_h

#define SUCCESS 			0
#define GEN_FAIL 			0xFF


//greater than 128 are possible avr return codes
#define ERR_UNKN_DICTIONARY 		128


#define ERR_UNKN_PP_OPCODE		140
#define ERR_CTL_PIN_NOT_PRESENT		141

#define ERR_UNKN_IO_OPCODE		150

//#define ERR_UNKN_NES_OPCODE_24BOP 	160
//#define ERR_UNKN_NES_OPCODE_16BOP_8BRV 	161
//
////reserved libusb erro			165
//
//#define ERR_UNKN_SNES_OPCODE_24BOP 	170
//#define ERR_UNKN_SNES_OPCODE_24BOP_8BRV	171
//
//#define ERR_UNKN_BUFF_OPCODE_NRV	180
//#define ERR_UNKN_BUFF_OPCODE_RV		181
//#define ERR_UNKN_BUFF_OPCODE_BUFN_NRV	182
//
//#define ERR_BUFF_ALLOC_RANGE		190
//#define ERR_BUFF_STATUS_ALREADY_ALLOC	191
//#define ERR_BUFF_ID_ALREADY_ALLOC	192
//#define ERR_BUFF_RAW_ALREADY_ALLOC	193
//#define ERR_BUFF_ALLOC_SIZE_ZERO	194
//#define ERR_BUFF_UNSUP_MEM_TYPE		195
//
//#define ERR_OUT_CURLDBUF_STATUS		200
//#define ERR_OUT_CURLDBUF_TO_SMALL	201
//
//#define ERR_UNKN_OPER_OPCODE_NRV	210
//#define ERR_UNKN_OPER_OPCODE_RV		211
//
////max error number			255


#endif
