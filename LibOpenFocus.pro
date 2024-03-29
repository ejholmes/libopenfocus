#-------------------------------------------------
#
# Project created by QtCreator 2011-06-10T04:20:59
#
#-------------------------------------------------

QT       -= gui

TARGET = openfocus
TEMPLATE = lib
CONFIG += staticlib
DEFINES += DEBUG

SOURCES += \
    src/helper.cpp \
    src/device.cpp \
    src/bootloader.cpp \
    src/intelhex.cpp \
    src/cdevice.cpp

HEADERS += src/openfocus.h \
    src/intelhex.h \
    src/util.h
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
