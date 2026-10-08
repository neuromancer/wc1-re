#include "swc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, \
                #expression, SDL_GetError()); \
        exit(1); \
    } \
} while (0)

static void WriteFixture(const char *path, const uint8_t *data, size_t size)
{
    SDL_RWops *file;

    file = SDL_RWFromFile(path, "wb");
    CHECK(file != NULL);
    CHECK(SDL_RWwrite(file, data, 1, size) == size);
    CHECK(SDL_RWclose(file) == 0);
}

static void TestRLE(void)
{
    const uint8_t commands[] = {2, 1, 2, 3, 0x82, 7, 2, 4, 5, 6};
    const uint8_t expected[] = {1, 2, 3, 7, 7, 7, 4, 5, 6};
    const uint8_t repeated[] = {0xff, 19};
    uint8_t literal[129];
    uint8_t output[130];
    size_t size;

    memset(output, 0xcc, sizeof(output));
    CHECK(SwcDecompressRLE(commands, sizeof(commands), output + 1, 9) == 0);
    CHECK(memcmp(output + 1, expected, 9) == 0);
    CHECK(output[0] == 0xcc && output[10] == 0xcc);
    for (size = 0; size < sizeof(commands); size++)
        CHECK(SwcDecompressRLE(commands, size, output, 9) != 0);
    CHECK(SwcDecompressRLE(commands, sizeof(commands), output, 8) != 0);
    CHECK(SwcDecompressRLE(commands, sizeof(commands), output, 10) != 0);
    CHECK(SwcDecompressRLE(commands, sizeof(commands), output, 0) != 0);
    CHECK(SwcDecompressRLE(NULL, 0, NULL, 0) == 0);
    CHECK(SwcDecompressRLE(repeated, sizeof(repeated), output + 1, 128) == 0);
    for (size = 1; size <= 128; size++)
        CHECK(output[size] == 19);
    literal[0] = 127;
    for (size = 1; size < sizeof(literal); size++)
        literal[size] = (uint8_t)size;
    CHECK(SwcDecompressRLE(literal, sizeof(literal), output, 128) == 0);
    CHECK(memcmp(output, literal + 1, 128) == 0);
    CHECK(SwcDecompressRLE(repeated, sizeof(repeated), output, 127) != 0);
    CHECK(SwcDecompressRLE(repeated, sizeof(repeated), output, 129) != 0);
}

