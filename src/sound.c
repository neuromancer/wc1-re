/*
 *  Wave playback, volume settings and INSTALL.DAT.
 *
 *  Address range 0x42b400-0x42cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: playWAVE/PlaySnowStaticSound/LoadInstallDat; string band 0x46A46C-0x46A710.
 */
#include "wc1.h"

/* Function start: 0x42B410 */
void ReleaseFinishedSoundEntries(void)
{
    ActiveSoundEntry *entry = pActiveSoundHead;

    while (entry != 0) {
        ActiveSoundEntry *next = entry->next;

        if (ix_sound_is_playing(entry->sound) == 0) {
            ix_system_delete_sound(entry->sound);
            RemoveActiveSoundEntry(entry);
        }
        entry = next;
    }
}

/* Function start: 0x42B450 */
void StopSoundsUsingWave(const char *name)
{
    WaveTableEntry *wave;
    ActiveSoundEntry *entry;

    if (bIxAudioEnabled != 0) {
        wave = FindWaveTableEntryByName(name);
        if (wave != 0) {
            for (;;) {
                entry = FindActiveSoundEntryBySample(wave->sample);
                if (entry == 0)
                    break;
                ix_system_delete_sound(entry->sound);
                RemoveActiveSoundEntry(entry);
            }
        }
    }
}

/* Function start: 0x42B4A0 */
void playWAVE(const char *filename, int looping, int volume)
{
    WaveTableEntry *wave;
    ActiveSoundEntry *active;
    IxSound *sound;
    unsigned char *fileData;
    long fileSize;
    int file;

    if (bIxAudioEnabled != 0) {
        ReleaseFinishedSoundEntries();
        wave = FindWaveTableEntryByName(filename);
        if (wave != 0) {
            if (looping != 0) {
                active = AllocateActiveSoundEntry();
                active->sound = ix_system_new_sound(wave->sample);
                active->sound->ix_system_sound_set_volume(volume);
                ix_sound_start(active->sound);
                return;
            }
            sound = ix_system_new_sound(wave->sample);
            sound->ix_sound_set_delete_on_stop(1);
            sound->ix_system_sound_set_volume(volume);
            ix_sound_start(sound);
            return;
        }

        file = _open(filename, 0x8000);
        if (file == -1) {
            MessageBoxA(0, szPlayWaveOpenError,
                        filename, MB_ICONHAND);
            _exit(1);
        }
        fileSize = _filelength(file);
        fileData = (unsigned char *)malloc((unsigned int)fileSize);
        _read(file, fileData, (unsigned int)fileSize);
        _close(file);

        wave = AllocateWaveTableEntry();
        wave->sample = ix_system_new_sample();
        wave->sample->ix_sample_load_wav(fileData, fileSize);
        if (looping != 0) {
            wave->sample->flags |= 2;
            active = AllocateActiveSoundEntry();
            active->sound = ix_system_new_sound(wave->sample);
            active->sound->ix_system_sound_set_volume(volume);
            sound = active->sound;
        } else {
            sound = ix_system_new_sound(wave->sample);
            sound->ix_sound_set_delete_on_stop(1);
            sound->ix_system_sound_set_volume(volume);
        }
        ix_sound_start(sound);

        wave->name = (char *)malloc(strlen(filename) + 1);
        strcpy(wave->name, filename);
        free(fileData);
    }
}

/* Function start: 0x42B640 */
void stop_all_sounds(void)
{
#ifdef WC1_SDL
    Wc1SdlStopDosSoundEffects();
#endif
    ix_system_delete_all_sounds();
    ix_system_delete_all_samples();
    FreeWaveTable();
    if (pSnowStaticSound != 0) {
        /* The bulk delete already stops, unlinks, and frees every IxSound.
           The original's following calls therefore use a stale pointer. */
#ifndef WC1_SDL
        ix_sound_stop(pSnowStaticSound);
        ix_sound_release(pSnowStaticSound);
#endif
        pSnowStaticSound = 0;
        pSnowStaticWave = 0;
    }
}

