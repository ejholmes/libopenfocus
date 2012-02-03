LibOpenFocus
============
This is a C++ library (with some C compatibility) for communicating with the OpenFocus device and
bootloader. For more information about OpenFocus, see https://github.com/ejholmes/openfocus.

Compiling From Source
---------------------
Download the most recent source package from the [downloads section](https://github.com/ejholmes/libopenfocus/downloads) and extract the contents.

**Linux**

```bash
$ ./configure --prefix=/usr
$ make
$ make install
```

**Windows**  
Requires [MSYS](http://www.mingw.org/wiki/msys)

```
> make -f Makefile.win
```

**Mac OS X**  
Requires Developer Tools/Xcode

```bash
$ ./configure LIBUSB_CFLAGS="<location of usb.h>" LIBUSB_LIBS"<location of libusb.a>"
$ make
```

Or, if you prefer Xcode:

```bash
$ open LibOpenFocus.xcodeproj
```

Usage
-----
The following shows a simple example of how to use the library in a C/C++ application.  

**C++**

```c++
#include <stdio.h>

#include "openfocus.h"

int main()
{
    OpenFocus::Device *device = new OpenFocus::Device();
    
    if (device->Connect()) {                             /* Connect to the focuser */
        printf("Connected to OpenFocus device\n");
        device->MoveTo(323);                             /* Move to position 323 */
        while (device->IsMoving()) { }
        printf("Moved to position %u\n", device->GetPosition());
        printf("Current ambient temperature read by device: %f\n", device->GetTemperature());
        device->Disconnect();                            /* Disconnect from the focuser */
    }
    else {
        printf("Could not find OpenFocus device\n");
    }
}
```

Compile on Linux

```bash
$ g++ main.cpp -lusb -lopenfocus
$ time ./a.out
Connected to OpenFocus device
Moved to position 323
Current ambient temperature read by the device: 299.800000

real	0m5.299s
user	0m0.017s
sys	0m0.000s
```

Compile on Windows

```
> g++ -I"path to openfocus.h" main.cpp libopenfocus.dll
> a.exe
Connected to OpenFocus device
Moved to position 323
Current ambient temperature read by the device: 299.800000
```

_Requires libopenfocus.dll to be in the same directory as the application_

---------------------------------------------------------------------------------------

**C**

```c++
#include <stdio.h>

#include "openfocus.h"

int main()
{
    Device *handle;
    
    if (device_connect(&handle)) {                        /* Connect to the focuser */
        printf("Connected to OpenFocus device\n");
        device_move_to(handle, 323);                      /* Move to position 323 */
        while (device_is_moving(handle)) { }
        printf("Moved to position %u\n", device_get_position(handle));
        printf("Current ambient temperature read by device: %f\n", device_get_temperature(handle));
        device_disconnect(handle);                        /* Disconnect from the focuser */
    }
    else {
        printf("Could not find OpenFocus device\n");
    }
}
```

Compile on Linux

```bash
$ gcc main.cpp -lusb -lopenfocus -lstdc++
$ time ./a.out
Connected to OpenFocus device
Moved to position 323
Current ambient temperature read by the device: 299.800000

real	0m5.299s
user	0m0.017s
sys	0m0.000s
```

Compile on Windows

```
> gcc -I"path to openfocus.h" main.cpp libopenfocus.dll -lstdc++
> a.exe
Connected to OpenFocus device
Moved to position 323
Current ambient temperature read by the device: 299.800000
```

_Requires libopenfocus.dll to be in the same directory as the application_
