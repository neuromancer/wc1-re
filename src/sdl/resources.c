/* SDL2-only support for the Origin packet containers used by WC1 data. */
#include "wc1.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ORIGIN_LZW_CLEAR_CODE 0x100
#define ORIGIN_LZW_STOP_CODE 0x101
#define ORIGIN_LZW_FIRST_CODE 0x102
#define ORIGIN_LZW_MAX_CODE_COUNT 0x1000
#define ORIGIN_PACKET_OFFSET_MASK 0x00ffffffU
#define ORIGIN_PACKET_MAX_SECTION_SIZE (16U * 1024U * 1024U)

typedef struct Wc1SdlOriginLzwEntry {
    uint16_t prefix;
    unsigned char value;
} Wc1SdlOriginLzwEntry;

typedef struct Wc1SdlOriginLzwBitReader {
    const unsigned char *bytes;
    size_t byteCount;
    size_t bitPosition;
} Wc1SdlOriginLzwBitReader;

static int g_nWc1SdlDosData = -1;

static uint32_t Wc1SdlReadLittleEndian32(const unsigned char *bytes)
{
    return (uint32_t)bytes[0] |
        ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) |
        ((uint32_t)bytes[3] << 24);
}

static int Wc1SdlReadOriginLzwCode(Wc1SdlOriginLzwBitReader *reader,
                                   unsigned int width,
                                   uint16_t *code)
{
    size_t bytePosition;
    unsigned int sourceShift;
    unsigned int bitsRead;
    unsigned int bitsAvailable;
    unsigned int bitsNeeded;
    unsigned int bitsTaken;
    unsigned int mask;
    unsigned int value;

    value = 0;
    bitsRead = 0;
    while (bitsRead < width) {
        bytePosition = reader->bitPosition >> 3;
        if (bytePosition >= reader->byteCount)
            return 0;
        sourceShift = (unsigned int)(reader->bitPosition & 7);
        bitsAvailable = 8 - sourceShift;
        bitsNeeded = width - bitsRead;
        bitsTaken = bitsAvailable < bitsNeeded
            ? bitsAvailable : bitsNeeded;
        mask = (1U << bitsTaken) - 1;
        value |= ((reader->bytes[bytePosition] >> sourceShift) & mask)
            << bitsRead;
        reader->bitPosition += bitsTaken;
        bitsRead += bitsTaken;
    }
    *code = (uint16_t)value;
    return 1;
}

static int Wc1SdlWriteOriginLzwCode(
    const Wc1SdlOriginLzwEntry *dictionary, uint16_t code,
    unsigned char *destination, size_t destinationSize,
    size_t *destinationPosition, unsigned char *firstValue)
{
    unsigned char reverse[ORIGIN_LZW_MAX_CODE_COUNT];
    size_t reverseSize;

    reverseSize = 0;
    while (code >= ORIGIN_LZW_FIRST_CODE) {
        if (code >= ORIGIN_LZW_MAX_CODE_COUNT ||
            reverseSize >= sizeof(reverse))
            return 0;
        reverse[reverseSize++] = dictionary[code].value;
        code = dictionary[code].prefix;
    }
    if (code > 0xff || *destinationPosition >= destinationSize)
        return 0;
    *firstValue = (unsigned char)code;
    destination[(*destinationPosition)++] = *firstValue;
    while (reverseSize != 0) {
        if (*destinationPosition >= destinationSize)
            return 0;
        destination[(*destinationPosition)++] = reverse[--reverseSize];
    }
    return 1;
}

int Wc1SdlDecompressOriginLzw(const unsigned char *source,
                              size_t sourceSize,
                              unsigned char *destination,
                              size_t destinationSize,
                              size_t *writtenSize)
{
    Wc1SdlOriginLzwEntry dictionary[ORIGIN_LZW_MAX_CODE_COUNT];
    Wc1SdlOriginLzwBitReader reader;
    size_t destinationPosition;
    unsigned int codeWidth;
    unsigned int codeWidthThreshold;
    unsigned int dictionarySize;
    uint16_t code;
    uint16_t decodedCode;
    uint16_t previousCode;
    unsigned char firstValue;
    int specialCode;

    if (source == 0 || destination == 0 || writtenSize == 0)
        return 0;
    reader.bytes = source;
    reader.byteCount = sourceSize;
    reader.bitPosition = 0;
    destinationPosition = 0;
    *writtenSize = 0;

    codeWidth = 9;
    if (!Wc1SdlReadOriginLzwCode(&reader, codeWidth, &code))
        return 0;
    if (code == ORIGIN_LZW_STOP_CODE)
        return destinationSize == 0;
    if (code != ORIGIN_LZW_CLEAR_CODE)
        return 0;

    for (;;) {
        codeWidth = 9;
        codeWidthThreshold = 1U << codeWidth;
        dictionarySize = ORIGIN_LZW_FIRST_CODE;
        previousCode = ORIGIN_LZW_CLEAR_CODE;

        for (;;) {
            if (!Wc1SdlReadOriginLzwCode(&reader, codeWidth, &code))
                return 0;
            if (code == ORIGIN_LZW_STOP_CODE) {
                *writtenSize = destinationPosition;
                return destinationPosition == destinationSize;
            }
            if (code == ORIGIN_LZW_CLEAR_CODE)
                break;
            if (code > dictionarySize)
                return 0;

            specialCode = code == dictionarySize;
            decodedCode = specialCode ? previousCode : code;
            if (decodedCode == ORIGIN_LZW_CLEAR_CODE ||
                !Wc1SdlWriteOriginLzwCode(
                    dictionary, decodedCode, destination,
                    destinationSize, &destinationPosition,
                    &firstValue))
                return 0;

            if (previousCode != ORIGIN_LZW_CLEAR_CODE) {
                if (dictionarySize >= ORIGIN_LZW_MAX_CODE_COUNT)
                    return 0;
                dictionary[dictionarySize].prefix = previousCode;
                dictionary[dictionarySize].value = firstValue;
                dictionarySize++;
                if (specialCode) {
                    if (destinationPosition >= destinationSize)
                        return 0;
                    destination[destinationPosition++] = firstValue;
                }
                if (dictionarySize == codeWidthThreshold &&
                    codeWidth < 12) {
                    codeWidth++;
                    codeWidthThreshold <<= 1;
                }
            }
            previousCode = code;
        }
    }
}

