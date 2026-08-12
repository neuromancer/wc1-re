#include "wc1.h"

#include <stdio.h>

_Static_assert(sizeof(BYTE) == 1, "BYTE must retain its Win32 width");
_Static_assert(sizeof(WORD) == 2, "WORD must retain its Win32 width");
_Static_assert(sizeof(DWORD) == 4, "DWORD must retain its Win32 width");
_Static_assert(sizeof(LONG) == 4, "LONG must retain its Win32 width");
_Static_assert(sizeof(GUID) == 16, "GUID must retain its Win32 layout");
_Static_assert(sizeof(LARGE_INTEGER) == 8,
               "LARGE_INTEGER must retain its Win32 layout");
_Static_assert(sizeof(JOYINFO) == 16,
               "JOYINFO must retain its Win32 layout");

int main(void)
{
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    if (linked.major != compiled.major) {
        fprintf(stderr, "SDL major-version mismatch: built for %u, found %u\n",
                compiled.major, linked.major);
        return 1;
    }
    if (SDL_Init(0) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Quit();
    return 0;
}