/* Function start: 0x42B680 */
void PlaySnowStaticSound(void)
{
    unsigned char *fileData;
    long fileSize;
    int file;

    if (bIxAudioEnabled != 0) {
        ReleaseFinishedSoundEntries();
        if (pSnowStaticSound == 0) {
            file = _open("sfx22.wav", 0x8000);
            if (file == -1) {
                MessageBoxA(0,
                            "playWAVE Unable to open file 'sfx23",
                            "Notice", MB_ICONHAND);
                _exit(1);
            }
            fileSize = _filelength(file);
            fileData = (unsigned char *)malloc((unsigned int)fileSize);
            _read(file, fileData, (unsigned int)fileSize);
            _close(file);

            pSnowStaticWave = AllocateWaveTableEntry();
            pSnowStaticWave->sample = ix_system_new_sample();
            pSnowStaticWave->sample->ix_sample_load_wav(
                fileData, fileSize);
            pSnowStaticSound = ix_system_new_sound(
                pSnowStaticWave->sample);
            pSnowStaticSound->ix_system_sound_set_volume(50000);
            ix_sound_start(pSnowStaticSound);

            pSnowStaticWave->name =
                (char *)malloc(strlen("SNOW") + 1);
            strcpy(pSnowStaticWave->name, "SNOW");
            free(fileData);
        } else if (ix_sound_is_playing(
                       pSnowStaticSound) == 0) {
            ix_sound_start(pSnowStaticSound);
        }
    }
}

/* Function start: 0x42B7D0 */
void ServiceSoundSystem(void)
{
    ix_system_service_sounds();
}

/* Function start: 0x42B7E0 */
void SetSoundEffectsVolume(int volume)
{
    if (volume >= 0 && volume < 65000)
        ix_system_set_master_volume((unsigned short)volume);
    SoundDebugPrintf("Setting SFX Volume to %d", volume);
}

/* Function start: 0x42B810 */
LONG RegistryQueryValue(HKEY key, LPCSTR name, DWORD type,
                        LPBYTE data, DWORD size)
{
    return RegQueryValueExA(key, name, 0, &type, data, &size);
}

/* Function start: 0x42B840 */
void RegistryStoreValue(HKEY key, LPCSTR name, DWORD type,
                        const BYTE *data, DWORD size)
{
    RegSetValueExA(key, name, 0, type, data, size);
}

/* Function start: 0x42B870 */
void LoadVolumeSettingsFromRegistry(void)
{
    HKEY key;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        if (RegistryQueryValue(key, "MusicVolume", REG_DWORD,
                               (LPBYTE)&nMusicVolumeSetting,
                               sizeof(nMusicVolumeSetting)) !=
            ERROR_SUCCESS) {
            nMusicVolumeSetting = 0x14;
            RegistryStoreValue(key, "MusicVolume", REG_DWORD,
                               (const BYTE *)&nMusicVolumeSetting,
                               sizeof(nMusicVolumeSetting));
        }
        if (RegistryQueryValue(key, "SFXVolume", REG_DWORD,
                               (LPBYTE)&nSfxVolumeSetting,
                               sizeof(nSfxVolumeSetting)) !=
            ERROR_SUCCESS) {
            nSfxVolumeSetting = 0x14;
            RegistryStoreValue(key, "SFXVolume", REG_DWORD,
                               (const BYTE *)&nSfxVolumeSetting,
                               sizeof(nSfxVolumeSetting));
        }
        RegCloseKey(key);
    }
}

/* Function start: 0x42B930 */
void SaveVolumeSettingsToRegistry(void)
{
    HKEY key;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        RegistryStoreValue(key, "MusicVolume", REG_DWORD,
                           (const BYTE *)&nMusicVolumeSetting,
                           sizeof(nMusicVolumeSetting));
        RegistryStoreValue(key, "SFXVolume", REG_DWORD,
                           (const BYTE *)&nSfxVolumeSetting,
                           sizeof(nSfxVolumeSetting));
    }
    RegCloseKey(key);
}

/* Function start: 0x42B9A0 */
void DrawLaunchDoorFrame(short distance)
{
    short bounds[4];
    short scale;

    if (distance > 10) {
        scale = (short)(0x1a00L / distance);
        GetTransformedShapeBounds(
            &stSpaceBuffer,
            (short)((short)nScreenWidth >> 1),
            (short)((short)nScreenHeight >> 1),
            pLaunchDoorShape, 1, 0, scale, 0, bounds);
#ifdef WC1_SDL
        if (!Wc1SdlRecordSpaceSprite(
                &stSpaceBuffer, (short)(bounds[0] - 1),
                (short)((short)nScreenHeight >> 1),
                pLaunchDoorShape, 0, 0, scale, 0))
#endif
        DrawSpriteScaled(
            &stSpaceBuffer, (short)(bounds[0] - 1),
            (short)((short)nScreenHeight >> 1),
            pLaunchDoorShape, 0, 0, scale, 0);
#ifdef WC1_SDL
        if (!Wc1SdlRecordSpaceSprite(
                &stSpaceBuffer,
                (short)((short)nScreenWidth >> 1),
                (short)((short)nScreenHeight >> 1),
                pLaunchDoorShape, 1, 0, scale, 0))
#endif
        DrawSpriteScaled(
            &stSpaceBuffer,
            (short)((short)nScreenWidth >> 1),
            (short)((short)nScreenHeight >> 1),
            pLaunchDoorShape, 1, 0, scale, 0);
#ifdef WC1_SDL
        if (!Wc1SdlRecordSpaceSprite(
                &stSpaceBuffer, bounds[2],
                (short)((short)nScreenHeight >> 1),
                pLaunchDoorShape, 2, 0, scale, 0))
#endif
        DrawSpriteScaled(
            &stSpaceBuffer, bounds[2],
            (short)((short)nScreenHeight >> 1),
            pLaunchDoorShape, 2, 0, scale, 0);
    }
}

