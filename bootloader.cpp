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
        unsigned short address; /* Address to put data */
        char data; /* Data to send */
    };
    char bytes; /* This struct as bytes */
} block;


OpenFocus::Bootloader::Bootloader()
{
    device = NULL;
}

bool OpenFocus::Bootloader::Connect()
{
    if (!usb_open_device(&device, Vendor_ID, Product_ID))
        return false;

    // unsigned char report = NULL;
    // GetReport(report);
    // endian_swap((unsigned char *)&PageSize, (unsigned char *)&report[0], 2);
    // endian_swap((unsigned char *)&FlashSize, (unsigned char *)&report[2], 4);
    // endian_swap((unsigned char *)&EEPROMSize, (unsigned char *)&report[6], 2);
    // free(report);

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

int OpenFocus::Bootloader::GetReport()
{
    struct {
        unsigned short pagesize;
        unsigned long flashsize;
        unsigned short eepromsize;
    } report;

    int retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_REPORT, 0, 0, (char *)&report, sizeof(report), 5000);

    if (retval != 0)
        return retval;

    endian_swap((unsigned char *)&PageSize, (unsigned char *)&report.pagesize, sizeof(report.pagesize));
    endian_swap((unsigned char *)&FlashSize, (unsigned char *)&report.flashsize, sizeof(report.flashsize));
    endian_swap((unsigned char *)&EEPROMSize, (unsigned char *)&report.eepromsize, sizeof(report.eepromsize));

    return retval;
}

int OpenFocus::Bootloader::WriteEepromBlock(unsigned short address, const char *data, int length)
{
    block *b = (block *)malloc(length + sizeof(address));
    endian_swap((unsigned char *)&b->address, (unsigned char *)&address, sizeof(b->address));
    memcpy(&b->data, data, length);

    int retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_EEPROM_BLOCK, 0, 0, &b->bytes, length + sizeof(address), 5000);

    free(b);

    return retval;
}

int OpenFocus::Bootloader::WriteEeprom(const char *data, int length)
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

int OpenFocus::Bootloader::WriteFlashBlock(unsigned short address, const char *data, int length)
{
    block *b = (block *)malloc(length + sizeof(address));
    endian_swap((unsigned char *)&b->address, (unsigned char *)&address, sizeof(b->address));
    memcpy(&b->data, data, length);

    int retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_WRITE_FLASH_BLOCK, 0, 0, &b->bytes, length + sizeof(address), 5000);

    free(b);

    return retval;
}

int OpenFocus::Bootloader::WriteFlash(const char *data, int length)
{
    for (unsigned short address = 0; address < length; address += PageSize) {
        if (WriteFlashBlock(address, data, PageSize) != 0)
            return -1;
        data += PageSize;
    }
    return 0;
}
