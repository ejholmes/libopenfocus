#include <time.h>

#include "OpenFocus.h"


int OpenFocus::Helper::ConnectBootloader()
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
            for (time_t t = time(NULL) + 2; time(NULL) < t;) {}
            return OpenFocus::Helper::ConnectBootloader();
        }
        else {
            return 0; /* Device not found */
        }
    }
    return 0;
}
