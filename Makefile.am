lib_LTLIBRARIES = libopenfocus.la
libopenfocus_la_CPPFLAGS = $(LIBUSB_CFLAGS)
libopenfocus_la_LIBADD = $(LIBUSB_LIBS)
libopenfocus_la_SOURCES = device.cpp bootloader.cpp cdevice.cpp intelhex.cpp helper.cpp
libopenfocus_la_LDFLAGS = -no-undefined -version-info 0:0:0
noinst_HEADERS = intelhex.h openfocus_priv.h
include_HEADERS = openfocus.h

noinst_PROGRAMS = openfocus bootloader

openfocus_LDADD = libopenfocus.la
openfocus_SOURCES = commandline/openfocus.cpp

bootloader_LDADD = libopenfocus.la
bootloader_SOURCES = commandline/bootloader.cpp
