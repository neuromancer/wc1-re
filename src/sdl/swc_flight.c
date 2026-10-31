/* SDL2 first-mission host. Mission state, ship setup, flight dynamics,
 * navigation and player weapons belong to the existing WC1 core. Mac
 * projection, cinematic cameras and object definitions live at this boundary. */
#include "wc1.h"
#include "swc.h"
#include "swc_objects.h"

#include <stdio.h>
#include <string.h>

typedef struct SwcFlightImages {
    SwcRasterImage *views;
    uint32_t count;
    int unavailable;
} SwcFlightImages;

enum SwcFlightScene {
    SWC_FLIGHT_TITLE,
    SWC_FLIGHT_DEATH,
    SWC_FLIGHT_EJECTION
};

static int swcFlightActive;
unsigned char aSwcWeaponAmmo[10][10]; /* Mac loadout byte +9 per slot. */
static int swcMouseAfterburner;
static int swcRightClickPending;
static Uint32 swcLastRightClick;
static int swcReleaseWeaponPending;
static int swcGunFirePending;
static int swcBothButtonsHeld;
static int swcJoystickSteering;
static int swcJoystickAfterburner;
static int swcJoystickClickPending;
static unsigned int swcJoystickButtons;
static Uint32 swcLastJoystickClick;

/* Borrowed only while SdlRunSwcMission owns these presentation resources. */
static SDL_Window *swcCinemaWindow;
static SDL_Renderer *swcCinemaRenderer;
static const SwcCmf *swcCinemaCockpitCmf;
static const SwcCmf *swcCinemaSpaceCmf;
static const SDL_Color *swcCinemaColors;
static SwcFlightImages *swcCinemaImages;
static SDL_Texture *swcRasterCanvas;
static SwcTextFont swcPauseFont;
static SDL_Texture *swcStarTextures[10];
static SDL_Rect swcStarBounds[10];
static int swcObjectDistances[SPACE_OBJECT_COUNT];
int nSwcCinemaResult;

static void SwcTransformObjectsToView(void);

/* CODE_02 generate_stars +0x38cc: eight dust particles in a 5,000-unit
 * volume, then seven stars using PLNT/1 frames 32..41. Random3DO's upper
 * bound is exclusive; WC1's generate_stars uses 1,400 and includes frame 42. */
static void SwcGenerateStars(void)
{
    FixedVector origin = {0};
    short object;
    short distance;
    short pitch;
    short yaw;

    for (object = 34; object < 49; object++) {
        if (object < 42) {
            distance = RandomBelowOrEqual(4999);
            start_dust(object, origin, distance,
                signed_random(distance) * 256, signed_random(distance) * 256);
        } else {
            aeObjectClass[object] = OBJECT_CLASS_STAR;
            pitch = signed_random(45);
            yaw = signed_random(45);
            copy_frame(EYE_OBJECT, 63);
            rotate_about_j(yaw, &aShipRightVector[63], &aShipFwdVec[63]);
            rotate_about_i(pitch, &aShipUpVector[63], &aShipFwdVec[63]);
            ScaleFixedVector(&aShipFwdVec[63], 15000 * 256, &aShipPositionsArray[object]);
            asObjectViewFrames[object] = (short)(32 + RandomBelowOrEqual(9));
        }
    }
}

/* CODE_02 update_star_field +0x39f2. Retain the common hazard objects,
 * with the Mac's camera offset, respawn ranges and exclusive choices. */
static void SwcUpdateStarField(void)
{
    FixedVector cameraMotion;
    FixedVector viewMotion;
    FixedVector origin;
    int distance;
    int shift;
    short choice;
    short object;
    short pitch;
    short yaw;

    vPreviousStarFieldMotion = vStarFieldMotion;
    ScaleFixedVector(&aShipFwdVec[EYE_OBJECT], 200 * 256, &cameraMotion);
    ScaleFixedVector(&aShipVelocity[EYE_OBJECT], 20 * 256, &vStarFieldMotion);
    AddFixedVectors(&cameraMotion, &vStarFieldMotion, &vStarFieldMotion);
    ComputeVectorDelta(&vPreviousStarFieldMotion, &vStarFieldMotion, &origin);
    for (object = 34; object < 49; object++) {
        if (asObjectScreenXList[object] != (short)0x8001)
            continue;
        choice = RandomBelowOrEqual(6);
        if (pActiveHazardField == NULL) {
            if (aeObjectClass[object] == OBJECT_CLASS_ASTEROID ||
                (int)aeObjectClass[object] == 0x21 || aeObjectClass[object] == OBJECT_CLASS_NULL) {
                set_objects_data(object, OBJECT_TYPE_SPACE_DUST, -1);
                choice = 0;
            }
        } else if (object < 42) {
            extra_hazard(object);
        }
        if (aeObjectClass[object] == OBJECT_CLASS_STAR && choice == 0) {
            if (anObjectYawRotation[0] == 0 && anObjPitchRot[0] == 0)
                continue;
            copy_frame(EYE_OBJECT, 63);
            if (anObjPitchRot[0] != 0) {
                pitch = anObjPitchRot[0] < 0 ? -45 : 45;
                yaw = signed_random(45);
            } else {
                /* Random3DO(0,1) is zero, so pitch wins when both turn. */
                yaw = anObjectYawRotation[0] < 0 ? -45 : 45;
                pitch = signed_random(45);
            }
            rotate_about_i(pitch, &aShipUpVector[63], &aShipFwdVec[63]);
            rotate_about_j(yaw, &aShipRightVector[63], &aShipFwdVec[63]);
            ScaleFixedVector(&aShipFwdVec[63], 15000 * 256, &aShipPositionsArray[object]);
            asObjectViewFrames[object] = (short)(32 + RandomBelowOrEqual(4));
            break;
        }
        if (aeObjectClass[object] == OBJECT_CLASS_DUST && choice < 2) {
            transform_to_objects_frame(&aShipVelocity[EYE_OBJECT], &viewMotion, EYE_OBJECT);
            ScaleFixedVector(&viewMotion, 10 * 256, &viewMotion);
            if (viewMotion.z < 0)
                distance = asObjectCollisionRadius[EYE_OBJECT] + RandomBelowOrEqual(141);
            else
                distance = asObjectCollisionRadius[EYE_OBJECT] * 2 + RandomBelowOrEqual(1249) +
                    RandomBelowOrEqual((short)SDL_max(0, (viewMotion.z >> 8) - 1)) * 2;
            shift = viewMotion.z <= 0 ? 512 : 256;
            start_dust(object, origin, (short)distance,
                viewMotion.x + signed_random((short)(distance >> 1)) * shift,
                viewMotion.y + signed_random((short)(distance >> 1)) * shift);
            break;
        }
    }
    if (pActiveHazardField != NULL)
        update_hazards();
}

/* CODE_02 shrink_vector +0x57c8 and shrink +0x5828. The Mac always
 * halves once and repeats until every component is below 0xf00. */
static void SwcShrinkCameraVector(FixedVector *vector)
{
    do {
        vector->x >>= 1;
        vector->y >>= 1;
        vector->z >>= 1;
    } while (vector->x <= -0xf00 || vector->x >= 0xf00 ||
             vector->y <= -0xf00 || vector->y >= 0xf00 ||
             vector->z <= -0xf00 || vector->z >= 0xf00);
}

/* CODE_04 point_at +0x1aa4, using the Mac vector reduction above. */
static void SwcPointCameraAt(short object, FixedVector point)
{
    FixedVector direction;

    ComputeVectorDelta(&aShipPositionsArray[object], &point, &direction);
    SwcShrinkCameraVector(&direction);
    aShipFwdVec[object] = direction;
    fix_objects_ijk(object);
}

/* CODE_02 battle_view +0x58f6. Same fleet-centre calculation as WC1's
 * SetFleetOverviewView, but Mac point_at uses a different reduction limit. */
static void SwcBattleView(void)
{
    FixedVector centre = {0};
    FixedVector orientation = {255, 255, 255};
    FixedVector offset;
    int ships = 0;
    int maximumRange = 1200 * 256;
    int playerRange = maximumRange;
    int distance;
    int object;

    for (object = 0; object < 10; object++) {
        if (aeObjectClass[object] >= OBJECT_CLASS_SHIP)
            ships++;
    }
    if (ships > 1) {
        for (object = 0; object < 10; object++) {
            if (aeObjectClass[object] >= OBJECT_CLASS_SHIP) {
                centre.x += aShipPositionsArray[object].x / ships;
                centre.y += aShipPositionsArray[object].y / ships;
                centre.z += aShipPositionsArray[object].z / ships;
            }
        }
        for (object = 9; object >= 0; object--) {
            if (aeObjectClass[object] < OBJECT_CLASS_SHIP)
                continue;
            ComputeVectorDelta(&centre, &aShipPositionsArray[object], &offset);
            distance = Vector_magnitude(&offset);
            maximumRange = SDL_max(maximumRange, distance);
            if (object == 0)
                playerRange = distance;
        }
        if (maximumRange <= 8000 * 256)
            playerRange = maximumRange;
    } else {
        position_relative_ijk(&centre, 0, 400, 400, 400);
    }
    distance = (playerRange >> 3) * 9 + 700 * 256;
    aShipPositionsArray[63] = centre;
    aShipRightVector[63] = orientation;
    aShipUpVector[63] = orientation;
    SwcPointCameraAt(63, aShipPositionsArray[0]);
    ScaleFixedVector(&aShipRightVector[63], distance >> 2, &offset);
    AddFixedVectors(&aShipPositionsArray[63], &offset, &aShipPositionsArray[63]);
    ScaleFixedVector(&aShipUpVector[63], 150 * 256, &offset);
    AddFixedVectors(&aShipPositionsArray[63], &offset, &aShipPositionsArray[63]);
    ScaleFixedVector(&aShipFwdVec[63], distance, &offset);
    AddFixedVectors(&aShipPositionsArray[63], &offset, &aShipPositionsArray[63]);
    aShipPositionsArray[EYE_OBJECT] = aShipPositionsArray[63];
    aShipRightVector[EYE_OBJECT] = orientation;
    aShipUpVector[EYE_OBJECT] = orientation;
    SwcPointCameraAt(EYE_OBJECT,
        maximumRange < 10000 * 256 ? centre : aShipPositionsArray[0]);
}

/* CODE_02 set_eye_direction_and_position +0x25d8, views 4 and 14.
 * Keep the shared cockpit/autopilot cases; scene view 16 has its own updater. */
static void SwcUpdateFlightCamera(void)
{
    FixedVector direction;
    FixedVector destination;
    FixedVector delta;

    if (nCameraViewMode == 14) {
        SwcBattleView();
    } else if (nCameraViewMode == 4) {
        if (aeObjectClass[cViewObject] != OBJECT_CLASS_NULL) {
            ComputeVectorDelta(&aShipPositionsArray[EYE_OBJECT],
                &aShipPositionsArray[cViewObject], &direction);
            destination = aShipFwdVec[0];
            SetVectorFixedPoint(&destination,
                (short)(bAlternateChaseView ? -500 : -700));
            AddFixedVectors(&destination, &aShipPositionsArray[cViewObject], &destination);
            ComputeVectorDelta(&aShipPositionsArray[EYE_OBJECT], &destination, &delta);
            divide_vector(&delta, (bAlternateChaseView ? 7 : 25) * 256,
                &aShipVelocity[EYE_OBJECT]);
            SwcShrinkCameraVector(&direction);
            aShipFwdVec[EYE_OBJECT] = direction;
            fix_objects_ijk(EYE_OBJECT);
            nEyeRollTargetValue = match_roll_orientation(EYE_OBJECT, cViewObject);
            if (nEyeRollTargetValue != 0) {
                if (abs((int)nEyeRollTargetValue) < 5) {
                    aShipUpVector[EYE_OBJECT] = aShipUpVector[0];
                    nEyeRollTargetValue = 0;
                } else {
                    nEyeRollRate = 4;
                    rotate_eye_to_goal();
                    rotate_object(EYE_OBJECT);
                }
            }
        }
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT], &aShipVelocity[EYE_OBJECT],
            &aShipPositionsArray[EYE_OBJECT]);
    } else {
        set_eye_direction_and_position();
    }
}

/* CODE_01 DoViewsMenu +0x3052 and process_player_input: Y/D/F select
 * cockpit/battle/chase. CODE_02 new_view +0x3190 supplies initial placement. */
