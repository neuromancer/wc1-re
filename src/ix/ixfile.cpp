/**
 * @file
 * @ingroup ixlib
 * @brief stdio wrappers used by the streamer and its thread.
 *
 * Address range 461630-4617bf. A separate object from the LZO decoder before
 * it: the linker pads to its 16-byte boundary with INT3 between the two, in
 * WC1 and WC2 alike. Like the decoder it came from a library of its own,
 * pulled only on LINK's second search pass.
 */
#include "ix.h"
#include <stdlib.h>
#include <string.h>
#ifdef SDL_PORT
#include <limits.h>
#endif

#pragma function(strcat)

/* Function start: 0x00461630 */
/* Unreferenced in WC1 and WC2 alike; the file module's empty init and
 * shutdown entry points, by analogy with the other ix modules. */
extern "C" void ix_file_init_hook(void)
{
}

/* Function start: 0x00461640 */
extern "C" void ix_file_shutdown_hook(void)
{
}

/* Function start: 0x00461650 */
extern "C" FILE *ix_file_open(const char *path, int mode)
{
    char modeString[4];
#ifdef SDL_PORT
    char resolvedPath[PATH_MAX];
#endif

    modeString[0] = 0;
    if ((mode & 1) != 0)
        strcat(modeString, "r");
    if ((mode & 2) != 0)
        strcat(modeString, "w");
    strcat(modeString, "b");
#ifdef SDL_PORT
    if (!SdlResolvePath(path, resolvedPath, sizeof(resolvedPath)))
        return 0;
    return fopen(resolvedPath, modeString);
#else
    return fopen(path, modeString);
#endif
}

/* Function start: 0x004616BE */
extern "C" void ix_file_close(FILE *file)
{
    fclose(file);
}

/* Function start: 0x004616DA */
extern "C" void ix_file_seek(FILE *file, long position)
{
    fseek(file, position, SEEK_SET);
}

/* Function start: 0x004616FC */
extern "C" long ix_file_tell(FILE *file)
{
    return ftell(file);
}

/* Function start: 0x00461718 */
extern "C" unsigned int ix_file_read(FILE *file, void *destination,
                                      unsigned int bytes)
{
    return fread(destination, 1, bytes, file);
}

/* Function start: 0x0046173E */
extern "C" unsigned int ix_file_write(FILE *file, const void *source,
                                       unsigned int bytes)
{
    return fwrite(source, 1, bytes, file);
}

/* Function start: 0x00461764 */
extern "C" long ix_file_size(FILE *file)
{
    long position;
    long size;

    position = ftell(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, position, SEEK_SET);
    return size;
}
