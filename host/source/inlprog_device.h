#include <stdbool.h>

#include "inlprog_core.h"
#include "rom_header.h"
#include "str_extensions.h"

#include "lambda.h"

#define EXPORT __declspec(dllexport)

typedef struct
{
	int libusb_log;
	char *scripts_path;
	
} ConnectionSettings;

typedef struct
{
	libusb_context *usb_context;
	USBtransfer *usb_transfer;
	lua_State *L;
	
	RomHeader *rom_header;
	
} ConnectionToken;

typedef void (*ProgressCallback)(int);

EXPORT bool Device_tryConnect(ConnectionSettings settings, ConnectionToken **connection);

EXPORT void device_dumpRom(ConnectionToken *connection, char *path, ProgressCallback callback);
EXPORT void device_dumpSRam(ConnectionToken *connection, char *path, ProgressCallback callback);

EXPORT void doCallback(ConnectionToken *connection, ProgressCallback callback);

EXPORT void device_disconnect(ConnectionToken *connection);