static void SwcSelectFlightView(int view)
{
    FixedVector offset;

    nCameraViewMode = view;
    cViewObject = 0;
    cCurrentCockpitView = view == 0 ? 0 : 4;
    asObjectCollisionRadius[EYE_OBJECT] = MaxShort(10, asObjectCollisionRadius[0]);
    if (view == 4) {
        /* The actual case-4 target is +0x35ba: -0x4b000, or 1,200
         * units behind the ship. Ghidra's decompiler misreads this case. */
        ScaleFixedVector(&aShipFwdVec[0], -1200 * 256, &offset);
        AddFixedVectors(&aShipPositionsArray[0], &offset, &aShipPositionsArray[EYE_OBJECT]);
        aShipUpVector[EYE_OBJECT] = aShipUpVector[0];
        aShipFwdVec[EYE_OBJECT] = offset;
        fix_objects_ijk(EYE_OBJECT);
    }
    zero_vector(&aShipVelocity[EYE_OBJECT]);
    remove_nav_pointer();
    SwcUpdateFlightCamera();
    SwcGenerateStars();
}

/* SWC CODE_09 +0x00c2: Update_3Space. Mac house_keep (+0x01d0)
 * has only the periodic nav/hazard checks, without WC1 palette rendering. */
void SdlSwcUpdateSpace(void)
{
    if (nCannedSceneMode == 0 && nTrainSimActive == 0) {
        if ((nCurrentSpaceFrameValue & 0x1f) == 0)
            ReleaseStaleNavTarget();
        if (nCurrentHazardFieldTotal != 0 && (nCurrentSpaceFrameValue & 0xf) == 0)
            check_hazards();
    }
    house_keep_objects();
    update_objects_in_space();
    SwcUpdateFlightCamera();
    if (nCameraViewMode != 16)
        SdlUpdateSwcFlightMusic();
    nCurrentSpaceFrameValue++;
}

static void SwcSetObjectData(ObjectTypeData saved[OBJECT_TYPE_COUNT])
{
    /* CODE_08 +0x0286/+0x47cc. The shared runtime has the same object IDs
       through 57, but packed seven-byte loadouts. */
    int type;

    memcpy(saved, aObjectTypeData, sizeof(aObjectTypeData));
    memcpy(aObjectTypeData, swcObjectDefinitions, sizeof(aObjectTypeData));
    for (type = 0; type < 22; type++) {
        ObjectTypeData *data = &aObjectTypeData[type];

        data->lifetime = swcShipFuel[type];
        data->weaponLoadout[0] = swcWeaponCounts[type];
        memcpy(data->weaponLoadout + 1, swcWeaponLoadouts[type],
               sizeof(swcWeaponLoadouts[type]));
    }
}

int SdlSwcFlightActive(void)
{
    return swcFlightActive;
}

SDL_Texture *SdlCreateSwcTexture(SDL_Renderer *renderer,
                                  const SwcBuffer *set, uint32_t index,
                                  const SDL_Color colors[256])
{
    SwcFrame frame;
    SDL_Surface *surface;
    SDL_Texture *texture = NULL;
    uint8_t *pixels;
    uint32_t *row;
    SDL_Color color;
    int x;
    int y;

    if (SwcGetFramePtr(set, index, &frame) != 0)
        return NULL;
    if (frame.width == 0 || frame.height == 0) {
        SDL_SetError("Empty SWC flight image");
        return NULL;
    }
    pixels = SDL_malloc((size_t)frame.width * frame.height);
    if (pixels == NULL) {
        SDL_OutOfMemory();
        return NULL;
    }
    if (SwcDecodeFrame(&frame, pixels, (size_t)frame.width * frame.height) != 0) {
        SDL_free(pixels);
        return NULL;
    }
    surface = SDL_CreateRGBSurfaceWithFormat(0, frame.width, frame.height,
                                             32, SDL_PIXELFORMAT_ARGB8888);
    if (surface != NULL) {
        for (y = 0; y < frame.height; y++) {
            row = (uint32_t *)((uint8_t *)surface->pixels + y * surface->pitch);
            for (x = 0; x < frame.width; x++) {
                color = colors[pixels[y * frame.width + x]];
                row[x] = pixels[y * frame.width + x] == 0 ? 0 :
                    UINT32_C(0xff000000) | (uint32_t)color.r << 16 |
                    (uint32_t)color.g << 8 | color.b;
            }
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture != NULL)
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(surface);
    }
    SDL_free(pixels);
    return texture;
}

static int SwcLoadFlightImages(const SwcCmf *cockpit, const SwcCmf *space,
                                enum ObjectType type, SwcFlightImages *images)
{
    SwcBuffer set = {0};
    const SwcCmf *cmf;
    char chunkType[5];
    uint32_t chunk = 1;
    uint32_t expected = 0;
    uint32_t count;
    uint32_t index;
    size_t entry;
    SwcFrame frame;
    int result = -1;