/* Function start: 0x42BA90 */
void LaunchPlayerShip(void)
{
    short doorDistances[4];
    signed char distanceStep;
    signed char frame;
    signed char door;

    doorDistances[0] = 50;
    doorDistances[1] = 40;
    doorDistances[2] = 30;
    doorDistances[3] = 20;
    distanceStep = 1;

    spacetrack(changetrack(), 1, 0);
    if (bEscapePressed == 0) {
        pLaunchDoorShape =
            (unsigned char *)FetchDiskPacketRetrying(1, 7, 0);
        nCannedSceneMode = 1;
        force_view(0, 0);
        PlaySfxWaveFileByNumber(20, -1, 0);
        nFrameSkipCounter = 1;
        bEscapePressed = 0;
        frame = 0;
        do {
            PumpWindowMessages();
            if (RefreshCockpitStatus() != 0) {
                door = 0;
                do {
                    DrawLaunchDoorFrame(doorDistances[door]);
                    doorDistances[door] =
                        (short)(doorDistances[door] - distanceStep);
                    door++;
                } while (door < 4);
                dump_buffer_to_screen();
                update_cockpit();
            }
            DIBslam();
            DIBslamReal();
            if (bEscapePressed == 1)
                break;
            if (frame % 5 == 0)
                distanceStep++;
            frame++;
        } while (frame < 25);

        if (bEscapePressed != 0) {
            StopMusicUnlessSuppressed();
            spacetrack(changetrack(), 1, 0);
        }
        ReleasePacketHandle(pLaunchDoorShape);
    } else {
        force_view(0, 0);
    }

    DIBslam();
    DIBslamReal();
    clear_view_buffer();
    nCannedSceneMode = 0;
    ResetSoundState();
    bEscapePressed = 0;
}