static void TestFrames(void)
{
    /* Frame 1 starts at offset 22, deliberately not divisible by four.
     * Frame 2 is a valid empty frame. Padding bytes need not be zero. */
    uint8_t bytes[] = {
        0, 0, 0, 3,
        0, 1, 0xff, 0xfe, 0, 3, 0, 3, 0, 1, 0, 0, 0, 3,
        1, 0, 2, 0xdd,
        0, 2, 0, 4, 0xff, 0xfb, 0, 2, 0, 2, 0, 0, 0, 2,
        0x83, 9, 0xaa, 0xbb,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    SwcBuffer set = {bytes, sizeof(bytes)};
    SwcFrame frame;
    uint32_t count;
    uint8_t output[4];

    CHECK(SwcGetFrameCount(&set, &count) == 0 && count == 3);
    CHECK(SwcGetFramePtr(&set, 0, &frame) == 0);
    CHECK(frame.x == -2 && frame.y == 3 && frame.width == 3 && frame.height == 1);
    CHECK(SwcDecodeFrame(&frame, output, sizeof(output)) == 0);
    CHECK(output[0] == 1 && output[1] == 0 && output[2] == 2);
    CHECK(SwcDecodeFrame(&frame, output, 2) != 0);
    CHECK(SwcGetFramePtr(&set, 1, &frame) == 0);
    CHECK(frame.x == 4 && frame.y == -5 && frame.pixels == bytes + 36);
    CHECK(SwcDecodeFrame(&frame, output, sizeof(output)) == 0);
    CHECK(output[0] == 9 && output[3] == 9);
    CHECK(SwcGetFramePtr(&set, 2, &frame) == 0);
    CHECK(SwcDecodeFrame(&frame, NULL, 0) == 0);
    CHECK(SwcGetFramePtr(&set, 3, &frame) != 0);
    CHECK(SwcGetFramePtr(&set, UINT32_MAX, &frame) != 0);
    set.size--;
    CHECK(SwcGetFramePtr(&set, 2, &frame) != 0);
    set.size = sizeof(bytes);
    bytes[3] = 1;
    set.size = 21;
    CHECK(SwcGetFramePtr(&set, 0, &frame) != 0);
    bytes[3] = 3;
    set.size = sizeof(bytes);
    bytes[0] = 0xff;
    CHECK(SwcGetFrameCount(&set, &count) != 0);
    bytes[0] = 0;
    bytes[5] = 3;
    CHECK(SwcGetFramePtr(&set, 0, &frame) != 0);
    bytes[5] = 1;
    bytes[14] = 0xff;
    CHECK(SwcGetFramePtr(&set, 0, &frame) != 0);
}

static void TestCmf(const char *path)
{
    uint8_t bytes[128] = {0};
    const uint8_t commands[] = {2, 1, 2, 3, 0x82, 7, 2, 4, 5, 6};
    const uint8_t expected[] = {1, 2, 3, 7, 7, 7, 4, 5, 6};
    SDL_RWops *writer;
    SwcCmf cmf;
    SwcBuffer chunk;
    size_t size;
    size_t prefix;

    writer = SDL_RWFromMem(bytes, sizeof(bytes));
    CHECK(writer != NULL);
    SDL_RWwrite(writer, "CMF1", 1, 4);
    SDL_WriteLE32(writer, 0x10000);
    SDL_WriteLE32(writer, 28);
    SDL_WriteLE32(writer, 45);
    SDL_WriteLE32(writer, 56);
    SDL_WriteLE32(writer, 0);
    SDL_WriteLE32(writer, 0);
    SDL_RWwrite(writer, "ABC", 1, 3);
    SDL_WriteBE32(writer, 9);
    SDL_RWwrite(writer, commands, 1, sizeof(commands));
    SDL_WriteLE32(writer, 8);
    SDL_WriteLE32(writer, 52);
    SDL_WriteLE32(writer, 1);
    SDL_RWwrite(writer, "TEST", 1, 4);
    SDL_WriteLE32(writer, 2);
    SDL_WriteLE32(writer, 1);
    SDL_WriteLE32(writer, 28);
    SDL_WriteLE32(writer, 0x21000003);
    SDL_WriteLE32(writer, 0xdeadbeef);
    SDL_WriteLE32(writer, 2);
    SDL_WriteLE32(writer, 31);
    SDL_WriteLE32(writer, 0x3000000e);
    SDL_WriteLE32(writer, 0xfeedface);
    SDL_WriteLE32(writer, 0);
    size = (size_t)SDL_RWtell(writer);
    CHECK(size == 101);
    SDL_RWclose(writer);
    WriteFixture(path, bytes, size);
    CHECK(SwcCMOpen(path, &cmf) == 0 && cmf.chunkCount == 2);
    CHECK(cmf.chunks[0].size == 3);
    CHECK(SwcCMGetChunk(&cmf, "TEST", 1, &chunk) == 0);
    CHECK(chunk.size == 3 && memcmp(chunk.data, "ABC", 3) == 0);
    SDL_free(chunk.data);
    CHECK(SwcCMGetChunk(&cmf, "TEST", 2, &chunk) == 0);
    CHECK(chunk.size == 9 && memcmp(chunk.data, expected, 9) == 0);
    SDL_free(chunk.data);
    CHECK(SwcCMGetChunk(&cmf, "TEST", 99, &chunk) != 0 && chunk.data == NULL);
    SwcCMClose(&cmf);
    for (prefix = 0; prefix < size; prefix++) {
        WriteFixture(path, bytes, prefix);
        CHECK(SwcCMOpen(path, &cmf) != 0);
        CHECK(cmf.data == NULL && cmf.chunks == NULL && cmf.chunkCount == 0);
    }
    bytes[53] = 0xff;
    WriteFixture(path, bytes, size);
    CHECK(SwcCMOpen(path, &cmf) != 0);
    bytes[53] = 1;
    bytes[69] = 0xff;
    WriteFixture(path, bytes, size);
    CHECK(SwcCMOpen(path, &cmf) != 0);
    bytes[69] = 28;
    bytes[31] = 0xff;
    WriteFixture(path, bytes, size);
    CHECK(SwcCMOpen(path, &cmf) == 0);
    CHECK(SwcCMGetChunk(&cmf, "TEST", 2, &chunk) != 0 && chunk.data == NULL);
    SwcCMClose(&cmf);
}

static void TestPalette(const char *path)
{
    uint8_t bytes[114] = {0};
    SDL_RWops *writer;
    SDL_Color colors[256];
    size_t prefix;

    /* Data at 16, map at 64. Nonsequential ColorSpec indices distinguish
     * proper BE index handling from just reading colors into array order. */
    writer = SDL_RWFromMem(bytes, sizeof(bytes));
    CHECK(writer != NULL);
    SDL_WriteBE32(writer, 16);
    SDL_WriteBE32(writer, 64);
    SDL_WriteBE32(writer, 28);
    SDL_WriteBE32(writer, 50);
    SDL_WriteBE32(writer, 24);
    SDL_WriteBE32(writer, 0x12345678);
    SDL_WriteBE16(writer, 0);
    SDL_WriteBE16(writer, 1);
    SDL_WriteBE16(writer, 9);
    SDL_WriteBE16(writer, 0x1234);
    SDL_WriteBE16(writer, 0x5678);
    SDL_WriteBE16(writer, 0x9abc);
    SDL_WriteBE16(writer, 251);
    SDL_WriteBE16(writer, 0xffff);
    SDL_WriteBE16(writer, 0);
    SDL_WriteBE16(writer, 0x8080);
    SDL_RWseek(writer, 64 + 24, RW_SEEK_SET);
    SDL_WriteBE16(writer, 28);
    SDL_WriteBE16(writer, 50);
    SDL_WriteBE16(writer, 0);
    SDL_RWwrite(writer, "clut", 1, 4);
    SDL_WriteBE16(writer, 0);
    SDL_WriteBE16(writer, 10);
    SDL_WriteBE16(writer, 251);
    SDL_WriteBE16(writer, 0xffff);
    SDL_WriteBE32(writer, 0x50000000);
    SDL_WriteBE32(writer, 0);
    CHECK(SDL_RWtell(writer) == sizeof(bytes));
    SDL_RWclose(writer);
    WriteFixture(path, bytes, sizeof(bytes));
    CHECK(SwcReadPalette(path, 251, colors) == 0);
    CHECK(colors[9].r == 0x12 && colors[9].g == 0x56 && colors[9].b == 0x9a);
    CHECK(colors[251].r == 255 && colors[251].g == 0 && colors[251].b == 128);
    CHECK(colors[0].r == 0 && colors[0].a == 255);
    CHECK(SwcReadPalette(path, 252, colors) != 0);
    for (prefix = 0; prefix < sizeof(bytes); prefix++) {
        WriteFixture(path, bytes, prefix);
        CHECK(SwcReadPalette(path, 251, colors) != 0);
    }
    bytes[24] = 0x80;
    WriteFixture(path, bytes, sizeof(bytes));
    CHECK(SwcReadPalette(path, 251, colors) == 0);
    CHECK(colors[0].r == 0x12 && colors[1].r == 255);
    bytes[24] = 0;
    bytes[28] = 1;
    WriteFixture(path, bytes, sizeof(bytes));
    CHECK(SwcReadPalette(path, 251, colors) != 0);
}

int main(int argc, char **argv)
{
    SwcCmf cmf;
    SwcBuffer chunk;
    SwcFrame frame;
    uint8_t *pixels;
    uint32_t count;
    uint32_t index;
    size_t entry;
    size_t pixelCount;
    size_t chunks;
    size_t sets;
    size_t frames;
    size_t decodedBytes;
    int argument;

    CHECK(argc >= 2);
    TestRLE();
    TestFrames();
    TestCmf(argv[1]);
    TestPalette(argv[1]);
    CHECK(remove(argv[1]) == 0);
    puts("SWC codec, frame layout, CMF, resource fork and malformed-input checks passed.");
    chunks = 0;
    sets = 0;
    frames = 0;
    decodedBytes = 0;
    for (argument = 2; argument < argc; argument++) {
        CHECK(SwcCMOpen(argv[argument], &cmf) == 0);
        for (entry = 0; entry < cmf.chunkCount; entry++) {
            CHECK(SwcCMGetChunk(&cmf, cmf.chunks[entry].type,
                                 cmf.chunks[entry].id, &chunk) == 0);
            chunks++;
            if ((cmf.chunks[entry].flags & SWC_CMF_COMPRESSED) != 0) {
                /* Every compressed chunk in this particular demo is a
                 * sprite set; this is a corpus check, not a format rule. */
                CHECK(SwcGetFrameCount(&chunk, &count) == 0);
                sets++;
                decodedBytes += chunk.size;
                for (index = 0; index < count; index++) {
                    CHECK(SwcGetFramePtr(&chunk, index, &frame) == 0);
                    pixelCount = (size_t)frame.width * frame.height;
                    pixels = SDL_malloc(pixelCount != 0 ? pixelCount : 1);
                    CHECK(pixels != NULL);
                    CHECK(SwcDecodeFrame(&frame, pixels, pixelCount) == 0);
                    SDL_free(pixels);
                    frames++;
                }
            }
            SDL_free(chunk.data);
        }
        SwcCMClose(&cmf);
    }
    if (argc > 2)
        printf("SWC corpus: %d CMFs, %zu chunks, %zu sprite sets, %zu frames, %zu decoded bytes: OK\n",
               argc - 2, chunks, sets, frames, decodedBytes);
    return 0;
}
