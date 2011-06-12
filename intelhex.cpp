#include "stdio.h"
#include "stdlib.h"
#include "string"

#include "intelhex.h"

char *IntelHexFile::Create(const char *data, size_t length, int byte_count)
{
    record current;

    return NULL;
}

record *IntelHexFile::Open(FILE *fp)
{
    if (fp == NULL)
        return NULL;

    record *current = NULL, *head = NULL, *last = NULL;

    while ((current = ParseLine(fp)) != NULL) {
        if (head == NULL)
            head = current;
        if (last != NULL)
            last->next = current;
        last = current;
    }

    return head;
}

char *IntelHexFile::RecordToString(record *rec)
{
    return NULL;
}

record *IntelHexFile::ParseLine(FILE *fp)
{
    record *rec = (record *)malloc(sizeof(record));
    char *buffer, start;

    while ((start = fgetc(fp)) != ':') { }

    /* Read start */
    rec->start = start;

    /* Read byte count */
    buffer = (char *)realloc(buffer, 3); /* 2 + '\0' */
    fread(buffer, 2, 1, fp);
    buffer[2] = '\0';
    sscanf(buffer, "%x", &rec->byte_count);

    /* Read address */
    buffer = (char *)realloc(buffer, 5); /* 4 + '\0' */
    fread(buffer, 4, 1, fp);
    buffer[4] = '\0';
    sscanf(buffer, "%x", &rec->address);

    /* Read record type */
    buffer = (char *)realloc(buffer, 3); /* 2 + '\0' */
    fread(buffer, 2, 1, fp);
    buffer[2] = '\0';
    sscanf(buffer, "%x", (int *)&rec->type);

    if (rec->type == end_of_file)
        return NULL;

    /* Read data */
    buffer = (char *)realloc(buffer, 3);
    unsigned int i;
    for (i = 0; i < rec->byte_count; i++) {
        fread(buffer, 2, 1, fp);
        buffer[2] = '\0';
        sscanf(buffer, "%x", &rec->data[i]);
    }

    /* Read checksum */
    buffer = (char *)realloc(buffer, 3); /* 2 + '\0' */
    fread(buffer, 2, 1, fp);
    buffer[2] = '\0';
    sscanf(buffer, "%x", &rec->checksum);

    free(buffer);

    rec->next = NULL;

    if (!VerifyChecksum(rec))
        return NULL;

    return rec;
}

int IntelHexFile::TwosCompliment(record *rec)
{
    int sum = 0;
    sum += (int)rec->byte_count;
    sum += (int)((rec->address & 0xff) + ((rec->address >> 8) & 0xff));
    sum += (int)rec->type;

    unsigned int i;
    for (i = 0; i < rec->byte_count; i++) {
        sum += (int)rec->data[i];
    }

    int twos_comp = (~sum + 1) & 0xff;

    return twos_comp;
}

bool IntelHexFile::VerifyChecksum(record *rec)
{
    if (TwosCompliment(rec) != rec->checksum)
        return false;
    else
        return true;
}