/* Function start: 0x42BC00 */
unsigned int ShowCarrierLaunchSequence(signed char sceneObject)
{
    unsigned char *carrierShape;
    unsigned char *actorShape;
    unsigned char *fighterShape;
    short approachScale;
    short approachDistance;
    short fighterScreenX;
    short fighterScreenY;
    short carrierScreenX;
    short actorX;
    short actorY;
    short frame;
    short object;

    bIntroSceneResourcesActive = 0;
    carrierScreenX = 180;
    free_ship(1);
    free_ship(2);
    free_ship(3);
    remove_nav_point_objects();
    ResetSoundState();
    PreloadMusicTrackHook(0x1c);
    spacetrack(0x1c, 2, 1);
    carrierShape =
        (unsigned char *)FetchDiskPacketRetrying(1, 8, 0);
    actorShape =
        (unsigned char *)FetchDiskPacketRetrying(1, 4, 0);
    pScrambleViewport = &stSpaceBuffer;
    object = (short)sceneObject;
    fighterShape = aObjectTypeData[
        stCampaignState.playerShipType].shapeSet;
    fighterScreenY = 64;
    fighterScreenX = 20;
    nScriptedViewObject = object;
    initialize_scripted_view(&asCarrierLaunchViewData[2]);
    nScrambleBackgroundY = 64;
    nScrambleBackgroundRightX = 520;
    asObjectFlip[0] = 0;
    asObjectViewFrame[0] = 36;
    asObjectScreenAngle[0] = 180;
    asObjectDistance[0] = 300;
    apObjectShape[0] = fighterShape;
    apObjectShape[object] = carrierShape;
    asObjectFlip[object] = 0;
    asObjectViewFrame[object] = 3;
    bEscapePressed = 0;
    asObjectScreenAngle[object] = 0;
    asObjectScreenScale[object] = 0x100;
    asObjectDistance[object] = 2000;
    nFrameSkipCounter = 1;
    frame = 0;
    approachDistance = 20;
    do {
        PumpWindowMessages();
        aeObjectClass[0] = OBJECT_CLASS_NULL;
        aeObjectClass[object] = OBJECT_CLASS_NULL;
        set_eye_direction_and_position();
        nFrameSkipCounter--;
        if (nFrameSkipCounter < 1) {
            nFrameSkipCounter = nFrameSkip;
            nRenderedSpaceFrame++;
            UpdateSpacePaletteFade();
            clear_view_buffer();
            house_keep_objects();
            update_objects_in_space();
            transform_objects_to_your_view();
            update_star_field();
            if (frame < 24) {
                fighterScreenY = (short)(fighterScreenY +
                    asCarrierLaunchApproachDeltaX[frame]);
                asObjectViewFrame[0] =
                    acCarrierLaunchApproachFrames[frame];
            } else if (frame < 48) {
                fighterScreenY = (short)(fighterScreenY -
                    asCarrierLaunchApproachDeltaX[47 - frame]);
                asObjectViewFrame[0] =
                    acCarrierLaunchApproachFrames[47 - frame];
            }
            aeObjectClass[0] = OBJECT_CLASS_SHIP;
            approachScale = (short)(
                ((unsigned int)(unsigned short)asObjectScale[0]
                    << 4) / approachDistance);
            aeObjectClass[object] = OBJECT_CLASS_SHIP;
            asObjectScreenX[0] =
                (short)(fighterScreenX - nViewCenterX);
            asObjectScreenY[0] =
                (short)(fighterScreenY - nViewCenterY);
            asObjectScreenScale[0] = approachScale;
            asObjectScreenX[object] =
                (short)(nScrambleBackgroundRightX -
                        nViewCenterX);
            asObjectScreenY[object] =
                (short)(nScrambleBackgroundY -
                        nViewCenterY);
            sort_object_depth();
#ifdef WC1_SDL
            Wc1SdlBeginSpaceFrame(
                pScreenViewportGeometry,
                (int)cScreenViewportMode,
                bCockpitlessView > 0,
                (unsigned char)cPrimaryViewBufferColour);
#endif
            draw_sorted_objects_to_buffer();
            dump_buffer_to_screen();
        }
        fighterScreenX = (short)(fighterScreenX + 2);
        nSpaceFrame++;
        nScrambleBackgroundRightX =
            (short)(nScrambleBackgroundRightX - 2);
        approachDistance = (short)(approachDistance + 2);
        asObjectDistance[0] =
            (short)(asObjectDistance[0] + 10);
        AddFixedVectors(&aShipPosition[61],
                        &aShipVelocity[61],
                        &aShipPosition[61]);
        if (bEscapePressed == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 100);

    aeObjectClass[0] = OBJECT_CLASS_NULL;
    aeObjectClass[object] = OBJECT_CLASS_NULL;
    if (bEscapePressed != 1) {
        copy_frame(object, 61);
        aShipPosition[61] = aShipPosition[object];
        nScrambleBackgroundRightX = 0;
        fighterScreenX = 200;
        fighterScreenY = 32;
        nScrambleBackgroundY = 0;
        asObjectCollisionRadius[61] =
            asObjectCollisionRadius[object];
        approachDistance = 100;
        ConfigureScrambleActor(100, 80, 1, 0, actorShape,
                               0x100, 0, 0, 0);
        ConfigureScrambleActor(116, 130, 0, 0, actorShape,
                               0x100, 0, 0, 1);
        ConfigureScrambleActor(300, 110, -4, 0, actorShape,
                               0xc0, 0, 0x10, 3);
        ConfigureScrambleActor(301, 110, -4, 0, actorShape,
                               0xc0, 0, 0x10, 4);
        PlaySfxWaveFileByNumber(18, -1, 0);
        frame = 0;
        actorX = 60;
        actorY = 10;
        nFrameSkipCounter = 1;
        do {
            PumpWindowMessages();
            alter_yaw(-1, 61);
            if (RefreshCockpitStatus() != 0) {
                DrawSpriteDefault(
                    &stSpaceBuffer,
                    (short)(nScrambleBackgroundRightX + 239),
                    nScrambleBackgroundY,
                    carrierShape, 0);
                DrawSpriteDefault(
                    &stSpaceBuffer,
                    (short)(nScrambleBackgroundRightX + 240),
                    nScrambleBackgroundY,
                    carrierShape, 1);
                DrawScrambleActor(0);
                approachScale = (short)(0x6000L / approachDistance);
                DrawSpriteScaled(&stSpaceBuffer, fighterScreenX,
                                 fighterScreenY, fighterShape, 16, 0,
                                 approachScale, 0);
                DrawScrambleActor(3);
                DrawScrambleActor(4);
                DrawScrambleActor(1);
                DrawSpriteDefault(
                    &stSpaceBuffer,
                    (short)(nScrambleBackgroundRightX + 60),
                    (short)(nScrambleBackgroundY + 10),
                    actorShape, 16);
                DrawSpriteDefault(
                    &stSpaceBuffer,
                    (short)(nScrambleBackgroundRightX + 80),
                    (short)(nScrambleBackgroundY + 134),
                    actorShape, 8);
                DrawSpriteDefault(&stSpaceBuffer, carrierScreenX,
                                  nScrambleBackgroundY,
                                  carrierShape, 2);
                WaitForVerticalBlankThunk();
                dump_buffer_to_screen();
            }
            nScrambleBackgroundRightX =
                (short)(nScrambleBackgroundRightX + 2);
            carrierScreenX = (short)(carrierScreenX + 4);
            fighterScreenX = (short)(fighterScreenX - 2);
            fighterScreenY++;
            approachDistance--;
            if (sceneObject != 0) {
                ((void (__cdecl *)(int, int, int, int))PaletteFadeHook)(
                    (int)sceneObject, 10, 0, 0);
            }
            if (bEscapePressed == 1)
                break;
            DIBslam();
            DIBslamReal();
            frame++;
        } while (frame < 35);

        if (bEscapePressed != 1) {
            nFrameSkipCounter = 1;
            frame = 0;
            do {
                PumpWindowMessages();
                if (RefreshCockpitStatus() != 0) {
                    DrawSpriteDefault(
                        &stSpaceBuffer,
                        (short)(nScrambleBackgroundRightX + 239),
                        nScrambleBackgroundY,
                        carrierShape, 0);
                    DrawSpriteDefault(
                        &stSpaceBuffer,
                        (short)(nScrambleBackgroundRightX + 240),
                        nScrambleBackgroundY,
                        carrierShape, 1);
                    DrawScrambleActor(0);
                    DrawSpriteScaled(&stSpaceBuffer, fighterScreenX,
                                     fighterScreenY, fighterShape, 16, 0,
                                     approachScale, 0);
                    DrawScrambleActor(3);
                    DrawScrambleActor(4);
                    DrawScrambleActor(1);
                    DrawSpriteDefault(
                        &stSpaceBuffer,
                        (short)(nScrambleBackgroundRightX + actorX),
                        (short)(nScrambleBackgroundY + actorY),
                        actorShape, 16);
                    DrawSpriteDefault(
                        &stSpaceBuffer,
                        (short)(nScrambleBackgroundRightX + 80),
                        (short)(nScrambleBackgroundY + 134),
                        actorShape, 8);
                    DrawSpriteDefault(&stSpaceBuffer, carrierScreenX,
                                      nScrambleBackgroundY,
                                      carrierShape, 2);
                    WaitForVerticalBlankThunk();
                    dump_buffer_to_screen();
                }
                frame++;
                if (frame < 9) {
                    fighterScreenX = (short)(fighterScreenX +
                        aCarrierLaunchFighterPath[frame].x);
                    fighterScreenY = (short)(fighterScreenY +
                        aCarrierLaunchFighterPath[frame].y);
                } else if (frame < 23) {
                    if (frame == 9)
                        PlaySfxWaveFileByNumber(11, -1, 0);
                    fighterScreenY = (short)(fighterScreenY +
                        asCarrierLaunchFighterDeltaY[frame - 7]);
                }
                if (frame == 23) {
                    ((void (__cdecl *)(int, int))
                        FlushSoundEffectsAndLog)((int)sceneObject, 0);
                    PlaySfxWaveFileByNumber(19, -1, 0);
                }
                actorX++;
                if (frame % 7 == 0)
                    actorY--;
                DIBslam();
                DIBslamReal();
            } while (bEscapePressed != 1 && frame < 50);
        }
    }

    bEscapePressed = 0;
    ResetSoundState();
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x1c);
    free_ship(0);
    ReleasePacketHandle(carrierShape);
    ReleasePacketHandle(actorShape);
    bScriptedView = 0;
    bIntroSceneResourcesActive = 1;
    return 0;
}

