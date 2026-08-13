#include "wc1sdl.h"

#include <string.h>
#include <sys/stat.h>

int main(int argumentCount, char **arguments)
{
    static const char payload[] = "Wing Commander";
    const char *casePath;
    const char *path;
    char resolvedPath[PATH_MAX];
    char text[32];
    FILE *stream;
    int file;
    int failed;
    int pathResolved;

    failed = 0;
    if (strcmp(_itoa(-42, text, 10), "-42") != 0)
        failed = 1;
    if (strcmp(_ultoa(0x1a2b, text, 16), "1a2b") != 0)
        failed = 1;
    if (strcmp(_strupr(text), "1A2B") != 0)
        failed = 1;

    path = "wc1-sdl-crt-smoke.tmp";
    file = _open(path, 0x8301, 0x0180);
    if (file == -1)
        return 1;
    if (_write(file, payload, sizeof(payload)) != sizeof(payload))
        failed = 1;
    if (_filelength(file) != sizeof(payload))
        failed = 1;
    if (_close(file) != 0)
        failed = 1;
    if (chmod(path, S_IRUSR) != 0)
        failed = 1;
    file = _open(path, 0x8002);
    if (file == -1)
        failed = 1;
    else if (_close(file) != 0)
        failed = 1;
    if (_unlink(path) != 0)
        failed = 1;

    casePath = "WC1-SDL-Case-Smoke.TMP";
    file = _open(casePath, 0x8301, 0x0180);
    if (file == -1)
        return 1;
    if (_close(file) != 0)
        failed = 1;
    pathResolved = Wc1SdlResolvePath("wc1-sdl-case-smoke.tmp",
                                     resolvedPath, sizeof(resolvedPath));
    if (!pathResolved)
        failed = 1;
#ifndef _WIN32
    else if (strcmp(resolvedPath, "./WC1-SDL-Case-Smoke.TMP") != 0)
        failed = 1;
#endif
    if (pathResolved) {
        stream = fopen(resolvedPath, "rb");
        if (stream == 0)
            failed = 1;
        else if (fclose(stream) != 0)
            failed = 1;
    }
    file = _open("wc1-sdl-case-smoke.tmp", 0x8000);
    if (file == -1)
        failed = 1;
    else if (_close(file) != 0)
        failed = 1;
    if (_unlink(casePath) != 0)
        failed = 1;
    return failed;
}
