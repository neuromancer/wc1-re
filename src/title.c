/**
 * @file
 * @ingroup campaign
 * @brief Opening cinematic, credits and title menu.
 *
 * Address range 40fa30-4102af.
 */
#include "wc1.h"
#include <sys/stat.h>

/* 0x00468910 */
static char *pszIntroOpeningText =
    NC_("intro", "In the distant future,\n"
    "mankind is locked in a deadly war...");
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
    NC_("intro", "Design\nby\nChris Roberts"),
    NC_("intro", "Software Engineers\nChris Roberts\nKen Demarest III\nPaul C. Isaac\nSteve Muchow\nHerman Miller\nSteve Beeman\n"),
    NC_("intro", "Dogfight Intelligence\nKen Demarest III\n\nDogfight Choreography\nSteve Beeman\nErin Roberts"),
    NC_("intro", "3Space System\nby\nChris Roberts\n\nOriginFX Graphic System\nChris Roberts\nJohn Miles"),
    NC_("intro", "OriginFX Sound System\nby\nHerman Miller"),
    NC_("intro", "Artwork\nDenis Loubet\nGlen Johnson\nDaniel Bourbonnais\nKeith Berdak\nJohn Watson"),
    NC_("intro", "Screenplay by Jeff George\n\nAdditional Writing\nSteve Cantrell\nPhilip Brogden"),
    NC_("intro", "Soundtrack by\nGeorge A. Sanger and Dave Govett"),
    NC_("intro", "Sound Effects by Marc Schaefgen"),
    NC_("intro", "Produced by\nChris Roberts and Warren Spector"),
    NC_("intro", "Directed by\nChris Roberts"),
    NC_("intro", "Windows 95 Team"),
    NC_("intro", "Combat Programmers\n\nJeff Mangler Everett\nJeff jefftep Grills\nChuck Bishop Karpiak\nKris Goblin Pelley"),
    NC_("intro", "Sound System\n\nRichard Cupcake Lyle"),
    NC_("intro", "Soundtrack Rescored by\n\nI Need Names"),
    NC_("intro", "Head Whiner\n\nAnthony Sommers"),
    NC_("intro", "Whiners\n\nMonte Mathis\nHal Milton\nDieter Martin"),
    NC_("intro", "Richard Zinser\nKanon Lillemon\n"),
    NC_("intro", "Special Thanks To\n\nSocks\nand\nCaffeine"),
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
struct Shape *pTitleShape; /* 0x005a7f08 */

/* Function start: 0x40FA30 */
unsigned short GetPaletteReadyUnused(void)
{
    return 1;
}

