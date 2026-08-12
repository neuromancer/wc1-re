#include "wc1sdl.h"

#ifndef _WIN32

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

int Wc1SdlResolvePath(const char *path, char *resolved,
                      unsigned long resolvedSize)
{
    char component[PATH_MAX];
    char normalized[PATH_MAX];
    char prefix[PATH_MAX];
    const char *cursor;
    unsigned long pathLength;

    if (path == 0 || resolved == 0 || resolvedSize == 0)
        return 0;
    pathLength = strlen(path);
    if (pathLength >= sizeof(normalized))
        return 0;
    memcpy(normalized, path, pathLength + 1);
    for (char *separator = normalized; *separator != '\0'; separator++) {
        if (*separator == '\\')
            *separator = '/';
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
        const char *selected;
        DIR *directory;
        struct dirent *entry;
        unsigned long componentLength;
        unsigned long prefixLength;

        end = cursor;
        while (*end != '\0' && *end != '/')
            end++;
        componentLength = (unsigned long)(end - cursor);
        if (componentLength >= sizeof(component))
            return 0;
        memcpy(component, cursor, componentLength);
        component[componentLength] = '\0';
        selected = component;
        if (strcmp(component, ".") != 0 && strcmp(component, "..") != 0) {
            directory = opendir(prefix);
            if (directory != 0) {
                while ((entry = readdir(directory)) != 0) {
                    if (strcasecmp(entry->d_name, component) == 0) {
                        selected = entry->d_name;
                        break;
                    }
                }
                prefixLength = strlen(prefix);
                if (prefixLength + strlen(selected) + 2 > sizeof(prefix)) {
                    closedir(directory);
                    return 0;
                }
                if (prefixLength != 1 || prefix[0] != '/')
                    strcat(prefix, "/");
                strcat(prefix, selected);
                closedir(directory);
            } else {
                prefixLength = strlen(prefix);
                if (prefixLength + componentLength + 2 > sizeof(prefix))
                    return 0;
                if (prefixLength != 1 || prefix[0] != '/')
                    strcat(prefix, "/");
                strcat(prefix, component);
            }
        } else {
            prefixLength = strlen(prefix);
            if (prefixLength + componentLength + 2 > sizeof(prefix))
                return 0;
            if (prefixLength != 1 || prefix[0] != '/')
                strcat(prefix, "/");
            strcat(prefix, component);
        }
        cursor = end;
        while (*cursor == '/')
            cursor++;
    }
    if (strlen(prefix) + 1 > resolvedSize)
        return 0;
    strcpy(resolved, prefix);
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