    if (images->views != NULL || images->unavailable)
        return 0;
    cmf = space;
    SDL_strlcpy(chunkType, "OBJT", sizeof(chunkType));
    /* CODE_08 get_Mac_mem +0x0050..+0x022c, followed by CODE_03
       init_3Space_objects aliases. Mac OBJT IDs differ from WC1 packets. */
    switch (type) {
    case OBJECT_TYPE_LASER_CANNON: chunk = 7; expected = 4; break;
    case OBJECT_TYPE_NEUTRON_PARTICLE_GUN:
        cmf = cockpit; chunk = 9; expected = 6; break;
    case OBJECT_TYPE_MASS_DRIVER_CANNON: chunk = 8; expected = 3; break;
    case OBJECT_TYPE_EXPLOSION0: chunk = 2; expected = 15; break;
    case OBJECT_TYPE_EXPLOSION1: chunk = 3; expected = 20; break;
    case OBJECT_TYPE_EXPLOSION2: chunk = 4; expected = 22; break;
    case OBJECT_TYPE_LASER_SPARK: chunk = 10; expected = 6; break;
    case OBJECT_TYPE_RED_SPARK: chunk = 12; expected = 4; break;
    case OBJECT_TYPE_BLUE_SPARK: chunk = 11; expected = 4; break;
    case OBJECT_TYPE_SPARK_TRAIL: chunk = 13; expected = 8; break;
    case OBJECT_TYPE_DEBRIS_METAL_SHEET:
    case OBJECT_TYPE_DEBRIS_WING: chunk = 6; expected = 29; break;
    case OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK:
    case OBJECT_TYPE_DEBRIS_SHIP_TUBING:
    case OBJECT_TYPE_DEBRIS_GLASS:
    case OBJECT_TYPE_DEBRIS_O_RING:
    case OBJECT_TYPE_DEBRIS_PIPE: chunk = 5; expected = 33; break;
    case OBJECT_TYPE_SPACE_MINE:
        cmf = cockpit; chunk = 16; expected = 14; break;
    case OBJECT_TYPE_EJECTED_PILOT:
        /* CODE_14 ejection_sequence +0x07d0 requests CKPT/17, which is
         * absent from the demo's CMFs. Keep the pilot's physics/camera
         * when that art is missing; never substitute a different shape. */
        SDL_strlcpy(chunkType, "CKPT", sizeof(chunkType));
        chunk = 17;
        expected = 13;
        for (entry = 0; entry < space->chunkCount; entry++) {
            if (space->chunks[entry].id == chunk &&
                memcmp(space->chunks[entry].type, chunkType, 4) == 0)
                break;
        }
        if (entry == space->chunkCount) {
            cmf = cockpit;
            for (entry = 0; entry < cockpit->chunkCount; entry++) {
                if (cockpit->chunks[entry].id == chunk &&
                    memcmp(cockpit->chunks[entry].type, chunkType, 4) == 0)
                    break;
            }
            if (entry == cockpit->chunkCount) {
                fprintf(stderr, "SWC ejection pilot artwork CKPT/17 is absent from the demo data\n");
                images->unavailable = 1;
                return 0;
            }
        }
        break;
    /* CODE_04 load_ship +0x04d2..+0x054e loads the two asteroid sets;
       CODE_05 set_objects_data +0x02b0 assigns the first set to rock fragments. */
    case OBJECT_TYPE_ASTEROID1:
    case OBJECT_TYPE_ASTEROID3:
    case OBJECT_TYPE_ASTEROID5:
    case OBJECT_TYPE_ROCK_CHUNK: chunk = 17; expected = 12; break;
    case OBJECT_TYPE_ASTEROID2:
    case OBJECT_TYPE_ASTEROID4:
    case OBJECT_TYPE_ASTEROID6: chunk = 18; expected = 12; break;
    default:
        if (type >= OBJECT_TYPE_DUMB_FIRE_MISSILE &&
            type <= OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE) {
            SDL_snprintf(chunkType, sizeof(chunkType), "MS%02d",
                         type - OBJECT_TYPE_DUMB_FIRE_MISSILE);
            expected = 22;
        } else if (aObjectTypeData[type].objectClass >= OBJECT_CLASS_SHIP) {
            cmf = aObjectTypeData[type].objectClass == OBJECT_CLASS_CAPITAL_SHIP ?
                space : cockpit;
            SDL_snprintf(chunkType, sizeof(chunkType), "%s%02d",
                         cmf == space ? "SH" : "ST", type);
            expected = 37;
        } else {
            return SDL_SetError("Unsupported SWC flight object type %d", type);
        }
        break;
    }
    if (SwcCMGetChunk(cmf, chunkType, chunk, &set) != 0)
        return -1;
    if (SwcGetFrameCount(&set, &count) != 0)
        goto done;
    if (count != expected) {
        SDL_SetError("Unexpected SWC frame count in %s/%u", chunkType, chunk);
        goto done;
    }
    images->views = SDL_calloc(count, sizeof(*images->views));
    if (images->views == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    images->count = count;
    for (index = 0; index < count; index++) {
        if (SwcGetFramePtr(&set, index, &frame) != 0)
            goto done;
        /* The final red-spark frame is intentionally empty. */
        if (frame.width == 0 || frame.height == 0)
            continue;
        if (SdlLoadSwcRasterImage(&set, index, &images->views[index]) != 0)
            goto done;
    }
    result = 0;
done:
    SDL_free(set.data);
    return result;
}

static void SwcInitCharacters(void)
{
    ResetCampaignData();
    /* CODE_07 InitCharacters +0x13fc copies ARMSTRONG / MAVERICK from
     * the ninth pilot record at expanded DATA/0 +0x2a70. */
    SDL_strlcpy(stCampaignState.currentPilot->name, "ARMSTRONG",
                sizeof(stCampaignState.currentPilot->name));
    SDL_strlcpy(stCampaignState.currentPilot->callsign, "MAVERICK",
                sizeof(stCampaignState.currentPilot->callsign));
}

static int SwcStartMissionShips(void)
{
    short records[10];
    short record;
    short count = 1;
    short index;
    short prior;

    records[0] = stCurrentMissionHeader.playerMissionShip;
    for (index = 0; index < 18; index++) {
        record = index < 8 ? stCurrentMissionHeader.initialMissionShips[index] :
            aMissionNavPoints[stCurrentMissionHeader.entryNavPoint].missionShips[index - 8];
        if (record < 0)
            continue;
        for (prior = 0; prior < count && records[prior] != record; prior++) {
        }
        if (prior != count)
            continue;
        if (count == (short)SDL_arraysize(records))
            return SDL_SetError("SWC entry nav exceeds WC1's ten active ships");
        if ((int)aMissionShips[record].type < 0 ||
            aObjectTypeData[aMissionShips[record].type].objectClass < OBJECT_CLASS_SHIP)
            return SDL_SetError("Unsupported SWC entry-nav object");
        records[count++] = record;
    }
    memset(aeObjectClass, 0, sizeof(aeObjectClass));
    memset(asObjectCounterArray, 0xff, sizeof(asObjectCounterArray));
    memset(anObjPitchRot, 0, sizeof(anObjPitchRot));
    memset(anObjectYawRotation, 0, sizeof(anObjectYawRotation));
    memset(anObjectRollRotationList, 0, sizeof(anObjectRollRotationList));
    memset(acPlayerComponentDamageList, 0, sizeof(acPlayerComponentDamageList));
    clear_cockpit_damage();
    memset(acObjCollisionGraceTicks, 0, sizeof(acObjCollisionGraceTicks));
    memset(nShipMissionIndices, 0xff, sizeof(nShipMissionIndices));
    memset(abHazardObjects, 0xff, sizeof(abHazardObjects));
    memset(acShipSpawnNavPt, 0xff, sizeof(acShipSpawnNavPt));
    memset(acTurnRegulatorsArray, 0, sizeof(acTurnRegulatorsArray));
    memset(acTurnIntervalArray, 0, sizeof(acTurnIntervalArray));
    memset(apObjShapeArray, 0, sizeof(apObjShapeArray));
    pActiveHazardField = NULL;
    nCurrentHazardFieldTotal = 0;
    nActiveHazards = 0;
    nYourWingman = -1;
    nPlayerKillTotal = 0;
    nWingmanKillCount = 0;
    nCurrentWingmanKilledThisMission = 0;
    /* CODE_09 GameFlow +0x1ba6..+0x1bc6 resets pending postflight events.
     * These globals are outside ResetCampaignData's campaign template. */
    nPostSeriesSequence = -1;
    nPendingMedalIndex = -1;
    bPlayerEjectedThisMission = 0;
    bPromotionPending = 0;
    bOfficeVisitPending = 0;
    bPlayerShipTypeChanged = 0;
    bRadioSilence = 0;
    clear_message_time();
    nEnemySighting = 0x7fff;
    nTargetLockMode = 0;
    bEngageAllowed = 0;
    nAutoEngageTimer = -1;
    nCommSpeakerObject = -1;
    bIntroSecondaryScene = 0;
    nCurrentNavigationPoint = stCurrentMissionHeader.entryNavPoint;
    stCampaignState.playerShipType = aMissionShips[stCurrentMissionHeader.playerMissionShip].type;
    bInitialFormationSetup = 1;
    for (index = 0; index < count; index++) {
        record = records[index];
        if (aMissionShips[record].leaderMissionIndex != -1 &&
            find_ship_index(aMissionShips[record].leaderMissionIndex) == -1)
            return SDL_SetError("SWC entry formation needs its leader initialized first");
        set_objects_data(index, aMissionShips[record].type, -1);
        Set_up_ship_info(index, record, is_team_member(record) ? -1 :
                                      (signed char)nCurrentNavigationPoint);
        if (index != 0) {
            find_next_ship_turn_slot(index);
            if (nYourWingman == -1 && is_team_member(record) &&
                aiPilotLevel[index] > 4 && aiPilotLevel[index] < 14)
                nYourWingman = index;
        }
        asObjectScreenXList[index] = (short)0x8001;
        if (aMissionShips[record].type == OBJECT_TYPE_TIGERS_CLAW)
            nCarrierMissionShipIndexValue = record;
    }
    bInitialFormationSetup = 0;
    Build_objective_list();
    initialize_direction_view_frames();
    nNavPointerObject = -1;
    nCameraViewMode = 0;
    bAlternateChaseView = 0;
    cViewObject = 0;
    cCurrentCockpitView = 0;
    nCannedSceneMode = 0;
    nCurrentSpaceFrameValue = 0;
    nRenderedSpaceFrame = 0;
    nCurrentWave = -1;
    bCurrentPlayerDestroyed = 0;
    /* Title combat disables damage; restore the launcher's cheat choice
     * whenever New Game recreates the mission. */
    bPlayerVulnerable = nOriginDevUnlock == 0;
    bPlayerCollisionResponse = nOriginDevUnlock == 0;
    bPlayerCollisionsEnabled = 1;
    bLandingAuthorized = 0;
    nPlayerCollisionObject = -1;
    nInFlightMusicActive = 0;
    nArcadeState = 0;
    nTrainSimActive = 0;
    nExternalViewShip = -1;
    nTargetLockCountdown = -1;
    bTargetLockAcquired = 0;
    bTargetLockReadoutDirty = 0;
    memset(aCockpitHudMessageSlotsTable, 0, sizeof(aCockpitHudMessageSlotsTable));
    memset(acVduModeStackDepth, 0, sizeof(acVduModeStackDepth));
    memset(ausVduModeStack, 0, sizeof(ausVduModeStack));
    ausVduModeStack[0] = 1;
    acVduModeStackDepth[1] = 0;
    /* CODE_03 init_vdus +0x0918 starts the right VDU in target mode. */
    ausVduModeStack[8] = 3;
    ResetCommMenuChoices(0);
    SetPendingMenuAction(0);
    nScreenWidth = SWC_FRAME_WIDTH;
    copy_frame(0, EYE_OBJECT);
    aShipPositionsArray[EYE_OBJECT] = aShipPositionsArray[0];
    asObjectCollisionRadius[EYE_OBJECT] = asObjectCollisionRadius[0];
    SwcGenerateStars();
    return count;
}

static void SwcResetFlightInput(void)
{
    bCurrentKey = 0;
    cPreviousKey = 0;
    nPitchInput = 0;
    nYawInputValue = 0;
    nCurrentRollInputValue = 0;
    bFlightRollLatch = 0;
    bMouseCursorVisible = 0;
    bMouseAfterburnerControl = 0;
    nMouseYawInput = 0;
    nMousePitchInput = 0;
    swcMouseAfterburner = 0;
    swcRightClickPending = 0;
    swcReleaseWeaponPending = 0;
    swcGunFirePending = 0;
    swcBothButtonsHeld = 0;
    swcJoystickSteering = 0;
    swcJoystickAfterburner = 0;
    swcJoystickClickPending = 0;
    swcJoystickButtons = 0;
    SdlEndJoystickSpaceflight();
}

static void SwcCentreFlightMouse(void)
{
    SwcResetFlightInput();
    nHostMouseMessageX = (stSpaceBufferViewport.left + stSpaceBufferViewport.right) / 2;
    nHostMouseMessageY = (stSpaceBufferViewport.top + stSpaceBufferViewport.bottom) / 2;
    WarpMouseTo((short)nHostMouseMessageX, (short)nHostMouseMessageY);
    bMouseCursorVisible = 1;
}

static void SwcFlightTick(const Uint8 *keys, Uint32 mouseButtons)
{
    InputDeviceSample joystick;
    Uint32 now = SDL_GetTicks();
    short throttle;
    int haveJoystick;
    int enhancedJoystick = SdlJoystickHasEnhancedControls();
    int joystickModifier;
    int joystickGuns;
    int sampledMouse = 0;
    int secondaryButton;
    int bothButtons;

    if (bCurrentPlayerDestroyed || nArcadeState != 0) {
        SwcResetFlightInput();
        return;
    }
    haveJoystick = SdlReadSwcJoystickFlightSample(&joystick);
    if (!enhancedJoystick && (joystick.buttons & 2) && !(swcJoystickButtons & 2)) {
        swcJoystickAfterburner = swcJoystickClickPending &&
            (Uint32)(now - swcLastJoystickClick) <= 20 * 1000 / 60;
        swcLastJoystickClick = now;
        swcJoystickClickPending = 1;
    }
    if (!(joystick.buttons & 2))
        swcJoystickAfterburner = 0;
    if (!enhancedJoystick && (joystick.buttons & 3) == 3 &&
        (swcJoystickButtons & 3) != 3 && !swcJoystickAfterburner)
        swcReleaseWeaponPending = 1;
    swcJoystickButtons = joystick.buttons;
    joystickModifier = !enhancedJoystick && (joystick.buttons & 2) &&
        !swcJoystickAfterburner;
    joystickGuns = (joystick.buttons & 1) &&
        (enhancedJoystick || (joystick.buttons & 3) != 3 || swcJoystickAfterburner);
    /* Retain WC1's key priority, diagonal keys, ramp and direction reversal.
       Its Win32 key polling is supplied by the same SDL sample for the whole
       tick, including modifiers. */
    SdlSetKeyboardSnapshot(keys);
    cPreviousKey = bCurrentKey;
    bCurrentKey = (signed char)PollKeyboardState();
    if (bCurrentKey == 0 && haveJoystick &&
        (joystick.x != 0 || joystick.y != 0 || swcJoystickSteering || joystickModifier)) {
        swcJoystickSteering = 1;
        bMouseCursorVisible = 0;
        bMouseAfterburnerControl = 0;
        nYawInputValue = joystickModifier ? 0 : (short)joystick.x;
        nPitchInput = joystickModifier ? 0 : (short)-joystick.y;
        nCurrentRollInputValue = joystickModifier ? (short)joystick.x : 0;
        if (joystickModifier)
            accelerate((short)-(joystick.y / 2));
    } else if (bCurrentKey == 0 && bMouseCursorVisible) {
        /* Feed the original mouse branch in player_input. A held position is
           sampled each tick, so right-button throttle is independent of SDL
           motion-event frequency. Button actions that fire weapons stay out
           of the queue; the right button is only a motion modifier here. */
        secondaryButton = (mouseButtons & SDL_BUTTON_RMASK) != 0;
        QueueInputEvent(13, (unsigned short)nHostMouseMessageX,
                        (unsigned short)nHostMouseMessageY, 0, 0, secondaryButton, 0);
        player_input();
        sampledMouse = 1; /* player_input also applies the enhanced axes. */
        /* WC1 recentres the pointer when right-button roll/throttle ends. */
        nHostMouseMessageX = stMouseCursorState.x;
        nHostMouseMessageY = stMouseCursorState.y;
    } else {
        /* Keyboard steering takes over until the next mouse movement.
           Release each keyboard axis independently, even if another is held. */
        if (!(nUpArrowKeyState || nDownArrowKeyState || nHomeState ||
              nPageUpKeyState || nEndKeyDown || nPageDownKeyHeld))
            nPitchInput = 0;
        if (!(nLeftArrowKeyState || nRightArrowKeyHeld || nHomeState ||
              nPageUpKeyState || nEndKeyDown || nPageDownKeyHeld))
            nYawInputValue = 0;
        if (!(nInsertKeyDown || nDeleteKeyDown ||
              nOemCommaKeyPressed || nOemPeriodKeyStatus))
            nCurrentRollInputValue = 0;
        if (bCurrentKey == 0x4c) {
            SwcCentreFlightMouse();
        } else if (bCurrentKey != 0) {
            bMouseAfterburnerControl = 0;
            process_player_input();
        } else {
            bFlightRollLatch = 0;
        }
    }
    SdlSetKeyboardSnapshot(NULL);
    if (!sampledMouse)
        SdlApplyJoystickFlightControls();

    /* Movement commands from WC1's HandleSpaceFlightControls. Poll held keys
       separately so thrust can change while steering, without host key repeat. */
    throttle = (short)((keys[SDL_SCANCODE_EQUALS] || keys[SDL_SCANCODE_KP_PLUS]) -
                       (keys[SDL_SCANCODE_MINUS] || keys[SDL_SCANCODE_KP_MINUS]));
    if (keys[SDL_SCANCODE_BACKSPACE])
        anShipSpeed[0] = 0;
    else if (throttle != 0)
        accelerate(throttle);
    if (keys[SDL_SCANCODE_TAB] || keys[SDL_SCANCODE_KP_MULTIPLY] ||
        (swcMouseAfterburner && (mouseButtons & SDL_BUTTON_RMASK)) ||
        swcJoystickAfterburner)
        your_afterburner();
    bothButtons = (mouseButtons & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) ==
        (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK);
    if (bothButtons && !swcBothButtonsHeld && !swcMouseAfterburner)
        swcReleaseWeaponPending = 1;
    swcBothButtonsHeld = bothButtons;
    if (keys[SDL_SCANCODE_SPACE] || joystickGuns ||
        (swcGunFirePending && !bothButtons &&
         (enhancedJoystick || (joystick.buttons & 3) != 3 || swcJoystickAfterburner)) ||
        ((mouseButtons & SDL_BUTTON_LMASK) && (!bothButtons || swcMouseAfterburner)))
        fire_players_lasers();
    swcGunFirePending = 0;
    if (swcReleaseWeaponPending) {
        swcReleaseWeaponPending = 0;
        if (nSelectedReleaseWeaponIndex >= 0) {
            ShipWeaponSlot *slot = &((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[
                nSelectedReleaseWeaponIndex];

            if (slot->type == OBJECT_TYPE_SPACE_MINE)
                drop_player_mine(0);
            else if (nExternalViewShip == -1)
                nExternalViewShip = (short)fire_missile(0);
        }
    }
    players_flight_dynamics();
    /* Shared object lifetimes, cooldowns, collisions/damage, animations,
       missile guidance, energy recharge, shields and movement. */
    house_keep_objects();
    if (nArcadeState == 1) {
        /* Landing ends FlyingLoop in SWC GameFlow. Do not let another
           movement/collision pass damage the ship after docking. */
        SwcResetFlightInput();
        return;
    }
    update_objects_in_space();
    /* CODE_13 check_stranded +0x1630: losing the carrier ends flight once
       there are no enemies within 30,000 units. Death takes precedence. */
    if (!bCurrentPlayerDestroyed && nArcadeState == 0 && nCannedSceneMode == 0 &&
        nCarrierMissionShipIndexValue >= 0 &&
        nCarrierMissionShipIndexValue < ACTIVE_MISSION_SHIP_COUNT)
        check_stranded();
    if (aeSpecialManeuvers[0] == SPECIAL_MANEUVER_AFTERBURNER)
        SdlPlaySwcSound(12, 127);
    else
        SdlStopSwcAfterburnerSound();
    ReleaseStaleNavTarget();
    check_hazards();
    check_next_wave();
    /* CODE_13 update_cockpit checks one objective per frame, including
       unselected objectives; flag_reached owns the escort-wait latch. */
    if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCountValue &&
        objective_lost((short)cCurrentObjective))
        cycle_next_objective();
    if (cMissionObjectiveCountValue > 0)
        update_objective_location((short)((unsigned short)nCurrentSpaceFrameValue % cMissionObjectiveCountValue));
    SwcUpdateFlightCamera();
    SwcUpdateStarField();
    /* CODE_09 Draw_3Space_Frame projects objects before CODE_13 checks
     * targets. Catch-up simulation ticks must not reuse the last render. */
    SwcTransformObjectsToView();
    if (IsCommChoiceMenuOpen()) {
        if (bCurrentPlayerDestroyed || !CanOpenCommMenu() ||
            (GetPendingMenuAction() == 2 &&
             (cCommMenuRecipient <= 0 || cCommMenuRecipient >= 10 ||
              aeObjectClass[cCommMenuRecipient] < OBJECT_CLASS_SHIP ||
              aeSpecialManeuvers[cCommMenuRecipient] == SPECIAL_MANEUVER_UNKNOWN_9)))
            CloseCommChoiceMenu();
        else
            RefreshCommunicationMenu();
    }
    SdlUpdateSwcCockpit();
    SdlUpdateSwcFlightMusic();
    nCurrentSpaceFrameValue++;
    nRenderedSpaceFrame++;
}

/* CODE_04 transform_objects_to_your_view +0x4150. Mac projection keeps
 * 32-bit depth and scale until clamping and divides the full XY product.
 * Only the compatibility cache for the shared WC1 targeting code is 16-bit;
 * the SDL depth sort below uses the original full-distance values. */
static void SwcTransformObjectsToView(void)
{
    FixedVector direction;
    FixedVector *view;
    int distance;
    int eyeRadius = asObjectCollisionRadius[EYE_OBJECT] * 256;
    int radius;
    int64_t scale;
    short dustSize;
    short object;

    nClosestVisibleObject = -1;
    draw_nav_pointer();
    memset(swcObjectDistances, 0, sizeof(swcObjectDistances));
    for (object = 0; object <= SPACE_LAST_MOVING_OBJECT; object++) {
        if (aeObjectClass[object] == OBJECT_CLASS_NULL ||
            aeObjectClass[object] == OBJECT_CLASS_FIXED_OBJECT || object == nNavPointerObject)
            continue;
        asPreviousObjectDistance[object] = asObjDistancesTable[object];
        asObjDistancesTable[object] = 0;
        asObjectScreenXList[object] = (short)0x8001;
        if (aeObjectClass[object] == OBJECT_CLASS_FUTURION)
            continue;
        if (aeObjectClass[object] == OBJECT_CLASS_STAR ||
            aeObjectClass[object] == OBJECT_CLASS_PLANET)
            direction = aShipPositionsArray[object];
        else
            ComputeVectorDelta(&aShipPositionsArray[EYE_OBJECT], &aShipPositionsArray[object], &direction);
        distance = Vector_magnitude(&direction);
        if (distance < eyeRadius || distance == 0 ||
            (aeObjectClass[object] == OBJECT_CLASS_DUST && distance > 5000 * 256))
            continue;
        view = &aObjectViewPositionsList[object];
        transform_to_objects_frame(&direction, view, EYE_OBJECT);
        if (view->z < eyeRadius || view->z == 0 || DivideFixed(view->z, distance) < 0x94)
            continue;
        radius = asObjectCollisionRadius[object] * 256;
        if (distance <= radius)
            distance = radius + 1;
        if (aeObjectClass[object] >= OBJECT_CLASS_EXPLOSION) {
            /* +0x434e divides the positive 64-bit value 0xa00000*256.
             * Widen before multiplying so nearby sprites cannot wrap. */
            scale = INT64_C(0xa0000000) / (distance - radius);
            scale = ((int64_t)awObjectScales[object] * scale) >> 16;
            asObjScreenScalesArray[object] = (short)SDL_clamp(scale, 0, 8192);
            if (asObjScreenScalesArray[object] <= 4)
                continue;
        }
        swcObjectDistances[object] = distance >> 8;
        asObjectScreenXList[object] = (short)(((int64_t)view->x * 0xa000 / view->z) >> 8);
        asObjectScreenYArray[object] = (short)(((int64_t)view->y * 0xa000 / view->z) >> 8);
        switch (aeObjectClass[object]) {
        case OBJECT_CLASS_STAR:
            swcObjectDistances[object] = 0x7ffe;
            break;
        case OBJECT_CLASS_PLANET:
            swcObjectDistances[object] = 0x7ffd;
            if (asObjScreenScalesArray[object] == 0xff)
                set_background_objects_rotation(object, &direction);
            break;
        case OBJECT_CLASS_DUST:
            dustSize = (short)(MultiplyFixed(0x2100, DivideFixed(eyeRadius, distance)) >> 8);
            dustSize = SDL_min(dustSize, 3);
            asObjectViewFrames[object] = (short)(((asObjectCounterArray[object] + nCurrentSpaceFrameValue) & 3) +
                (asObjScreenAngles[object] & 0x10) + (3 - dustSize) * 4);
            break;
        case OBJECT_CLASS_MISSILE:
        case OBJECT_CLASS_SHIP:
        case OBJECT_CLASS_CAPITAL_SHIP:
            get_right_shape(object, &direction);
            break;
        default:
            break;
        }
        asObjDistancesTable[object] = (short)SDL_min(swcObjectDistances[object], UINT16_MAX);
    }
}

static int SwcDrawFlight(SDL_Renderer *renderer, SDL_Texture *cockpit,
                          const SwcCmf *cockpitCmf, const SwcCmf *spaceCmf,
                          const SDL_Color colors[256],
                          SwcFlightImages images[OBJECT_TYPE_COUNT],
                          int cockpitless)
{
    static const SDL_Rect frameBorder[] = {
        {0, 0, SWC_FRAME_WIDTH, 6},
        {0, 6, 8, SWC_FRAME_HEIGHT - 12},
        {SWC_FRAME_WIDTH - 8, 6, 8, SWC_FRAME_HEIGHT - 12},
        {0, SWC_FRAME_HEIGHT - 6, SWC_FRAME_WIDTH, 6}
    };
    SDL_Rect rectangle;
    SDL_Rect objectBounds[10] = {{0}};
    SwcRasterImage *image;
    SwcBuffer stars = {0};
    SwcFrame star;
    short object;
    short frame;
    short sorted[SPACE_OBJECT_COUNT];
    short count = 0;
    short index;
    /* CODE_02 draw_sorted_objects_to_buffer uses (160,100) + projected XY
       in both cockpit modes. Positive view Y also points down in WC1. */
    int centreY = 100;

    SwcTransformObjectsToView();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if (SDL_RenderClear(renderer) != 0)
        return -1;
    if (SdlDrawSwcSpaceGlare() != 0)
        return -1;
    for (object = nCameraViewMode > 3 ? 0 : 1;
         object <= SPACE_LAST_MOVING_OBJECT; object++) {
        if ((aeObjectClass[object] < OBJECT_CLASS_EXPLOSION &&
             aeObjectClass[object] != OBJECT_CLASS_STAR && aeObjectClass[object] != OBJECT_CLASS_DUST) ||
            aeObjectClass[object] == OBJECT_CLASS_FIXED_OBJECT ||
            asObjectScreenXList[object] == (short)0x8001 || object == nNavPointerObject)
            continue;
        index = count++;
        while (index > 0 && swcObjectDistances[sorted[index - 1]] < swcObjectDistances[object]) {
            sorted[index] = sorted[index - 1];
            index--;
        }
        sorted[index] = object;
    }
    for (index = 0; index < count; index++) {
        object = sorted[index];
        /* CODE_02 +0x0142: class 4 dust is one palette-128 pixel.
         * Class 2 stars use unscaled MacDraw2 with the sprite's origin. */
        if (aeObjectClass[object] == OBJECT_CLASS_DUST) {
            if (SDL_SetRenderDrawColor(renderer, colors[0x80].r,
                    colors[0x80].g, colors[0x80].b, 255) != 0 ||
                SDL_RenderDrawPoint(renderer, 160 + asObjectScreenXList[object],
                    centreY + asObjectScreenYArray[object]) != 0)
                return -1;
            continue;
        }
        if (aeObjectClass[object] == OBJECT_CLASS_STAR) {
            frame = (short)(asObjectViewFrames[object] - 32);
            if (frame < 0 || frame >= (short)SDL_arraysize(swcStarTextures))
                return SDL_SetError("Invalid SWC star frame %d", asObjectViewFrames[object]);
            if (swcStarTextures[frame] == NULL) {
                if (SwcCMGetChunk(cockpitCmf, "PLNT", 1, &stars) != 0)
                    return -1;
                if (SwcGetFramePtr(&stars, (uint32_t)(frame + 32), &star) == 0) {
                    swcStarBounds[frame] = (SDL_Rect){star.x, star.y, star.width, star.height};
                    swcStarTextures[frame] = SdlCreateSwcTexture(renderer, &stars,
                        (uint32_t)(frame + 32), colors);
                }
                SDL_free(stars.data);
                SDL_zero(stars);
                if (swcStarTextures[frame] == NULL)
                    return -1;
            }
            rectangle = swcStarBounds[frame];
            rectangle.x += 160 + asObjectScreenXList[object];
            rectangle.y += centreY + asObjectScreenYArray[object];
            if (SDL_RenderCopy(renderer, swcStarTextures[frame], NULL, &rectangle) != 0)
                return -1;
            continue;
        }
        if (SwcLoadFlightImages(cockpitCmf, spaceCmf,
                               aeObjectTypeList[object], &images[aeObjectTypeList[object]]) != 0)
            return -1;
        if (images[aeObjectTypeList[object]].unavailable)
            continue;
        frame = asObjectViewFrames[object];
        if (frame < 0 || (uint32_t)frame >= images[aeObjectTypeList[object]].count)
            return SDL_SetError("Unsupported SWC frame %d for object type %d",
                                frame, aeObjectTypeList[object]);
        image = &images[aeObjectTypeList[object]].views[frame];
        if (SdlDrawSwcRotatedImage(renderer, swcRasterCanvas, image, colors,
                160 + asObjectScreenXList[object], centreY + asObjectScreenYArray[object],
                asObjScreenAngles[object], (unsigned short)asObjScreenScalesArray[object],
                asObjectFlipArray[object], object < 10 ? &objectBounds[object] : NULL) != 0)
            return -1;
    }
    /* CODE_13 update_cockpit draws the HUD and instruments only in
       cockpit views. External views include the player's own ship. */
    if (nCameraViewMode > 3)
        goto border;
    if (SdlDrawSwcSpaceHud(objectBounds) != 0)
        return -1;
    if (!cockpitless) {
        rectangle = (SDL_Rect){swcCockpitOffset.x, swcCockpitOffset.y, 320, 240};
        if (SDL_RenderCopy(renderer, cockpit, NULL, &rectangle) != 0 ||
            SdlDrawSwcCockpit() != 0)
            return -1;
    }
    if (SdlDrawSwcHud() != 0 || (!cockpitless && SdlDrawSwcCockpitDamage() != 0))
        return -1;
border:
    /* CODE_01 dump_buffer_to_screen +0x06dc calls DrawBorder +0x065c
     * in flight modes. Its fixed 8x6 margins cover the full banking range,
     * leaving a stable (8,6)-(312,234) picture at every steering offset.
     * Apply it after all cockpit layers, including the pilot's hand. */
    if (SDL_SetRenderDrawColor(renderer, colors[255].r, colors[255].g,
            colors[255].b, 255) != 0 ||
        SDL_RenderFillRects(renderer, frameBorder, SDL_arraysize(frameBorder)) != 0)
        return -1;
    return 0;
}

/* CODE_09 FlyingLoop +0x0ad2..+0x0b60 restores its pause prompt after a
 * cancelled quit dialog: Helvetica 10, color 0x81, x=10, y=180/200/220.
 * Use that layout with SDL's resume keys and WC1's explicit pause heading.
 * The opaque panel keeps it legible over the retained flight/title picture. */
static int SwcDrawPauseMessage(SDL_Renderer *renderer, const SDL_Color colors[256],
                                 int flightControls, int paused)
{
    const SDL_Rect panel = {8, 166, 304, 62};

    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0 ||
        SDL_RenderFillRect(renderer, &panel) != 0 ||
        SdlDrawSwcProportionalText(renderer, &swcPauseFont, 10, 180,
            colors[0x81], "GAME PAUSED") != 0 ||
        SdlDrawSwcProportionalText(renderer, &swcPauseFont, 10, 200,
            colors[0x81], !paused ? "Return to the game to continue" :
                flightControls ? "Press Esc or P to continue" : "Press P to continue") != 0 ||
        SdlDrawSwcProportionalText(renderer, &swcPauseFont, 10, 220,
            colors[0x81], "playing Super Wing Commander.") != 0)
        return -1;
    return 0;
}

/* SWC CODE_14 +0x000c: visit_the_cinema. CODE_02 new_view's case 12
 * (+0x3474) supplies the camera setup; the shared case-12 camera update
 * matches CODE_02 +0x29f0. SDL owns pacing, window events and presentation. */
int SdlSwcVisitCinema(int view, int obj, short frames)
{
    SDL_Event event;
    SDL_Scancode skipKey = SDL_SCANCODE_UNKNOWN;
    Uint32 previous;
    Uint32 current;
    Uint32 elapsed = 0;
    Uint32 interval = (Uint32)SDL_max(1, nFrameIntervalMs);
    int savedView = nCameraViewMode;
    signed char savedViewObject = cViewObject;
    signed char savedCockpit = cCurrentCockpitView;
    int focused;
    int paused = 0;
    int result = 0;

    if (swcCinemaRenderer == NULL || view != 12 || obj != 0)
        return SDL_SetError("SWC autopilot camera is unavailable");
    SwcResetFlightInput();
    nCameraViewMode = view;
    cViewObject = (signed char)obj;
    cCurrentCockpitView = 4;
    asObjectCollisionRadius[EYE_OBJECT] = MaxShort(10, asObjectCollisionRadius[obj]);
    copy_frame(0, EYE_OBJECT);
    position_relative_ijk(&aShipPositionsArray[EYE_OBJECT], 0, 500, 0, 2000);
    zero_vector(&aShipVelocity[EYE_OBJECT]);
    look_at(0);
    set_eye_direction_and_position();
    SwcGenerateStars();
    remove_nav_pointer();
    SdlStopSwcAfterburnerSound();
    SdlPlaySwcSound(17, 255); /* CODE_14 visit_the_cinema +0x0028. */
    focused = (SDL_GetWindowFlags(swcCinemaWindow) & SDL_WINDOW_INPUT_FOCUS) != 0;
    previous = SDL_GetTicks();
    while (frames > 0 || skipKey != SDL_SCANCODE_UNKNOWN) {
        current = SDL_GetTicks();
        if (focused && !paused)
            elapsed += SDL_min(current - previous, 250);
        previous = current;
        while (SDL_PollEvent(&event)) {
            if (SdlHandleSwcEvent(swcCinemaWindow, &event, 0))
                continue;
            if (event.type == SDL_QUIT ||
                (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                result = 1;
                goto done;
            }
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
                    event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    focused = 0;
                    skipKey = SDL_SCANCODE_UNKNOWN;
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                         event.window.event == SDL_WINDOWEVENT_RESTORED)
                    focused = (SDL_GetWindowFlags(swcCinemaWindow) & SDL_WINDOW_INPUT_FOCUS) != 0;
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat && focused) {
                if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_BACKSPACE)
                    skipKey = event.key.keysym.scancode;
                else if (event.key.keysym.sym == SDLK_p)
                    paused = !paused;
            }
            /* Consume the release before returning to flight. A skipped
             * camera finishes normally so autopilot still completes travel. */
            if (event.type == SDL_KEYUP && skipKey != SDL_SCANCODE_UNKNOWN &&
                event.key.keysym.scancode == skipKey)
                goto done;
        }
        SDL_SetWindowMouseGrab(swcCinemaWindow,
            focused && !paused ? SDL_TRUE : SDL_FALSE);
        SDL_ShowCursor(focused && !paused ? SDL_DISABLE : SDL_ENABLE);
        if (!focused || paused)
            elapsed = 0;
        SdlPauseSwcSound(!focused || paused);
        if (!focused || paused) {
            if (SwcDrawFlight(swcCinemaRenderer, NULL, swcCinemaCockpitCmf,
                    swcCinemaSpaceCmf, swcCinemaColors, swcCinemaImages, 1) != 0 ||
                SwcDrawPauseMessage(swcCinemaRenderer, swcCinemaColors, 0, paused) != 0) {
                result = -1;
                goto done;
            }
            SDL_RenderPresent(swcCinemaRenderer);
        }
        if (frames > 0 && elapsed >= interval) {
            nTickCount60Hz = (unsigned int)((uint64_t)current * 60 / 1000);
            SdlSwcUpdateSpace();
            SwcUpdateStarField();
            check_next_wave();
            /* CODE_13 update_cockpit +0x0316..+0x034e also runs during
               external views and checks one objective per space frame. */
            if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCountValue &&
                objective_lost((short)cCurrentObjective))
                cycle_next_objective();
            if (cMissionObjectiveCountValue > 0)
                update_objective_location((short)((unsigned short)nCurrentSpaceFrameValue % cMissionObjectiveCountValue));
            if (SwcDrawFlight(swcCinemaRenderer, NULL, swcCinemaCockpitCmf,
                              swcCinemaSpaceCmf, swcCinemaColors, swcCinemaImages, 1) != 0) {
                result = -1;
                goto done;
            }
            SDL_RenderPresent(swcCinemaRenderer);
            nRenderedSpaceFrame++;
            frames--;
            elapsed -= interval;
        }
        SDL_Delay(!focused || paused ? 8 : 1);
    }