/* Function start: 0x40FA40 */
void DrawTitleLogo(short distance, short y)
{
    ShortRect bounds;
    short scale;

    if (distance <= 10)
        return;
    scale = (0x1000 / distance);
    GetTransformedShapeBounds(&stSpaceBufferViewport,
                              (nScreenWidth >> 1), y,
                              pTitleShape, 1, 0, scale, 0,
                              &bounds);
#ifdef SDL_PORT
    if (!SdlRecordSpaceSprite(
            &stSpaceBufferViewport, (short)(bounds.left - 1), y,
            pTitleShape, 0, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBufferViewport, (bounds.left - 1), y,
                         pTitleShape, 0, 0, scale, 0);
#ifdef SDL_PORT
    if (!SdlRecordSpaceSprite(
            &stSpaceBufferViewport,
            (short)(nScreenWidth >> 1), y,
            pTitleShape, 1, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBufferViewport,
                         (nScreenWidth >> 1), y,
                         pTitleShape, 1, 0, scale, 0);
#ifdef SDL_PORT
    if (!SdlRecordSpaceSprite(
            &stSpaceBufferViewport, bounds.right, y,
            pTitleShape, 2, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBufferViewport, bounds.right, y,
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
    for (region = aTitleMenuRegions; region->frame != -1; region++) {
        if (IsPointInRect(mouseX, mouseY,
                          &region->left) != 0)
            frame = region->frame;
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
    struct Shape *menuShape;
    struct Shape *alternateMenuShape;
    InputEventState event;
    int activate;
    signed char state;
    signed char selectedIndex;
#ifdef SDL_PORT
    short startFrame;
#endif

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
            for (missionShip = 32; missionShip < 46; missionShip++) {
                aMissionShips[missionShip].state = 0;
            }
            titleDistance = 200;
            remove_all_hazards();
            bIntroSecondaryScene = 0;
            set_up_action_sphere(16);
            pTitleShape =
                FetchDiskPacketRetrying(9, 0, 0);
            spacetrack(0x17, 2, 1);
            initialize_scripted_view(asIntroCameraSequence);
            nFrameSkipCounter = 1;

            for (frame = 0; frame < 25; frame++) {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    print_subtitle(&stSpaceBufferViewport, 0x32,
                                   C_("intro", pszIntroOpeningText));
                    dump_buffer_to_screen();
                    DIBslam();
                    DIBslamReal();
                    intro_drawbackgroundships();
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                }
            }
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

            for (frame = 0; frame < 100; frame++) {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    DrawTitleLogo(titleDistance,
                                  (nCurrentViewCenterPosY - 6));
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
            }
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
                for (frame = 0; frame < 70; frame++) {
                    Update_3Space();
                    if (Draw_3Space_Frame() != 0) {
                        print_subtitle(&stSpaceBufferViewport, 0x32,
                            C_("intro", apszIntroCredits[credit]));
                        dump_buffer_to_screen();
                        DIBslam();
                        DIBslamReal();
                        clear_view_buffer();
                    }
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                }
                if (state != 0)
                    break;

                for (frame = 0; frame < 40; frame++) {
                    Update_3Space();
                    RenderSpaceViewFrame();
                    DIBslam();
                    DIBslamReal();
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                }
            }
            if (state != 0)
                break;

            for (frame = 0; frame < 150; frame++) {
                Update_3Space();
                RenderSpaceViewFrame();
                DIBslam();
                DIBslamReal();
                if (CheckEscaped() != 0) {
                    state++;
                    break;
                }
            }
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
#ifdef SDL_PORT
    alternateMenuShape = 0;
    menuShape = nLaunchCampaign == 2 ? FetchDiskPacketRetrying(75, 0, 0) :
                                     FetchDiskPacketRetrying(9, 4, 0);
    startFrame = nLaunchCampaign == 1 ? 2 : 0;
#else
    menuShape = FetchDiskPacketRetrying(9, 4, 0);
    alternateMenuShape =
        FetchDiskPacketRetrying(0x4b, 0, 0);
#endif
    menuOptions[0] = 0;
    optionCount = 1;
    if (AnySavedGames() != 0) {
        optionCount = 2;
        menuOptions[1] = 1;
    }
    for (; optionCount < 4; optionCount++)
        menuOptions[optionCount] = -1;

    for (menuIndex = 0; menuIndex < 4; menuIndex++) {
#ifdef SDL_PORT
        aTitleMenuRegions[menuIndex].left = 49;
        aTitleMenuRegions[menuIndex].top = 48 + menuIndex * 43;
#endif
        if (menuOptions[menuIndex] != -1) {
            aTitleMenuRegions[menuIndex].frame = 1;
            if (menuOptions[menuIndex] < 3) {
                GetShapeFrameBounds(
                    &aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    menuShape,
#ifdef SDL_PORT
                    menuOptions[menuIndex] == 0 ? startFrame : menuOptions[menuIndex]);
#else
                    menuOptions[menuIndex]);
#endif
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
    }

    ClearViewport(&stScreenView, cBlackColour);
    for (menuIndex = 0; menuIndex < 4; menuIndex++) {
        if (menuOptions[menuIndex] != -1) {
            if (menuOptions[menuIndex] < 3) {
                DrawSpriteDefault(&stScreenView,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    menuShape,
#ifdef SDL_PORT
                    menuOptions[menuIndex] == 0 ? startFrame : menuOptions[menuIndex]);
#else
                    menuOptions[menuIndex]);
#endif
            } else {
                DrawSpriteDefault(&stScreenView,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    alternateMenuShape, 0);
            }
        }
    }
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
        eventType = PollInputEvent(&event, INPUT_EVENT_FILTER_ALL);
        switch (eventType) {
        case INPUT_EVENT_KEY_DOWN:
        case INPUT_EVENT_KEY_REPEAT:
            activate = 0;
            ClearInputKeyStatePreservingModifiers();
            switch ((short)event.value) {
            case SCAN_RETURN:
            case SCAN_S:
            case SCAN_C:
            case SCAN_SPACE:
#ifdef SDL_PORT
                if ((short)event.value == SCAN_S ||
                    (short)event.value == SCAN_C) {
                    int option = (short)event.value == SCAN_S ? 0 : 1;
                    for (menuIndex = 0; menuIndex < 4; menuIndex++) {
                        if (menuOptions[menuIndex] == option)
                            selectedIndex = menuIndex;
                    }
                    if (selectedIndex == -1)
                        break;
                }
#else
                if ((short)event.value == SCAN_S)
                    selectedIndex = 0;
                if ((short)event.value == SCAN_C &&
                    menuOptions[2] != -1)
                    selectedIndex = 1;
                if ((short)event.value == SCAN_M &&
                    menuOptions[2] != -1)
                    selectedIndex = 2;
#endif
                activate = 1;
                break;
            case SCAN_J:
                CalibrateJoystickInteractive(9, 9, 1, 1);
                break;
            default:
                MoveMenuPointerFromKeyboard(&event);
                break;
            }
            if (activate == 0)
                break;
            /* Fall through to the shared mouse/keyboard activation. */
        case INPUT_EVENT_BUTTON_DOWN:
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
            switch (selection) {
            case 0:
            case 1:
            case 2:
            case 3:
                state = (menuOptions[selection] + 1);
                break;
            default:
                state = 0;
                break;
            }
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
