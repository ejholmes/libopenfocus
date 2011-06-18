#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

#include "openfocus.h"
#include "util.h"

#define USB_RQ_REBOOT 0x01
#define USB_RQ_WRITE_FLASH_BLOCK 0x02
#define USB_RQ_GET_REPORT 0x03
#define USB_RQ_WRITE_EEPROM_BLOCK 0x04
#define USB_RQ_READ_EEPROM_BLOCK 0x05

const unsigned short OpenFocus::Bootloader::Vendor_ID = BOOTLOADER_VID;
const unsigned short OpenFocus::Bootloader::Product_ID = BOOTLOADER_PID;

dev_handle *OpenFocus::Bootloader::device = NULL;

OpenFocus::Bootloader::Bootloader()
{
    if (device)
        GetReport();
}

bool OpenFocus::Bootloader::Connect()
{
    if (!usb_open_device(&device, Vendor_ID, Product_ID, NULL))
        return false;

    return true;
}

void OpenFocus::Bootloader::Disconnect()
{
    if (device)
        usb_close(device);
    device = NULL;
}

bool OpenFocus::Bootloader::IsConnected()
{
    return (device);
}

int OpenFocus::Bootloader::Reboot()
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_REBOOT, 0, 0, NULL, 0, 5000);
}

int OpenFocus::Bootloader::GetReport()
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

    return retval;
}

block *OpenFocus::Bootloader::ReadEepromBlock(unsigned short address, int length)
{
    block *data = (block *)malloc(sizeof(char) * length);
    if (usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_READ_EEPROM_BLOCK, address, 0, &data->bytes, length, 5000) < 0)
        return NULL;
    return data;
}

eeprom *OpenFocus::Bootloader::ReadEeprom()
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

int OpenFocus::Bootloader::WriteEepromBlock(unsigned short address, const unsigned char *data, int length)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_EEPROM_BLOCK, address, 0, (char *)data, length, 5000);
}

int OpenFocus::Bootloader::WriteEeprom(const unsigned char *data, int length)
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

int OpenFocus::Bootloader::WriteFlashBlock(unsigned short address, const unsigned char *data, int length)
{
    printf("Writing flash block at address %4x\n", address);
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_FLASH_BLOCK, address, 0, (char *)data, length, 5000);
}

int OpenFocus::Bootloader::WriteFlash(const unsigned char *data, int length)
{
    for (unsigned short address = 0; address < length; address += PageSize) {
        if (WriteFlashBlock(address, data, PageSize) <= 0)
            return 0;
        data += PageSize;
    }
    return 1;
}