done:
    SdlStopSwcSound(17);
    SdlPauseSwcSound(0);
    nCameraViewMode = savedView;
    cViewObject = savedViewObject;
    cCurrentCockpitView = savedCockpit;
    SwcUpdateFlightCamera();
    SwcGenerateStars();
    SwcResetFlightInput();
    return result;
}

/* CODE_02 set_eye_direction_and_position's Mac-only case 16 +0x2a3a.
 * CODE_04 try2point_object_at_object +0x319e limits each Q8 direction
 * component to three units per update. WC1's camera switch ends at 15. */
static void SwcUpdateSceneCamera(void)
{
    FixedVector delta;
    FixedVector adjustment;
    int object;

    if (aeObjectClass[(int)cViewObject] == OBJECT_CLASS_NULL) {
        for (object = 1; object < 10; object++) {
            if (aeObjectClass[object] >= OBJECT_CLASS_SHIP) {
                cViewObject = (signed char)object;
                break;
            }
        }
        return;
    }
    ComputeVectorDelta(&aShipPositionsArray[EYE_OBJECT],
        &aShipPositionsArray[(int)cViewObject], &delta);
    adjustment = delta;
    NormalizeFixedVector(&adjustment);
    ComputeVectorDelta(&aShipFwdVec[EYE_OBJECT], &adjustment, &adjustment);
    adjustment.x = SDL_clamp(adjustment.x, -3, 3);
    adjustment.y = SDL_clamp(adjustment.y, -3, 3);
    adjustment.z = SDL_clamp(adjustment.z, -3, 3);
    AddFixedVectors(&aShipFwdVec[EYE_OBJECT], &adjustment,
        &aShipFwdVec[EYE_OBJECT]);
    fix_objects_ijk(EYE_OBJECT);
    if (Vector_magnitude(&delta) > 0x138800) {
        /* Mac shrink_vector +0x57c8 halves all components until each is
         * below 0xf00. WC1's same-named helper has a different limit. */
        SwcShrinkCameraVector(&delta);
        AddFixedVectors(&aShipVelocity[EYE_OBJECT], &delta,
            &aShipVelocity[EYE_OBJECT]);
        if (Vector_magnitude(&aShipVelocity[EYE_OBJECT]) > 0x5000) {
            NormalizeFixedVector(&aShipVelocity[EYE_OBJECT]);
            ScaleFixedVector(&aShipVelocity[EYE_OBJECT], 0x5000,
                &aShipVelocity[EYE_OBJECT]);
        }
    } else {
        ScaleFixedVector(&aShipVelocity[EYE_OBJECT], -12, &adjustment);
        AddFixedVectors(&aShipVelocity[EYE_OBJECT], &adjustment,
            &aShipVelocity[EYE_OBJECT]);
    }
    AddFixedVectors(&aShipPositionsArray[EYE_OBJECT], &aShipVelocity[EYE_OBJECT],
        &aShipPositionsArray[EYE_OBJECT]);
}

