#include "inlprog_device.h"

void *build_path(char *path, char *fallbackPath)
{
	char *result = fallbackPath;
	
	if (path && *path)
		result = path;
	
	strrpl(result, '\\', '/');
	
	return result;
}

bool try_open_lua(char *path, lua_State **result)
{
	lua_State *L = open_lua();
	*result = L;
	
	if (!lua_loadtable(L, path))
	{
		printf("Failed to open lua: %s%c", lua_tostring(L, -1), '\0');
		
		lua_close(L);
		
		return false;
	}
	
	return true;
}

bool getRomHeader(lua_State *L, RomHeader **result)
{
	void *header = NULL;
	
	if (!lua_callfunc(
			L,
			"get_rom_header\0",
			NULL,
			LAMBDA(bool _(lua_State *l, int i, void **r)
			{
				*r = NULL;
				
				if (!lua_istable(l, i))
					return false;
				
				*r = RomHeader_fromLuaTable(l, i);
				
				return true;
			}),
			&header))
		return false;
		
	*result = header;
	
	return true;
}

ConnectionToken *ConnectionToken_create(
	libusb_device_handle *handle,
	libusb_context *context,
	lua_State *L,
	RomHeader *rom_header)
{
	USBtransfer *transfer = calloc(1, sizeof(USBtransfer));
	transfer->handle = handle;
	
	ConnectionToken *s = calloc(1, sizeof(ConnectionToken));
	s->usb_context = context;
	s->usb_transfer = transfer;
	s->L = L;
	s->rom_header = rom_header;
	
	return s;
}

EXPORT bool Device_tryConnect(ConnectionSettings settings, ConnectionToken **connection)
{
	int libusb_log = settings.libusb_log;
	char *scripts_path = settings.scripts_path;
	
	*connection = NULL;
	
	libusb_context *context = NULL;
	libusb_device_handle *handle = open_usb_device(context, libusb_log);
	
	if (handle == NULL)
		return false;
	
	const char *CURRENT_DIRECTORY = "./\0";
	const char *DEFAULT_SCRIPT = "inlretro2.lua\0";
	
	char fallbackPath[strlen(CURRENT_DIRECTORY) + strlen(DEFAULT_SCRIPT) + 1];
	strcpy(fallbackPath, CURRENT_DIRECTORY);
	strcat(fallbackPath, DEFAULT_SCRIPT);
	
	char *scriptPath = build_path(scripts_path, fallbackPath);
	
	lua_State *L = NULL;
	if (!try_open_lua(scriptPath, &L))
	{
		close_usb(context, handle);
		return false;
	}
	
	RomHeader *header = NULL;
	if (!getRomHeader(L, &header))
	{
		close_usb(context, handle);
		lua_close(L);
		return false;
	}
	
	*connection = ConnectionToken_create(handle, context, L, header);
	
	return true;
}

EXPORT void device_dumpRom(ConnectionToken *connection, char *path, ProgressCallback callback)
{
	if (!connection)
		return;
	
	// lua_State *L = connection->L;
	
	// lua_getglobal(L, "dump_rom\0");
	
	// romHeader_toLuaTable(connection->rom_header, L);
	// lua_pushstring(L, path);
	
	// lua_pushcfunction(L, LAMBDA(int _(lua_State *l)
	// {
		// int complete = lua_tointeger(l, 1);
		
		// callback(complete);
		// return 0;
	// }));
	
	// if (lua_pcall(L, 3, 0, 0))
		// lua_printcallfailed(L);
	
	lua_callfunc(
		connection->L,
		"dump_rom\0",
		LAMBDA(int _(lua_State *L)
		{
			romHeader_toLuaTable(connection->rom_header, L);
			lua_pushstring(L, path);
			
			lua_pushlightuserdata(L, callback);
			lua_pushcclosure(L, LAMBDA(int _(lua_State *l)
			{
				ProgressCallback cb = lua_touserdata(l, lua_upvalueindex(1));
				
				int complete = lua_tointeger(l, 1);
				
				cb(complete);
				return 0;
			}), 1);
			
			return 3;
		}),
		NULL,
		NULL);
}

EXPORT void device_dumpSRam(ConnectionToken *connection, char *path, ProgressCallback callback)
{
	if (!connection)
		return;
	
	lua_callfunc(
		connection->L,
		"dump_sram\0",
		LAMBDA(int _(lua_State *L)
		{
			romHeader_toLuaTable(connection->rom_header, L);
			lua_pushstring(L, path);
			
			lua_pushlightuserdata(L, callback);
			lua_pushcclosure(L, LAMBDA(int _(lua_State *l)
			{
				ProgressCallback cb = lua_touserdata(l, lua_upvalueindex(1));
				
				int complete = lua_tointeger(l, 1);
				
				cb(complete);
				return 0;
			}), 1);
			
			return 3;
		}),
		NULL,
		NULL);
}

EXPORT void device_disconnect(ConnectionToken *connection)
{
	if (!connection)
		return;
	
	usb_close_inldevice(connection->usb_context, connection->usb_transfer);
	lua_close(connection->L);
	
	free(connection->rom_header);
	
	free(connection);
}