/* Function start: 0x42C410 */
void FxDriverShutdownHook(void)
{
}

/* Function start: 0x42C420 */
unsigned short InitializeDiskPromptTextContext(void)
{
    short textWidth;
    short textHeight;

    nDiskPromptBorderColour = 0x50;
    if (stScreen.pixels == 0)
        InitializeGameTextContexts();
    stDiskPromptTextContext = stDefaultTextContext;
    stDiskPromptTextContext.viewport =
        &stDiskPromptViewport;
    stDiskPromptTextContext.text =
        szTextScratchBuffer;
    stDiskPromptTextContext.alignment = 2;
    stDiskPromptViewport = stScreen;
    textWidth =
        (short)(MeasureTextPixelWidthClamped(
                    szPressAnyKeyWhenReady) +
                10);
    textHeight =
        (short)((ReadWord((unsigned short *)
                              stDiskPromptTextContext.font) +
                 1) *
                3);
    ((short *)&dwDiskPromptTopLeft)[1] =
        (short)(100 - textHeight / 2);
    ((short *)&dwDiskPromptTopLeft)[0] =
        (short)(160 - textWidth / 2);
    ((short *)&dwDiskPromptBottomRight)[1] =
        (short)(((short *)&dwDiskPromptTopLeft)[1] +
                textHeight);
    ((short *)&dwDiskPromptBottomRight)[0] =
        (short)(((short *)&dwDiskPromptTopLeft)[0] + textWidth);
    InitializeTextContextFromFont(&stDiskPromptTextContext, 1,
                                  cBlackColour,
                                  (signed char)cViewportClearColour);
    bGraphicsActive = 1;
    return 0;
}

