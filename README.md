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
The following shows a simple example of how to use the library in a C/C++ application.  

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
        printf("Current ambient temperature read by device: %f", device->GetTemperature());
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
$ ./a.out
Connected to OpenFocus device
Current ambient temperature read by device: 299.800000
```

Compile on Windows

```
> g++ -I"path to openfocus.h" main.cpp libopenfocus.dll
> a.exe
Connected to OpenFocus device
Current ambient temperature read by device: 299.800000
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
        printf("Current ambient temperature read by device: %f", device_get_temperature(handle));
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
$ ./a.out
Connected to OpenFocus device
Current ambient temperature read by device: 299.800000
```

Compile on Windows

```
> gcc -I"path to openfocus.h" main.cpp libopenfocus.dll -lstdc++
> a.exe
Connected to OpenFocus device
Current ambient temperature read by device: 299.800000
```
