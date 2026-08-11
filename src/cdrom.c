/*
 *  CD-ROM location and disc-swap prompting.
 *
 *  Address range 0x403100-0x4034ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: LocateStreamsDirOnDisc..PromptInsertCorrectCd; string band 0x46535C-0x4653FC.
 */
#include "wc1.h"

/* Function start: 0x403100 */
FontWorkspace **AllocateFontWorkspace(short fontIndex)
{
    FontWorkspace **workspace;
    FontWorkspace *entry;
    int offset;
    int dimension;

    (void)fontIndex;
    workspace = (FontWorkspace **)malloc(sizeof(*workspace));
    *workspace = (FontWorkspace *)malloc(sizeof(**workspace));
    offset = 0;
    dimension = 5;
    do {
        entry = (FontWorkspace *)((unsigned char *)*workspace + offset);
        offset += sizeof(FontWorkspace);
        entry->width = dimension;
        entry->height = dimension;
        entry->pixels = (unsigned char *)malloc(
            entry->width * entry->height);
        memset(entry->pixels, dimension, entry->width * entry->height);
    } while (offset < (int)sizeof(FontWorkspace));
    return workspace;
}

/* Function start: 0x403180 */
void FreeFontWorkspace(FontWorkspace **workspace)
{
    FontWorkspace *entry;
    int offset;

    offset = 0;
    do {
        entry = (FontWorkspace *)((unsigned char *)*workspace + offset);
        offset += sizeof(FontWorkspace);
        free(entry->pixels);
    } while (offset < (int)sizeof(FontWorkspace));
    free(*workspace);
    free(workspace);
}

/* Function start: 0x4031C0 */
char *LocateStreamsDirOnDisc(void)
{
    char current[256];
    char *result;
    char drive;

    result = g_szStreamsPath_00475c18;
    GetCurrentDirectoryA(0xff, current);
    drive = FindCdRomDriveByVolumeLabel("<anydisc>", "\\wc1\\streams\\");
    if (drive != 0) {
        sprintf(result, "%c:%s", drive,
                "\\wc1\\streams\\");
        return result;
    }

    if (strstr(current, "gamedat") != 0)
        sprintf(result, "%s\\..\\%s", current, "streams\\");
    else
        sprintf(result, "%s\\%s", current, "streams\\");
    GetCurrentDirectoryA(0xff, current);
    if (!SetCurrentDirectoryA(result))
        result = 0;
    SetCurrentDirectoryA(current);
    return result;
}

/* Function start: 0x403290 */
char FindCdRomDriveByVolumeLabel(const char *label,
                                 const char *directory)
{
    char drives[26];
    char root[12];
    char volume[256];
    char filesystem[64];
    DWORD serial;
    DWORD maximumComponentLength;
    DWORD flags;
    int driveCount = 0;
    int i;
    int scanDrive;
    char drive;
    char result;

    for (scanDrive = 'a'; scanDrive <= 'z'; scanDrive++) {
        sprintf(root, "%c:\\", scanDrive);
        if (GetDriveTypeA(root) == DRIVE_CDROM)
            drives[driveCount++] = (char)scanDrive;
    }

    result = 0;
    i = 0;
    while (i < driveCount) {
        drive = drives[i];
        sprintf(root, "%c:\\", drive);
        volume[0] = '\0';
        GetVolumeInformationA(root, volume, 0xff, &serial,
                              &maximumComponentLength, &flags,
                              filesystem, sizeof(filesystem));
        if (memcmp(label, "<anydisc>", 10) == 0) {
            if (SetCurrentDirOnDrive(drive, directory) != 0) {
                result = drive;
                break;
            }
        } else if (strcmp(volume, label) == 0) {
            if (SetCurrentDirOnDrive(drive, directory) != 0) {
                result = drives[i];
                break;
            }
        }
        i++;
    }
    return result;
}

/* Function start: 0x4033E0 */
int SetCurrentDirOnDrive(char drive, const char *directory)
{
    char path[256];
    char current[256];
    int result;

    GetCurrentDirectoryA(0xff, current);
    sprintf(path, "%c:%s", drive, directory);
    result = SetCurrentDirectoryA(path) != 0;
    SetCurrentDirectoryA(current);
    return result;
}

/* Function start: 0x403450 */
int PromptInsertCorrectCd(void)
{
    char title[18];
    char message[1024];

    strcpy(title, "Insert Correct CD");
    sprintf(message,
            "Please place The Kilrathi Saga disc %d into your CD-ROM drive and click OK\n"
            "or click cancel to quit",
            1);
    do {
        if (LocateStreamsDirOnDisc() != 0)
            return 1;
    } while (MessageBoxA(0, message, title,
                         MB_OKCANCEL | MB_ICONEXCLAMATION) != IDCANCEL);
    return 0;
}

/* Function start: 0x4034D0 */
short __stdcall OpenDataFileOrDie(const char *path)
{
    int fd = _open(path, 0x8002);

    g_nPacketError_00465460 = (short)errno;
    return (short)fd;
}
