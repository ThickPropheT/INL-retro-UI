===================
Linux
===================

HOST APPLICATION:
install libusb:
sudo apt-get install libusb-1.0-0-dev

make:
gcc -I include inlretro.c -o inlretro -lusb-1.0

run:
./inlretro


FIRMWARE:
install avr-gcc and avr-libc:
sudo apt-get install gcc-avr
sudo apt-get install avr-libc


BOOTLOADER:
bootloadHID-master:
https://github.com/ajd4096/bootloadHID
fork of original obdev bootloader has option to remove BL switch with timeout.

bootloadHID.2012-12-08:
https://www.obdev.at/products/vusb/bootloadhid.html
the original believe it has more upto date V-USB drivers.

Both have identical commandline folders so they're identical on the host side.
need to have libusb-dev installed can check by typing "libusb-config" in terminal
will present usage options if installed on your system
if not installed should report so with suggestion for apt-get:
sudo apt-get install libusb-dev
then just run 'make' should build sucessfully

With the bootloader commandline app built, the firmware can be loaded
onto the INL retro-programmer via USB:
-place BL/RUN switch in BL
-hit RESET button or plug into USB for first time
-run 'make program_unix' from firmware dir.

If bootloader commandline app was successfully built and you have permission
to access HIDbootloader should have successful output similar to this:
$ make program_unix
../bootloader/commandline/bootloadHID -r main.hex
Warning: could not set configuration: could not set config 1: Device or resource busy
Page size   = 128 (0x80)
Device size = 16384 (0x4000); 14336 bytes remaining
Uploading 1920 (0x780) bytes starting at 0 (0x0)
0x00700 ... 0x00780

-take BL switch back to RUN
-enjoy new firmware


===================
Windows
===================
HOST APPLICATION:
Install minGW:
	download: http://www.mingw.org/wiki/Getting_Started
	launch: Installation manager default settings
	select: mingw32-base 
		(primary need is gcc)
	select: msys-base 
		(primary need is make, basic unix commands are nice to have)
	optional: msys-openssh 
		(helpful if using gitlab to pull updates)
	minGW utilities can be easily added or removed at any time with minGW installation manager.
	Add C:\MinGW\bin & C:\MinGW\msys\1.0\bin to your PC's enviroment PATH variable
		-control panel search: "edit system environment variables
		-System properties window, Advanced tab, click Environment Variables...
		-System Variables pane: Select and edit "PATH"
		-Add new entries below assuming you used default location for minGW
			C:\MinGW\bin
			C:\MinGW\msys\1.0\bin
	
Now host app can be built from windows command prompt command make when in host directory

FIRMWARE:
Download and Install WinAVR
http://www.webring.org/l/rd?ring=avr;id=59;url=http%3A%2F%2Fwinavr%2Esourceforge%2Enet%2Findex%2Ehtml  
	optional: install programmer's notepad has handy feature to make clean, all, program in tools menu
		this is nifty if you are scared of the command prompt for some strange reason...
	installation process should modify PATH environment variables for you.
	incase they don't add them just like MinGW above
		C:\WinAVR-20100110\bin
		C:\WinAVR-20100110\utils\bin

Now firmware can be built from windows command prompt with command "make" when in firmware directory

There is a bootloader installed on all "kazzo" INL retro programmer-dumper's which allows new firmware
builds to be easily flashed on to the device without a avr programmer.
Place BL/RUN switch in BL, then hit reset button in INL retro-prog
from firmware folder run command "make program" this will flash target build onto device using bootloadHID.exe
Take BL/RUN switch back to RUN and device will reset into INL retro-prog you just built.

BOOTLOADER:
If you wish to build bootloader for kazzo (shouldn't be necessary for most ppl) follow the following
Requires you to have an avr programmer to reflash atmega164a mcu
Helpful to download more recent version of avrdude than included with WinAVR.
Download latest version with mingw32.zip from following link:
http://download.savannah.gnu.org/releases/avrdude/
unzip and copy paste both avrdude.exe and avrdude.conf to C:\WinAVR-20100110\bin directory
Assuming the recent build still doesn't support atmega164a..
You'll also have to add a definition for atmega164a in the avrdude.conf file
Copy paste the atmega324P section and rename it atmega164A
Then change the following lines to match:
    id               = "m164a";
    desc             = "ATmega164A";
    signature        = 0x1e 0x94 0x0f;

Now the bootloader can be built and flashed from the bootloadHID/firmware directory with make
If trying to flash mcu from make file you'll have to modify AVRDUDE line to match your avr programmer
Once completed you can make the bootloader and flash it with command "make flash"


