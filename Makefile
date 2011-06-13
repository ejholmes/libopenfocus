# Name: Makefile
# Author: Eric J. Holmes

# Configure the following definitions according to your system.
# This Makefile has been tested on Mac OS X, Linux and Windows.

UNAME = $(shell uname)

ifeq ($(UNAME), Darwin)
	# Use the following 3 lines on Unix (uncomment the framework on Mac OS X):
	USBFLAGS = `libusb-config --cflags`
	USBLIBS = `libusb-config --libs`
	LINKER = 
else
	# Use the following 3 lines on Windows and comment out the 3 above. You may
	# have to change the include paths to where you installed libusb-win32
	USBFLAGS = -I/usr/local/include
	USBLIBS = -L/usr/local/lib -lusb
	LINKER = -Wl,--out-implib,lib$(NAME).a
endif

NAME = openfocus

OBJECTS = device.o bootloader.o helper.o intelhex.o cdevice.o

CC		= gcc
CPP     = g++
CFLAGS	= $(CPPFLAGS) $(USBFLAGS) -O -g -Wall
LIBS	= $(USBLIBS)

OUTPUT = lib$(NAME)$(EXTENSION)


all: $(OUTPUT)

.cpp.o:
	$(CPP) $(CFLAGS) -c $<

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OUTPUT): $(OBJECTS)
	ar rcs $(OUTPUT) $(OBJECTS)

clean:
	rm -f *.o $(OUTPUT) *.exe *.a
