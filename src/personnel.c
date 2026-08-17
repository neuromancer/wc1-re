/*
 *  Wing Commander II personnel database and WC1 pilot transfer flow.
 *
 *  The WC2 retail image keeps the menu, transfer, and savegame helpers in the
 *  contiguous 0x433AD0-0x436A8F band.  The registry lookup and text-entry
 *  adapter are linked from adjacent Win32 units.
 */
#include "wc1.h"

#pragma function(strcat, strcpy)

/* Function start: 0x428C35 */
char *GetWingCommanderOneGameDataPath(void)
{
#ifdef WC1_SDL
    return 0;
#else
    HKEY key;
    DWORD valueData;
    DWORD valueType;
    DWORD valueSize;
    char *path;

    path = 0;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        valueType = REG_DWORD;
        valueSize = sizeof(valueData);
        if (RegQueryValueExA(key, "Wing 1 Installed", 0, &valueType,
                             (unsigned char *)&valueData,
                             &valueSize) == ERROR_SUCCESS &&
            valueData != 0) {
            valueType = REG_SZ;
            valueSize = sizeof(g_szWingCommanderOneGameDataPath_005b2898);
            if (RegQueryValueExA(
                    key, "Installed To:", 0, &valueType,
                    (unsigned char *)
                        g_szWingCommanderOneGameDataPath_005b2898,
                    &valueSize) == ERROR_SUCCESS) {
                path = g_szWingCommanderOneGameDataPath_005b2898;
                strcat(g_szWingCommanderOneGameDataPath_005b2898,
                       "wc1\\gamedat");
            }
        }
        RegCloseKey(key);
    }
    return path;
#endif
}

/* Function start: 0x433AD0 */
void PollPersonnelMenuInput(void)
{
    InputEventState event;
    short eventType;

    g_cPersonnelPreviousKey_0049312c =
        (signed char)g_sPersonnelPreviousInput_0049a6bc;
    ServiceInputDevices(15);
    g_bPersonnelPrimaryInputDown_0049a6d4 =
        FindQueuedInputEvent(3) != 0;
    if (g_bPersonnelPrimaryInputDown_0049a6d4 == 0) {
        g_cPersonnelMenuKey_00493128 |= (signed char)0x80;
        g_sPersonnelPreviousInput_0049a6bc |= 0x80;
    }
    if (FindQueuedInputEvent(5) != 0) {
        g_cPersonnelMenuKey_00493128 |= (signed char)0x80;
        g_sPersonnelPreviousInput_0049a6bc |= 0x80;
        g_bPersonnelPrimaryInputDown_0049a6d4 = 0;
    }
    while ((eventType = GetNextInputEvent(&event)) != 0) {
        switch (eventType) {
        case 1:
            g_cPersonnelMenuKey_00493128 = 0x1c;
            g_sPersonnelPreviousInput_0049a6bc = 0x1c;
            if ((event.modifiers & 1) != 0) {
                g_cPersonnelMenuKey_00493128 = 0x1c;
                g_sPersonnelPreviousInput_0049a6bc = 0x1c;
            }
            if ((event.modifiers & 2) != 0) {
                g_cPersonnelMenuKey_00493128 = 0x1c;
                g_sPersonnelPreviousInput_0049a6bc = 0x1c;
            }
            break;
        case 3:
            g_nPersonnelCursorX_005c8470 = event.x;
            g_nPersonnelCursorY_005c8472 = event.y;
            break;
        case 4:
        case 6:
            g_cPersonnelMenuKey_00493128 = (signed char)event.status;
            g_sPersonnelPreviousInput_0049a6bc =
                (short)(signed char)event.status;
            break;
        }
    }
    if ((int)g_cPersonnelPreviousKey_0049312c ==
        (int)g_sPersonnelPreviousInput_0049a6bc) {
        g_cPersonnelMenuKey_00493128 = (signed char)0x80;
    }
}

/* Function start: 0x433F94 */
void DrawPersonnelMenuBackdrop(unsigned char *shape)
{
    short width;

    SetMenuInputPump();
    DisableMouseCursorDrawing();
    DrawSpriteDefault(&g_stSecondaryViewBuffer_005d2c90, 0, 0,
                      shape, 0);
    InitializeTextContextFromFont(&g_stDefaultTextContext_005d2d20, 0,
                                  g_cPersonnelTextColour_0049cb50, -1);
    if (g_pszPersonnelFooter_00492658 != 0) {
        width = MeasureTextPixelWidthClamped(
            g_pszPersonnelFooter_00492658);
        SetTextCursor((unsigned short)((320 - width) >> 1), 180);
        DrawFormattedText("%S", g_pszPersonnelFooter_00492658);
    }
}