int Wc1SdlExtractOriginPacketSection(const unsigned char *archive,
                                     size_t archiveSize,
                                     unsigned int sectionIndex,
                                     unsigned char **section,
                                     size_t *sectionSize)
{
    unsigned char *output;
    size_t writtenSize;
    uint32_t declaredFileSize;
    uint32_t directorySize;
    uint32_t entry;
    uint32_t nextEntry;
    uint32_t outputSize;
    uint32_t sectionCount;
    uint32_t sectionOffset;
    uint32_t sectionEnd;
    unsigned int compression;

    if (section == 0 || sectionSize == 0)
        return 0;
    *section = 0;
    *sectionSize = 0;
    if (archive == 0 || archiveSize < 8)
        return 0;

    declaredFileSize = Wc1SdlReadLittleEndian32(archive);
    entry = Wc1SdlReadLittleEndian32(archive + 4);
    directorySize = entry & ORIGIN_PACKET_OFFSET_MASK;
    if (declaredFileSize > archiveSize || directorySize < 8 ||
        directorySize > declaredFileSize || (directorySize & 3U) != 0)
        return 0;
    sectionCount = (directorySize >> 2) - 1;
    if (sectionIndex >= sectionCount)
        return 0;

    entry = Wc1SdlReadLittleEndian32(
        archive + ((size_t)sectionIndex + 1U) * 4U);
    compression = entry >> 24;
    sectionOffset = entry & ORIGIN_PACKET_OFFSET_MASK;
    if (sectionIndex + 1 == sectionCount) {
        sectionEnd = declaredFileSize;
    } else {
        nextEntry = Wc1SdlReadLittleEndian32(
            archive + ((size_t)sectionIndex + 2U) * 4U);
        sectionEnd = nextEntry & ORIGIN_PACKET_OFFSET_MASK;
    }
    if (sectionOffset < directorySize || sectionEnd <= sectionOffset ||
        sectionEnd > declaredFileSize)
        return 0;

    /* PacketLoad treats compression 1 as LZW and every other value as raw.
     * Kilrathi Saga MUSIC.MID uses 0xe0 for its uncompressed MIDI sections. */
    if (compression != 1) {
        outputSize = sectionEnd - sectionOffset;
        if (outputSize > ORIGIN_PACKET_MAX_SECTION_SIZE)
            return 0;
        output = (unsigned char *)malloc(outputSize != 0 ? outputSize : 1);
        if (output == 0)
            return 0;
        memcpy(output, archive + sectionOffset, outputSize);
        *section = output;
        *sectionSize = outputSize;
        return 1;
    }
    if (sectionEnd - sectionOffset < 4)
        return 0;

    outputSize = Wc1SdlReadLittleEndian32(archive + sectionOffset);
    if (outputSize == 0 || outputSize > ORIGIN_PACKET_MAX_SECTION_SIZE)
        return 0;
    output = (unsigned char *)malloc(outputSize);
    if (output == 0)
        return 0;
    if (!Wc1SdlDecompressOriginLzw(
            archive + sectionOffset + 4,
            sectionEnd - sectionOffset - 4,
            output, outputSize, &writtenSize)) {
        free(output);
        return 0;
    }
    *section = output;
    *sectionSize = writtenSize;
    return 1;
}

int Wc1SdlUsingDosData(void)
{
    unsigned char header[8];
    int file;
    int bytesRead;

    if (g_nWc1SdlDosData >= 0)
        return g_nWc1SdlDosData;
    file = _open("GAMEDAT/MODULE.000", 0x8000);
    if (file == -1)
        file = _open("MODULE.000", 0x8000);
    if (file == -1)
        return 0;
    g_nWc1SdlDosData = 0;
    bytesRead = (int)_read(file, header, sizeof(header));
    _close(file);
    if (bytesRead == (int)sizeof(header) && header[7] == 1)
        g_nWc1SdlDosData = 1;
    return g_nWc1SdlDosData;
}

void Wc1SdlCompleteDosInstallTable(DiskFileRecord *records)
{
    const DiskFileRecord expansionRecords[4] = {
        { "MODULE.002", 1, 7, 73 },
        { "BRIEFING.002", 1, 2, 74 },
        { "CAMP.002", 1, 2, 75 },
        { "TITLE1.VGA", 1, 2, 76 }
    };

    memset(&records[72], 0, sizeof(DiskFileRecord) * 5);
    memcpy(&records[72], expansionRecords, sizeof(expansionRecords));
}
