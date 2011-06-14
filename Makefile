# Name: Makefile
# Author: Eric J. Holmes

UNAME = $(shell uname)

USBFLAGS = `libusb-config --cflags`
USBLIBS = `libusb-config --libs`

# USBFLAGS = -I/usr/local/include
# USBLIBS = -L/usr/local/lib -lusb

NAME = openfocus

OBJECTS = device.o bootloader.o helper.o intelhex.o cdevice.o

CC		= gcc
CPP     = g++
CFLAGS	= $(CPPFLAGS) $(USBFLAGS) -O -g -Wall -fpic
LIBS	= $(USBLIBS)
OUTPUT  = lib$(NAME).a

ifeq ($(UNAME), Linux)
	LDFLAGS = -Wl,-soname,lib$(NAME).so -o lib$(NAME).so
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
	rm -f *.o $(OUTPUT) *.exe *.a *.dll *.lib *.so
