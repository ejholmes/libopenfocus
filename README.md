This is a C++ (with some C comatability) library for communicating with the OpenFocus device and
bootloader. For more information about OpenFocus, see https://github.com/CortexAstronomy/OpenFocus.

Includes a QT project and a makefile for compiling from command line.

Compiling
---------
If you have QT, open OpenFocus.pro and build the project. If not, run make in
the directory. Both will generate a libopenfocus.a that you can statically link
in your program.

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
    handle = device_create();
    
    if (device_connect(handle)) {
        printf("Connected to OpenFocus device\n");
        device_move_to(handle, 100);
    }
    else {
        printf("Could not find OpenFocus device\n");
    }
}
```

Compile with ``gcc -lstdc++ -I"path to openfocus.h" main.cpp `libusb-config --libs` -L"path to libopenfocus.a" -lopenfocus``.