/* Function start: 0x4342E8 */
void OpenDiskDataFile(short logicalFile)
{
    unsigned int campaignBytes;
    struct _finddata_t findData;
    short emptySlot;
    short findHandle;
    short occupiedSlot;
    short slot;
    short file;

    occupiedSlot = 1;
    emptySlot = 0;
    if (logicalFile != 0) {
        LoadTemporaryCampaignGlobals();
    }
    campaignBytes =
        (unsigned int)g_pCampaignGlobals_00499c94->wordCount * 2;
    findHandle = (short)_findfirst("savegame.wc2", &findData);
    if (findHandle == -1 ||
        campaignBytes * 9 + 0x4b6 != findData.size) {
        file = CreateDataFile("savegame.wc2");
        if (file < 0) {
            ReportFatalErrorCode("005");
        }
        for (slot = 0; slot < 9; slot++) {
            WriteDataFileAtOffset(file, -1, 2, &emptySlot);
            WriteDataFileAtOffset(file, -1, 0x60,
                                  &g_stDefaultPilotProfile_00492660);
            WriteDataFileAtOffset(
                file, -1, 0x24,
                "Wing Commander II (c) 1991 Origin");
            WriteDataFileAtOffset(file, -1, campaignBytes,
                                  g_pCampaignGlobals_00499c94);
        }
        CloseDataFile(file);
    }
    _findclose((long)findHandle);
    file = OpenDataFileOrDie("savegame.wc2");
    if (file < 0) {
        ReportFatalErrorCode("006");
    }
    strcpy(g_stCurrentPilotProfile_00493408.firstName,
           g_szPilotFirstName_00499f28);
    strcpy(g_stCurrentPilotProfile_00493408.lastName,
           g_szPilotLastName_00499f10);
    strcpy(g_stCurrentPilotProfile_00493408.callsign,
           g_szPilotCallsign_00499ef8);
    SeekDataFile(file, campaignBytes * 8 + 0x430, 0);
    WriteDataFileAtOffset(file, -1, 2, &occupiedSlot);
    WriteDataFileAtOffset(file, -1, 0x60,
                          &g_stCurrentPilotProfile_00493408);
    WriteDataFileAtOffset(file, -1, 0x24,
                          "Wing Commander II (c) 1991 Origin");
    WriteDataFileAtOffset(file, -1, campaignBytes,
                          g_pCampaignGlobals_00499c94);
    CloseDataFile(file);
    if (logicalFile != 0) {
        ReleasePacketSlot((void **)&g_pCampaignGlobals_00499c94);
    }
}

/* Function start: 0x43456E */
void RestorePersonnelMenuBackground(Viewport *viewport)
{
    Viewport savedViewport;

    savedViewport = *viewport;
    DrawSpriteDefault(&savedViewport, 0, 0,
                      g_pPersonnelMenuBackdrop_0049a6b8, 1);
}

/* Function start: 0x4346E9 */
void DrawPersonnelSourceChoices(short choice)
{
    if (choice != g_nPersonnelMenuHighlight_0049a6a0) {
        DisableMouseCursorDrawing();
        if (choice == 0x11) {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuHighlightColour_005c5874, -1);
        } else {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuShadowColour_005c5876, -1);
        }
        SetTextCursor(0x50, 0x2e);
        DrawFormattedText("[W]ing Commander");
        if (choice == 0x20) {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuHighlightColour_005c5874, -1);
        } else {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuShadowColour_005c5876, -1);
        }
        SetTextCursor(0x50, 0x42);
        DrawFormattedText("[D]os Version of Wing Commander");
        g_nPersonnelMenuHighlight_0049a6a0 = choice;
        EnableMouseCursorDrawing();
    }
}

/* Function start: 0x4347D6 */
void DrawPersonnelMenuChoices(short choice)
{
    if (g_nPersonnelMenuHighlight_0049a6a0 != choice) {
        DisableMouseCursorDrawing();
        if (choice == 0x14) {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuHighlightColour_005c5874, -1);
        } else {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuShadowColour_005c5876, -1);
        }
        SetTextCursor(0x3c, 0x38);
        DrawFormattedText("[T]ransfer an existing personnel file");
        if (choice == 0x2e) {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuHighlightColour_005c5874, -1);
        } else {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuShadowColour_005c5876, -1);
        }
        SetTextCursor(0x3c, 0x4c);
        DrawFormattedText("[C]reate a new personnel file");
        g_nPersonnelMenuHighlight_0049a6a0 = choice;
        EnableMouseCursorDrawing();
    }
}

