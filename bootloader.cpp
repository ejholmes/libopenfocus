#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

#include "openfocus.h"
#include "util.h"

#ifdef DEBUG
#define DBG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

enum USB_REQUEST {
    USB_RQ_REBOOT = 0x01,
    USB_RQ_WRITE_FLASH_BLOCK = 0x02,
    USB_RQ_GET_REPORT = 0x03,
    USB_RQ_WRITE_EEPROM_BLOCK = 0x04,
    USB_RQ_READ_EEPROM_BLOCK = 0x05
};

using namespace OpenFocus;

const unsigned short Bootloader::Vendor_ID = BOOTLOADER_VID;
const unsigned short Bootloader::Product_ID = BOOTLOADER_PID;

unsigned short Bootloader::PageSize = 0;
unsigned short Bootloader::FlashSize = 0;
unsigned short Bootloader::EEPROMSize = 0;

dev_handle *Bootloader::device = NULL;

Bootloader::Bootloader()
{
}

bool Bootloader::Connect()
{
    if (!usb_open_device(&device, Vendor_ID, Product_ID, NULL))
        return false;

    if (device)
        GetReport();

    return true;
}

void Bootloader::Disconnect()
{
    if (device)
        usb_close(device);
    device = NULL;
}

bool Bootloader::IsConnected()
{
    return (device);
}

int Bootloader::Reboot()
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_REBOOT, 0, 0, NULL, 0, 5000);
}

int Bootloader::GetReport()
{
#pragma pack(1)
    union {
        struct {
            unsigned short pagesize;
            unsigned long flashsize;
            unsigned short eepromsize;
        };
        char bytes[8];
    } report;
#pragma pack()

    int retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_REPORT, 0, 0, report.bytes, sizeof(report), 5000);

    PageSize = report.pagesize;
    FlashSize = report.flashsize;
    EEPROMSize = report.eepromsize;

    DBG("Device Information:\n");
    DBG("  Page Size: %d\n", PageSize);
    DBG("  Flash Size: %d\n", FlashSize);
    DBG("  EEPROM Size: %d\n", EEPROMSize);

    return retval;
}

block *Bootloader::ReadEepromBlock(unsigned short address, size_t length)
{
    block *data = (block *)malloc(sizeof(char) * length);
    if (usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_READ_EEPROM_BLOCK, address, 0, &data->bytes, length, 5000) < 0)
        return NULL;
    return data;
}

eeprom *Bootloader::ReadEeprom()
{
    unsigned short blocksize = 128;

    eeprom *ep = (eeprom *)malloc(sizeof(eeprom));
    ep->data = (char *)malloc(EEPROMSize); /* Allocate a buffer to store the eeprom data */
    ep->size = EEPROMSize;

    unsigned short address;
    for (address = 0; address < EEPROMSize; address += blocksize) {
        block *b = ReadEepromBlock(address, blocksize + sizeof(address)); /* Read a block of eeprom */
        if (!b)
            return NULL;
        memcpy(&ep->data[b->address], &b->data, blocksize); /* Copy the data to the eeprom buffer */
        free(b);
    }

    return ep;
}

int Bootloader::WriteEepromBlock(unsigned short address, const unsigned char *data, size_t length)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_EEPROM_BLOCK, address, 0, (char *)data, length, 5000);
}

int Bootloader::WriteEeprom(const unsigned char *data, size_t length)
{
    unsigned short blocksize = 2;
    for (unsigned short address = 0; address < length; address += blocksize) {
        if ((address + blocksize) > length)
            blocksize = length - address;
        if (WriteEepromBlock(address, data, blocksize) != 0)
            return 0;
        data += blocksize;
    }
    return 1;
}

int Bootloader::WriteFlashBlock(unsigned short address, const unsigned char *data, size_t length)
{
    DBG("Writing flash block at address %4x\n", address);
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_FLASH_BLOCK, address, 0, (char *)data, length, 5000);
}

int Bootloader::WriteFlash(const unsigned char *data, size_t length)
{
    for (unsigned short address = 0; address < length; address += PageSize) {
        if (WriteFlashBlock(address, data, PageSize) <= 0)
            return 0;
        data += PageSize;
    }
    return 1;
}
