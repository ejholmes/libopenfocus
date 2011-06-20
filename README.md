LibOpenFocus
============
This is a C++ library (with some C compatability) for communicating with the OpenFocus device and
bootloader. For more information about OpenFocus, see https://github.com/CortexAstronomy/OpenFocus.

Compiling From Source
---------------------

**Linux**

```
$ git clone git@github.com:CortexAstronomy/LibOpenFocus.github
$ cd LibOpenFocus
$ autoreconf --install && ./configure --prefix=/usr
$ make
$ make install
```

**Windows**  
Requires [MSYS](http://www.mingw.org/wiki/msys)

```
> git clone git@github.com:CortexAstronomy/LibOpenFocus.github
> cd LibOpenFocus
> make -f Makefile.win
```

**Mac OS X**  
Requires Xcode

```
$ git clone git@github.com:CortexAstronomy/LibOpenFocus.github
$ open LibOpenFocus.xcodeproj
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

```
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

```
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

```
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

