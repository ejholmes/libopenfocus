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
    bootloader.cpp

HEADERS += openfocus.h \
    libusb/usb.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:LIBS += -Llibusb -lusb

INCLUDEPATH += libusb
