===================================
RUNNING THE HOST APPLICATION:
===================================

On windows install device drivers by running InstallDriver.exe from WindowsDriverPackage folder.

An easy way to open the commandline in windows 10 is to open the host folder in explorer.
Then click on the address bar at the top, the address should highlight, simply type the
3 letters "cmd" in the address bar and hit enter.

From the host folder, run the main application from the commandline by typing:
inlretro.exe -s scripts\inlretro.lua

This will run the main application and after the device is detected and USB comms established,
control is passed to the main lua script.

See the USER notes at the top of scripts\inlretro.lua file for how to select different mappers,
cartridges, etc.  You'll need to modify inlretro.lua with your favorite text editor or even
something as simple as notepad included with windows.  If you don't have a favorite text editor,
I recommend something like notepad++ which color codes lua files/code for easier reading.
https://notepad-plus-plus.org/download

If the device errors out, or is not responding you may have to press the RESET button near the
USB connector on the INLretro programmer/dumper to reset the device, and try again.


===================================
UPDATING DEVICE FIRMWARE
===================================
This is done via USB using STmicro DfuSe:
https://www.st.com/en/development-tools/stsw-stm32080.html

Detailed instructions to come, will update this section with the next firmware update.



===================================
SOFTWARE & FIRMWARE BUILDING
===================================
Some build instructions follow, but they shouldn't be needed if you're running the
released firmware on Windows.

This section needs some updating....  Don't be afraid to contact me if you'd like to build
your own firmware.  Or are having issues building the host application on linux/mac


===================
Linux
===================

HOST APPLICATION:
install libusb:
sudo apt-get install libusb-1.0-0-dev

make:
gcc -I include inlretro.c -o inlretro -lusb-1.0
or just run make unix from host folder.
As noted in windows instructions below, lua must be built first
-cd host/source/lua, make o a, then make from host/source

run:
./inlretro


AVR FIRMWARE:
install avr-gcc and avr-libc:
sudo apt-get install gcc-avr
sudo apt-get install avr-libc


AVR BOOTLOADER:
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

STM32 FIRMWARE:
Need arm-none-eabi-gcc


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
		lua for host app dev
	minGW utilities can be easily added or removed at any time with minGW installation manager.
	Add C:\MinGW\bin & C:\MinGW\msys\1.0\bin to your PC's enviroment PATH variable
		-control panel search: "edit system environment variables
		-System properties window, Advanced tab, click Environment Variables...
		-System Variables pane: Select and edit "PATH"
		-Add new entries below assuming you used default location for minGW
			C:\MinGW\bin
			C:\MinGW\msys\1.0\bin
		-I had troubles once with cp (copy) commands in Makefile
causing a crash, even though the commands works outside of make.  Bumping the
mingw path variables to the top of all my path variables corrected this issue.
So that might help if you have similar issues...
	
Now host app can be built from windows command prompt command make when in host directory

Currently setup to compile lua separate from host app.  Need to get better at writing makefiles..
But whatever it works and saves ~12sec of compile time with current setup.
-go to host/source/lua
-make o a
-go back to host
-make
This way lua is compiled separately using it's provided make file.
make clean still deletes lua object files, so this process must be reperformed if cleaned.

AVR FIRMWARE:
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

AVR BOOTLOADER:
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


STM32 FIRMWARE:

Need arm-none-eabi-gcc:
	https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads

Download .zip file and place in folder like C:\ARM and create environment variables to point to bin folder

