#include "wc1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TestBitWriter {
    unsigned char *bytes;
    size_t byteCapacity;
    size_t bitPosition;
} TestBitWriter;

static int WriteTestCode(TestBitWriter *writer, unsigned int code,
                         unsigned int width)
{
    unsigned int bit;

    bit = 0;
    while (bit < width) {
        size_t bytePosition;
        unsigned int destinationBit;

        bytePosition = writer->bitPosition >> 3;
        if (bytePosition >= writer->byteCapacity)
            return 0;
        destinationBit = (unsigned int)(writer->bitPosition & 7);
        if ((code & (1U << bit)) != 0)
            writer->bytes[bytePosition] |=
                (unsigned char)(1U << destinationBit);
        writer->bitPosition++;
        bit++;
    }
    return 1;
}

static int CheckShortStreams(void)
{
    const unsigned char simpleStream[5] = { 0x00, 0x83, 0x08, 0x09, 0x08 };
    const unsigned char specialStream[5] = { 0x00, 0x83, 0x08, 0x0c, 0x08 };
    const unsigned char invalidStream[2] = { 0x41, 0x00 };
    unsigned char output[4];
    size_t written;

    memset(output, 0, sizeof(output));
    if (!Wc1SdlDecompressOriginLzw(simpleStream, sizeof(simpleStream),
                                   output, 2, &written) ||
        written != 2 || memcmp(output, "AB", 2) != 0)
        return 0;
    memset(output, 0, sizeof(output));
    if (!Wc1SdlDecompressOriginLzw(specialStream,
                                   sizeof(specialStream), output, 3,
                                   &written) ||
        written != 3 || memcmp(output, "AAA", 3) != 0)
        return 0;
    if (Wc1SdlDecompressOriginLzw(invalidStream,
                                  sizeof(invalidStream), output, 1,
                                  &written))
        return 0;
    if (Wc1SdlDecompressOriginLzw(simpleStream, sizeof(simpleStream),
                                  output, 3, &written))
        return 0;
    return 1;
}

static int CheckCodeWidthGrowth(void)
{
    unsigned char compressed[512];
    unsigned char output[300];
    unsigned char expected[300];
    TestBitWriter writer;
    unsigned int codeWidth;
    unsigned int threshold;
    unsigned int dictionarySize;
    unsigned int index;
    size_t written;

    memset(compressed, 0, sizeof(compressed));
    writer.bytes = compressed;
    writer.byteCapacity = sizeof(compressed);
    writer.bitPosition = 0;
    codeWidth = 9;
    threshold = 1U << codeWidth;
    dictionarySize = 0x102;
    if (!WriteTestCode(&writer, 0x100, codeWidth))
        return 0;
    index = 0;
    while (index < sizeof(expected)) {
        expected[index] = (unsigned char)(index % 251);
        if (!WriteTestCode(&writer, expected[index], codeWidth))
            return 0;
        if (index != 0) {
            dictionarySize++;
            if (dictionarySize == threshold && codeWidth < 12) {
                codeWidth++;
                threshold <<= 1;
            }
        }
        index++;
    }
    if (!WriteTestCode(&writer, 0x101, codeWidth))
        return 0;
    if (!Wc1SdlDecompressOriginLzw(
            compressed, (writer.bitPosition + 7) >> 3,
            output, sizeof(output), &written))
        return 0;
    return written == sizeof(output) &&
        memcmp(output, expected, sizeof(output)) == 0;
}

static int CheckPacketSections(void)
{
    const unsigned char archive[23] = {
        0x17, 0x00, 0x00, 0x00,
        0x0c, 0x00, 0x00, 0x00,
        0x0e, 0x00, 0x00, 0x01,
        'X', 'Y',
        0x02, 0x00, 0x00, 0x00,
        0x00, 0x83, 0x08, 0x09, 0x08
    };
    unsigned char *section;
    size_t sectionSize;

    section = 0;
    sectionSize = 0;
    if (!Wc1SdlExtractOriginPacketSection(
            archive, sizeof(archive), 0, &section, &sectionSize) ||
        sectionSize != 2 || memcmp(section, "XY", 2) != 0) {
        free(section);
        return 0;
    }
    free(section);
    section = 0;
    if (!Wc1SdlExtractOriginPacketSection(
            archive, sizeof(archive), 1, &section, &sectionSize) ||
        sectionSize != 2 || memcmp(section, "AB", 2) != 0) {
        free(section);
        return 0;
    }
    free(section);
    return !Wc1SdlExtractOriginPacketSection(
        archive, sizeof(archive), 2, &section, &sectionSize);
}

static int CheckDosInstallCompletion(void)
{
    DiskFileRecord records[77];

    memset(records, 0, sizeof(records));
    Wc1SdlCompleteDosInstallTable(records);
    return strcmp(records[72].name, "MODULE.002") == 0 &&
        records[72].logicalFile == 73 &&
        strcmp(records[73].name, "BRIEFING.002") == 0 &&
        records[73].logicalFile == 74 &&
        strcmp(records[74].name, "CAMP.002") == 0 &&
        records[74].logicalFile == 75 &&
        strcmp(records[75].name, "TITLE1.VGA") == 0 &&
        records[75].logicalFile == 76 &&
        records[76].name[0] == '\0';
}

int main(int argumentCount, char **arguments)
{
    if (!CheckShortStreams()) {
        fprintf(stderr, "Origin LZW short-stream test failed\n");
        return 1;
    }
    if (!CheckCodeWidthGrowth()) {
        fprintf(stderr, "Origin LZW width-growth test failed\n");
        return 1;
    }
    if (!CheckPacketSections()) {
        fprintf(stderr, "Origin packet-section test failed\n");
        return 1;
    }
    if (!CheckDosInstallCompletion()) {
        fprintf(stderr, "DOS INSTALL.DAT completion test failed\n");
        return 1;
    }
    return 0;
}
