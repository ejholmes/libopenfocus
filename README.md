LibOpenFocus
============
This is a C++ library (with some C comatability) for communicating with the OpenFocus device and
bootloader. For more information about OpenFocus, see https://github.com/CortexAstronomy/OpenFocus.

Includes a QT project and a makefile for compiling from command line.

Compiling From Source
---------------------

```
$ make
$ make install # Only on linux
```

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

Compile on Linux

```
$ g++ main.cpp -lusb -lopenfocus
```

Compile on Windows

```
$ g++ -I"path to openfocus.h" main.cpp libopenfocus.dll
```

_Requires libopenfocus.dll to be in the same directory as the application_

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

Compile on Linux

```
$ gcc main.cpp -lusb -lopenfocus -lstdc++
```

Compile on Windows

```
$ gcc -I"path to openfocus.h" main.cpp libopenfocus.dll -lstdc++
```
