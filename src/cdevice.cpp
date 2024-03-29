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

    int device_connect_serial(Device **device, const char *serial)
    {
        Device *dev = new Device();

        if (!dev->Connect(serial))
            return 0;

        *device = dev;

        return 1;
    }
    int device_connect(Device **device) { return device_connect_serial(device, NULL); }
    void device_disconnect(Device *device) { device->Disconnect(); delete device; }
    int device_move_to(Device *device, unsigned short position) { return device->MoveTo(position); }
    int device_halt(Device *device) { return device->Halt(); }
    void device_reboot_to_bootloader(Device *device) { device->RebootToBootloader(); }
    int device_set_position(Device *device, unsigned short position) { return device->SetPosition(position); }
    double device_get_temperature(Device *device) { return device->GetTemperature(); }
    unsigned short device_get_position(Device *device) { return device->GetPosition(); }
    int device_reverse_direction(Device *device, int reverse) { return device->ReverseDirection(reverse); }
    int device_is_moving(Device *device) { return device->IsMoving(); }
    int device_is_connected(Device *device) { return device->IsConnected(); }
    char *device_get_serial(Device *device) { return device->Serial; }
    version device_get_firmware_version(Device *device) { return device->FirmwareVersion; }
    int device_can_absolute_position(Device *device) { return device->CanAbsolutePosition; }
    int device_can_temperature_compensate(Device *device) { return device->CanTemperatureCompensate; }
    void device_set_temperature_coefficient(Device *device, double coefficient) { device->TemperatureCoefficient = coefficient; }
    double device_get_temperature_coefficient(Device *device) { return device->TemperatureCoefficient; }
    void device_enable_temperature_compensation(Device *device) { device->EnableTemperatureCompensation(); }
    void device_disable_temperature_compensation(Device *device) { device->DisableTemperatureCompensation(); }
    bool device_temperature_compensation_enabled(Device *device) { return device->TemperatureCompensationEnabled(); }
    void device_do_temp_comp(Device *device) { device->DoTempComp(); }
}
