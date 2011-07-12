#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "openfocus.h"

#ifdef DEBUG
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif

int OpenFocus::Helper::ConnectBootloader()
{
    return OpenFocus::Helper::ConnectBootloader(true);
}

int OpenFocus::Helper::ConnectBootloader(bool timer)
{
    OpenFocus::Bootloader *bootloader = new OpenFocus::Bootloader();

    if (bootloader->Connect()) { /* Bootloader found */
        DBG("Connected to bootloader\n");
        return 1;
    }
    else { /* Bootloader not found, try rebooting device into bootloader */
        DBG("Attempting to connec to device before rebooting\n");
        OpenFocus::Device *device = new OpenFocus::Device();
        if (device->Connect()) {
            DBG("Connected to device. Attempting to reboot into bootloader\n");
            device->RebootToBootloader();
            device->Disconnect();
            /* Wait 2000 ms */
            if (timer) {
                DBG("Device is rebooting. Waiting 2 seconds before connecting\n");
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
