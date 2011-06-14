#include <iostream>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include "openfocus.h"
#include "usb.h"
#include "util.h"

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

const unsigned short OpenFocus::Device::Vendor_ID = DEVICE_VID;
const unsigned short OpenFocus::Device::Product_ID = DEVICE_PID;

OpenFocus::Device::Device()
{
    device = NULL;
    TemperatureCoefficient = 0.0;
    TempCompEnabled = false;
    LastTemperature = 0.0;
}

bool OpenFocus::Device::Connect(const char *serial)
{
    if (!usb_open_device(&device, Vendor_ID, Product_ID, serial))
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

bool OpenFocus::Device::Connect()
{
    return Connect(NULL);
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

void OpenFocus::Device::RebootToBootloader()
{
    usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_REBOOT_TO_BOOTLOADER, 0, 0, NULL, 0, 5000);
}

int OpenFocus::Device::MoveTo(unsigned short position)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_MOVE_TO, (int)position, 0, NULL, 0, 5000);
}

int OpenFocus::Device::SetPosition(unsigned short position)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_SET_POSITION, (int)position, 0, NULL, 0, 5000);
}

int OpenFocus::Device::Halt()
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_HALT, 0, 0, NULL, 0, 5000);
}

bool OpenFocus::Device::IsMoving()
{
    char ismoving;
    usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_IS_MOVING, 0, 0, &ismoving, sizeof(bool), 5000);
    return (bool)ismoving;
}

int OpenFocus::Device::GetTemperature(double *temperature)
{
    int retval;
    unsigned short adc;
    retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_TEMPERATURE, 0, 0, (char *)&adc, sizeof(unsigned short), 5000);

    if (retval < 0)
        return retval;

    *temperature = (5.00 * (double)adc * 100.00) / 1024.00; /* Convert ADC value to absolute temperature */
    *temperature = round(*temperature * 100.00) / 100.00; /* Round to two decimal places */

    return retval;
}

int OpenFocus::Device::GetPosition(unsigned short *position)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_POSITION, 0, 0, (char *)position, sizeof(unsigned short), 5000);
}

int OpenFocus::Device::GetCapabilities(unsigned char *capabilities)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_CAPABILITIES, 0, 0, (char *)capabilities, sizeof(unsigned char), 5000);
}

void OpenFocus::Device::DoTempComp()
{
    if (TempCompEnabled && !IsMoving() && (LastTemperature != 0.0)) {
        double CurrentTemperature;
        GetTemperature(&CurrentTemperature);

        unsigned short position;
        GetPosition(&position);

        double delta = CurrentTemperature - LastTemperature;
        MoveTo((unsigned short)(position + (TemperatureCoefficient * delta)));

        LastTemperature = CurrentTemperature;
    }
}

void OpenFocus::Device::EnableTemperatureCompensation()
{
    TempCompEnabled = true;
    GetTemperature(&LastTemperature);
}

void OpenFocus::Device::DisableTemperatureCompensation()
{
    TempCompEnabled = false;
}

bool OpenFocus::Device::TemperatureCompensationEnabled()
{
    return TempCompEnabled;
}
