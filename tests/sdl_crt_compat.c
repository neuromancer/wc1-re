#include "wc1sdl.h"

#include <string.h>
#include <sys/stat.h>

int main(int argumentCount, char **arguments)
{
    static const char payload[] = "Wing Commander";
    const char *path;
    char text[32];
    int file;
    int failed;

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
    return failed;
}
