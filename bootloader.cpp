#include "usb.h"
#include "OpenFocus.h"

#define USB_RQ_REBOOT 0x01
#define USB_RQ_WRITE_FLASH_BLOCK 0x02
#define USB_RQ_GET_REPORT 0x03
#define USB_RQ_WRITE_EEPROM_BLOCK 0x04
#define USB_RQ_READ_EEPROM_BLOCK 0x05

unsigned short OpenFocus::Bootloader::Vendor_ID = 0x20a0;
unsigned short OpenFocus::Bootloader::Product_ID = 0x416d;

struct usb_dev_handle *OpenFocus::Bootloader::device = NULL;

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
    data = (char *)realloc(data, length + sizeof(address));
    memmove((void *)(data+2), data, length);
    memcpy(data, ToUsbInt(address, sizeof(address)), sizeof(address));

    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_EEPROM_BLOCK, 0, 0, data, length + sizeof(address), 5000);
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
    free(data);
    return 0;
}

int OpenFocus::Bootloader::WriteFlashBlock(unsigned short address, char *data, int length)
{
    data = (char *)realloc(data, length + sizeof(address));                 /* Reallocate extra space for address */
    memmove((void *)(data+2), data, length);                                /* Move the data 2 bytes forward */
    memcpy(data, ToUsbInt(address, sizeof(address)), sizeof(address));      /* Copy the address into the first 2 bytes */

    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_FLASH_BLOCK, 0, 0, data, length + sizeof(address), 5000);
}

int OpenFocus::Bootloader::WriteFlash(char *data, int length)
{
    for (unsigned short address = 0; address < length; address += PageSize) {
        if (WriteFlashBlock(address, data, PageSize) != 0)
            return -1;
        data += PageSize;
    }
    free(data);
    return 0;
}

unsigned char* OpenFocus::Bootloader::ToUsbInt(unsigned int value, int length)
{
    unsigned char *bytes = (unsigned char *)malloc(sizeof(unsigned char) * length);

    for (int i = 0; i < length; i++)
    {
        bytes[i] = (unsigned char)(value & 0xff);
        value >>= 8;
    }

    return bytes;
}

unsigned int OpenFocus::Bootloader::GetUsbInt(unsigned char *bytes, int length)
{
    unsigned int value = 0;
    int shift = 0;

    for (int i = 0; i < length; i++)
    {
        value |= (unsigned int)(bytes[i] << shift);
        shift += 8;
    }

    return value;
}


