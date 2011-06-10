#include <iostream>
#include <string.h>

#include "openfocus.h"

#define USB_RQ_MOVE_TO 0x00
#define USB_RQ_HALT 0x01
#define USB_RQ_SET_POSITION 0x02
#define USB_RQ_SET_TEMPERATURE_COMPENSATION 0x03
#define USB_RQ_REBOOT_TO_BOOTLOADER 0x04
#define USB_RQ_GET_POSITION 0x10
#define USB_RQ_IS_MOVING 0x11
#define USB_RQ_GET_CAPABILITIES 0x12
#define USB_RQ_GET_TEMPERATURE 0x13

#define CAP_ABSOLUTE_POSITIONING 0x01
#define CAP_TEMPERATURE_COMPENSATION 0x02

const unsigned short OpenFocus::Device::Vendor_ID = 0x20a0;
const unsigned short OpenFocus::Device::Product_ID = 0x416b;

OpenFocus::Device::Device()
{
    device = NULL;
}

bool OpenFocus::Device::Connect()
{
    if (!usb_open_device(&device, Vendor_ID, Product_ID))
        return false;

    /* Get the capabilities from the device */
    unsigned char caps = 0;
    GetCapabilities(&caps);
    AbsolutePositioning = ((caps & CAP_ABSOLUTE_POSITIONING) == CAP_ABSOLUTE_POSITIONING);
    TemperatureCompensation = ((caps & CAP_TEMPERATURE_COMPENSATION) == CAP_TEMPERATURE_COMPENSATION);

    struct usb_device *dev = usb_device(device);

    /* Get firmware version and serial number */
    memcpy(&FirmwareVersion, &dev->descriptor.bcdDevice, sizeof(FirmwareVersion));
    usb_get_string_simple(device, dev->descriptor.iSerialNumber, Serial, sizeof(Serial));

    return true;
}

void OpenFocus::Device::Disconnect()
{
    if (device != NULL)
        usb_close(device);
    device = NULL;
}

bool OpenFocus::Device::IsConnected()
{
    return (device != NULL);
}

int OpenFocus::Device::RebootToBootloader()
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_REBOOT_TO_BOOTLOADER, 0, 0, NULL, 0, 5000);
}

int OpenFocus::Device::MoveTo(unsigned short position)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_MOVE_TO, (int)position, 0, NULL, 0, 5000);
}

int OpenFocus::Device::SetPosition(unsigned short position)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_MOVE_TO, (int)position, 0, NULL, 0, 5000);
}

int OpenFocus::Device::Halt()
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_HALT, 0, 0, NULL, 0, 5000);
}

bool OpenFocus::Device::IsMoving()
{
    char ismoving;
    usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_IS_MOVING, 0, 0, &ismoving, 1, 5000);
    return (bool)ismoving;
}

int OpenFocus::Device::GetTemperature(unsigned short *temperature)
{
    char temp[2];
    int retval;
    retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_TEMPERATURE, 0, 0, temp, 2, 5000);

    if (retval != 0)
        return retval;

    *temperature = endian_swap(*(unsigned short *)temp);
    return retval;
}

int OpenFocus::Device::GetPosition(unsigned short *position)
{
    char pos[2];
    int retval;
    retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_POSITION, 0, 0, pos, 2, 5000);

    if (retval != 0)
        return retval;

    *position = endian_swap(*(unsigned short *)pos);
    return retval;
}

int OpenFocus::Device::GetCapabilities(unsigned char *capabilities)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_CAPABILITIES, 0, 0, (char *)capabilities, 1, 5000);
}
