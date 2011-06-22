#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openfocus.h>

using namespace OpenFocus;

void usage()
{

}

int main(int argc, char **argv)
{
    Device *device = new Device();
    if (!device->Connect()) {
        printf("Could not connect to device!\n");
        return -1;
    }
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "lrptsm:c:")) != -1) {
        switch (c) {
            case 'l': /* List currently connected devices */
                {
                    device_info *devs = Device::EnumerateDevices();
                    for (devs; devs != NULL; devs = devs->next) {
                        printf("%s\n", devs->serial);
                    }
                }
                goto done;
            case 'r': /* Reverse direction */
                device->ReverseDirection(true);
                break;
            case 'p':
                printf("Position: %d\n", device->GetPosition());
                goto done;
            case 't': /* Get temperature */
                printf("Temperature: %f\n", device->GetTemperature());
                goto done;
            case 's': /* Stop/Halt */
                device->Halt();
                goto done;
            case 'm': /* Move */
                {
                    unsigned short position = (unsigned short)atoi(optarg);
                    device->MoveTo(position);
                }
                goto done;
            case 'c': /* Temperature compensation */
                if (!device->CanTemperatureCompensate) {
                    printf("Device does not support temperature compensation\n");
                    goto done;
                }
                {
                    double temp_coefficient = strtod(optarg, NULL);
                    device->TemperatureCoefficient = temp_coefficient;
                    device->EnableTemperatureCompensation();
                    for (;;) {
                        printf("Temp: %f, Moving: %d\n", device->GetTemperature(), device->DoTempComp());
                        sleep(4);
                    }
                }
                goto done;
            default:
                usage();
                goto done;
        }
    }
done:
    if (device && device->IsConnected())
        device->Disconnect();
    return 0;
}