/* Function start: 0x434EBC */
void DrawTransferredPilotRow(short row)
{
    SetTextCursor(0x32, (unsigned short)(row * 10 + 0x1e));
    DrawFormattedText("%d", row);
    SetTextCursor(0x3c, (unsigned short)(row * 10 + 0x1e));
    DrawFormattedText(g_apszPersonnelTransferLabels_005d2100[row]);
    SetTextCursor(0xc4, (unsigned short)(row * 10 + 0x1e));
    DrawFormattedText(
        g_apszPersonnelTransferLabels_005d2100[row] + 0x28);
    SetTextCursor(0xda, (unsigned short)(row * 10 + 0x1e));
    DrawFormattedText(
        g_apszPersonnelTransferLabels_005d2100[row] + 0x2c);
}

/* Function start: 0x434F7D */
void HighlightTransferredPilot(short choice)
{
    short row;

    if (choice != g_nPersonnelMenuHighlight_0049a6a0) {
        if (choice == 0x0b) {
            row = 0;
        } else {
            row = (short)(choice - 1);
        }
        DisableMouseCursorDrawing();
        if (choice != -1) {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuHighlightColour_005c5874, -1);
            DrawTransferredPilotRow(row);
        }
        if (g_nPersonnelMenuHighlight_0049a6a0 != -1) {
            InitializeTextContextFromFont(
                &g_stDefaultTextContext_005d2d20, 1,
                (unsigned char)g_nMenuShadowColour_005c5876, -1);
            if (g_nPersonnelMenuHighlight_0049a6a0 == 0x0b) {
                row = 0;
            } else {
                row = (short)(g_nPersonnelMenuHighlight_0049a6a0 - 1);
            }
            DrawTransferredPilotRow(row);
        }
        g_nPersonnelMenuHighlight_0049a6a0 = choice;
        EnableMouseCursorDrawing();
    }
}

/* Function start: 0x435064 */
short SelectTransferredPilot(short count)
{
    short row;
    short hover;
    int key;

    DisableMouseCursorDrawing();
    RestorePersonnelMenuBackground(&g_stSecondaryViewBuffer_005d2c90);
    InitializeTextContextFromFont(&g_stDefaultTextContext_005d2d20, 2,
                                  (unsigned char)
                                      g_nMenuShadowColour_005c5876,
                                  -1);
    SetTextCursor(0x32, 0x0f);
    DrawFormattedText("Select a character to transfer");
    SetTextCursor(0xba, 0x16);
    DrawFormattedText("Missions Kills");
    InitializeTextContextFromFont(&g_stDefaultTextContext_005d2d20, 1,
                                  (unsigned char)
                                      g_nMenuShadowColour_005c5876,
                                  -1);
    for (row = 0; row < count; row++) {
        DrawTransferredPilotRow(row);
    }
    g_nPersonnelMenuHighlight_0049a6a0 = -1;
    HighlightTransferredPilot(0x0b);
    DisableMouseCursorDrawing();
    g_cPersonnelMenuKey_00493128 = (signed char)0x80;
    SetPersonnelMousePosition(0xdc, 0x12);
    for (;;) {
        if (g_cPersonnelMenuKey_00493128 != (signed char)0x80) {
            return g_cPersonnelMenuKey_00493128;
        }
        PollPersonnelMenuInput();
        if (g_nPersonnelCursorX_005c8470 < 0x29 ||
            g_nPersonnelCursorX_005c8470 > 0xeb) {
            hover = -1;
            HighlightTransferredPilot(-1);
            SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape, 0);
        } else if (g_nPersonnelCursorY_005c8472 < 0x1e ||
                   g_nPersonnelCursorY_005c8472 > 0x27) {
            if (g_nPersonnelCursorY_005c8472 < 0x28 ||
                g_nPersonnelCursorY_005c8472 >
                    (count * 5 - 10) * 2 + 0x31) {
                hover = -1;
                HighlightTransferredPilot(-1);
                SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape,
                                    0);
            } else {
                hover = (short)((g_nPersonnelCursorY_005c8472 - 0x28) /
                                10 + 2);
                HighlightTransferredPilot(hover);
                SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape,
                                    1);
            }
        } else {
            hover = 0x0b;
            HighlightTransferredPilot(0x0b);
            SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape, 1);
        }
        g_bPersonnelMenuDrawing_0049a6c0 = 0;
        DrawPersonnelMenuBackdrop(g_pPersonnelMenuBackdrop_0049a6b8);
        g_bPersonnelMenuDrawing_0049a6c0 = 1;
        EnableMouseCursorDrawing();
        RefreshMemoryStatusOverlay();
        if ((g_cPersonnelMenuKey_00493128 == 0x1c ||
             g_cPersonnelMenuKey_00493128 == 0x39) &&
            hover != -1) {
            g_cPersonnelMenuKey_00493128 = (signed char)hover;
        }
        if (g_cPersonnelMenuKey_00493128 == 1 ||
            ((g_cPersonnelMenuKey_00493128 == 0x1c ||
              g_cPersonnelMenuKey_00493128 == 0x39) &&
             hover == -1)) {
            return -1;
        }
        key = g_cPersonnelMenuKey_00493128 - 2;
        switch (key) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            return 3;
        case 3:
            return 4;
        case 4:
            return 5;
        case 5:
            return 6;
        case 6:
            return 7;
        case 9:
            return 0;
        default:
            g_cPersonnelMenuKey_00493128 = (signed char)0x80;
            break;
        }
    }
}

