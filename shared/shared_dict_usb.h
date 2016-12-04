#ifndef _shared_dict_usb_h
#define _shared_dict_usb_h

//define dictionary's reference number in the shared_dictionaries.h file
//then include this dictionary file in shared_dictionaries.h
//The dictionary number is literally used as usb transfer request field
//the opcodes and operands in this dictionary are fed directly into usb setup packet's wValue wIndex fields

#define RETURN_BUFF_SIZE	8	//number of bytes in generic return buffer
#define RV_ERR_IDX		0	//(first) index of buffer that contains SUCCESS/ERROR#
#define RV_DATA0_IDX		RV_ERR_IDX + 1	//first index of return data
#define RV_DATA_MAX_IDX		RETURN_BUFF_SIZE - 1	//last index available for return data

//=============================================================================================
//=============================================================================================
// USB DICTIONARY
//
// opcodes contained in this dictionary must be implemented in firmware/source/io.c
//
//=============================================================================================
//=============================================================================================

#endif
