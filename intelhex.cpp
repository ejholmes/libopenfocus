#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "intelhex.h"

#ifdef DEBUG
#define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

/*char *IntelHexFile::Create(const char *data, size_t length, int byte_count)
{
    record current;

    return NULL;
}*/

record *IntelHexFile::Open(const char *data, int length)
{
    FILE *tmp = tmpfile();

    assert(tmp != NULL);

    fwrite(data, 1, length, tmp);
    rewind(tmp);

    return Open(tmp);
}

record *IntelHexFile::Open(FILE *fp)
{
    assert(fp != NULL);

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

flash *IntelHexFile::RecordsToFlashData(record *records)
{
    flash *fl = (flash *)malloc(sizeof(flash));
    record *current;

    fl->size = 0;
    fl->data = NULL;

    for (current = records; current != NULL; current = current->next) {
        fl->size = current->address + current->byte_count;
        fl->data = (unsigned char *)realloc(fl->data, fl->size);
        memcpy(&fl->data[current->address], current->data, current->byte_count);
    }

    return fl;
}

void IntelHexFile::FreeRecords(record *records)
{
    record *current, *next;

    for (current = records; current != NULL; current = current->next) {
        next = current;
        if (current->data != NULL)
            free(current->data);
        free(current);
        current = next;
    }
}

void IntelHexFile::FreeFlashData(flash *flashdata)
{
    if (flashdata->data != NULL)
        free(flashdata->data);
    free(flashdata);
    flashdata = NULL;
}

char *IntelHexFile::RecordToString(record *rec)
{
    return NULL;
}

record *IntelHexFile::ParseLine(FILE *fp)
{
    record *rec = (record *)malloc(sizeof(record));
    rec->data = NULL;
    char  start;

    while ((start = fgetc(fp)) != ':') { if (start == EOF) return NULL; } /* move forward until we find the start of the record */

    /* Read start */
    rec->start = start;

    /* Read byte count */
    rec->byte_count = (unsigned char)ReadBytes(fp, 2);

    /* Read address */
    rec->address = (unsigned short)ReadBytes(fp, 4);

    /* Read record type */
    rec->type = (enum record_type)ReadBytes(fp, 2);

    if (rec->type == end_of_file)
        return NULL;

    /* Read data */
    rec->data = (unsigned char *)malloc(rec->byte_count * sizeof(char));
    unsigned int i;
    for (i = 0; i < rec->byte_count; i++) {
        rec->data[i] = (unsigned char)ReadBytes(fp, 2);
    }

    /* Read checksum */
    rec->checksum = ReadBytes(fp, 2);

    rec->next = NULL;

    if (!VerifyChecksum(rec))
        return NULL;

    return rec;
}

int IntelHexFile::ReadBytes(FILE *fp, int length)
{
    char *buffer = NULL;
    int field = 0;

    buffer = (char *)malloc(length + 1);
    fread(buffer, length, sizeof(char), fp);
    buffer[length] = '\0';
    sscanf(buffer, "%x", &field); /* Conver the string from a hex number to int */

    free(buffer);

    return field;
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

