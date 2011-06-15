#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "openfocus.h"
#include "intelhex.h"

void test_intelhex_open()
{
    const char line[] = ":100000000C946D000C94A3010C948A000C948A004B";
    FILE *fp = tmpfile();
    fwrite(line, 1, sizeof(line), fp);
    rewind(fp);
    //static unsigned char payload[0x10] = {0x0c, 0x94, 0x6d, 0x00, 0x0c, 0x94, 0xa3, 0x01, 0x0c, 0x94, 0x8a, 0x00};

    record *r = IntelHexFile::Open(fp);

    assert(r->start == ':');
    assert(r->byte_count == 0x10);
    assert(r->address == 0x00);
    assert(r->type == data);
    assert(r->checksum == 0x4b);

    fclose(fp);
}

void test_intelhex_records_to_flash_data()
{
    const char line[] = ":100000000C946D000C94A3010C948A000C948A004B";
    FILE *fp = tmpfile();
    fwrite(line, 1, sizeof(line), fp);
    rewind(fp);

    record *r = IntelHexFile::Open(fp);

    flash *fl = IntelHexFile::RecordsToFlashData(r);

    fclose(fp);
}

void test_block()
{
    char payload[] = {10, 12};
    block *b = (block *)malloc(4);
    b->address = 134;
    memcpy(&b->data, payload, 2);

    unsigned char *bytes = (unsigned char *)b;

    assert(bytes[0] == 134);
    assert(bytes[1] == 0);
    assert(bytes[2] == 10);
    assert(bytes[3] == 12);
}

int main(int argc, const char *argv[])
{
    test_intelhex_open();
    test_intelhex_records_to_flash_data();
    test_block();
    return 0;
}
