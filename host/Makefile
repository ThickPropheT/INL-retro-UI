#compiler flags: -Wall Warnings all,  -g gdb debug markings, 
CFLAGS=-Wall -g -O
INCLUDE= -I ./include
WINLIB= -L ./winlib
LIBUSB= -lusb-1.0
CC= gcc

SOURCES=$(wildcard source/**/*.c source/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=inlretro

#default to windows build
all:
	$(CC) $(INCLUDE) $(SOURCES) -o $(TARGET) $(WINLIB) $(CFLAGS) $(LIBUSB) 

#unix build doesn't need winlib directory as libusb should be installed on OS.
#sudo apt-get install libusb-1.0-0-dev
unix:
	$(CC) $(INCLUDE) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBUSB)

# "make debug" will build program with debug print messages
# -DNDEBUG show debug logs
debug: CFLAGS += -DNDEBUG
debug: all

#unix debug build
unixdebug: CFLAGS += -DNDEBUG
unixdebug: unix

#clean on unix and windows(.exe)
clean:
	rm -f $(TARGET) $(TARGET).exe $(OBJECTS)

