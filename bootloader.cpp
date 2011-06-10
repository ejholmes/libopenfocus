#include <stdlib.h>
#include <string.h>

#include "usb.h"
#include "openfocus.h"

#define USB_RQ_REBOOT 0x01
#define USB_RQ_WRITE_FLASH_BLOCK 0x02
#define USB_RQ_GET_REPORT 0x03
#define USB_RQ_WRITE_EEPROM_BLOCK 0x04
#define USB_RQ_READ_EEPROM_BLOCK 0x05

const unsigned short OpenFocus::Bootloader::Vendor_ID = 0x20a0;
const unsigned short OpenFocus::Bootloader::Product_ID = 0x416d;

struct usb_dev_handle *OpenFocus::Bootloader::device = NULL;

/* Type that is used to send a block of data for eeprom or flash writing */
typedef union block {
    struct {
        unsigned short address;
        char data;
    };
    char bytes;
} block;

OpenFocus::Bootloader::Bootloader()
{
    device = NULL;
}

bool OpenFocus::Bootloader::Connect()
{
    if (!usb_open_device(&device, Vendor_ID, Product_ID))
        return false;

    unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * 8);
    GetReport(data);

    if (data == NULL)
        return false;

    PageSize = (unsigned short)((data[1] << 8) | data[0]);
    FlashSize = (unsigned short)((data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2]);
    EEPROMSize = (unsigned short)((data[7] << 8) | data[6]);

    free(data);

    return true;
}

void OpenFocus::Bootloader::Disconnect()
{
    if (device)
        usb_close(device);
    device = NULL;
}

int OpenFocus::Bootloader::Reboot()
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_REBOOT, 0, 0, NULL, 0, 5000);
}

int OpenFocus::Bootloader::GetReport(unsigned char *data)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_REPORT, 0, 0, (char *)data, 8, 5000);
}

int OpenFocus::Bootloader::WriteEepromBlock(unsigned short address, char *data, int length)
{
    block *b = (block *)malloc(length + sizeof(address));
    b->address = endian_swap(address);
    memcpy(&b->data, data, length);

    int retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_EEPROM_BLOCK, 0, 0, &b->bytes, length + sizeof(address), 5000);

    free(b);

    return retval;
}

int OpenFocus::Bootloader::WriteEeprom(char *data, int length)
{
    unsigned short blocksize = 2;
    for (unsigned short address = 0; address < length; address += blocksize) {
        if ((address + blocksize) > length)
            blocksize = length - address;
        if (WriteEepromBlock(address, data, blocksize) != 0)
            return -1;
        data += blocksize;
    }
    return 0;
}

int OpenFocus::Bootloader::WriteFlashBlock(unsigned short address, char *data, int length)
{
    block *b = (block *)malloc(length + sizeof(address));
    b->address = endian_swap(address);
    memcpy(&b->data, data, length);

    int retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_FLASH_BLOCK, 0, 0, &b->bytes, length + sizeof(address), 5000);

    free(b);

    return retval;
}

int OpenFocus::Bootloader::WriteFlash(char *data, int length)
{
    for (unsigned short address = 0; address < length; address += PageSize) {
        if (WriteFlashBlock(address, data, PageSize) != 0)
            return -1;
        data += PageSize;
    }
    return 0;
}
