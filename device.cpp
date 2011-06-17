#include <iostream>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <usb.h>

#include "openfocus.h"
#include "util.h"

#define USB_RQ_MOVE_TO 0x00
#define USB_RQ_HALT 0x01
#define USB_RQ_SET_POSITION 0x02
#define USB_RQ_REVERSE 0x03
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
    have_error = false;
}

device_info *OpenFocus::Device::EnumerateDevices()
{
    struct usb_bus *bus;
    struct usb_device *dev;
    struct usb_dev_handle *handle = NULL;
    device_info *head = NULL, *last = NULL, *current = NULL;

    usb_init();

    usb_find_busses();
    usb_find_devices();

    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == Vendor_ID &&
                    dev->descriptor.idProduct == Product_ID) {
                handle = usb_open(dev);
                if (handle) {
                    current = (device_info *)malloc(sizeof(device_info *));
                    current->next = NULL;
                    /* Copy serial */
                    usb_get_string_simple(handle, dev->descriptor.iSerialNumber, current->serial, sizeof(current->serial));
                    if (!head)
                        head = current;
                    if (last)
                        last->next = current;

                    last = current;
                }
            }
        }
    }

    return head;
}

bool OpenFocus::Device::Connect(const char *serial)
{
    if (!usb_open_device(&device, Vendor_ID, Product_ID, serial))
        return false;

    /* Get the capabilities from the device */
    unsigned char caps = 0;
    GetCapabilities(&caps);
    CanAbsolutePosition = ((caps & CAP_ABSOLUTE_POSITIONING) == CAP_ABSOLUTE_POSITIONING);
    CanTemperatureCompensate = ((caps & CAP_TEMPERATURE_COMPENSATION) == CAP_TEMPERATURE_COMPENSATION);

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

int OpenFocus::Device::ReverseDirection(bool reverse)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USB_RQ_REVERSE, (int)((reverse)?1:0), 0, NULL, 0, 5000);
}

double OpenFocus::Device::GetTemperature()
{
    unsigned short adc;
    double temperature;

    int retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_TEMPERATURE, 0, 0, (char *)&adc, sizeof(temperature), 5000);

    if (retval != sizeof(temperature))
        have_error = true;

    temperature = (5.00 * (double)adc * 100.00) / 1024.00; /* Convert ADC value to absolute temperature */
    temperature = round(temperature * 100.00) / 100.00; /* Round to two decimal places */

    return temperature;
}

unsigned short OpenFocus::Device::GetPosition()
{
    unsigned short position;
    int retval = usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_POSITION, 0, 0, (char *)&position, sizeof(position), 5000);
    if (retval != sizeof(position))
        have_error = true;

    return position;
}

int OpenFocus::Device::GetCapabilities(unsigned char *capabilities)
{
    return usb_control_msg(device, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_RQ_GET_CAPABILITIES, 0, 0, (char *)capabilities, sizeof(unsigned char), 5000);
}

void OpenFocus::Device::DoTempComp()
{
    if (TempCompEnabled && !IsMoving() && (LastTemperature != 0.0)) {
        double CurrentTemperature = GetTemperature();

        unsigned short position = GetPosition();

        double delta = CurrentTemperature - LastTemperature;
        MoveTo((unsigned short)(position + (TemperatureCoefficient * delta)));

        LastTemperature = CurrentTemperature;
    }
}

void OpenFocus::Device::EnableTemperatureCompensation()
{
    TempCompEnabled = true;
    LastTemperature = GetTemperature();
}

void OpenFocus::Device::DisableTemperatureCompensation()
{
    TempCompEnabled = false;
}

bool OpenFocus::Device::TemperatureCompensationEnabled()
{
    return TempCompEnabled;
}

bool OpenFocus::Device::HaveError()
{
    bool retval = have_error;
    have_error = false;
    return retval;
}
