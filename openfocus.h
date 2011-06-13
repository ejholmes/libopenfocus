#ifndef OPENFOCUS_H
#define OPENFOCUS_H

#include <iostream>

//#include "usb.h"

#ifdef __WIN32__
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

/* ------------------
 * VID/PID pairs
 * ------------------ */
#define DEVICE_VID 0x20a0
#define DEVICE_PID 0x416b

#define BOOTLOADER_VID 0x20a0
#define BOOTLOADER_PID 0x416d

/* Type used for reporting firmware version */
typedef struct version {
    unsigned char minor; /* Minor version number */
    unsigned char major; /* Major version number */
} version;

/* Type that is used to send a block of data for eeprom or flash writing */
#pragma pack(1)
typedef union block {
    struct {
        unsigned short address; /* Address to put data */
        char data[]; /* Data to send */
    };
    char bytes[]; /* This struct as bytes */
} block;
#pragma pack()

/* Used to return eeprom data */
typedef struct eeprom {
    unsigned short size;
    char *data;
} eeprom;

/* Type used for storing capabilities */
typedef struct capabilities {
    unsigned char absolutepositioning : 1;
    unsigned char temperaturecompensation : 2;
    unsigned char unused : 6;
} capabilities;

namespace OpenFocus
{
    class EXPORT Device
    {
    private:
        /* Device Vendor ID */
        static const unsigned short Vendor_ID; /* 0x20a0 */
        /* Device Product ID */
        static const unsigned short Product_ID; /* 0x416b */

        /* Returns a single byte and populates AbsolutePositioning and TemperatureCompensation bools */
        int GetCapabilities(unsigned char *capabilities);
    public:
        Device();
        /* Connect to the device */
        bool Connect();
        /* Connect to the device with a specific serial number */
        bool Connect(const char *serial);
        /* Disconnect from the device */
        void Disconnect();
        /* Asynchronously move to position */
        int MoveTo(unsigned short position);
        /* Halt the device if it's currently moving */
        int Halt();
        /* Reboot the device into the bootloader. Don't use this if you don't
         * know what you're doing */
        void RebootToBootloader();
        /* Set the current position on the device */
        int SetPosition(unsigned short position);
        /* Gets the current temperature read by the LM335 on the device in degrees kelvin */
        int GetTemperature(double *temperature);
        /* Gets the current position from device */
        int GetPosition(unsigned short *position);

        /* Returns true if the device is currently moving */
        bool IsMoving();
        /* Returns true if the device is connected */
        bool IsConnected();


        /* Device serial number */
        char Serial[256];
        /* Device firmware version */
        version FirmwareVersion;

        /* True if the device supports absolute positioning */
        bool AbsolutePositioning;
        /* True if the device supports temperature compensation */
        bool TemperatureCompensation;
    };

    /* Functions for communicating with the bootloader. DON'T USE THIS IF YOU DON'T KNOW WHAT YOU'RE DOING */
    class EXPORT Bootloader
    {
    private:
        /* Bootloader Vendor ID */
        static const unsigned short Vendor_ID; /* 0x20a0 */
        /* Bootloader Product ID */
        static const unsigned short Product_ID; /* 0x416d */
    public:
        Bootloader();
        /* Connect to the bootloader */
        bool Connect();
        /* Disconnects the bootloader */
        void Disconnect();
        /* Write block of data to address */
        int WriteEepromBlock(unsigned short address, const char *data, int length);
        /* Write data to eeprom */
        int WriteEeprom(const char *data, int length);
        /* Write block of data to address in flash */
        int WriteFlashBlock(unsigned short address, const char *data, int length);
        /* Write data to flash */
        int WriteFlash(const char *data, int length);
        /* Exits bootloader by jumping to (void *)0 */
        int Reboot();

        /* Requests length bytes from eeprom at address and puts it in data, returns an eeprom block */
        block *ReadEepromBlock(unsigned short address, int length);
        /* Requests all the eeprom and stores it in data, returns eeprom data */
        eeprom *ReadEeprom();

        /* Returns true if connected to bootloader */
        bool IsConnected();

        /* Returns report from device, returns 8 bytes */
        int GetReport();

        /* Page size of device */
        unsigned short PageSize;
        /* Flash size of device */
        unsigned short FlashSize;
        /* EEPROM size */
        unsigned short EEPROMSize;
    };

    class Helper
    {
    public:
        /* Helper function that will attempt to connect to the bootloader. If it fails,
          it will try to connect to the device and reboot it. If that also fails, it will
          return 0 */
        static int ConnectBootloader();
        /* Converts kelvin to celsius */
        static double Celsius(double kelvin);
        /* Converts kelvin to fahrenheit */
        static double Fahrenheit(double kelvin);
    };
}

#endif // OPENFOCUS_H
