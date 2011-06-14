# Name: Makefile
# Author: Eric J. Holmes

NAME = openfocus

OBJECTS = device.o bootloader.o helper.o intelhex.o cdevice.o

CC		= gcc
CPP     = g++
CFLAGS	= $(CPPFLAGS) -O -g -Wall -fpic
LIBS	= 

OUTPUT = lib$(NAME).a


all: $(OUTPUT)

.cpp.o:
	$(CPP) $(CFLAGS) -c $< 

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OUTPUT): $(OBJECTS)
	ar rcs $(OUTPUT) $(OBJECTS)
	gcc -shared -Wl,-soname,lib$(NAME).so.1 -o lib$(NAME).so.1.0 $(OBJECTS)

clean:
	rm -f *.o $(OUTPUT) *.exe *.a *.so.*
