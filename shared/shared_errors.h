#ifndef _error_codes_h
#define _error_codes_h

#define SUCCESS 			0

//greater than 128 are possible avr return codes
#define ERR_UNKN_DICTIONARY 		128
#define ERR_BAD_PP_OP_MINMAX 		129
#define ERR_BAD_IO_OP_MINMAX 		130
#define ERR_BAD_NES_OP_MINMAX 		131


#define ERR_UNKN_PP_OPCODE_ONLY 	140
#define ERR_UNKN_PP_OPCODE_8BOP 	141
#define ERR_UNKN_PP_OPCODE_16BOP 	142
#define ERR_UNKN_PP_OPCODE_24BOP 	143
#define ERR_UNKN_PP_OPCODE_8BRV 	144

#define ERR_UNKN_IO_OPCODE_ONLY 	150

#define ERR_UNKN_NES_OPCODE_24BOP 	160
#define ERR_UNKN_NES_OPCODE_16BOP_8BRV 	161

#endif
