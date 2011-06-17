# Name: Makefile
# Author: Eric J. Holmes

UNAME = $(shell uname)

ifeq ($(UNAME), Linux)
	USBFLAGS = `libusb-config --cflags`
	USBLIBS = `libusb-config --libs`
endif
ifeq ($(UNAME), Darwin)
	USBFLAGS = `libusb-config --cflags`
	USBLIBS = `libusb-config --libs`
endif
ifeq ($(UNAME), MINGW32_NT-6.1)
	USBFLAGS = -I/usr/local/include
	USBLIBS = -L/usr/local/lib -lusb
endif

NAME = openfocus

OBJECTS = device.o bootloader.o helper.o intelhex.o cdevice.o

CC		= gcc
CPP     = g++
CFLAGS	= $(CPPFLAGS) $(USBFLAGS) -O -g -fpic -Wall
LIBS	= $(USBLIBS)
OUTPUT  = lib$(NAME).a

ifeq ($(UNAME), Linux)
	LDFLAGS = -Wl,-soname,lib$(NAME).so.0.0.0 -o lib$(NAME).so.0.0.0
endif
ifeq ($(UNAME), MINGW32_NT-6.1)
	LDFLAGS = -Wl,--out-implib,lib$(NAME).lib -o lib$(NAME).dll
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS =
endif


all: $(OUTPUT)

ifeq ($(UNAME), Linux)
install: all
	install -m 0644 lib$(NAME).a /usr/lib
	install -m 0755 lib$(NAME).so.0.0.0 /usr/lib
	ln -s /usr/lib/lib$(NAME).so.0.0.0 /usr/lib/lib$(NAME).so
	install -m 0755 openfocus.h /usr/include
endif

.cpp.o:
	$(CPP) $(CFLAGS) -c $< 

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OUTPUT): $(OBJECTS)
	ar rcs $(OUTPUT) $(OBJECTS)
	$(CPP) -shared $(LDFLAGS) $(OBJECTS) $(USBLIBS)

clean:
	rm -f *.o $(OUTPUT) *.exe *.a *.dll *.lib *.so *.so*
