#include <stdlib.h>

#include "lambda.h"

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_extensions.h"

#include "str_extensions.h"

typedef struct
{
	char *rom_name;
	
	int map_mode;
	char *mapping;
	
	int rom_type;
	char* rom_type_name;
	
	int rom_address_base;
	int rom_start_bank;
	int rom_bank_size;
	int rom_size;
	
	int sram_address_base;
	int sram_start_bank;
	int sram_bank_size;
	int sram_size;
	
	int destination_code;
	char *destination_name;
	
	int developer_code;
	char *developer_name;
	
	int version;
	int compliment_check;
	int checksum;
	
} RomHeader;

RomHeader *RomHeader_fromLuaTable(lua_State *L, int i);
void romHeader_toLuaTable(RomHeader *header, lua_State *L);