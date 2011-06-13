# Name: Makefile
# Author: Eric J. Holmes

NAME = openfocus

OBJECTS = device.o bootloader.o helper.o intelhex.o cdevice.o

CC		= gcc
CPP     = g++
CFLAGS	= $(CPPFLAGS) -O -g -Wall
LIBS	= 

OUTPUT = lib$(NAME).a


all: $(OUTPUT)

.cpp.o:
	$(CPP) $(CFLAGS) -c $< 

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OUTPUT): $(OBJECTS)
	ar rcs $(OUTPUT) $(OBJECTS)

clean:
	rm -f *.o $(OUTPUT) *.exe *.a
