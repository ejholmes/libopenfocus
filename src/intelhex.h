#ifndef INTELHEX_H
#define INTELHEX_H

#include <stdlib.h>
#include <stdio.h>

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
struct record {
    char start;                 /* ':' */
    unsigned char byte_count;   /* Number of bytes in data, usually 8 or 16 */
    unsigned short address;     /* Address where data goes */
    enum record_type type;      /* Record type */
    unsigned char *data;        /* Data */
    int checksum;               /* Twos compliment of the sum of all fields except start and checksum */
    struct record *next;        /* Next record in the list */
};

struct flash {
    size_t size;
    unsigned char *data;
};

class IntelHexFile
{
private:

    /* Calculates the twos compliment of a line */
    static int TwosCompliment(struct record *rec);

    /* Checks the checksum against the calculated twos compliment */
    static bool VerifyChecksum(struct record *rec);

    /* Parses a line */
    static struct record *ParseLine(FILE *fp);

    /* Reads some number of characters from fp and converts it from a hex number to int */
    static int ReadBytes(FILE *fp, size_t length);

public:
    /* Creates an intel hex file from data */
    static char *Create(const char *data, size_t length, int byte_count);

    /* Opens a file in intel hex format and returns a pointer to a linked list of records */
    static struct record *Open(FILE *fp);
    static struct record *Open(const char *data, size_t length);

    /* Converts a linked list of records to an array of data */
    static struct flash *RecordsToFlashData(struct record *records);

    /* Functions for freeing lists */
    static void FreeRecords(struct record *records);
    static void FreeFlashData(struct flash *flashdata);
};


#endif // INTELHEX_H