/* Function start: 0x43641C */
short SelectLegacyGameSource(void)
{
    short result;
    short done;
    short handled;
    short hover;
    int key;

    done = 0;
    result = -1;
    RestorePersonnelMenuBackground(&g_stSecondaryViewBuffer_005d2c90);
    SetPersonnelMousePosition(200, 0x2c);
    while (done == 0) {
        InitializeTextContextFromFont(
            &g_stDefaultTextContext_005d2d20, 1,
            (unsigned char)g_nMenuShadowColour_005c5876, -1);
        SetTextCursor(0x3c, 0x1e);
        DrawFormattedText("Transfer a character from:");
        RefreshMemoryStatusOverlay();
        g_nPersonnelMenuHighlight_0049a6a0 = -1;
        DrawPersonnelSourceChoices(hover);
        DisableMouseCursorDrawing();
        g_cPersonnelMenuKey_00493128 = (signed char)0x80;
        while (g_cPersonnelMenuKey_00493128 == (signed char)0x80) {
            PollPersonnelMenuInput();
            if (g_nPersonnelCursorX_005c8470 < 0x1f ||
                g_nPersonnelCursorX_005c8470 > 0xdf) {
                hover = -1;
                DrawPersonnelSourceChoices(-1);
                SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape,
                                    0);
            } else if (g_nPersonnelCursorY_005c8472 < 0x26 ||
                       g_nPersonnelCursorY_005c8472 > 0x39) {
                if (g_nPersonnelCursorY_005c8472 < 0x3a ||
                    g_nPersonnelCursorY_005c8472 > 0x4d) {
                    hover = -1;
                    DrawPersonnelSourceChoices(-1);
                    SetMouseCursorShape(
                        g_pPersonnelCursor_005c8464->shape, 0);
                } else {
                    hover = 0x20;
                    DrawPersonnelSourceChoices(0x20);
                    SetMouseCursorShape(
                        g_pPersonnelCursor_005c8464->shape, 1);
                }
            } else {
                hover = 0x11;
                DrawPersonnelSourceChoices(0x11);
                SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape,
                                    1);
            }
            g_bPersonnelMenuDrawing_0049a6c0 = 0;
            DrawPersonnelMenuBackdrop(g_pPersonnelMenuBackdrop_0049a6b8);
            g_bPersonnelMenuDrawing_0049a6c0 = 1;
            EnableMouseCursorDrawing();
            RefreshMemoryStatusOverlay();
            handled = 0;
            result = -1;
            if ((g_cPersonnelMenuKey_00493128 == 0x1c ||
                 g_cPersonnelMenuKey_00493128 == 0x39) &&
                hover != -1) {
                g_cPersonnelMenuKey_00493128 = (signed char)hover;
            }
            if (g_cPersonnelMenuKey_00493128 == 1 ||
                ((g_cPersonnelMenuKey_00493128 == 0x1c ||
                  g_cPersonnelMenuKey_00493128 == 0x39) &&
                 hover == -1)) {
                done++;
                handled++;
            }
            key = toupper((int)g_cPersonnelMenuKey_00493128);
            if (key == 0x11) {
                result = 0;
                done++;
                handled++;
                g_nLegacySaveSource_0049a600 = 0;
            } else if (key == 0x20) {
                result = 0;
                done++;
                handled++;
                g_nLegacySaveSource_0049a600 = 1;
            }
            if (handled != 0) {
                g_cPersonnelMenuKey_00493128 = 0;
            }
        }
    }
    RestorePersonnelMenuBackground(&g_stSecondaryViewBuffer_005d2c90);
    return result;
}

