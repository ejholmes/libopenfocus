#ifndef OPENFOCUS_H
#define OPENFOCUS_H

#ifdef __WIN32__
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define LIBOPENFOCUS_VER 0x01, 0x00 /* minor, major */

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
        unsigned short address;
        char data[1];
    };
    char bytes[1];
} block;
#pragma pack()

/* Used to return eeprom data */
typedef struct eeprom {
    unsigned short size;
    char *data;
} eeprom;

typedef struct usb_dev_handle dev_handle;

/* Typedef the classes to void for use with C */
#ifndef __cplusplus
typedef void Device;
typedef void Bootloader;
#endif

#ifdef __cplusplus
namespace OpenFocus
{
    /*
     * This is the main class for interfacing with an OpenFocus device
     */
    class EXPORT Device
    {
    private:
        /* Device Vendor ID */
        static const unsigned short Vendor_ID; /* 0x20a0 */
        /* Device Product ID */
        static const unsigned short Product_ID; /* 0x416b */

        /* Returns a single byte and populates AbsolutePositioning and TemperatureCompensation bools */
        int GetCapabilities(unsigned char *capabilities);

        /* Handle to the usb device */
        dev_handle *device;

        bool TempCompEnabled;
        double LastTemperature;
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

        /* Starts temperature compensation */
        void EnableTemperatureCompensation();
        /* Stops temperature compensation */
        void DisableTemperatureCompensation();
        /* Returns true if temperature compensation is enabled */
        bool TemperatureCompensationEnabled();
        /* Should be called by the client. A good idea is to setup a timer that calls this
         * consistently at a set interval. It will only perform temperature compensation if
         * EnableTemperatureCompensation() has been called. If you call DisableTemperatureCompensation(),
         * this function will have no effect. */
        void DoTempComp();

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

        /* Current temperature coefficient to use during temperature compensation */
        double TemperatureCoefficient;
    };

    /* Functions for communicating with the bootloader. DON'T USE THIS IF YOU DON'T KNOW WHAT YOU'RE DOING */
    class EXPORT Bootloader
    {
    private:
        /* Bootloader Vendor ID */
        static const unsigned short Vendor_ID; /* 0x20a0 */
        /* Bootloader Product ID */
        static const unsigned short Product_ID; /* 0x416d */

        static dev_handle *device;
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
        static unsigned short PageSize;
        /* Flash size of device */
        static unsigned short FlashSize;
        /* EEPROM size */
        static unsigned short EEPROMSize;
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
#endif

/* C Comptability functions */
#ifdef __cplusplus
    extern "C" {
#endif
        int device_connect(Device **device);
        int device_connect_serial(Device **device, const char *serial);
        void device_disconnect(Device *device);
        int device_move_to(Device *device, unsigned short position);
        int device_halt(Device *device);
        void device_reboot_to_bootloader(Device *device);
        int device_set_position(Device *device, unsigned short position);
        int device_get_temperature(Device *device, double *temperature);
        int device_get_position(Device *device, unsigned short *position);
        int device_is_moving(Device *device);
        int device_is_connected(Device *device);
        char *device_get_serial(Device *device);
        version device_get_firmware_version(Device *device);
        int device_can_absolute_position(Device *device);
        int device_can_temperature_compensate(Device *device);
        void device_set_temperature_coefficient(Device *device, double coefficient);
        double device_get_temperature_coefficient(Device *device);
        void device_enable_temperature_compensation(Device *device);
        void device_disable_temperature_compensation(Device *device);
        bool device_temperature_compensation_enabled(Device *device);
        void device_do_temp_comp(Device *device);
#ifdef __cplusplus
    }
}
#endif

#endif // OPENFOCUS_H