/* Function start: 0x42C510 */
unsigned short RewriteDiskFileGraphicsExtensions(short videoMode)
{
    DiskFileRecord *record;
    char *extensionPosition;
    char extension;

    record = pDiskFileRecords;
    switch (videoMode) {
    case 0:
        extension = 'v';
        break;
    case 1:
        extension = 'e';
        break;
    case 3:
        extension = 't';
        break;
    }

    while (record->name[0] != '\0') {
        extensionPosition = strrchr(record->name, '.');
#ifdef WC1_SDL
        if (extensionPosition != 0) {
            extensionPosition++;
            if (toupper((int)*extensionPosition) == 'V')
                *extensionPosition = extension;
        }
#else
        if (extensionPosition++ != 0 &&
            toupper((int)*extensionPosition) == 'V')
            *extensionPosition = extension;
#endif
        record++;
    }
    return 0;
}

/* Function start: 0x42C580 */
short LoadWingCmdrCfgFile(short argc, char **argv)
{
    FILE *file;
    short argumentCount;
    char *destination;
    short argumentIndex;
#ifdef WC1_SDL
    char resolvedPath[PATH_MAX];
#endif

    argumentIndex = 1;
    argumentCount = 0;
    destination = szTextScratchBuffer;
#ifdef WC1_SDL
    if (Wc1SdlResolvePath("WINGCMDR.CFG", resolvedPath,
                          sizeof(resolvedPath)))
        file = fopen(resolvedPath, "rt");
    else
        file = 0;
#else
    file = fopen("WINGCMDR.CFG", "rt");
#endif
    while (file != 0) {
        if ((short)fscanf(file, "%s", destination) == -1) {
            fclose(file);
            break;
        }
        pStartupArguments[argumentCount++] =
            destination;
        destination = strchr(destination, 0) + 1;
    }

    while (argc-- != 0) {
        strcpy(destination, argv[argumentIndex]);
        pStartupArguments[argumentCount++] = destination;
        argumentIndex++;
        destination = strchr(destination, 0) + 1;
    }
    return (short)(argumentCount - 1);
}

