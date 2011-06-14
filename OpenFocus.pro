#-------------------------------------------------
#
# Project created by QtCreator 2011-06-10T04:20:59
#
#-------------------------------------------------

QT       -= gui

TARGET = openfocus
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    helper.cpp \
    device.cpp \
    bootloader.cpp \
    intelhex.cpp \
    cdevice.cpp

HEADERS += openfocus.h \
    libusb/usb.h \
    intelhex.h \
    util.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:LIBS += -Llibusb -lusb
macx:LIBS += -L/usr/local/CrossPack-AVR/lib -lusb -Wl,-framework -Wl,IOKit -Wl,-framework -Wl,CoreFoundation -Wl,-prebind
unix:LIBS += -lusb

win32:INCLUDEPATH += libusb
macx:INCLUDEPATH += /usr/local/CrossPack-AVR/include
unix:INCLUDEPATH += /usr/include
