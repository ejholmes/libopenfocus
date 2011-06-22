#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <usb.h>

/* Opens a usb_dev_handle based on the vendor id and product id */
static inline bool usb_open_device(usb_dev_handle **device, int vendorID, int productId, const char *serial)
{
    struct usb_bus *bus;
    struct usb_device *dev;
    struct usb_dev_handle *handle = NULL;

    usb_init();

    usb_find_busses();
    usb_find_devices();

    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == vendorID &&
                    dev->descriptor.idProduct == productId) {
                handle = usb_open(dev);
                if (handle) {
                    if (serial == NULL) {
                        goto havedevice;
                    }
                    else {
                        char devserial[256];
                        int len = 0;
                        if (dev->descriptor.iSerialNumber > 0) {
                            len = usb_get_string_simple(handle, dev->descriptor.iSerialNumber, devserial, sizeof(devserial));
                        }
                        if ((len > 0) && (strcmp(devserial, serial) == 0)) {
                            goto havedevice;
                        }
                    }
                }
            }
        }
    }
    return false;
havedevice:
    usb_set_configuration(handle, 1);
    usb_claim_interface(handle, 0);
#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
    if (usb_detach_kernel_driver_np(handle, 0) < 0)
        fprintf(stderr, "Warning: Could not detach kernel driver: %s\nYou may need to run this as root or add yourself to the usb group\n", usb_strerror());
#endif
    *device = handle;
    return true;
}

/* Copies bytes from source to dest and swapping between big and little endian
 * in the process */
static inline void endian_swap(void *dest, const void *source, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        ((char *)dest)[i] = (((char *)source)[length - i]);
    }
}

#endif // UTIL_H
