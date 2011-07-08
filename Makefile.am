lib_LTLIBRARIES = libopenfocus.la
libopenfocus_la_CPPFLAGS = $(LIBUSB_CFLAGS)
libopenfocus_la_LIBADD = $(LIBUSB_LIBS)
libopenfocus_la_SOURCES = device.cpp bootloader.cpp cdevice.cpp intelhex.cpp helper.cpp
libopenfocus_la_LDFLAGS = -no-undefined -version-info 0:0:0
include_HEADERS = openfocus.h intelhex.h util.h