/* CODE_07 Title_Sequence +0x1c86, CODE_14 death_sequence +0x362a and
 * ejection_sequence +0x07c4 share the live space renderer and view 16.
 * The title's return 3 maps to SKIPPED (replay IntroFilm); FINISHED starts
 * a fresh Enyo 1 campaign.
 * Saved-game selection stays disabled until the host has a save loader. */
static int SwcRunFlightScene(enum SwcFlightScene scene)
{
    static const MissionShipRecord titleShips[4] = {
        {.type = OBJECT_TYPE_DRALTHI, .side = SIDE_KILRATHI, .leader = -1,
         .field_9 = -1, .missionType = MISSION_TYPE_PATROL, .navPoint = 16,
         .position = {0, 0, 0}, .pitch = -155, .speed = 40,
         .rating = 4, .behaviour = {.pilot = 4}, .leaderMissionIndex = -1},
        {.type = OBJECT_TYPE_DRALTHI, .side = SIDE_KILRATHI, .leader = -1,
         .field_9 = -1, .missionType = MISSION_TYPE_PATROL, .navPoint = 16,
         .position = {256000, 0, 0}, .pitch = -155, .speed = 40,
         .rating = 4, .behaviour = {.pilot = 4}, .leaderMissionIndex = 32},
        {.type = OBJECT_TYPE_HORNET, .side = SIDE_IMPERIAL, .leader = -1,
         .field_9 = -1, .missionType = MISSION_TYPE_PATROL, .navPoint = 16,
         .position = {768000, 0, 512000}, .pitch = -155, .speed = 40,
         .rating = 4, .behaviour = {.pilot = 4}, .leaderMissionIndex = -1},
        {.type = OBJECT_TYPE_HORNET, .side = SIDE_IMPERIAL, .leader = -1,
         .field_9 = -1, .missionType = MISSION_TYPE_PATROL, .navPoint = 16,
         .position = {1024000, 0, 512000}, .pitch = -155, .speed = 40,
         .rating = 4, .behaviour = {.pilot = 4}, .leaderMissionIndex = 34}
    };
    MissionShipRecord savedShips[4];
    MissionNavPoint savedNav = aMissionNavPoints[16];
    SwcBuffer menu = {0};
    SwcTextFont font = {0};
    SwcFrame frame;
    SDL_Texture *menuTextures[2] = {NULL, NULL};
    SDL_Cursor *savedCursor = SDL_GetCursor();
    SDL_Rect bounds[2];
    SDL_Event event;
    SDL_Point mouse = {-1, -1};
    char failure[256];
    SDL_Scancode armedKey = SDL_SCANCODE_UNKNOWN;
    Uint32 previous;
    Uint32 now;
    Uint32 elapsed = 50;
    unsigned int ticks = 0;
    int sides[2];
    int focused;
    int paused = 0;
    int mouseDown = 0;
    int activate;
    int configured = 0;
    int titleMenu = scene == SWC_FLIGHT_TITLE;
    int savedVulnerable = bPlayerVulnerable;
    short pilot = -1;
    int cursorVisible = SDL_ShowCursor(SDL_QUERY);
    int object;
    int index;
    int result = SWC_MOVIE_ERROR;

    SwcResetFlightInput();
    SDL_SetWindowMouseGrab(swcCinemaWindow, SDL_FALSE);
    SDL_ShowCursor(titleMenu ? SDL_ENABLE : SDL_DISABLE);
    if (titleMenu) {
        SDL_SetCursor(SDL_GetDefaultCursor());
        if (SwcCMGetChunk(swcCinemaSpaceCmf, "RCRM", 47, &menu) != 0)
            goto done;
        for (index = 0; index < 2; index++) {
            if (SwcGetFramePtr(&menu, (uint32_t)index, &frame) != 0)
                goto done;
            bounds[index] = (SDL_Rect){frame.x, frame.y, frame.width, frame.height};
            menuTextures[index] = SdlCreateSwcTexture(swcCinemaRenderer,
                &menu, (uint32_t)index, swcCinemaColors);
            if (menuTextures[index] == NULL)
                goto done;
        }
        /* Expanded DATA/0 mission records 32..35 (+0xa0a + index*76),
         * nav 16 (+0x14ba + 16*88). No campaign mission is borrowed. */
        memcpy(savedShips, &aMissionShips[32], sizeof(savedShips));
        memcpy(&aMissionShips[32], titleShips, sizeof(titleShips));
        memset(&aMissionNavPoints[16], 0, sizeof(aMissionNavPoints[16]));
        aMissionNavPoints[16].type = 1;
        configured = 1;
        memset(aeObjectClass, 0, sizeof(aeObjectClass));
        memset(nShipMissionIndices, 0xff, sizeof(nShipMissionIndices));
        memset(acShipTargetTable, 0xff, sizeof(acShipTargetTable));
        zero_vector(&aShipPositionsArray[0]);
        zero_vector(&aShipVelocity[0]);
        nCurrentNavigationPoint = 16;
        nCurrentWave = -1;
        cMissionObjectiveCountValue = 0;
        nYourWingman = -1;
        nCannedSceneMode = 1;
        bCurrentPlayerDestroyed = 0;
        bPlayerVulnerable = 0;
        nArcadeState = 0;
        bInitialFormationSetup = 1;
        for (index = 0; index < 4; index++) {
            set_objects_data((short)(index + 1), titleShips[index].type, -1);
            Set_up_ship_info((short)(index + 1), (short)(index + 32), 16);
            find_next_ship_turn_slot((short)(index + 1));
        }
        bInitialFormationSetup = 0;
        cViewObject = 1;
        SdlPlaySwcMusic("VICTORY.MooV", 50);
    } else {
        if (SdlCreateSwcTextFont(swcCinemaRenderer, &font, 10) != 0)
            goto done;
        cViewObject = 0;
        remove_nav_pointer();
        if (scene == SWC_FLIGHT_EJECTION) {
            if (SwcLoadFlightImages(swcCinemaCockpitCmf, swcCinemaSpaceCmf,
                    OBJECT_TYPE_EJECTED_PILOT, &swcCinemaImages[OBJECT_TYPE_EJECTED_PILOT]) != 0)
                goto done;
            pilot = find_vacant_3d_object();
            /* WC1's starfield owns slots 34..48 and repopulates them
             * on camera changes. Keep the pilot outside that range. */
            if (pilot >= 34 && pilot < 49) {
                for (pilot = 49; pilot <= SPACE_LAST_MOVING_OBJECT; pilot++) {
                    if (aeObjectClass[pilot] == OBJECT_CLASS_NULL)
                        break;
                }
                if (pilot > SPACE_LAST_MOVING_OBJECT)
                    pilot = -1;
            }
            if (pilot == -1) {
                /* A full effects pool must not prevent escape. Reclaim
                 * one transient effect, preserving ships and hazards. */
                for (object = 10; object <= SPACE_LAST_MOVING_OBJECT; object++) {
                    if ((object < 34 || object >= 49) &&
                        (aeObjectClass[object] == OBJECT_CLASS_EXPLOSION ||
                         aeObjectClass[object] == OBJECT_CLASS_DEBRIS ||
                         aeObjectClass[object] == OBJECT_CLASS_PROJECTILE)) {
                        pilot = (short)object;
                        remove_object(pilot);
                        break;
                    }
                }
            }
            if (pilot == -1) {
                SDL_SetError("No free SWC object slot for the ejected pilot");
                goto done;
            }
            set_objects_data(pilot, OBJECT_TYPE_EJECTED_PILOT, -1);
            asObjectCounterArray[pilot] = 32000;
            copy_frame(0, pilot);
            aShipPositionsArray[pilot] = aShipPositionsArray[0];
            ScaleFixedVector(&aShipUpVector[pilot], -0x500, &aShipVelocity[pilot]);
            AddFixedVectors(&aShipVelocity[pilot], &aShipVelocity[0], &aShipVelocity[pilot]);
            cViewObject = (signed char)pilot;
            /* The pilot has left the ship; later ship damage must not
             * change the already-selected ejection into a player death. */
            bPlayerVulnerable = 0;
            SdlPlaySwcMusic("EJECT-IM.MooV", 50);
        } else {
            SdlPlaySwcMusic("FUNERAL.MooV", 50);
        }
    }
    nCameraViewMode = 16;
    cCurrentCockpitView = 4;
    asObjectCollisionRadius[EYE_OBJECT] = MaxShort(10,
        asObjectCollisionRadius[(int)cViewObject]);
    copy_frame(cViewObject, EYE_OBJECT);
    position_relative_ijk(&aShipPositionsArray[EYE_OBJECT], cViewObject, 500, 0, 2000);
    zero_vector(&aShipVelocity[EYE_OBJECT]);
    look_at(cViewObject);
    SwcUpdateSceneCamera();
    SwcGenerateStars();
    nRenderedSpaceFrame = 0;
    focused = (SDL_GetWindowFlags(swcCinemaWindow) & SDL_WINDOW_INPUT_FOCUS) != 0;
    previous = SDL_GetTicks();
    for (;;) {
        now = SDL_GetTicks();
        activate = 0;
        while (SDL_PollEvent(&event)) {
            if (SdlHandleSwcEvent(swcCinemaWindow, &event, 0))
                continue;
            if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                result = SWC_MOVIE_QUIT;
                goto done;
            }
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
                    event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    focused = 0;
                    armedKey = SDL_SCANCODE_UNKNOWN;
                    mouseDown = 0;
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                           event.window.event == SDL_WINDOWEVENT_RESTORED) {
                    focused = (SDL_GetWindowFlags(swcCinemaWindow) & SDL_WINDOW_INPUT_FOCUS) != 0;
                }
            }
            if (!focused)
                continue;
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_p ||
                    (titleMenu && event.key.keysym.sym == SDLK_ESCAPE)) {
                    paused = !paused;
                    armedKey = SDL_SCANCODE_UNKNOWN;
                    mouseDown = 0;
                } else if (!titleMenu && (event.key.keysym.sym == SDLK_ESCAPE ||
                           event.key.keysym.sym == SDLK_BACKSPACE)) {
                    armedKey = event.key.keysym.scancode;
                } else if (!paused && (event.key.keysym.sym == SDLK_SPACE ||
                    event.key.keysym.sym == SDLK_RETURN ||
                    event.key.keysym.sym == SDLK_KP_ENTER)) {
                    armedKey = event.key.keysym.scancode;
                } else if (titleMenu && !paused &&
                    (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN ||
                     event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT ||
                     event.key.keysym.sym == SDLK_TAB)) {
                    SdlPlaySwcSound(30, 255);
                }
            }
            if (event.type == SDL_KEYUP && armedKey != SDL_SCANCODE_UNKNOWN &&
                event.key.keysym.scancode == armedKey) {
                if (!titleMenu && (event.key.keysym.sym == SDLK_ESCAPE ||
                                   event.key.keysym.sym == SDLK_BACKSPACE)) {
                    result = SWC_MOVIE_SKIPPED;
                    goto done;
                }
                activate = !paused;
                armedKey = SDL_SCANCODE_UNKNOWN;
            }
            if (paused)
                continue;
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                mouseDown = 1;
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                mouse = (SDL_Point){event.button.x, event.button.y};
                if (mouseDown && (!titleMenu || SDL_PointInRect(&mouse, &bounds[1])))
                    activate = 1;
                mouseDown = 0;
            }
        }
        SdlPauseSwcSound(!focused || paused);
        if (activate && focused && !paused && (titleMenu || ticks > 40)) {
            result = SWC_MOVIE_FINISHED;
            break;
        }
        if (focused && !paused)
            elapsed += SDL_min(now - previous, 250);
        else
            elapsed = 0;
        previous = now;
        /* Paused scenes still redraw the frozen picture and pause prompt,
         * including after window exposure, resize or fullscreen changes. */
        if (!focused || paused)
            goto draw_scene;
        if (elapsed < 50) {
            SDL_Delay(8);
            continue;
        }
        elapsed %= 50;
        if (titleMenu) {
            sides[0] = sides[1] = 0;
            for (object = 1; object < 10; object++) {
                if (aeObjectClass[object] < OBJECT_CLASS_SHIP)
                    continue;
                sides[aeShipSide[object] != SIDE_IMPERIAL]++;
                if (aeObjectClass[object] == OBJECT_CLASS_SHIP &&
                    (Vector_magnitude(&aShipPositionsArray[object]) >> 8) > 100000 &&
                    aeSpecialManeuvers[object] != SPECIAL_MANEUVER_UNKNOWN_9)
                    Explosion((short)object);
            }
            for (index = 0; index < 4; index++) {
                if (sides[titleShips[index].side != SIDE_IMPERIAL] == 0) {
                    aMissionShips[index + 32].state = 0;
                    init_ship((short)(index + 32), 16);
                }
            }
        } else if (ticks == 40) {
            Explosion(0);
        } else if (ticks > 40 && !wingman_dead() && !any_enemy(nYourWingman, 14000)) {
            aeShipMissionType[nYourWingman] = MISSION_TYPE_COME_HOME;
        }
        nTickCount60Hz = (unsigned int)((uint64_t)now * 60 / 1000);
        SdlSwcUpdateSpace();
        SwcUpdateSceneCamera();
        SwcUpdateStarField();
        nRenderedSpaceFrame++;
        ticks++;
