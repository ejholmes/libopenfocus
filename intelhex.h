#ifndef INTELHEX_H
#define INTELHEX_H

#include "stdio.h"

/* Record types */
enum record_type {
    data = 0,
    end_of_file = 1,
    extd_segment_address = 2,
    start_segment_address = 3,
    extd_linear_address = 4,
    start_linear_address = 5
};

/* Struct that represents intel hex format */
typedef struct record {
    char start;
    unsigned int byte_count;
    unsigned int address;
    enum record_type type;
    unsigned int *data;
    int checksum;
    struct record *next;
} record;

class IntelHexFile
{
private:

    /* Calculates the twos compliment of a line */
    int TwosCompliment(record *rec);

    /* Checks the checksum against the calculated twos compliment */
    bool VerifyChecksum(record *rec);

    /* Parses a line */
    record *ParseLine(FILE *fp);

public:
    /* Creates an intel hex file from data */
    static IntelHexFile *Create(const char *data, int byte_count);

    /* Opens a file in intel hex format and returns a pointer to a linked list records */
    record *Open(FILE *fp);
};


#endif // INTELHEX_H
