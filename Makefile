# Name: Makefile
# Project: custom-class example
# Author: Christian Starkjohann
# Creation Date: 2008-04-06
# Tabsize: 4
# Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
# This Revision: $Id: Makefile 692 2008-11-07 15:07:40Z cs $


# Concigure the following definitions according to your system.
# This Makefile has been tested on Mac OS X, Linux and Windows.

# Use the following 3 lines on Unix (uncomment the framework on Mac OS X):
# USBFLAGS = `libusb-config --cflags`
# USBLIBS = `libusb-config --libs`
# EXTENSION = 

# Use the following 3 lines on Windows and comment out the 3 above. You may
# have to change the include paths to where you installed libusb-win32
USBFLAGS = -I/usr/local/include
USBLIBS = -L/usr/local/lib -lusb
EXTENSION = .dll

NAME = openfocus

OBJECTS = device.o bootloader.o helper.o

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
	$(CPP) -shared -o $(OUTPUT) $(OBJECTS) $(LIBS) -Wl,--out-implib,lib$(NAME).a

clean:
	rm -f *.o $(OUTPUT) *.exe *.a
