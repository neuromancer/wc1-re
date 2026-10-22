/*
 * Opening cinematic, credits, and title menu.
 * Placement and boundary evidence are recorded in docs/ORDER.md.
 */
#include "wc1.h"

/* 0x00468910 */
static char *pszIntroOpeningText =
    "In the distant future,\n"
    "mankind is locked in a deadly war...";
/* Retained original message; no references survive in the shipped code. */
/* 0x00468918 */
char szMissingSecretMissionsMessage[279] =
    "The SECRET MISSIONS files are not present on your WING COMMANDER game disk\n\n"
    "If you have not obtained the SECRET MISSIONS disk package, purchase it from "
    "ORIGIN for $19.95 (US dollars) by calling 1-800-999-4939, Mon-Fri 8am-5pm "
    "central.\n(Price and availability subject to change).";
static int nIntroCreditCount = 11; /* 0x00468a30 */
/* 0x00468a38 */
static char *apszIntroCredits[20] = {
    "Design\nby\nChris Roberts",
    "Software Engineers\nChris Roberts\nKen Demarest III\nPaul C. Isaac\nSteve Muchow\nHerman Miller\nSteve Beeman\n",
    "Dogfight Intelligence\nKen Demarest III\n\nDogfight Choreography\nSteve Beeman\nErin Roberts",
    "3Space System\nby\nChris Roberts\n\nOriginFX Graphic System\nChris Roberts\nJohn Miles",
    "OriginFX Sound System\nby\nHerman Miller",
    "Artwork\nDenis Loubet\nGlen Johnson\nDaniel Bourbonnais\nKeith Berdak\nJohn Watson",
    "Screenplay by Jeff George\n\nAdditional Writing\nSteve Cantrell\nPhilip Brogden",
    "Soundtrack by\nGeorge A. Sanger and Dave Govett",
    "Sound Effects by Marc Schaefgen",
    "Produced by\nChris Roberts and Warren Spector",
    "Directed by\nChris Roberts",
    "Windows 95 Team",
    "Combat Programmers\n\nJeff Mangler Everett\nJeff jefftep Grills\nChuck Bishop Karpiak\nKris Goblin Pelley",
    "Sound System\n\nRichard Cupcake Lyle",
    "Soundtrack Rescored by\n\nI Need Names",
    "Head Whiner\n\nAnthony Sommers",
    "Whiners\n\nMonte Mathis\nHal Milton\nDieter Martin",
    "Richard Zinser\nKanon Lillemon\n",
    "Special Thanks To\n\nSocks\nand\nCaffeine",
    0
};

/* 0x00468a88 */
static TitleMenuRegion aTitleMenuRegions[5] = {
    { 1, 49, 48, 283, 99 },
    { 1, 49, 91, 283, 149 },
    { 1, 49, 134, 283, 149 },
    { 1, 49, 177, 283, 209 },
    { -1, 0, 0, 0, 0 }
};

/* 0x00468ac0 */
static PacketResourceDescriptor aIntroResourceDescriptors[3] = {
    { &aObjectTypeData[OBJECT_TYPE_EXPLOSION1].shapeSet,
      3, 2 },
    { &aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet,
      3, 5 },
    { 0, 0, 0 }
};

static int bTitleMenuSceneInitialized = 0; /* 0x00468ad8 */

/* Linker commons defined here; LINK orders them by identifier hash. */
short nExternalViewAngle; /* 0x005a7f06 */
unsigned char *pTitleShape; /* 0x005a7f08 */

/* Function start: 0x40FA30 */
unsigned short GetPaletteReadyUnused(void)
{
    return 1;
}

