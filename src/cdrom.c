/*
 *  CD-ROM location and disc-swap prompting.
 *
 *  Address range 0x403100-0x4034ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: LocateStreamsDirOnDisc..PromptInsertCorrectCd; string band 0x46535C-0x4653FC.
 */
#include "wc1.h"

/* Function start: 0x4034D0 */
short OpenDataFileOrDie(const char *path)
{
    int fd = _open(path, 0x8002);

    DAT_00465460 = (short)errno;
    return (short)fd;
}
