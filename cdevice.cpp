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
    CDevice *device_create() { return (CDevice *)new Device(); }
    int device_connect(CDevice *device) { return device->Connect(); }
    int device_connect_serial(CDevice *device, const char *serial) { return device->Connect(serial); }
    int device_move_to(CDevice *device, unsigned short position) { return device->MoveTo(position); }
    int device_halt(CDevice *device) { return device->Halt(); }
    void device_reboot_to_bootloader(CDevice *device) { device->RebootToBootloader(); }
    int device_set_position(CDevice *device, unsigned short position) { return device->SetPosition(position); }
    int device_get_temperature(CDevice *device, double *temperature) { return device->GetTemperature(temperature); }
    int device_get_position(CDevice *device, unsigned short *position) { return device->GetPosition(position); }
    int device_is_moving(CDevice *device) { return device->IsMoving(); }
    int device_is_connected(CDevice *device) { return device->IsConnected(); }
    char *device_get_serial(CDevice *device) { return device->Serial; }
    version device_get_firmware_version(CDevice *device) { return device->FirmwareVersion; }
    int device_can_absolute_position(CDevice *device) { return device->AbsolutePositioning; }
    int device_can_temperature_compensate(CDevice *device) { return device->TemperatureCompensation; }
}