draw_scene:
        if (SwcDrawFlight(swcCinemaRenderer, NULL, swcCinemaCockpitCmf,
                swcCinemaSpaceCmf, swcCinemaColors, swcCinemaImages, 1) != 0)
            goto done;
        if (titleMenu) {
            if (SDL_RenderCopy(swcCinemaRenderer, menuTextures[0], NULL, &bounds[0]) != 0 ||
                (nRenderedSpaceFrame % 6 < 4 &&
                 SDL_RenderCopy(swcCinemaRenderer, menuTextures[1], NULL, &bounds[1]) != 0))
                goto done;
        } else if (ticks > 40 &&
            SdlDrawSwcProportionalText(swcCinemaRenderer, &font,
                scene == SWC_FLIGHT_EJECTION ? 10 :
                    160 - SdlSwcTextWidth(&font, "Press <sp> to continue") / 2,
                200, swcCinemaColors[129], "Press <sp> to continue") != 0) {
            goto done;
        }
        if ((!focused || paused) &&
            SwcDrawPauseMessage(swcCinemaRenderer, swcCinemaColors, titleMenu, paused) != 0)
            goto done;
        SDL_RenderPresent(swcCinemaRenderer);
        if (titleMenu && ticks > 10000) {
            result = SWC_MOVIE_SKIPPED;
            break;
        }
        SDL_Delay(8);
    }
done:
    SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
    if (scene == SWC_FLIGHT_EJECTION) {
        /* Skipping the presentation still loses the abandoned ship. */
        if (pilot != -1 && ticks <= 40 && result == SWC_MOVIE_SKIPPED)
            Explosion(0);
        bPlayerVulnerable = savedVulnerable;
    }
    if (configured) {
        memcpy(&aMissionShips[32], savedShips, sizeof(savedShips));
        aMissionNavPoints[16] = savedNav;
    }
    if (titleMenu)
        SdlStopSwcMusic();
    SdlStopSwcSounds();
    if (titleMenu && result == SWC_MOVIE_FINISHED)
        SdlPlaySwcSound(31, 255);
    if (titleMenu && savedCursor != NULL)
        SDL_SetCursor(savedCursor);
    SDL_ShowCursor(cursorVisible);
    SDL_DestroyTexture(font.texture);
    for (index = 0; index < 2; index++)
        SDL_DestroyTexture(menuTextures[index]);
    SDL_free(menu.data);
    SwcResetFlightInput();
    if (result == SWC_MOVIE_ERROR)
        SDL_SetError("%s", failure);
    return result;
}

