#include "wc1sdl.h"

#include <string.h>

#ifndef _WIN32

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <strings.h>
#include <sys/stat.h>

static int Wc1SdlAppendPathComponent(char *path, size_t pathSize,
                                     const char *component)
{
    size_t componentLength;
    size_t pathLength;
    int needsSeparator;

    pathLength = strlen(path);
    componentLength = strlen(component);
    needsSeparator = pathLength != 1 || path[0] != '/';
    if (pathLength + (size_t)needsSeparator + componentLength + 1 > pathSize)
        return 0;
    if (needsSeparator)
        path[pathLength++] = '/';
    memcpy(path + pathLength, component, componentLength + 1);
    return 1;
}

static int Wc1SdlAppendResolvedComponent(char *path, size_t pathSize,
                                         const char *component)
{
    DIR *directory;
    struct dirent *entry;
    int result;

    if (strcmp(component, ".") == 0 || strcmp(component, "..") == 0)
        return Wc1SdlAppendPathComponent(path, pathSize, component);
    directory = opendir(path);
    if (directory == 0)
        return Wc1SdlAppendPathComponent(path, pathSize, component);
    while ((entry = readdir(directory)) != 0) {
        if (strcasecmp(entry->d_name, component) == 0) {
            result = Wc1SdlAppendPathComponent(path, pathSize,
                                               entry->d_name);
            closedir(directory);
            return result;
        }
    }
    closedir(directory);
    return Wc1SdlAppendPathComponent(path, pathSize, component);
}

int Wc1SdlResolvePath(const char *path, char *resolved,
                      unsigned long resolvedSize)
{
    char component[PATH_MAX];
    char normalized[PATH_MAX];
    char prefix[PATH_MAX];
    const char *cursor;
    char *separator;
    size_t pathLength;

    if (path == 0 || resolved == 0 || resolvedSize == 0)
        return 0;
    pathLength = strlen(path);
    if (pathLength >= sizeof(normalized))
        return 0;
    memcpy(normalized, path, pathLength + 1);
    separator = normalized;
    while (*separator != '\0') {
        if (*separator == '\\')
            *separator = '/';
        separator++;
    }
    if (normalized[0] == '/')
        strcpy(prefix, "/");
    else
        strcpy(prefix, ".");
    cursor = normalized;
    while (*cursor == '/')
        cursor++;
    while (*cursor != '\0') {
        const char *end;
        size_t componentLength;

        end = cursor;
        while (*end != '\0' && *end != '/')
            end++;
        componentLength = (size_t)(end - cursor);
        if (componentLength >= sizeof(component))
            return 0;
        memcpy(component, cursor, componentLength);
        component[componentLength] = '\0';
        if (!Wc1SdlAppendResolvedComponent(prefix, sizeof(prefix), component))
            return 0;
        cursor = end;
        while (*cursor == '/')
            cursor++;
    }
    pathLength = strlen(prefix);
    if (pathLength + 1 > resolvedSize)
        return 0;
    memcpy(resolved, prefix, pathLength + 1);
    return 1;
}

int Wc1SdlChangeDirectory(const char *path)
{
    char resolved[PATH_MAX];

    if (!Wc1SdlResolvePath(path, resolved, sizeof(resolved)))
        return -1;
    return chdir(resolved);
}

int Wc1SdlOpen(const char *path, int flags, ...)
{
    int hostFlags;
    int hostMode;
    int file;
    char hostPath[PATH_MAX];

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

    if (!Wc1SdlResolvePath(path, hostPath, sizeof(hostPath)))
        return -1;
    file = open(hostPath, hostFlags, hostMode);
    if (file == -1 && (hostFlags & O_ACCMODE) == O_RDWR &&
        (hostFlags & O_CREAT) == 0) {
        hostFlags = (hostFlags & ~O_ACCMODE) | O_RDONLY;
        file = open(hostPath, hostFlags, hostMode);
    }
    return file;
}

long Wc1SdlFileLength(int file)
{
    struct stat status;

    if (fstat(file, &status) != 0)
        return -1;
    return (long)status.st_size;
}

static char *Wc1SdlLowercaseDigits(char *text)
{
    char *cursor;

    cursor = text;
    while (*cursor != '\0') {
        *cursor = (char)tolower((unsigned char)*cursor);
        cursor++;
    }
    return text;
}

char *Wc1SdlItoa(int value, char *text, int radix)
{
    return Wc1SdlLtoa((long)value, text, radix);
}

char *Wc1SdlLtoa(long value, char *text, int radix)
{
    if (radix == 10)
        return SDL_ltoa(value, text, radix);
    else if (radix == 16)
        return Wc1SdlLowercaseDigits(
            SDL_ultoa((unsigned long)value, text, radix));
    text[0] = '\0';
    return text;
}

char *Wc1SdlUltoa(unsigned long value, char *text, int radix)
{
    if (radix == 10)
        return SDL_ultoa(value, text, radix);
    else if (radix == 16)
        return Wc1SdlLowercaseDigits(SDL_ultoa(value, text, radix));
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

#else

int Wc1SdlChangeDirectory(const char *path)
{
    return _chdir(path);
}

int Wc1SdlResolvePath(const char *path, char *resolved,
                      unsigned long resolvedSize)
{
    size_t pathLength;

    if (path == 0 || resolved == 0 || resolvedSize == 0)
        return 0;
    pathLength = strlen(path);
    if (pathLength + 1 > resolvedSize)
        return 0;
    memcpy(resolved, path, pathLength + 1);
    return 1;
}

#endif
