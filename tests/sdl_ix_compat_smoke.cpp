#include "ix.h"

static_assert(sizeof(DWORD) == 4, "ix requires a 32-bit DWORD");
static_assert(sizeof(CRITICAL_SECTION) == sizeof(void *),
              "the SDL lock wrapper must contain one native handle");

int main(int argumentCount, char **arguments)
{
    CRITICAL_SECTION section = { 0 };
    int result;

    (void)argumentCount;
    (void)arguments;
    if (section.mutex != 0)
        return 1;
    if (SDL_Init(0) != 0)
        return 1;

    g_nVoiceCount_00598600 = 32;
    g_nStreamCount_00598130 = 2;
    ix_dsps_alloc(0, 262144U, 22050, 16, 2);
    result = g_streams_00598138[0].buffer == 0 ||
             (g_streams_00598138[0].flags & IX_STREAM_ALLOCATED) == 0 ||
             g_nStreamsAllocated_00598134 != 1;
    ix_dsps_free(0);
    if ((g_streams_00598138[0].flags & IX_STREAM_ALLOCATED) != 0 ||
        g_nStreamsAllocated_00598134 != 0)
        result = 1;

    SDL_Quit();
    return result;
}