int SdlRunSwcMission(const char *missionPath, int cockpitless)
{
    SwcCmf cockpitCmf = {0};
    SwcCmf spaceCmf = {0};
    SwcBuffer cockpitSet = {0};
    SwcFlightImages images[OBJECT_TYPE_COUNT] = {{0}};
    ObjectTypeData savedObjects[OBJECT_TYPE_COUNT];
    SDL_Color colors[256];
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *cockpit = NULL;
    Viewport savedSpaceBuffer = stSpaceBufferViewport;
    Viewport *savedMouseViewport = stMouseCursorState.viewport;
    void (*savedInputPump)(void) = pCurrentEventManagerPump;
    int savedCockpitlessView = bCockpitlessView;
    SDL_Event event;
    Uint32 previous;
    Uint32 current;
    Uint32 tickInterval;
    Uint32 elapsed = 0;
    char path[PATH_MAX];
    char failure[256];
    int shipCount;
    int object;
    int view;
    int paused = 0;
    int focused = 1;
    int savedCursorVisible = -1;
    int objectsConfigured = 0;
    int movieResult;
    int choice;
    int command;
    int recipient;
    int done = 0;
    int result = -1;

    SwcInitCharacters();
    nCampaignDataSet = 0;
    if (SdlLoadSwcMissionData(missionPath, 1, 0) != 0)
        goto done;
    if (!SdlResolvePath("CMFs/PCShipV00.CMF", path, sizeof(path)) ||
        SwcCMOpen(path, &cockpitCmf) != 0)
        goto done;
    if (!SdlResolvePath("CMFs/Spaceflight.CMF", path, sizeof(path)) ||
        SwcCMOpen(path, &spaceCmf) != 0)
        goto done;
    SdlResolvePath("SuperWingCommanderDemo.rsrc", path, sizeof(path));
    if (SwcReadPalette(path, 251, colors) != 0) {
        SdlResolvePath("Super Wing Commander Demo/..namedfork/rsrc", path, sizeof(path));
        if (SwcReadPalette(path, 251, colors) != 0) {
            SDL_SetError("Cannot read the SWC palette. Preserve the demo's resource fork "
                         "or place the raw fork in SuperWingCommanderDemo.rsrc.");
            goto done;
        }
    }
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER |
                 SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
        goto done;
    savedCursorVisible = SDL_ShowCursor(SDL_QUERY);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    window = SDL_CreateWindow("Super Wing Commander",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 720,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL)
        goto done;
    renderer = SdlCreateSwcRenderer(window);
    if (renderer == NULL || SDL_RenderSetLogicalSize(renderer, 320, 240) != 0)
        goto done;
    if (SwcCMGetChunk(&cockpitCmf, "PC00", 1, &cockpitSet) != 0)
        goto done;
    cockpit = SdlCreateSwcTexture(renderer, &cockpitSet, 0, colors);
    if (cockpit == NULL)
        goto done;
    swcRasterCanvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, SWC_FRAME_WIDTH, SWC_FRAME_HEIGHT);
    if (swcRasterCanvas == NULL ||
        SDL_SetTextureBlendMode(swcRasterCanvas, SDL_BLENDMODE_BLEND) != 0)
        goto done;
    if (SdlInitSwcCockpit(renderer, &cockpitCmf, &spaceCmf, colors, swcRasterCanvas) != 0 ||
        SdlCreateSwcTextFont(renderer, &swcPauseFont, 10) != 0)
        goto done;
    /* Prepare shared mission state for the briefing map. No flight ticks or
       player input run while visiting the carrier or watching launch. */
    swcFlightActive = 1;
    bSwcWaitingForEscort = 0;
    nSwcCinemaResult = 0;
    swcCinemaWindow = window;
    swcCinemaRenderer = renderer;
    swcCinemaCockpitCmf = &cockpitCmf;
    swcCinemaSpaceCmf = &spaceCmf;
    swcCinemaColors = colors;
    swcCinemaImages = images;
    SwcSetObjectData(savedObjects);
    objectsConfigured = 1;
    srand(1);
    shipCount = SwcStartMissionShips();
    if (shipCount < 0)
        goto done;
    if (SdlInitSwcSound(path) != 0)
        goto done;
    SetEventManagerPump(NULL);
    stMouseCursorState.viewport = &stSpaceBufferViewport;
    bCockpitlessView = 0;
    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
    if (SdlInitSwcMusic() != 0)
        goto done;
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_PumpEvents();
    movieResult = SdlPlaySwcStartup(window, renderer, path, 0);
    if (movieResult == SWC_MOVIE_ERROR)
        goto done;
    if (movieResult == SWC_MOVIE_QUIT) {
        result = 0;
        goto done;
    }
