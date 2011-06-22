#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <openfocus.h>

#include "intelhex.h"

using namespace OpenFocus;

void usage()
{
    printf("Usage: bootloader [OPTION]...\n");
    printf("Upload new firmware to the device.\n\n");
    
    printf("  -u, --upload=FILE              write FILE to device flash memory. FILE should be in intel hex format.\n");
}

int main(int argc, char **argv)
{
    Bootloader *bootloader = new Bootloader();
    int c;
    opterr = 0;
    while (1) {
        static struct option long_options[] = {
            {"help",        no_argument,       0, 'h'}, /* Help */
            {"upload",      required_argument, 0, 'u'}, /* Uplaod firmware */
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        c = getopt_long(argc, argv, "hu:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'h':
                usage();
                goto done;
            case 'u':
                if (!Helper::ConnectBootloader()) {
                    fprintf(stderr, "Could not connect to bootloader\n");
                    return -1;
                }
                {
                    FILE *fp = NULL;
                    fp = fopen(optarg, "r");
                    if (fp) {
                        record *records = NULL;
                        flash *flashdata = NULL;
                        records = IntelHexFile::Open(fp);
                        flashdata = IntelHexFile::RecordsToFlashData(records);
                        if (records) {
                            if (bootloader->WriteFlash(flashdata->data, flashdata->size)) {
                                printf("Uploaded %u bytes to device\n", flashdata->size);
                            } else {
                                fprintf(stderr, "An error occurred while writing flash data\n");
                            }
                        } else {
                            fprintf(stderr, "No data to upload. Is the file in hex format?\n");
                        }
                        IntelHexFile::FreeRecords(records);
                        IntelHexFile::FreeFlashData(flashdata);
                    } else {
                        fprintf(stderr, "Could not open file for reading\n");
                        return -1;
                    }
                }
                goto done;
            default:
                usage();
                goto done;
        }
    }
done:
    if (bootloader && bootloader->IsConnected()) {
        bootloader->Reboot();
        bootloader->Disconnect();
    }
    return 0;
}
