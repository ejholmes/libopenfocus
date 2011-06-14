This is a C++ library (with some C comatability) for communicating with the OpenFocus device and
bootloader. For more information about OpenFocus, see https://github.com/CortexAstronomy/OpenFocus.

Includes a QT project and a makefile for compiling from command line.

Compiling From Source
---------------------
Run `make`. This will build a libopenfocus.a that you can link against in your program.

Usage
-----
The follow shows a simple example of how to use the library in a C/C++ application.  

**C++**

```
#include <stdio.h>

#include "openfocus.h"

int main()
{
    OpenFocus::Device *device = new OpenFocus::Device();
    
    if (device->Connect()) {                             /* Connect to the focuser */
        printf("Connected to OpenFocus device\n");
        device->MoveTo(100);                             /* Move to position 100 */
        double temperature;
        device->GetTemperature(&temperature);            /* Get the current temperature read by the on-board temperature sensor */
        printf("Current ambient temperature read by device: %f", temperature);
        device->Disconnect();                            /* Disconnect from the focuser */
    }
    else {
        printf("Could not find OpenFocus device\n");
    }
}
```

Linux/Mac: Compile with ``g++ -I"path to openfocus.h" main.cpp `libusb-config --libs` -L"path to libopenfocus.a" -lopenfocus``. 

Windows: Compile with ``g++ -I"path to openfocus.h" main.cpp libopenfocus.dll``. _Requires libopenfocus.dll to be in the same directory as the application_

---------------------------------------------------------------------------------------

**C**

```
#include <stdio.h>

#include "openfocus.h"

int main()
{
    Device *handle;
    
    if (device_connect(&handle)) {                        /* Connect to the focuser */
        printf("Connected to OpenFocus device\n");
        device_move_to(handle, 100);                      /* Move to position 100 */
        double temperature;
        device_get_temperature(handle, &temperature);     /* Get the current temperature read by the on-board temperature sensor */
        printf("Current ambient temperature read by device: %f", temperature);
        device_disconnect(handle);                        /* Disconnect from the focuser */
    }
    else {
        printf("Could not find OpenFocus device\n");
    }
}
```

Linux/Mac: Compile with ``gcc -I"path to openfocus.h" main.c `libusb-config --libs` -L"path to libopenfocus.a" -lopenfocus -lstdc++``. 

Windows: Compile with ``gc -I"path to openfocus.h" main.c libopenfocus.dll -lstdc++``. _Requires libopenfocus.dll to be in the same directory as the application_
