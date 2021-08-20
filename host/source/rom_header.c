#include "rom_header.h"

RomHeader *RomHeader_fromLuaTable(lua_State *L, int i)
{
	RomHeader *header = calloc(1, sizeof(RomHeader));
	
	lua_gettablefield(L, i, "rom_name\0", LAMBDA(void _() { header->rom_name = strmarshc(lua_tostring(L, i)); }));
	
	lua_gettablefield(L, i, "map_mode\0", LAMBDA(void _() { header->map_mode = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "mapping\0", LAMBDA(void _() { header->mapping = strmarshc(lua_tostring(L, i)); }));
	
	lua_gettablefield(L, i, "rom_type\0", LAMBDA(void _() { header->rom_type = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "rom_type_name\0", LAMBDA(void _() { header->rom_type_name = strmarshc(lua_tostring(L, i)); }));
	
	lua_gettablefield(L, i, "rom_address_base\0", LAMBDA(void _() { header->rom_address_base = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "rom_start_bank\0", LAMBDA(void _() { header->rom_start_bank = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "rom_bank_size\0", LAMBDA(void _() { header->rom_bank_size = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "rom_size\0", LAMBDA(void _() { header->rom_size = lua_tointeger(L, i); }));
	
	lua_gettablefield(L, i, "sram_address_base\0", LAMBDA(void _() { header->sram_address_base = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "sram_start_bank\0", LAMBDA(void _() { header->sram_start_bank = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "sram_bank_size\0", LAMBDA(void _() { header->sram_bank_size = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "sram_size\0", LAMBDA(void _() { header->sram_size = lua_tointeger(L, i); }));
	
	lua_gettablefield(L, i, "destination_code\0", LAMBDA(void _() { header->destination_code = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "destination_name\0", LAMBDA(void _() { header->destination_name = strmarshc(lua_tostring(L, i)); }));
	
	lua_gettablefield(L, i, "developer_code\0", LAMBDA(void _() { header->developer_code = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "developer_name\0", LAMBDA(void _() { header->developer_name = strmarshc(lua_tostring(L, i)); }));
	
	lua_gettablefield(L, i, "version\0", LAMBDA(void _() { header->version = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "compliment_check\0", LAMBDA(void _() { header->compliment_check = lua_tointeger(L, i); }));
	lua_gettablefield(L, i, "checksum\0", LAMBDA(void _() { header->checksum = lua_tointeger(L, i); }));
	
	return header;
}

void romHeader_toLuaTable(RomHeader *header, lua_State *L)
{
	lua_newtable(L);
	
	lua_pushtablefield(L, "rom_name\0", LAMBDA(void _() { lua_pushstring(L, header->rom_name); }));
	
	lua_pushtablefield(L, "map_mode\0", LAMBDA(void _() { lua_pushinteger(L, header->map_mode); }));
	lua_pushtablefield(L, "mapping\0", LAMBDA(void _() { lua_pushstring(L, header->mapping); }));
	
	lua_pushtablefield(L, "rom_type\0", LAMBDA(void _() { lua_pushinteger(L, header->rom_type); }));
	lua_pushtablefield(L, "rom_type_name\0", LAMBDA(void _() { lua_pushstring(L, header->rom_type_name); }));
	
	lua_pushtablefield(L, "rom_address_base\0", LAMBDA(void _() { lua_pushinteger(L, header->rom_address_base); }));
	lua_pushtablefield(L, "rom_start_bank\0", LAMBDA(void _() { lua_pushinteger(L, header->rom_start_bank); }));
	lua_pushtablefield(L, "rom_bank_size\0", LAMBDA(void _() { lua_pushinteger(L, header->rom_bank_size); }));
	lua_pushtablefield(L, "rom_size\0", LAMBDA(void _() { lua_pushinteger(L, header->rom_size); }));
	
	lua_pushtablefield(L, "sram_address_base\0", LAMBDA(void _() { lua_pushinteger(L, header->sram_address_base); }));
	lua_pushtablefield(L, "sram_start_bank\0", LAMBDA(void _() { lua_pushinteger(L, header->sram_start_bank); }));
	lua_pushtablefield(L, "sram_bank_size\0", LAMBDA(void _() { lua_pushinteger(L, header->sram_bank_size); }));
	lua_pushtablefield(L, "sram_size\0", LAMBDA(void _() { lua_pushinteger(L, header->sram_size); }));
	
	lua_pushtablefield(L, "destination_code\0", LAMBDA(void _() { lua_pushinteger(L, header->destination_code); }));
	lua_pushtablefield(L, "destination_name\0", LAMBDA(void _() { lua_pushstring(L, header->destination_name); }));
	
	lua_pushtablefield(L, "developer_code\0", LAMBDA(void _() { lua_pushinteger(L, header->developer_code); }));
	lua_pushtablefield(L, "developer_name\0", LAMBDA(void _() { lua_pushstring(L, header->developer_name); }));
	
	lua_pushtablefield(L, "version\0", LAMBDA(void _() { lua_pushinteger(L, header->version); }));
	lua_pushtablefield(L, "compliment_check\0", LAMBDA(void _() { lua_pushinteger(L, header->compliment_check); }));
	lua_pushtablefield(L, "checksum\0", LAMBDA(void _() { lua_pushinteger(L, header->checksum); }));
}