/* Function start: 0x42C660 */
unsigned short LoadInstallDat(void)
{
    DiskFileRecord *records;
    DiskFileRecord *record;
    DiskFileRecord *entry;
    unsigned int size;
    short file;
    short maximumId;

    maximumId = 0;
    SystemDebugPrintf("Loading INSTALL.DAT\n");
    file = OpenDataFileOrDie("install.dat");
    if (file == -1) {
        SystemDebugPrintf("Unable to open INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }
    size = (unsigned int)_filelength(file);
    records = (DiskFileRecord *)AllocateTaggedMemory(size, 0);
    if (records == 0) {
        SystemDebugPrintf("Unable to load INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }
    ReadDataFileAtOffset(file, 0, size, records);
    CloseDataFile(file);

    record = records;
    while (record->name[0] != 0) {
        if (maximumId < record->logicalFile &&
            record->logicalFile != 0xff)
            maximumId = record->logicalFile;
        record++;
    }
    maximumId++;

#ifdef WC1_SDL
    pDiskFileRecords =
        (DiskFileRecord *)AllocateTaggedMemory(
            sizeof(DiskFileRecord) * 78, 0);
    if (pDiskFileRecords != 0)
        memset(pDiskFileRecords, 0,
               sizeof(DiskFileRecord) * 78);
#else
    pDiskFileRecords =
        (DiskFileRecord *)AllocateTaggedMemory(0x4b0, 0);
    memset(pDiskFileRecords, 0,
           (maximumId + 1) * sizeof(DiskFileRecord));
#endif
    if (pDiskFileRecords == 0) {
        SystemDebugPrintf("Unable to copy INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }

    entry = pDiskFileRecords;
    while (maximumId > 0) {
        entry->name[0] = ' ';
        entry++;
        maximumId--;
    }

    record = records;
    while (record->name[0] != 0) {
        if (record->logicalFile != 0xff)
            pDiskFileRecords[record->logicalFile] = *record;
        record++;
    }
    ReleasePacketHandle(records);
    pDiskFileRecords++;
#ifdef WC1_SDL
    if (Wc1SdlUsingDosData())
        Wc1SdlCompleteDosInstallTable(pDiskFileRecords);
#endif
    return 0;
}

/* Function start: 0x42C800 */
void show_damage_disp(void)
{
    signed char component;
    char message[40];

    nDamageDisplayState = 0;
    cDamagedComponentCount = 0;
    for (component = 0; component < 9; component++) {
        if (acPlayerComponentDamage[component] >= 1)
            cDamagedComponentCount++;
    }

    set_new_vdu(0);
    DrawTextAt(&stLeftVduTextContext,
               stLeftVdu.left, stLeftVdu.top,
               szDamageReport, 2);
    DrawViewportLine(&stLeftVdu,
                     (short)(stLeftVdu.left + 2),
                     (short)(stLeftVdu.top + 6),
                     (short)(stLeftVdu.right - 2),
                     (short)(stLeftVdu.top + 6),
                     cPrimaryTextColour);
    if (cDamagedComponentCount == 0) {
        DrawTextAt(&stLeftVduTextContext, stLeftVdu.left,
                   (short)(stLeftVdu.top + 20),
                   szNoInternalDamage, 2);
        return;
    }

    component = cCockpitView;
    nWeaponDisplayOriginX =
        (short)(aWeaponDisplayOrigins[component].x +
                stLeftVdu.left);
    nWeaponDisplayOriginY =
        (short)(aWeaponDisplayOrigins[component].y +
                stLeftVdu.top);
    DrawSpriteDefault(&stLeftVdu, nWeaponDisplayOriginX,
                      nWeaponDisplayOriginY,
                      pCockpitWeaponShape, 0);
    sprintf(message, szDamagedUnitCountFormat,
            (int)cDamagedComponentCount,
            (int)(cDamagedComponentCount == 1 ? ' ' : 's'));
    ShowComponentHitHudMessage(message, cPrimaryTextColour, -1);
}

/* Function start: 0x42C970 */
void UpdateDamageDisplay(void)
{
    signed char componentCount;
    signed char component;
    signed char attempts;
    signed char damage;

    componentCount = 0;
    for (component = 0; component < 9; component++) {
        if (acPlayerComponentDamage[component] >= 1)
            componentCount++;
    }

    if ((short)componentCount !=
        (short)cDamagedComponentCount) {
        cDamagedComponentCount = componentCount;
        InvalidateVduMode(0);
        return;
    }
    cDamagedComponentCount = componentCount;
    if (componentCount == 0)
        return;

    if (bCockpitlessView == 0) {
        nDamageDisplayTicks--;
        if (nDamageDisplayTicks > 0)
            return;
        if (nDamageDisplayPhase == 1) {
            component = cDamageDisplayComponent;
            attempts = 0;
            nDamageDisplayTicks = 50;
            do {
                component++;
                if (component >= 9)
                    component = 0;
                damage = acPlayerComponentDamage[component];
                nDisplayedComponentDamage = (int)damage;
                if (nDisplayedComponentDamage >= 1) {
                    cDamageDisplayComponent = component;
                    break;
                }
                attempts++;
            } while (attempts < 9);

            sprintf(
                szDamageStatusText,
                szDamageStatusFormat,
                apszComponentNames[
                    cDamageDisplayComponent],
                apszDamageSeverityNames[(int)damage]);
            DrawTextAt(&stLeftVduTextContext,
                       (short)(stLeftVdu.left + 1),
                       (short)(stLeftVdu.top + 7),
                       szDamageStatusText, 2);
            cDamageDisplayFrame =
                (signed char)abDamageDisplayFrames[
                    cDamageDisplayComponent];
            stDamageSpritePosition =
                aDamageDisplayPositions[
                    cDamageDisplayComponent];
            stDamageSpritePosition.x =
                (short)(stDamageSpritePosition.x +
                        nWeaponDisplayOriginX);
            stDamageSpritePosition.y =
                (short)(stDamageSpritePosition.y +
                        nWeaponDisplayOriginY);
            CaptureSpriteBackground(
                &stLeftVdu, pDamageDisplayBackground,
                stDamageSpritePosition.x,
                stDamageSpritePosition.y,
                pCockpitWeaponShape,
                (short)cDamageDisplayFrame);
            DrawViewportLine(
                &stLeftVdu, (short)(stLeftVdu.left + 36),
                (short)(stLeftVdu.top + 22),
                stDamageSpritePosition.x,
                stDamageSpritePosition.y,
                0xa9);
            DrawSpriteDefault(
                &stLeftVdu, stDamageSpritePosition.x,
                stDamageSpritePosition.y,
                pCockpitWeaponShape,
                (short)cDamageDisplayFrame);
        } else {
            RestoreSpriteBackground(
                &stLeftVdu, pDamageDisplayBackground,
                stDamageSpritePosition.x,
                stDamageSpritePosition.y,
                pCockpitWeaponShape,
                (short)cDamageDisplayFrame);
            stLeftVduTextContext.colour = cBlackColour;
            DrawTextAt(&stLeftVduTextContext,
                       (short)(stLeftVdu.left + 1),
                       (short)(stLeftVdu.top + 7),
                       szDamageStatusText, 2);
            stLeftVduTextContext.colour = cPrimaryTextColour;
            DrawViewportLine(
                &stLeftVdu, (short)(stLeftVdu.left + 36),
                (short)(stLeftVdu.top + 22),
                stDamageSpritePosition.x,
                stDamageSpritePosition.y,
                cBlackColour);
            nDamageDisplayTicks = 2;
        }
        nDamageDisplayPhase =
            nDamageDisplayPhase == 0;
        return;
    }

    nDamageDisplayTicks--;
    if (nDamageDisplayTicks <= 0) {
        component = cDamageDisplayComponent;
        attempts = 0;
        nDamageDisplayTicks = 50;
        for (;;) {
            component++;
            if (component >= 9)
                component = 0;
            nDisplayedComponentDamage =
                (int)acPlayerComponentDamage[component];
            if (nDisplayedComponentDamage >= 1) {
                cDamageDisplayComponent = component;
                return;
            }
            attempts++;
            if (attempts >= 9)
                return;
        }
    }

    sprintf(
        szDamageStatusText,
        szDamageStatusFormatHighRes,
        apszComponentNames[cDamageDisplayComponent],
        apszDamageSeverityNames[
            nDisplayedComponentDamage]);
    DrawTextAt(&stLeftVduTextContext, (short)(stLeftVdu.left + 1),
               (short)(stLeftVdu.top + 7),
               szDamageStatusText, 2);
    cDamageDisplayFrame =
        (signed char)abDamageDisplayFrames[
            cDamageDisplayComponent];
    stDamageSpritePosition =
        aDamageDisplayPositions[
            cDamageDisplayComponent];
    stDamageSpritePosition.x =
        (short)(stDamageSpritePosition.x +
                nWeaponDisplayOriginX);
    stDamageSpritePosition.y =
        (short)(stDamageSpritePosition.y +
                nWeaponDisplayOriginY);
    CaptureSpriteBackground(
        &stLeftVdu, pDamageDisplayBackground,
        stDamageSpritePosition.x,
        stDamageSpritePosition.y,
        pCockpitWeaponShape,
        (short)cDamageDisplayFrame);
    DrawViewportLine(&stLeftVdu,
                     (short)(stLeftVdu.left + 36),
                     (short)(stLeftVdu.top + 22),
                     stDamageSpritePosition.x,
                     stDamageSpritePosition.y, 0xa9);
    DrawSpriteDefault(&stLeftVdu,
                      stDamageSpritePosition.x,
                      stDamageSpritePosition.y,
                      pCockpitWeaponShape,
                      (short)cDamageDisplayFrame);
}

/* Function start: 0x42CDA0 */
unsigned short GetJoystickPresentUnused(void)
{
    return 1;
}
