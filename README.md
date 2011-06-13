This is a C++ (with some C comatability) library for communicating with the OpenFocus device and
bootloader. For more information about OpenFocus, see https://github.com/CortexAstronomy/OpenFocus.

Includes a QT project and a makefile for compiling from command line.

Compiling
---------
Run `make`. This will build a libopenfocus.a that you can link against in your program.

Usage
-----
**C++**

```
#include <stdio.h>

#include "openfocus.h"

int main()
{
    OpenFocus::Device *device = new OpenFocus::Device();
    
    if (device->Connect()) {
        printf("Connected to OpenFocus device\n");
        device->MoveTo(100);
    }
    else {
        printf("Could not find OpenFocus device\n");
    }
}
```

Compile with ``g++ -I"path to openfocus.h" main.cpp `libusb-config --libs` -L"path to libopenfocus.a" -lopenfocus``.

**C**

```
#include <stdio.h>

#include "openfocus.h"

int main()
{
    CDevice *handle;
    
    if ((handle = device_connect()) != NULL) {
        printf("Connected to OpenFocus device\n");
        device_move_to(handle, 100);
    }
    else {
        printf("Could not find OpenFocus device\n");
    }
}
```

Compile with ``gcc -lstdc++ -I"path to openfocus.h" main.cpp `libusb-config --libs` -L"path to libopenfocus.a" -lopenfocus``.
