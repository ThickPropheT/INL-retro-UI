===================
Linux
===================
HOST APPLICATION:
install libusb:
sudo apt-get install libusb-1.0-0-dev

make:
gcc -I include usbtest.c -o usbtest -lusb-1.0

run:
./usbtest

FIRMWARE:
install avr-gcc and avr-libc:
sudo apt-get install gcc-avr
sudo apt-get install avr-libc

setting up permissions so don't need to run with sudo
cd to /etc/udev/rules.d and run ls:
paul@eeepc:/etc/udev/rules.d$ ls
70-persistent-net.rules  README
here you can see I only have one default number 70 rule.
So we need to create a rule number greater than 70 to override that rule not giving us the permission we want.
copy the file:
sudo cp <exsistingrule##.rules> <## + 1>-libusb-permission.rules
here's what I did, you rule number and name may differ:
paul@eeepc:/etc/udev/rules.d$ sudo cp 70-persistent-net.rules 71-libusb-permission.rules
paul@eeepc:/etc/udev/rules.d$ ls
70-persistent-net.rules  71-libusb-permission.rules  README

udevadm monitor
plug in device:
KERNEL[867.744361] add      /devices/pci0000:00/0000:00:1d.1/usb3/3-1 (usb)
KERNEL[867.748427] add      /devices/pci0000:00/0000:00:1d.1/usb3/3-1/3-1:1.0 (usb)
UDEV  [867.764925] add      /devices/pci0000:00/0000:00:1d.1/usb3/3-1 (usb)
UDEV  [867.770804] add      /devices/pci0000:00/0000:00:1d.1/usb3/3-1/3-1:1.0 (usb)

lsusb
Bus 001 Device 003: ID 05e3:0505 Genesys Logic, Inc. 
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 005 Device 002: ID 0b05:b700 ASUSTek Computer, Inc. Broadcom Bluetooth 2.1
Bus 005 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub
Bus 004 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub
Bus 003 Device 010: ID 16c0:05dc Van Ooijen Technische Informatica shared ID for use with libusb
Bus 003 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub
Bus 002 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub

ls -ltr /dev/bus/usb/003/
total 0
crw-rw-r-- 1 root root 189, 256 Nov 12 00:37 001
crw-rw-r-- 1 root root 189, 265 Nov 12 01:24 010

sudo chown paul /dev/bus/usb/003/010

ls -ltr /dev/bus/usb/003/010 
crw-rw-r-- 1 paul root 189, 265 Nov 12 01:24 /dev/bus/usb/003/010

https://ubuntuforums.org/showthread.php?t=168221



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
	paul@eeepc:~/Dropbox/retro_pgmr/INL-retro-progdump/bootloader/commandline$ libusb-config
	The program 'libusb-config' is currently not installed. You can install it by typing:
	sudo apt-get install libusb-dev
then just run 'make' will hopefully build sucessfully


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


