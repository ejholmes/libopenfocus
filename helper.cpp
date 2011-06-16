#include <stdlib.h>
#include <time.h>

#include "openfocus.h"

int OpenFocus::Helper::ConnectBootloader()
{
    return OpenFocus::Helper::ConnectBootloader(true);
}

int OpenFocus::Helper::ConnectBootloader(bool timer)
{
    OpenFocus::Bootloader *bootloader = new OpenFocus::Bootloader();

    if (bootloader->Connect()) { /* Bootloader found */
        return 1;
    }
    else { /* Bootloader not found, try rebooting device into bootloader */
        OpenFocus::Device *device = new OpenFocus::Device();
        if (device->Connect()) {
            device->RebootToBootloader();
            device->Disconnect();
            /* Wait 2000 ms */
            if (timer) {
                for (time_t t = time(NULL) + 2; time(NULL) < t;) {}
                return OpenFocus::Helper::ConnectBootloader();
            }
        }
        else {
            return 0; /* Device not found */
        }
    }
    return 0;
}

double OpenFocus::Helper::Celsius(double kelvin)
{
    return kelvin - 273.15;
}

double OpenFocus::Helper::Fahrenheit(double kelvin)
{
    return ((9.00 / 5.00) * Celsius(kelvin)) + 32.00;
}
