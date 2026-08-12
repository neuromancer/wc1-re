#include "wc1sdl.h"

#ifndef _WIN32

#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>

int Wc1SdlOpen(const char *path, int flags, ...)
{
    int hostFlags;
    int hostMode;
    int file;
    char *hostPath;
    char *cursor;

    hostFlags = flags & 3;
    if ((flags & 0x0008) != 0)
        hostFlags |= O_APPEND;
    if ((flags & 0x0100) != 0)
        hostFlags |= O_CREAT;
    if ((flags & 0x0200) != 0)
        hostFlags |= O_TRUNC;
    if ((flags & 0x0400) != 0)
        hostFlags |= O_EXCL;

    hostMode = S_IRUSR | S_IWUSR;
    if ((flags & 0x0100) != 0) {
        va_list arguments;
        int dosMode;

        va_start(arguments, flags);
        dosMode = va_arg(arguments, int);
        va_end(arguments);
        hostMode = 0;
        if ((dosMode & 0x0100) != 0)
            hostMode |= S_IRUSR;
        if ((dosMode & 0x0080) != 0)
            hostMode |= S_IWUSR;
    }

    hostPath = SDL_strdup(path);
    if (hostPath == 0)
        return -1;
    cursor = hostPath;
    while (*cursor != '\0') {
        if (*cursor == '\\')
            *cursor = '/';
        cursor++;
    }
    file = open(hostPath, hostFlags, hostMode);
    SDL_free(hostPath);
    return file;
}

long Wc1SdlFileLength(int file)
{
    struct stat status;

    if (fstat(file, &status) != 0)
        return -1;
    return (long)status.st_size;
}

char *Wc1SdlItoa(int value, char *text, int radix)
{
    return Wc1SdlLtoa((long)value, text, radix);
}

char *Wc1SdlLtoa(long value, char *text, int radix)
{
    if (radix == 10)
        sprintf(text, "%ld", value);
    else if (radix == 16)
        sprintf(text, "%lx", (unsigned long)value);
    else
        text[0] = '\0';
    return text;
}

char *Wc1SdlUltoa(unsigned long value, char *text, int radix)
{
    if (radix == 10)
        sprintf(text, "%lu", value);
    else if (radix == 16)
        sprintf(text, "%lx", value);
    else
        text[0] = '\0';
    return text;
}

char *Wc1SdlStrupr(char *text)
{
    char *cursor;

    cursor = text;
    while (*cursor != '\0') {
        *cursor = (char)toupper((unsigned char)*cursor);
        cursor++;
    }
    return text;
}

#endif
