/*
 * This file exports wrapper functions for use with C
 *
 * If compiling with gcc, you must add -lstdc++ flag
 */
#include <stdio.h>
#include <stdlib.h>

#include "openfocus.h"

using namespace OpenFocus;

extern "C" {
    Device *device_connect()
    {
        return device_connect_serial(NULL);
    }

    Device *device_connect_serial(const char *serial)
    {
        Device *dev = new Device();

        if (dev->Connect(serial))
            return (Device *)dev;
        else
            return NULL;
    }

    void device_disconnect(Device *device) { device->Disconnect(); }
    int device_move_to(Device *device, unsigned short position) { return device->MoveTo(position); }
    int device_halt(Device *device) { return device->Halt(); }
    void device_reboot_to_bootloader(Device *device) { device->RebootToBootloader(); }
    int device_set_position(Device *device, unsigned short position) { return device->SetPosition(position); }
    int device_get_temperature(Device *device, double *temperature) { return device->GetTemperature(temperature); }
    int device_get_position(Device *device, unsigned short *position) { return device->GetPosition(position); }
    int device_is_moving(Device *device) { return device->IsMoving(); }
    int device_is_connected(Device *device) { return device->IsConnected(); }
    char *device_get_serial(Device *device) { return device->Serial; }
    version device_get_firmware_version(Device *device) { return device->FirmwareVersion; }
    int device_can_absolute_position(Device *device) { return device->AbsolutePositioning; }
    int device_can_temperature_compensate(Device *device) { return device->TemperatureCompensation; }
}