/* Function start: 0x40FA40 */
void DrawTitleLogo(short distance, short y)
{
    short bounds[4];
    short scale;

    if (distance <= 10)
        return;
    scale = (short)(0x1000 / distance);
    GetTransformedShapeBounds(&stSpaceBufferViewport,
                              (short)(nScreenWidth >> 1), y,
                              pTitleShape, 1, 0, scale, 0,
                              bounds);
#ifdef SDL_PORT
    if (!SdlRecordSpaceSprite(
            &stSpaceBufferViewport, (short)(bounds[0] - 1), y,
            pTitleShape, 0, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBufferViewport, (short)(bounds[0] - 1), y,
                         pTitleShape, 0, 0, scale, 0);
#ifdef SDL_PORT
    if (!SdlRecordSpaceSprite(
            &stSpaceBufferViewport,
            (short)(nScreenWidth >> 1), y,
            pTitleShape, 1, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBufferViewport,
                         (short)(nScreenWidth >> 1), y,
                         pTitleShape, 1, 0, scale, 0);
#ifdef SDL_PORT
    if (!SdlRecordSpaceSprite(
            &stSpaceBufferViewport, bounds[2], y,
            pTitleShape, 2, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBufferViewport, bounds[2], y,
                         pTitleShape, 2, 0, scale, 0);
}

/* Function start: 0x40FB10 */
void UpdateTitleMenuCursor(void)
{
    short mouseX;
    short mouseY;
    short frame;
    TitleMenuRegion *region;

    frame = 0;
    mouseX = stHostMouseState.x;
    mouseY = stHostMouseState.y;
    region = aTitleMenuRegions;
    while (region->frame != -1) {
        if (IsPointInRect(mouseX, mouseY,
                          &region->left) != 0)
            frame = region->frame;
        region++;
    }
    SetMouseCursorShape(stMouseCursorState.shape, frame);
}

/* Function start: 0x40FB70 */
int Title_Sequence(void)
{
    short frame;
    short credit;
    short titleDistance;
    short missionShip;
    short menuIndex;
    int optionCount;
    short eventType;
    short menuOptions[4];
    unsigned char *menuShape;
    unsigned char *alternateMenuShape;
    InputEventState event;
    int activate;
    signed char state;
    signed char selectedIndex;

    state = 0;
    if (bShowKilrathiSagaCredits != 0)
        nIntroCreditCount += 9;
    if (bEscapePressedFlag == 0) {
        PreloadMusicTrackHook(0x17);
        SetEventManagerPump(PollJoystickButtonEvents);
        bIntroSceneResourcesActive = 0;
        init_3Space_objects(0);
        nCannedSceneMode = 2;
        pIntroFont =
            FetchDiskPacketRetrying(9, 1, 0);
        nSceneResourceBudgetValue = 0x3e8000;
        nSceneResourceBudgetValue = LoadPacketResourceList(
            aIntroResourceDescriptors, 0, 0x3e8000);
        aObjectTypeData[OBJECT_TYPE_DEBRIS_WING].shapeSet =
            aObjectTypeData[
                OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
        ClearInputKeyStatePreservingModifiers();
        FlushInputEvents();
        bEscapePressedFlag = 0;

        while (state == 0) {
            PumpWindowMessages();
            missionShip = 32;
            do {
                aMissionShips[missionShip].state = 0;
                missionShip++;
            } while (missionShip < 46);
            titleDistance = 200;
            remove_all_hazards();
            bIntroSecondaryScene = 0;
            set_up_action_sphere(16);
            pTitleShape =
                FetchDiskPacketRetrying(9, 0, 0);
            spacetrack(0x17, 2, 1);
            initialize_scripted_view(asIntroCameraSequence);
            nFrameSkipCounter = 1;

            frame = 0;
            do {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    print_subtitle(&stSpaceBufferViewport, 0x32,
                                   pszIntroOpeningText);
                    dump_buffer_to_screen();
                    DIBslam();
                    DIBslamReal();
                    intro_drawbackgroundships();
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                }
                frame++;
            } while (frame < 25);
            clear_view_buffer();
            if (state != 0)
                break;

            frame = 0;
            do {
                Update_3Space();
                RenderSpaceViewFrame();
                if (CheckEscaped() != 0) {
                    state++;
                    break;
                }
                frame++;
                DIBslam();
                DIBslamReal();
            } while (frame < 110);
            if (state != 0)
                break;

            frame = 0;
            do {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    DrawTitleLogo(titleDistance,
                                  (short)(nCurrentViewCenterPosY - 6));
                    dump_buffer_to_screen();
                    DIBslam();
                    DIBslamReal();
                    clear_view_buffer();
                }
                if (titleDistance > 16)
                    titleDistance -= 4;
                if (CheckEscaped() != 0) {
                    state++;
                    break;
                }
                frame++;
            } while (frame < 100);
            FreePacketAndClear(&pTitleShape, 0);
            if (state != 0)
                break;

            ScaleFixedVector(&aShipFwdVec[61], 0x9600,
                             &aShipVelocity[61]);
            set_up_action_sphere(17);
            bIntroSecondaryScene = 1;
            anObjPitchRot[0] = 0;
            anObjectYawRotation[0] = 0;
            anObjectRollRotationList[0] = 0;
            start_hazard_field(0);

            credit = 0;
            for (; credit < nIntroCreditCount; credit++) {
                frame = 0;
                do {
                    Update_3Space();
                    if (Draw_3Space_Frame() != 0) {
                        print_subtitle(&stSpaceBufferViewport, 0x32,
                            apszIntroCredits[credit]);
                        dump_buffer_to_screen();
                        DIBslam();
                        DIBslamReal();
                        clear_view_buffer();
                    }
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                    frame++;
                } while (frame < 70);
                if (state != 0)
                    break;

                frame = 0;
                do {
                    Update_3Space();
                    RenderSpaceViewFrame();
                    DIBslam();
                    DIBslamReal();
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                    frame++;
                } while (frame < 40);
            }
            if (state != 0)
                break;

            frame = 0;
            do {
                Update_3Space();
                RenderSpaceViewFrame();
                DIBslam();
                DIBslamReal();
                if (CheckEscaped() != 0) {
                    state++;
                    break;
                }
                frame++;
            } while (frame < 150);
        }

        state = 0;
        StopMusicUnlessSuppressed();
        ResetSoundState();
        ReleasePacketHandle(pIntroFont);
        ReleasePacketHandle(pTitleShape);
        FreeShapeSet(aIntroResourceDescriptors, 0);
        aObjectTypeData[OBJECT_TYPE_DEBRIS_WING].shapeSet =
            aObjectTypeData[
                OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
        free_all_slots();
        free_3Space();
        bIntroSecondaryScene = 0;
        nCannedSceneMode = 0;
        bScriptedView = 0;
        bIntroSceneResourcesActive = 1;
        ReleaseMusicTrackHook(0x17);
    }

    bEscapePressedFlag = 0;
    if (bTitleMenuSceneInitialized == 0) {
        SceneEnterHook();
        bTitleMenuSceneInitialized = 1;
    }
    menuShape = FetchDiskPacketRetrying(9, 4, 0);
    optionCount = 1;
    alternateMenuShape =
        FetchDiskPacketRetrying(0x4b, 0, 0);
    menuOptions[0] = 0;
    if (AnySavedGames() != 0) {
        optionCount = 2;
        menuOptions[1] = 1;
    }
    for (; optionCount < 4; optionCount++)
        menuOptions[optionCount] = -1;

    menuIndex = 0;
    do {
        if (menuOptions[menuIndex] != -1) {
            aTitleMenuRegions[menuIndex].frame = 1;
            if (menuOptions[menuIndex] < 3) {
                GetShapeFrameBounds(
                    &aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    menuShape, menuOptions[menuIndex]);
            } else {
                GetShapeFrameBounds(
                    &aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    alternateMenuShape, 0);
            }
        } else {
            aTitleMenuRegions[menuIndex].frame = -1;
        }
        menuIndex++;
    } while (menuIndex < 4);

    ClearViewport(&stScreenView, cBlackColour);
    menuIndex = 0;
    do {
        if (menuOptions[menuIndex] != -1) {
            if (menuOptions[menuIndex] < 3) {
                DrawSpriteDefault(&stScreenView,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    menuShape, menuOptions[menuIndex]);
            } else {
                DrawSpriteDefault(&stScreenView,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    alternateMenuShape, 0);
            }
        }
        menuIndex++;
    } while (menuIndex < 4);
    DIBslam();
    DIBslamReal();

    stMouseCursorState.viewport = &stScreenView;
    SetEventManagerPump(PollMenuInputDevices);
    nMenuInputRepeatDelay = 6;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    bInputModeState = 1;
    bKeyEventQueueEnabled = 0;
    while (state == 0) {
        selectedIndex = -1;
        UpdateTitleMenuCursor();
        eventType = PollInputEvent(&event, 0xff);
        switch (eventType) {
        case 3:
        case 5:
            activate = 0;
            ClearInputKeyStatePreservingModifiers();
            switch ((short)event.value) {
            case 0x1c:
            case 0x1f:
            case 0x2e:
            case 0x39:
                if ((short)event.value == 0x1f)
                    selectedIndex = 0;
                if ((short)event.value == 0x2e &&
                    menuOptions[2] != -1)
                    selectedIndex = 1;
                if ((short)event.value == 0x32 &&
                    menuOptions[2] != -1)
                    selectedIndex = 2;
                activate = 1;
                break;
            case 0x24:
                CalibrateJoystickInteractive(9, 9, 1, 1);
                break;
            default:
                MoveMenuPointerFromKeyboard(&event);
                break;
            }
            if (activate == 0)
                break;
            /* Fall through to the shared mouse/keyboard activation. */
        case 2:
        {
            int selection;

            if (selectedIndex == -1) {
                int mouseX;
                int mouseY;

                mouseY = event.y;
                mouseX = event.x;
                selectedIndex = FindMenuRegionAtPoint(
                    mouseX, mouseY, aTitleMenuRegions);
            }
            selection = selectedIndex;
            if (selection < 0 || selection > 3)
                state = 0;
            else
                state = (signed char)(menuOptions[selection] + 1);
            break;
        }
        default:
            break;
        }
        DIBslam();
        DIBslamReal();
    }

    bKeyEventQueueEnabled = 1;
    ClearDebugPauseFlags();
    ReleasePacketHandle(menuShape);
    ReleasePacketHandle(alternateMenuShape);
    SetEventManagerPump(0);
    EventManagerHook(0);
    LeaveAllocationScope();
    FadeViewportPaletteToColour(&stScreenView, cBlackColour, 1);
    ClearViewport(&stScreenView, cBlackColour);
    DIBslam();
    DIBslamReal();
    RestoreGamePalette();
    return state - 1;
}
