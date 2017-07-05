#compiler flags: -Wall Warnings all,  -g gdb debug markings, 
CFLAGS=-Wall -O
INCLUDE= -I ./include
WINLIB= -L ./winlib
LIBUSB= -lusb-1.0
CC= gcc

SOURCES=$(wildcard source/**/*.c source/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=inlretro

#default to windows build
all:	shared
	$(CC) $(INCLUDE) $(SOURCES) -o $(TARGET) $(WINLIB) $(CFLAGS) $(LIBUSB) 

#unix build doesn't need winlib directory as libusb should be installed on OS.
#sudo apt-get install libusb-1.0-0-dev
unix:	shared
	$(CC) $(INCLUDE) $(SOURCES) -o $(TARGET) $(CFLAGS) -lm $(LIBUSB)

# "make debug" will build program with debug print messages
# -DDEBUG show debug logs
# -g build with gdb debug tags
debug: CFLAGS += -DDEBUG -g
debug: all

#unix debug build
unixdebug: CFLAGS += -DDEBUG -g
unixdebug: unix


#copy shared .h files which are used in host and firmware
shared:
	cp -r ../shared/* source/

#clean on unix and windows(.exe)
clean:
	rm -f $(TARGET) $(TARGET).exe $(OBJECTS)
	rm -f source/shared_*

