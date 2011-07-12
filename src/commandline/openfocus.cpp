#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <openfocus.h>

using namespace OpenFocus;

void usage()
{
    printf("Usage: openfocus [OPTION]...\n");
    printf("Control an openfocus device.\n\n");
    
    printf("  -l, --list                        list currently connected devices.\n");
    printf("  -r, --reverse                     reverse the direction that the stepper motor rotates.\n");
    printf("  -p, --position                    print the current position.\n");
    printf("  -t, --temperature                 print the current temperature.\n");
    printf("  -s, --stop                        stop focuser movement.\n");
    printf("  -m, --move=POSITION               move to POSITION.\n");
    printf("  -c, --compensate=COEFFICIENT      start temperature compensation using COEFFICIENT for the temperature coefficient.\n");
}

int main(int argc, char **argv)
{
    if (argc <= 1) {
        usage();
        return 0;
    }
    Device *device = new Device();
    /* Try to connect to the device */
    if (!device->Connect()) {
        fprintf(stderr, "Could not connect to device!\n");
        return -1;
    }
    int c;
    opterr = 0;
    while (1) {
        static struct option long_options[] = {
            {"help",        no_argument,       0, 'h'}, /* Help */
            {"list",        no_argument,       0, 'l'}, /* List currently connected devices */
            {"reverse",     no_argument,       0, 'r'}, /* Reverse motor direction */
            {"position",    no_argument,       0, 'p'}, /* Print current position */
            {"temperature", no_argument,       0, 't'}, /* Print current temperature */
            {"stop",        no_argument,       0, 's'}, /* Stop movement */
            {"move",        required_argument, 0, 'm'}, /* Move to position */
            {"compensate",  required_argument, 0, 'c'}, /* Start temperature compensation */
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        c = getopt_long(argc, argv, "hlrptsm:c:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'h':
                usage();
                goto done;
            case 'l':
                {
                    /* Enumerate all the connected devices and iterate through
                     * the list */
                    device_info *devs = Device::EnumerateDevices();
                    for (devs; devs != NULL; devs = devs->next) {
                        printf("%s\n", devs->serial);
                    }
                }
                goto done;
            case 'r':
                device->ReverseDirection(true);
                break;
            case 'p':
                printf("Position: %d\n", device->GetPosition());
                goto done;
            case 't':
                printf("Temperature: %.2f\n", device->GetTemperature());
                goto done;
            case 's':
                device->Halt();
                goto done;
            case 'm':
                {
                    unsigned short position = (unsigned short)atoi(optarg);
                    device->MoveTo(position);
                }
                goto done;
            case 'c':
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
