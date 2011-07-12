#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "intelhex.h"

struct record *IntelHexFile::Open(const char *data, size_t length)
{
    FILE *tmp = tmpfile();

    assert(tmp);

    if (fwrite(data, 1, length, tmp) > 0)
        rewind(tmp);

    return Open(tmp);
}

struct record *IntelHexFile::Open(FILE *fp)
{
    assert(fp);

    struct record *current = NULL, *head = NULL, *last = NULL;

    while ((current = ParseLine(fp)) != NULL) {
        if (!head)
            head = current;
        if (last)
            last->next = current;
        last = current;
    }

    return head;
}

struct flash *IntelHexFile::RecordsToFlashData(struct record *records)
{
    struct flash *fl = (struct flash *)malloc(sizeof(struct flash));
    struct record *current;

    fl->size = 0;
    fl->data = NULL;

    for (current = records; current != NULL; current = current->next) {
        fl->size = current->address + current->byte_count;
        fl->data = (unsigned char *)realloc(fl->data, fl->size);
        memcpy(&fl->data[current->address], current->data, current->byte_count);
    }

    return fl;
}

void IntelHexFile::FreeRecords(struct record *records)
{
    struct record *current, *next;

    for (current = records; current != NULL; current = current->next) {
        next = current;
        free(current->data);
        free(current);
        current = next;
    }

    records = NULL;
}

void IntelHexFile::FreeFlashData(flash *flashdata)
{
    free(flashdata->data);
    free(flashdata);
    flashdata = NULL;
}

struct record *IntelHexFile::ParseLine(FILE *fp)
{
    struct record *rec = (struct record *)malloc(sizeof(struct record));
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

int IntelHexFile::ReadBytes(FILE *fp, size_t length)
{
    char *buffer = NULL;
    int field = 0;

    buffer = (char *)malloc(length + 1);
    if (fread(buffer, length, sizeof(char), fp) > 0) {
        buffer[length] = '\0';
        sscanf(buffer, "%x", &field); /* Conver the string from a hex number to int */
    }

    free(buffer);

    return field;
}

int IntelHexFile::TwosCompliment(struct record *rec)
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

bool IntelHexFile::VerifyChecksum(struct record *rec)
{
    if (TwosCompliment(rec) != rec->checksum)
        return false;
    else
        return true;
}