title_menu:
    SdlStopSwcSounds();
    SdlStopSwcMusic();
    SwcResetFlightInput();
    FlushInputEvents();
    paused = 0;
    elapsed = 0;
    bSwcWaitingForEscort = 0;
    nSwcCinemaResult = 0;
    /* Title combat can destroy ships and change pilot/objective state.
     * Recreate the campaign on each menu visit and again on New Game. */
    SwcInitCharacters();
    if (SdlLoadSwcMissionData(missionPath, 1, 0) != 0 || SwcStartMissionShips() < 0)
        goto done;
    movieResult = SwcRunFlightScene(SWC_FLIGHT_TITLE);
    if (movieResult == SWC_MOVIE_ERROR)
        goto done;
    if (movieResult == SWC_MOVIE_SKIPPED) {
        movieResult = SdlPlaySwcStartup(window, renderer, path, 1);
        if (movieResult == SWC_MOVIE_ERROR)
            goto done;
        if (movieResult != SWC_MOVIE_QUIT)
            goto title_menu;
    }
    if (movieResult == SWC_MOVIE_QUIT) {
        result = 0;
        goto done;
    }
    SwcInitCharacters();
    if (SdlLoadSwcMissionData(missionPath, 1, 0) != 0)
        goto done;
    shipCount = SwcStartMissionShips();
    if (shipCount < 0 || SdlInitSwcCockpit(renderer, &cockpitCmf, &spaceCmf,
            colors, swcRasterCanvas) != 0)
        goto done;
    /* CODE_09 squadron +0x84 calls opening_scene before StartNewCampaign
       and GameFlow. Each host session starts a fresh Enyo 1 campaign. */
    movieResult = SdlPlaySwcOpening(window, renderer);
    if (movieResult == SWC_MOVIE_ERROR)
        goto done;
    if (movieResult == SWC_MOVIE_QUIT) {
        result = 0;
        goto done;
    }
    movieResult = SdlRunSwcCarrier(window, renderer, missionPath, path, SWC_CARRIER_PREFLIGHT);
    if (movieResult == -1)
        goto done;
    if (movieResult == SWC_MOVIE_QUIT) {
        result = 0;
        goto done;
    }
    movieResult = SdlPlaySwcLaunch(window, renderer);
    if (movieResult == SWC_MOVIE_ERROR)
        goto done;
    if (movieResult == SWC_MOVIE_QUIT) {
        result = 0;
        goto done;
    }
    /* Carrier/movie input never advances shared WC1 flight state. */
    focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    SdlUpdateSwcFlightMusic();
    SdlPauseSwcSound(!focused);
    SwcResetFlightInput();
    FlushInputEvents();
    SetEventManagerPump(NULL);
    stMouseCursorState.viewport = &stSpaceBufferViewport;
    /* SWC uses the same (160,100) scene center in either cockpit mode. */
    bCockpitlessView = 0;
    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
    bHostPrimaryMouseButton = 0;
    bHostSecondaryMouseButton = 0;
    nCurrentSpaceFrameValue = 0;
    nRenderedSpaceFrame = 0;
    SwcTransformObjectsToView();
    SdlUpdateSwcCockpit();
    SetSpaceFlightFrameTiming();
    tickInterval = (Uint32)nFrameIntervalMs;
    for (object = 1; object < shipCount; object++) {
        if (SwcLoadFlightImages(&cockpitCmf, &spaceCmf,
                                aeObjectTypeList[object], &images[aeObjectTypeList[object]]) != 0)
            goto done;
    }
    previous = SDL_GetTicks();
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    /* Mouse grabbing does not hide the system cursor. The WC1
       bMouseCursorVisible flag controls steering input, not SDL visibility. */
    SDL_ShowCursor(focused ? SDL_DISABLE : SDL_ENABLE);
    if (focused) {
        SDL_SetWindowMouseGrab(window, SDL_TRUE);
        SwcCentreFlightMouse();
    }
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_JOYDEVICEREMOVED)
                SwcResetFlightInput();
            if (SdlHandleSwcEvent(window, &event, 1))
                continue;
            if (event.type == SDL_QUIT)
                done = 1;
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
                    event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    focused = 0;
                    elapsed = 0;
                    SwcResetFlightInput();
                    SDL_SetWindowMouseGrab(window, SDL_FALSE);
                    SDL_ShowCursor(SDL_ENABLE);
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                           event.window.event == SDL_WINDOWEVENT_RESTORED) {
                    focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
                    previous = SDL_GetTicks();
                    if (focused && !paused) {
                        SDL_ShowCursor(SDL_DISABLE);
                        SDL_SetWindowMouseGrab(window, SDL_TRUE);
                        SwcCentreFlightMouse();
                    }
                } else if ((event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                            event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED) &&
                           focused && !paused) {
                    SDL_SetWindowMouseGrab(window, SDL_FALSE);
                    SDL_SetWindowMouseGrab(window, SDL_TRUE);
                }
            }
            if (focused && !paused) {
                /* SDL_RenderSetLogicalSize already converts these positions
                   to 320x240, including window scaling and letterboxing. */
                if (event.type == SDL_MOUSEMOTION) {
                    swcJoystickSteering = 0;
                    nHostMouseMessageX = SDL_clamp(event.motion.x,
                                                   stSpaceBufferViewport.left, stSpaceBufferViewport.right);
                    nHostMouseMessageY = SDL_clamp(event.motion.y,
                                                   stSpaceBufferViewport.top, stSpaceBufferViewport.bottom);
                    bMouseCursorVisible = 1;
                } else if (event.type == SDL_MOUSEWHEEL &&
                           !bCurrentPlayerDestroyed && nArcadeState == 0) {
                    int wheel = event.wheel.y;

                    if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                        wheel = -wheel;
                    if (wheel != 0)
                        accelerate(wheel > 0 ? 1 : -1);
                } else if (event.type == SDL_MOUSEBUTTONDOWN &&
                           event.button.button == SDL_BUTTON_LEFT) {
                    /* Preserve clicks that begin and end between simulation ticks. */
                    swcGunFirePending = 1;
                } else if ((event.type == SDL_MOUSEBUTTONDOWN ||
                            event.type == SDL_MOUSEBUTTONUP) &&
                           event.button.button == SDL_BUTTON_RIGHT) {
                    nHostMouseMessageX = SDL_clamp(event.button.x,
                                                   stSpaceBufferViewport.left, stSpaceBufferViewport.right);
                    nHostMouseMessageY = SDL_clamp(event.button.y,
                                                   stSpaceBufferViewport.top, stSpaceBufferViewport.bottom);
                    if (event.type == SDL_MOUSEBUTTONDOWN) {
                        /* WC1 uses a timing-only window of 20 ticks at 60 Hz.
                           Recentring after the first click must not cancel it. */
                        swcMouseAfterburner = swcRightClickPending &&
                            (Uint32)(event.button.timestamp - swcLastRightClick) <=
                                20 * 1000 / 60;
                        swcLastRightClick = event.button.timestamp;
                        swcRightClickPending = 1;
                    } else {
                        swcMouseAfterburner = 0;
                    }
                    bMouseCursorVisible = 1;
                }
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    if (focused && !paused)
                        swcGunFirePending = 1;
                    break;
                case SDLK_BACKSPACE:
                    if (focused && !paused)
                        anShipSpeed[0] = 0;
                    break;
                case SDLK_BACKSLASH:
                    if (focused && !paused)
                        celerate(0, (int)asShipMaximumSpeedArray[0] * 256 - anShipSpeed[0]);
                    break;
                case SDLK_n:
                    if (focused && !paused)
                        cycle_next_objective();
                    break;
                case SDLK_y:
                case SDLK_d:
                case SDLK_f:
                    if (focused && !paused && !bCurrentPlayerDestroyed && nArcadeState == 0) {
                        view = event.key.keysym.sym == SDLK_y ? 0 :
                            event.key.keysym.sym == SDLK_d ? 14 : 4;
                        if (view == nCameraViewMode)
                            view = 0;
                        if (view != nCameraViewMode)
                            SwcSelectFlightView(view);
                    }
                    break;
                case SDLK_a:
                    if (focused && !paused && !bCurrentPlayerDestroyed && nArcadeState == 0) {
                        FixedVector departure = aShipPositionsArray[0];

                        auto_pilot_sequence();
                        if (nSwcCinemaResult < 0)
                            goto done;
                        if (nSwcCinemaResult > 0) {
                            result = 0;
                            goto done;
                        }
                        focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
                        SDL_SetWindowMouseGrab(window, focused ? SDL_TRUE : SDL_FALSE);
                        SDL_ShowCursor(focused ? SDL_DISABLE : SDL_ENABLE);
                        /* Travel can move the ship by an entire nav leg.
                         * Reposition the selected camera at the arrival. */
                        if (!equ_vector(&departure, &aShipPositionsArray[0]))
                            SwcSelectFlightView(nCameraViewMode);
                        elapsed = 0;
                        previous = SDL_GetTicks();
                        if (focused)
                            SwcCentreFlightMouse();
                    }
                    break;
                case SDLK_l:
                    if (focused && !paused)
                        nTargetLockMode = (short)(nTargetLockMode == 0);
                    break;
                case SDLK_t:
                    if (focused && !paused)
                        cycle_onscreen_targets();
                    break;
                case SDLK_g:
                    if (focused && !paused)
                        select_new_gun();
                    break;
                case SDLK_w:
                    if (focused && !paused) {
                        select_new_release_weapon((enum ObjectType)-1);
                        SdlSwcLockOff();
                    }
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    if (focused && !paused)
                        swcReleaseWeaponPending = 1;
                    break;
                case SDLK_c:
                    if (focused && !paused && !bCurrentPlayerDestroyed && nArcadeState == 0) {
                        if (IsCommChoiceMenuOpen())
                            CloseCommChoiceMenu();
                        else
                            HandleCommunicationMenuRequest();
                    }
                    break;
                case SDLK_e:
                    if (focused && !paused && !bCurrentPlayerDestroyed && nArcadeState == 0 &&
                        (event.key.keysym.mod & KMOD_CTRL) && acPlayerComponentDamageList[7] < 4) {
                        /* CODE_01 process_player_input +0x3cbe: an intact
                         * ejector succeeds; damaged ejectors use Random3DO
                         * (0, damage), with its exclusive upper bound. */
                        if (acPlayerComponentDamageList[7] <= 1 ||
                            RandomBelowOrEqual((short)(acPlayerComponentDamageList[7] - 1)) == 0)
                            nArcadeState = 2;
                        else
                            SdlPlaySwcSound(25, 127);
                    }
                    break;
                case SDLK_1:
                case SDLK_2:
                case SDLK_3:
                case SDLK_4:
                case SDLK_5:
                case SDLK_6:
                case SDLK_7:
                    choice = (int)(event.key.keysym.sym - SDLK_1);
                    if (focused && !paused && !bCurrentPlayerDestroyed && IsCommChoiceMenuOpen() &&
                        choice < nCommMenuChoiceCount) {
                        command = GetPendingMenuAction() == 2 ? abCommMenuChoiceCommands[choice] : -1;
                        recipient = cCommMenuRecipient;
                        Chosen_communicate_option((short)choice);
                        if (command > 0)
                            SdlSwcVocalizeMessage(command);
                        if (command == 12)
                            SdlSwcCockpitMessage(bLandingAuthorized ? "Landing clearance granted" :
                                "Landing clearance denied", 0x81, 0, -1);
                        else if (command > 0 && recipient > 0 && recipient < 10)
                            SdlSwcCockpitMessage(acWingmanMessageStatesArray[recipient] == 1 ?
                                "Unable to comply" : "Message sent", 0x81, 0, -1);
                    }
                    break;
                case SDLK_F1:
                    if (event.key.keysym.mod & KMOD_CTRL) {
                        cockpitless = !cockpitless;
                        if (focused && !paused)
                            SwcCentreFlightMouse();
                    }
                    break;
                case SDLK_m:
                    if (focused && !paused && !bCurrentPlayerDestroyed && nArcadeState == 0) {
                        SwcResetFlightInput();
                        if (IsCommChoiceMenuOpen())
                            CloseCommChoiceMenu();
                        movieResult = SdlShowSwcComputer(window, renderer, path);
                        if (movieResult == SWC_MOVIE_ERROR)
                            goto done;
                        if (movieResult == SWC_MOVIE_QUIT) {
                            result = 0;
                            goto done;
                        }
                        focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
                        SwcResetFlightInput();
                        elapsed = 0;
                        previous = SDL_GetTicks();
                        if (focused)
                            SwcCentreFlightMouse();
                    }
                    break;
                case SDLK_ESCAPE:
                case SDLK_p:
                    paused = !paused;
                    elapsed = 0;
                    previous = SDL_GetTicks();
                    SwcResetFlightInput();
                    SDL_SetWindowMouseGrab(window,
                        focused && !paused ? SDL_TRUE : SDL_FALSE);
                    SDL_ShowCursor(focused && !paused ? SDL_DISABLE : SDL_ENABLE);
                    if (focused && !paused)
                        SwcCentreFlightMouse();
                    break;
                default:
                    break;
                }
            }
        }
        if (done)
            break;
        SdlPauseSwcSound(!focused || paused);
        current = SDL_GetTicks();
        if (focused && !paused)
            elapsed += SDL_min(current - previous, 250);
        else
            elapsed = 0;
        previous = current;
        while (elapsed >= tickInterval && nArcadeState == 0 && !bCurrentPlayerDestroyed) {
            nTickCount60Hz = (unsigned int)((uint64_t)SDL_GetTicks() * 60 / 1000);
            SwcFlightTick(SDL_GetKeyboardState(NULL), SDL_GetMouseState(NULL, NULL));
            elapsed -= tickInterval;
        }
        if ((nArcadeState >= 1 && nArcadeState <= 4) || bCurrentPlayerDestroyed) {
            SdlStopSwcSounds();
            SdlStopSwcMusic();
            SDL_SetWindowMouseGrab(window, SDL_FALSE);
            SwcResetFlightInput();
            FlushInputEvents();
            if (IsCommChoiceMenuOpen())
                CloseCommChoiceMenu();
            if (nArcadeState == 2) {
                movieResult = SwcRunFlightScene(SWC_FLIGHT_EJECTION);
                if (movieResult == SWC_MOVIE_ERROR)
                    goto done;
                if (movieResult == SWC_MOVIE_QUIT) {
                    result = 0;
                    goto done;
                }
                nArcadeState = 2;
                check_stranded();
                if (nArcadeState != 3) {
                    /* CODE_09 GameFlow +0x1ec0..+0x1ef2: rescue costs one
                     * promotion point; first ejection queues medal 3. */
                    nArcadeState = 0;
                    bPlayerEjectedThisMission = 1;
                    stCampaignState.promotionScore = MaxShort(0,
                        (short)(stCampaignState.promotionScore - 1));
                    stCampaignState.elapsedDate.year++;
                    if (stCampaignState.elapsedDate.year == 1)
                        nPendingMedalIndex = 3;
                    bOfficeVisitPending = 1;
                }
            } else if (nArcadeState == 4 || bCurrentPlayerDestroyed) {
                /* CODE_09 GameFlow +0x1b8e: death leads to the player's funeral,
                   without landing or PostMission statistics. */
                movieResult = SwcRunFlightScene(SWC_FLIGHT_DEATH);
                if (movieResult == SWC_MOVIE_ERROR)
                    goto done;
                if (movieResult != SWC_MOVIE_QUIT)
                    movieResult = SdlRunSwcCarrier(window, renderer, missionPath, path,
                                                    SWC_CARRIER_PLAYER_FUNERAL);
                if (movieResult == SWC_MOVIE_ERROR)
                    goto done;
                if (movieResult != SWC_MOVIE_QUIT)
                    goto title_menu;
                result = 0;
                goto done;
            }
            if (nArcadeState == 3) {
                movieResult = SdlShowSwcEnding(window, renderer, path, 1);
                if (movieResult == SWC_MOVIE_ERROR)
                    goto done;
                if (movieResult != SWC_MOVIE_QUIT)
                    goto title_menu;
                result = 0;
                goto done;
            }
            cleanup_objectives();
            movieResult = bPlayerEjectedThisMission ? SWC_MOVIE_FINISHED :
                SdlPlaySwcLanding(window, renderer, evaluate_damage(0));
            if (movieResult == SWC_MOVIE_ERROR)
                goto done;
            if (movieResult != SWC_MOVIE_QUIT) {
                /* CODE_09 PostMission +0x1672 shares WC1's player/wingman
                   statistics and badges. Keep the flown mission selected. */
                PostMission();
                /* GameFlow +0x1f6a..+0x1f70 skips DeBriefing after rescue.
                 * Enyo 1 cannot finish its two-mission series (CMP0/2 +4),
                 * so +0x2002..+0x200a also bypasses Office/MedalSequence,
                 * even when the first ejection has queued medal 3. */
                if (!bPlayerEjectedThisMission) {
                    movieResult = SdlRunSwcCarrier(window, renderer, missionPath, path,
                                                    SWC_CARRIER_DEBRIEFING);
                    if (movieResult == SWC_MOVIE_ERROR)
                        goto done;
                }
                if (movieResult != SWC_MOVIE_QUIT && nCurrentWingmanKilledThisMission) {
                    /* GameFlow +0x218c dispatches the lost-wingman service
                       after any debriefing. The flown mission remains selected. */
                    movieResult = SdlRunSwcCarrier(window, renderer, missionPath, path,
                                                    SWC_CARRIER_WINGMAN_FUNERAL);
                    if (movieResult == SWC_MOVIE_ERROR)
                        goto done;
                }
            }
            if (movieResult != SWC_MOVIE_QUIT)
                goto title_menu;
            result = 0;
            goto done;
        }
        if (SwcDrawFlight(renderer, cockpit, &cockpitCmf, &spaceCmf, colors,
                          images, cockpitless) != 0)
            goto done;
        if ((!focused || paused) && SwcDrawPauseMessage(renderer, colors, 1, paused) != 0)
            goto done;
        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }
    result = 0;
done:
    SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
    SdlFreeSwcSound();
    SdlSetKeyboardSnapshot(NULL);
    SwcResetFlightInput();
    if (swcFlightActive) {
        FlushInputEvents();
        SdlEndJoystickSpaceflight();
        SetCinematicFrameTiming();
    }
    stSpaceBufferViewport = savedSpaceBuffer;
    stMouseCursorState.viewport = savedMouseViewport;
    SetEventManagerPump(savedInputPump);
    bCockpitlessView = savedCockpitlessView;
    swcFlightActive = 0;
    swcCinemaWindow = NULL;
    swcCinemaRenderer = NULL;
    swcCinemaCockpitCmf = NULL;
    swcCinemaSpaceCmf = NULL;
    swcCinemaColors = NULL;
    swcCinemaImages = NULL;
    if (objectsConfigured)
        memcpy(aObjectTypeData, savedObjects, sizeof(aObjectTypeData));
    if (window != NULL)
        SDL_SetWindowMouseGrab(window, SDL_FALSE);
    if (savedCursorVisible >= 0)
        SDL_ShowCursor(savedCursorVisible);
    for (object = 0; object < OBJECT_TYPE_COUNT; object++) {
        for (view = 0; (uint32_t)view < images[object].count; view++)
            SdlFreeSwcRasterImage(&images[object].views[view]);
        SDL_free(images[object].views);
    }
    SdlFreeSwcCockpit();
    SDL_DestroyTexture(cockpit);
    SDL_DestroyTexture(swcRasterCanvas);
    swcRasterCanvas = NULL;
    SDL_DestroyTexture(swcPauseFont.texture);
    SDL_zero(swcPauseFont);
    for (view = 0; view < (int)SDL_arraysize(swcStarTextures); view++) {
        SDL_DestroyTexture(swcStarTextures[view]);
        swcStarTextures[view] = NULL;
    }
    SDL_DestroyRenderer(renderer);
    SdlShutdownJoysticks();
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_free(cockpitSet.data);
    SwcCMClose(&spaceCmf);
    SwcCMClose(&cockpitCmf);
    if (result != 0)
        SDL_SetError("%s", failure);
    return result;
}
