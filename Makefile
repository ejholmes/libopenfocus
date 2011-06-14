# Name: Makefile
# Author: Eric J. Holmes

UNAME = $(shell uname)

ifeq ($(UNAME), Linux)
	# Use the following 3 lines on Unix (uncomment the framework on Mac OS X):
	USBFLAGS = `libusb-config --cflags`
	USBLIBS = `libusb-config --libs`
else
	# Use the following 3 lines on Windows and comment out the 3 above. You may
	# have to change the include paths to where you installed libusb-win32
	USBFLAGS = -I/usr/local/include
	USBLIBS = -L/usr/local/lib -lusb
endif

NAME = openfocus

OBJECTS = device.o bootloader.o helper.o intelhex.o cdevice.o

CC		= gcc
CPP     = g++
CFLAGS	= $(CPPFLAGS) $(USBFLAGS) -O -g -Wall -fpic
LIBS	= $(USBLIBS)
OUTPUT  = lib$(NAME).a

ifeq ($(UNAME), Linux)
	LDFLAGS = -Wl,-soname,lib$(NAME).so.1 -o lib$(NAME).so.1.0
else
	LDFLAGS = -Wl,--out-implib,lib$(NAME).lib -o lib$(NAME).dll
endif


all: $(OUTPUT)

.cpp.o:
	$(CPP) $(CFLAGS) -c $< 

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OUTPUT): $(OBJECTS)
	ar rcs $(OUTPUT) $(OBJECTS)
	$(CPP) -shared $(LDFLAGS) $(OBJECTS) $(USBLIBS)

clean:
	rm -f *.o $(OUTPUT) *.exe *.a *.so.* *.dll *.lib
