#include "ix.h"

static_assert(sizeof(DWORD) == 4, "ix requires a 32-bit DWORD");
static_assert(sizeof(CRITICAL_SECTION) == sizeof(void *),
              "the SDL lock wrapper must contain one native handle");

int main(int argumentCount, char **arguments)
{
    CRITICAL_SECTION section = { 0 };

    return section.mutex != 0;
}
