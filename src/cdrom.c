/*
 *  CD-ROM location and disc-swap prompting.
 *
 *  Address range 0x403100-0x4034ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: LocateStreamsDirOnDisc..PromptInsertCorrectCd; string band 0x46535C-0x4653FC.
 */
#include "wc1.h"

/* Function start: WC2_UNMAPPED */
FontWorkspace **AllocateFontWorkspace(short fontIndex)
{
    FontWorkspace **workspace;
    int offset;
    int dimension;

    (void)fontIndex;
    workspace = (FontWorkspace **)malloc(sizeof(*workspace));
    offset = 0;
    dimension = 5;
    *workspace = malloc(sizeof(**workspace));
    do {
        offset += sizeof(FontWorkspace);
        ((FontWorkspace *)((unsigned char *)*workspace + offset -
                           sizeof(FontWorkspace)))->width = dimension;
        ((FontWorkspace *)((unsigned char *)*workspace + offset -
                           sizeof(FontWorkspace)))->height = dimension;
        ((FontWorkspace *)((unsigned char *)*workspace + offset -
                           sizeof(FontWorkspace)))->pixels =
            malloc((*workspace)->width *
                                    (*workspace)->height);
        memset(((FontWorkspace *)((unsigned char *)*workspace + offset -
                                  sizeof(FontWorkspace)))->pixels,
               dimension, (*workspace)->width * (*workspace)->height);
    } while (offset < (int)sizeof(FontWorkspace));
    return workspace;
}

/* Function start: WC2_UNMAPPED */
void FreeFontWorkspace(FontWorkspace **workspace)
{
    int offset;

    offset = 0;
    do {
        free(((FontWorkspace *)((unsigned char *)*workspace + offset))->pixels);
        offset += sizeof(FontWorkspace);
    } while (offset < (int)sizeof(FontWorkspace));
    free(*workspace);
    free(workspace);
}

/* Function start: 0x456123 */
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

#ifdef WC1_SDL
    if (strstr(current, "gamedat") != 0 ||
        strstr(current, "GAMEDAT") != 0)
#else
    if (strstr(current, "gamedat") != 0)
#endif
        sprintf(result, "%s\\..\\%s", current, "streams\\");
    else
        sprintf(result, "%s\\%s", current, "streams\\");
    GetCurrentDirectoryA(0xff, current);
    if (!SetCurrentDirectoryA(result))
        result = 0;
    SetCurrentDirectoryA(current);
    return result;
}

/* Function start: 0x456236 */
char FindCdRomDriveByVolumeLabel(const char *label,
                                 const char *directory)
{
    char drives[26];
    char scanRoot[12];
    char root[12];
    char volume[256];
    char filesystem[64];
    DWORD maximumComponentLength;
    DWORD flags;
    int driveCount = 0;
    int i;
    int scanDrive;
    char drive;
    char result;

    for (scanDrive = 'a'; scanDrive <= 'z'; scanDrive++) {
        sprintf(scanRoot, "%c:\\", (char)scanDrive);
        if (GetDriveTypeA(scanRoot) == DRIVE_CDROM) {
            drives[driveCount] = (char)scanDrive;
            driveCount++;
        }
    }

    result = 0;
    i = 0;
    while (i < driveCount) {
        drive = drives[i];
        sprintf(root, "%c:\\", drive);
        GetVolumeInformationA(root, volume, 0xff, 0,
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

/* Function start: 0x4563A3 */
int SetCurrentDirOnDrive(char drive, const char *directory)
{
    char path[256];
    char current[256];
    int result;

    result = 0;
    GetCurrentDirectoryA(0xff, current);
    sprintf(path, "%c:%s", drive, directory);
    if (SetCurrentDirectoryA(path) != 0)
        result = 1;
    SetCurrentDirectoryA(current);
    return result;
}

/* Function start: 0x456443 */
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

/* Function start: 0x45A010 */
short __stdcall OpenDataFileOrDie(const char *path)
{
    short fd = (short)_open(path, 0x8002);

    g_nPacketError_0049ca90 = (short)errno;
    return fd;
}