/* Function start: 0x436722 */
int LocateLegacySaveGame(short source)
{
    const char *gameNames[3];
    char *installedPath;
    struct _finddata_t findData;
    long findHandle;
    unsigned int alpha;
    short pathEntered;
    short drive;
    short index;

    gameNames[0] = "Wing Commander";
    gameNames[1] = "Secret Missions 1";
    gameNames[2] = "Crusade:Secret Missions 2";
    InitializeTextContextFromFont(
        &g_stDefaultTextContext_005d2d20, 2,
        (unsigned char)g_nMenuShadowColour_005c5876, -1);
    if (g_nLegacySaveSource_0049a600 == 0) {
        installedPath = GetWingCommanderOneGameDataPath();
        if (installedPath == 0) {
            strcpy(g_szLegacySavePath_005d2130, "");
        } else {
            strcpy(g_szLegacySavePath_005d2130, installedPath);
        }
    } else {
        strcpy(g_szLegacySavePath_005d2130, "");
        RestorePersonnelMenuBackground(&g_stSecondaryViewBuffer_005d2c90);
        SetTextCursor(0x32, 0x28);
        DrawFormattedText("Please enter the full path name of");
        SetTextCursor(0x32, 0x34);
        DrawFormattedText("your %s saved game file.", gameNames[source]);
        SetTextCursor(0x32, 0x40);
        DrawFormattedText("Example: c:\\wing\\gamedat");
        RefreshMemoryStatusOverlay();
        DisableMouseCursorDrawing();
        pathEntered = ReadPersonnelTextField(
            0x32, 0x50, 2, "Path name: ",
            g_szLegacySavePath_005d2130, 0x19, 0, 1);
        EnableMouseCursorDrawing();
    }
    if (pathEntered == 0) {
        return 0;
    }
    if (strlen(g_szLegacySavePath_005d2130) > 1) {
        index = (short)strlen(g_szLegacySavePath_005d2130);
        do {
            index--;
            if (index < 0 ||
                g_szLegacySavePath_005d2130[index] == '\\') {
                break;
            }
            alpha = isalpha(g_szLegacySavePath_005d2130[index]);
            if (alpha != 0) {
                strcat(g_szLegacySavePath_005d2130, "\\");
                break;
            }
        } while (alpha == 0);
    }
    strcat(g_szLegacySavePath_005d2130,
           g_apszWc1SaveGameFileNames_0049a6c8[source]);
    for (index = 0;
         (unsigned int)index < strlen(g_szLegacySavePath_005d2130);
         index++) {
        if (g_szLegacySavePath_005d2130[index] == ':') {
            if (index == 0) {
                return 0;
            }
            drive = (short)(toupper(
                g_szLegacySavePath_005d2130[index - 1]) - 'A');
            PersonnelDriveHook(drive);
        }
    }
    findHandle = _findfirst(g_szLegacySavePath_005d2130, &findData);
    if ((short)findHandle == -1) {
        return 0;
    }
    return 1;
}

/* Function start: 0x436A4D */
void ShowNoTransferablePilots(void)
{
    RestorePersonnelMenuBackground(&g_stSecondaryViewBuffer_005d2c90);
    SetTextCursor(0x50, 0x3c);
    DrawFormattedText("No valid characters found.");
    RefreshMemoryStatusOverlay();
    g_cPersonnelMenuKey_00493128 = (signed char)0x80;
    WaitForInputKey();
}

/* Function start: 0x436A8F */
short RunPilotDatabaseMenu(void)
{
    char firstName[13] = "Christopher";
    char lastName[13] = "Blair";
    char callsign[13] = "Maverick";
    char text[80];
    SaveGameDiskRecord *record;
    PilotRecord *pilot;
    short file;
    short inputResult;
    short transferCount;
    short index;
    short selected;
    short hover;
    short previousDrive;
    short done;
    short actionHandled;
    short result;
    int key;

    done = 0;
    actionHandled = 0;
    result = 0;
    g_stSecondaryViewBuffer_005d2c90.left = 0;
    g_stSecondaryViewBuffer_005d2c90.top = 0;
    g_stSecondaryViewBuffer_005d2c90.right = 319;
    g_stSecondaryViewBuffer_005d2c90.bottom = 199;
    if ((short)AllocateViewport(
            &g_stSecondaryViewBuffer_005d2c90,
            g_cSecondaryViewBufferColour_0049cb4c, 0) == 0) {
        ReportFatalErrorCode("012");
    }
    SetInputViewport(&g_stSecondaryViewBuffer_005d2c90);
    DisableMouseCursorDrawing();
    InitializeTextContextFromFont(&g_stDefaultTextContext_005d2d20, 0,
                                  g_cPersonnelTextColour_0049cb50, -1);
    InitializeTextContextFromFont(&g_stDefaultTextContext_005d2d20, 2,
                                  g_cPersonnelTextColour_0049cb50, -1);
    g_pPersonnelMenuBackdrop_0049a6b8 = FetchDiskPacketRetrying(
        "options.v00", (short)(g_nCockpitArchiveVariant_005c901a * 3 + 1),
        0);
    SetMenuInputPump();
    ClearViewport(&g_stSecondaryViewBuffer_005d2c90, 0);
    DrawSpriteDefault(&g_stSecondaryViewBuffer_005d2c90, 0, 0,
                      g_pPersonnelMenuBackdrop_0049a6b8, 0);
    DrawSpriteDefault(&g_stSecondaryViewBuffer_005d2c90, 0, 0,
                      g_pPersonnelMenuBackdrop_0049a6b8, 1);
    record = AllocateTaggedMemory(0x33c, 0x40);
    if (record == 0) {
        ReportFatalErrorCode("013");
    }
    for (index = 0; index < 8; index++) {
        g_apszPersonnelTransferLabels_005d2100[index] =
            AllocateTaggedMemory(0x50, 0x40);
        if (g_apszPersonnelTransferLabels_005d2100[index] == 0) {
            ReportFatalErrorCode("014");
        }
    }
    SetInputViewport(&g_stSecondaryViewBuffer_005d2c90);
    while (done == 0) {
        DisableMouseCursorDrawing();
        g_stDefaultTextContext_005d2d20.viewport =
            &g_stSecondaryViewBuffer_005d2c90;
        g_stDefaultTextContext_005d2d20.text =
            g_szDefaultTextBuffer_005d2b80;
        RestorePersonnelMenuBackground(&g_stSecondaryViewBuffer_005d2c90);
        SetTextContext(&g_stDefaultTextContext_005d2d20);
        InitializeTextContextFromFont(
            &g_stDefaultTextContext_005d2d20, 1,
            (unsigned char)g_nMenuShadowColour_005c5876, -1);
        SetTextCursor(0x50, 0x12);
        DrawFormattedText("TERRAN CONFEDERATION NAVY");
        SetTextCursor(0x60, 0x1c);
        DrawFormattedText("PERSONNEL DATABASE");
        hover = 0x26;
        g_nPersonnelMenuHighlight_0049a6a0 = -1;
        DrawPersonnelMenuChoices(0x26);
        DisableMouseCursorDrawing();
        SetPersonnelMousePosition(200, 0x36);
        RefreshMemoryStatusOverlay();
        g_cPersonnelMenuKey_00493128 = (signed char)0x80;
        while (g_cPersonnelMenuKey_00493128 == (signed char)0x80) {
            PollPersonnelMenuInput();
            if (g_nPersonnelCursorX_005c8470 < 0x29 ||
                g_nPersonnelCursorX_005c8470 > 0xe9) {
                hover = -1;
                DrawPersonnelMenuChoices(-1);
                SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape,
                                    0);
            } else if (g_nPersonnelCursorY_005c8472 < 0x30 ||
                       g_nPersonnelCursorY_005c8472 > 0x43) {
                if (g_nPersonnelCursorY_005c8472 < 0x44 ||
                    g_nPersonnelCursorY_005c8472 > 0x57) {
                    hover = -1;
                    DrawPersonnelMenuChoices(-1);
                    SetMouseCursorShape(
                        g_pPersonnelCursor_005c8464->shape, 0);
                } else {
                    hover = 0x2e;
                    DrawPersonnelMenuChoices(0x2e);
                    SetMouseCursorShape(
                        g_pPersonnelCursor_005c8464->shape, 1);
                }
            } else {
                hover = 0x14;
                DrawPersonnelMenuChoices(0x14);
                SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape,
                                    1);
            }
            g_bPersonnelMenuDrawing_0049a6c0 = 0;
            DrawPersonnelMenuBackdrop(g_pPersonnelMenuBackdrop_0049a6b8);
            g_bPersonnelMenuDrawing_0049a6c0 = 1;
            EnableMouseCursorDrawing();
            RefreshMemoryStatusOverlay();
            actionHandled = 0;
            if ((g_cPersonnelMenuKey_00493128 == 0x1c ||
                 g_cPersonnelMenuKey_00493128 == 0x39) &&
                hover != -1) {
                g_cPersonnelMenuKey_00493128 = (signed char)hover;
            }
            if ((g_cPersonnelMenuKey_00493128 == 0x1c ||
                 g_cPersonnelMenuKey_00493128 == 0x39) &&
                hover == -1) {
                done++;
                actionHandled++;
            }
            key = toupper((int)g_cPersonnelMenuKey_00493128);
            if (key == 0x14) {
                previousDrive = 99;
                actionHandled = 1;
                DisableMouseCursorDrawing();
                RestorePersonnelMenuBackground(
                    &g_stSecondaryViewBuffer_005d2c90);
                RefreshMemoryStatusOverlay();
                InitializeTextContextFromFont(
                    &g_stDefaultTextContext_005d2d20, 1,
                    (unsigned char)g_nMenuShadowColour_005c5876, -1);
                selected = SelectLegacyGameSource();
                if (selected == -1) {
                    result = 0;
                } else {
                    done = (short)LocateLegacySaveGame(selected);
                    if (done == 0) {
                        ShowNoTransferablePilots();
                        result = 0;
                        g_cPersonnelMenuKey_00493128 = 0x14;
                    } else {
                        file = (short)_open(g_szLegacySavePath_005d2130,
                                            0x8000);
                        transferCount = 0;
                        for (index = 0; index < 8; index++) {
                            _read(file, record, 0x33c);
                            if (record->occupied != 0 &&
                                record->campaign.campaignIndex ==
                                    selected) {
                                pilot = &record->pilots[8];
                                strcpy(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        transferCount],
                                    g_apszWc1PilotRankNames_0049a620[
                                        pilot->rank]);
                                strcat(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        transferCount],
                                    " \"");
                                strcat(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        transferCount],
                                    pilot->callsign);
                                strcat(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        transferCount],
                                    "\" ");
                                strcat(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        transferCount],
                                    pilot->name);
                                _itoa(pilot->missions, text, 10);
                                strcpy(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        transferCount] + 0x28,
                                    text);
                                _itoa(pilot->kills, text, 10);
                                strcpy(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        transferCount] + 0x2c,
                                    text);
                                _itoa(index, text, 10);
                                strcpy(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        transferCount] + 0x3c,
                                    text);
                                transferCount++;
                            }
                        }
                        _close(file);
                        if (transferCount == 0) {
                            ShowNoTransferablePilots();
                            result = 0;
                            g_cPersonnelMenuKey_00493128 = 0x14;
                            done = 0;
                        } else {
                            selected = SelectTransferredPilot(
                                transferCount);
                            if (selected == -1) {
                                done = 0;
                                result = 0;
                                g_cPersonnelMenuKey_00493128 = 0x14;
                            } else {
                                index = (short)atoi(
                                    g_apszPersonnelTransferLabels_005d2100[
                                        selected] + 0x3c);
                                file = (short)_open(
                                    g_szLegacySavePath_005d2130, 0x8000);
                                _lseek(file, (long)index * 0x33c, 0);
                                _read(file, record, 0x33c);
                                pilot = &record->pilots[8];
                                _close(file);
                                PersonnelDriveHook(previousDrive);
                                InitializeTextContextFromFont(
                                    &g_stDefaultTextContext_005d2d20, 1,
                                    (unsigned char)
                                        g_nMenuHighlightColour_005c5874,
                                    -1);
                                DisableMouseCursorDrawing();
                                RestorePersonnelMenuBackground(
                                    &g_stSecondaryViewBuffer_005d2c90);
                                RefreshMemoryStatusOverlay();
                                strcpy(text,
                                       g_apszWc1PilotRankNames_0049a620[
                                           pilot->rank]);
                                strcat(text, " ");
                                strcat(text, pilot->name);
                                strcat(text, "'s first name?");
                                SetTextCursor(0x32, 0x32);
                                DrawFormattedText(text);
                                RefreshMemoryStatusOverlay();
                                ReadPersonnelTextField(
                                    0x32, 0x3c, 1, "", firstName,
                                    0x0c, 0, 0);
                                strcpy(g_szPilotFirstName_00499f28,
                                       firstName);
                                strcpy(g_szPilotLastName_00499f10,
                                       pilot->name);
                                strcpy(g_szPilotCallsign_00499ef8,
                                       pilot->callsign);
                                strcpy(
                                    g_stCurrentPilotProfile_00493408.firstName,
                                    firstName);
                                strcpy(
                                    g_stCurrentPilotProfile_00493408.lastName,
                                    pilot->name);
                                strcpy(
                                    g_stCurrentPilotProfile_00493408.callsign,
                                    pilot->callsign);
                                g_stCurrentPilotProfile_00493408.field_3f = 2;
                                g_stCurrentPilotProfile_00493408.field_41 =
                                    pilot->missions;
                                g_stCurrentPilotProfile_00493408.field_43 =
                                    pilot->kills;
                                LoadStartingCampaignGlobals(0);
                                SaveAndFreeTemporaryCampaignGlobals();
                                OpenDiskDataFile(1);
                                done = 1;
                                result = 1;
                            }
                        }
                    }
                }
                PersonnelDriveHook(previousDrive);
            } else if (key == 0x2e) {
                result = 1;
                actionHandled = 1;
                DisableMouseCursorDrawing();
                RestorePersonnelMenuBackground(
                    &g_stSecondaryViewBuffer_005d2c90);
                RefreshMemoryStatusOverlay();
                InitializeTextContextFromFont(
                    &g_stDefaultTextContext_005d2d20, 1,
                    (unsigned char)g_nMenuHighlightColour_005c5874, -1);
                inputResult = ReadPersonnelTextField(
                    0x32, 0x1e, 2, "Enter first name: ", firstName,
                    0x0b, 0, 0);
                if (inputResult != 0) {
                    strcpy(g_szPilotFirstName_00499f28, firstName);
                    strcpy(g_stCurrentPilotProfile_00493408.firstName,
                           firstName);
                    inputResult = ReadPersonnelTextField(
                        0x32, 0x32, 2, "Enter last name: ", lastName,
                        0x0b, 0, 0);
                    if (inputResult != 0) {
                        strcpy(g_szPilotLastName_00499f10, lastName);
                        strcpy(g_stCurrentPilotProfile_00493408.lastName,
                               lastName);
                        inputResult = ReadPersonnelTextField(
                            0x32, 0x46, 2, "Enter callsign: ", callsign,
                            0x0b, 0, 0);
                        if (inputResult != 0) {
                            strcpy(g_szPilotCallsign_00499ef8, callsign);
                            strcpy(
                                g_stCurrentPilotProfile_00493408.callsign,
                                callsign);
                            g_stCurrentPilotProfile_00493408.field_3f = 2;
                            g_stCurrentPilotProfile_00493408.field_41 = 0;
                            g_stCurrentPilotProfile_00493408.field_43 = 0;
                            LoadStartingCampaignGlobals(0);
                            SaveAndFreeTemporaryCampaignGlobals();
                            OpenDiskDataFile(1);
                        }
                    }
                }
                if (inputResult == 0) {
                    result = 0;
                }
                done++;
                g_cPersonnelMenuKey_00493128 = 0x2e;
            }
            if (actionHandled == 0) {
                g_cPersonnelMenuKey_00493128 = (signed char)0x80;
            }
        }
        g_cPersonnelMenuKey_00493128 = (signed char)0x80;
    }
    FlushInputEvents();
    ReleasePacketSlot(&g_stSceneHotspotTable_005d3bf8.data);
    ReleasePacketSlot(&g_stSceneTextTable_005d3c00.data);
    g_pActiveScenePacket_00492654 = 0;
    FreePacketAndClear(&g_pPersonnelMenuBackdrop_0049a6b8, 0);
    g_stDefaultTextContext_005d2d20.viewport =
        &g_stScreenViewport_005d21a0;
    ReleasePacketHandle(record);
    for (index = 0; index < 8; index++) {
        ReleasePacketHandle(
            g_apszPersonnelTransferLabels_005d2100[index]);
    }
    free_viewport(&g_stSecondaryViewBuffer_005d2c90);
    FlushInputEvents();
    return result;
}

/* Function start: 0x459BC8 */
short ReadPersonnelTextField(unsigned int x, unsigned int y,
                             int font, const char *prompt,
                             char *destination, int maximumLength,
                             int mode, int required)
{
    short result;

    g_stDefaultTextContext_005d2d20.viewport =
        &g_stScreenViewport_005d21a0;
    SetTextCursor((unsigned short)x, (unsigned short)y);
    DrawFormattedText("%s", prompt);
    MarkDibDirty();
    DIBslamReal();
    result = ReadTextInput(destination, (short)maximumLength,
                           (short)mode, (short)required);
    if (result != 0) {
        result = 1;
    }
    g_stDefaultTextContext_005d2d20.viewport =
        &g_stSecondaryViewBuffer_005d2c90;
    return result;
}
