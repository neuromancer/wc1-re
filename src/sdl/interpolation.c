/* Spaceflight frame interpolation (issues #5 and #38), proof of concept.
 *
 * WC1 steps its spaceflight once per frame, at 20 frames a second by default
 * (fSpaceFlightFrameRate). This keeps that step and its timing, and fills the
 * wait before the next step with frames drawn between the last two steps:
 * each object is moved and turned part of the way from where the previous
 * step left it, and the view is drawn again and presented. The cockpit
 * around the view keeps the step's own drawing.
 *
 * An in-between frame runs the drawing half of RenderSpaceViewFrame with the
 * state it writes saved and restored, so the simulation never sees it: its
 * results are bit-identical with or without interpolation. It does not
 * respawn stars or place exhaust, which call rand(), nor advance the missile
 * lock or the bracket blink. What is shown runs one step behind the
 * simulation. */
#include "wc1.h"

#include "video_internal.h"

#include <stddef.h>

/* Orientation vectors are Q8; a step turning more than this per component
   (about 22 degrees) is a cut, such as a view change, not motion. */
#define INTERPOLATION_MAX_TURN 96

#define FLIGHT_SCREEN_PIXELS (SDL_PORT_FRAME_WIDTH * SDL_PORT_FRAME_HEIGHT)

typedef struct SavedRange {
    void *data;
    size_t size;
} SavedRange;

#define SAVED(name) {(void *)&(name), sizeof(name)}

/* Everything an in-between frame writes: the poses it moves, the view
   lists, sorting and screen positions drawing produces, the HUD state the
   overlay touches, and the slot draw_nav_pointer takes or frees for the nav
   pointer when the right VDU changed mode since the step was drawn. */
static const SavedRange g_aSavedFlightState[] = {
    SAVED(aeObjectClass), SAVED(acObjOwnersTable), SAVED(nYourWingman),
    SAVED(abHazardObjects),
    SAVED(aShipPositionsArray), SAVED(aShipFwdVec), SAVED(aShipRightVector),
    SAVED(aShipUpVector), SAVED(asPreviousObjectDistance),
    SAVED(asObjDistancesTable), SAVED(asObjectScreenXList),
    SAVED(asObjectScreenYArray), SAVED(aObjectViewPositionsList),
    SAVED(asObjScreenScalesArray), SAVED(asObjScreenAngles),
    SAVED(asObjectFlipArray), SAVED(asObjectViewFrames), SAVED(apObjShapeArray),
    SAVED(asObjectDrawPosX), SAVED(asObjectDrawPosY), SAVED(anSortedObjects),
    SAVED(anEventObjectDepthPlacedList), SAVED(nClosestVisibleObject),
    SAVED(nNavPointerObject), SAVED(cLastTargetObject), SAVED(DAT_0046c05c),
    SAVED(nStoredMouseCursorPosX), SAVED(nSavedMouseCursorY),
    SAVED(pszPendingHudMessage), SAVED(nPendingHudMessageColour),
    SAVED(nTargetLockCountdown), SAVED(nTargetLockMode)
};

static int g_bFrameInterpolation;
static int g_bFlightFrameDeferred;
static int g_bInterpolatedPass;
static int g_bPreviousPosesValid;
/* The screen as RenderSpaceViewFrame left it, the screen the step presents,
   and which of its pixels RunSpaceFlight drew in between: update_cockpit's
   instruments, which the cockpitless view draws over the view (issue #51),
   as wc2-re lays its full-screen HUD over the in-between frames. */
static unsigned char g_abFlightViewScreen[FLIGHT_SCREEN_PIXELS];
static unsigned char g_abFlightStepScreen[FLIGHT_SCREEN_PIXELS];
static unsigned char g_abFlightOverlayMask[FLIGHT_SCREEN_PIXELS];
static int g_bFlightViewCaptured;
static Uint32 g_dwInterpolatedPresentMs = 16;
static unsigned char *g_pSavedFlightState;
static size_t g_nSavedFlightStateSize;

/* The previous step's poses. */
static FixedVector g_aPreviousPositions[SPACE_OBJECT_COUNT];
static FixedVector g_aPreviousVelocities[SPACE_OBJECT_COUNT];
static FixedVector g_aPreviousFwd[SPACE_OBJECT_COUNT];
static FixedVector g_aPreviousRight[SPACE_OBJECT_COUNT];
static FixedVector g_aPreviousUp[SPACE_OBJECT_COUNT];
static enum ObjectClass g_aePreviousClass[SPACE_OBJECT_COUNT];
static enum ObjectType g_aePreviousType[SPACE_OBJECT_COUNT];

/* Child objects' offsets from their parents, as reposition_fixed_child_objects
   read them in the step's own drawing; it leaves them absolute. */
static short g_asChildScreenX[SPACE_OBJECT_COUNT];
static short g_asChildScreenY[SPACE_OBJECT_COUNT];
static unsigned short g_asChildDistance[SPACE_OBJECT_COUNT];
static int g_bChildOffsetsValid;

void SdlSetFrameInterpolation(int enabled)
{
    g_bFrameInterpolation = enabled != 0;
}

int SdlDrawingInterpolatedFrame(void)
{
    return g_bInterpolatedPass;
}

/* RunSpaceFlight, at its start and end: nothing to interpolate from. */
void SdlResetFrameInterpolation(void)
{
    g_bPreviousPosesValid = 0;
    g_bChildOffsetsValid = 0;
    g_bFlightFrameDeferred = 0;
    g_bFlightViewCaptured = 0;
}

/* RunSpaceFlight, before each Update_3Space. */
void SdlCaptureFlightPoses(void)
{
    short obj;

    if (!g_bFrameInterpolation)
        return;
    for (obj = 0; obj < SPACE_OBJECT_COUNT; obj++) {
        g_aPreviousPositions[obj] = aShipPositionsArray[obj];
        g_aPreviousVelocities[obj] = aShipVelocity[obj];
        g_aPreviousFwd[obj] = aShipFwdVec[obj];
        g_aPreviousRight[obj] = aShipRightVector[obj];
        g_aPreviousUp[obj] = aShipUpVector[obj];
        g_aePreviousClass[obj] = aeObjectClass[obj];
        g_aePreviousType[obj] = aeObjectTypeList[obj];
    }
    g_bPreviousPosesValid = 1;
}

/* Draw_3Space_Frame, just before reposition_fixed_child_objects. */
void SdlSnapshotFlightChildren(void)
{
    if (!g_bFrameInterpolation || g_bInterpolatedPass)
        return;
    memcpy(g_asChildScreenX, asObjectScreenXList, sizeof(g_asChildScreenX));
    memcpy(g_asChildScreenY, asObjectScreenYArray, sizeof(g_asChildScreenY));
    memcpy(g_asChildDistance, asObjDistancesTable, sizeof(g_asChildDistance));
    g_bChildOffsetsValid = 1;
}

/* RunSpaceFlight, after RenderSpaceViewFrame and before update_cockpit. */
void SdlCaptureFlightView(void)
{
    if (!g_bFrameInterpolation || stScreenView.pixels == 0)
        return;
    memcpy(g_abFlightViewScreen, stScreenView.pixels,
           sizeof(g_abFlightViewScreen));
    g_bFlightViewCaptured = 1;
}

/* RunSpaceFlight, before presenting a drawn frame: DIBslamReal leaves the
   present to ThrottleFrameAndDrawFps, which interpolates up to it. */
void SdlDeferFlightFrame(void)
{
    size_t pixel;

    g_bFlightFrameDeferred = g_bFrameInterpolation && g_bChildOffsetsValid &&
        g_bFlightViewCaptured && stScreenView.pixels != 0;
    g_bFlightViewCaptured = 0;
    if (!g_bFlightFrameDeferred)
        return;
    memcpy(g_abFlightStepScreen, stScreenView.pixels,
           sizeof(g_abFlightStepScreen));
    for (pixel = 0; pixel < FLIGHT_SCREEN_PIXELS; pixel++)
        g_abFlightOverlayMask[pixel] =
            g_abFlightStepScreen[pixel] != g_abFlightViewScreen[pixel];
}

int SdlFlightFrameDeferred(void)
{
    return g_bFlightFrameDeferred;
}

static int SaveFlightState(void)
{
    size_t index;
    size_t offset;

    if (g_pSavedFlightState == 0) {
        for (index = 0; index < SDL_arraysize(g_aSavedFlightState); index++)
            g_nSavedFlightStateSize += g_aSavedFlightState[index].size;
        g_pSavedFlightState = malloc(g_nSavedFlightStateSize);
        if (g_pSavedFlightState == 0)
            return 0;
    }
    offset = 0;
    for (index = 0; index < SDL_arraysize(g_aSavedFlightState); index++) {
        memcpy(g_pSavedFlightState + offset, g_aSavedFlightState[index].data,
               g_aSavedFlightState[index].size);
        offset += g_aSavedFlightState[index].size;
    }
    return 1;
}

static void RestoreFlightState(void)
{
    size_t index;
    size_t offset;

    offset = 0;
    for (index = 0; index < SDL_arraysize(g_aSavedFlightState); index++) {
        memcpy(g_aSavedFlightState[index].data, g_pSavedFlightState + offset,
               g_aSavedFlightState[index].size);
        offset += g_aSavedFlightState[index].size;
    }
}

static int Lerp(int previous, int current, int alpha)
{
    return previous + (int)(((int64_t)current - previous) * alpha / 256);
}

static void LerpVector(FixedVector *current, const FixedVector *previous,
                       int alpha)
{
    current->x = Lerp(previous->x, current->x, alpha);
    current->y = Lerp(previous->y, current->y, alpha);
    current->z = Lerp(previous->z, current->z, alpha);
}

static int64_t LargestComponent(const FixedVector *vector)
{
    int64_t x = vector->x < 0 ? -(int64_t)vector->x : vector->x;
    int64_t y = vector->y < 0 ? -(int64_t)vector->y : vector->y;
    int64_t z = vector->z < 0 ? -(int64_t)vector->z : vector->z;

    return x > y ? (x > z ? x : z) : (y > z ? y : z);
}

static int64_t LargestDifference(const FixedVector *current,
                                 const FixedVector *previous)
{
    FixedVector difference;

    difference.x = current->x - previous->x;
    difference.y = current->y - previous->y;
    difference.z = current->z - previous->z;
    return LargestComponent(&difference);
}

/* Moves each object part of the way back to its previous pose. A slot whose
   object changed, a jump its velocity does not explain (a respawn or the
   autopilot), and a sharp turn (a view change) show the step's own pose.
   Stars and dust are respawned by direction, and children follow their
   parents. */
static void InterpolatePoses(int alpha)
{
    int64_t moved;
    int64_t speed;
    short obj;

    for (obj = 0; obj < SPACE_OBJECT_COUNT; obj++) {
        /* The eye is no object of its own: set_eye_direction_and_position
           gives it the player's or the camera's pose every step. */
        if (obj != EYE_OBJECT &&
            (aeObjectClass[obj] == OBJECT_CLASS_NULL ||
             aeObjectClass[obj] != g_aePreviousClass[obj] ||
             aeObjectTypeList[obj] != g_aePreviousType[obj] ||
             aeObjectClass[obj] == OBJECT_CLASS_STAR ||
             aeObjectClass[obj] == OBJECT_CLASS_DUST ||
             aeObjectClass[obj] == OBJECT_CLASS_FIXED_OBJECT))
            continue;
        moved = LargestDifference(&aShipPositionsArray[obj],
                                  &g_aPreviousPositions[obj]);
        speed = LargestComponent(&aShipVelocity[obj]);
        if (LargestComponent(&g_aPreviousVelocities[obj]) > speed)
            speed = LargestComponent(&g_aPreviousVelocities[obj]);
        if (moved <= 3 * speed + 256)
            LerpVector(&aShipPositionsArray[obj], &g_aPreviousPositions[obj],
                       alpha);
        if (LargestDifference(&aShipFwdVec[obj], &g_aPreviousFwd[obj]) <=
                INTERPOLATION_MAX_TURN &&
            LargestDifference(&aShipRightVector[obj], &g_aPreviousRight[obj]) <=
                INTERPOLATION_MAX_TURN &&
            LargestDifference(&aShipUpVector[obj], &g_aPreviousUp[obj]) <=
                INTERPOLATION_MAX_TURN) {
            LerpVector(&aShipFwdVec[obj], &g_aPreviousFwd[obj], alpha);
            LerpVector(&aShipRightVector[obj], &g_aPreviousRight[obj], alpha);
            LerpVector(&aShipUpVector[obj], &g_aPreviousUp[obj], alpha);
        }
    }
}

/* The drawing half of RenderSpaceViewFrame at alpha/256 of the way from the
   previous step to this one, presented and then undone. */
void SdlDrawInterpolatedFlightFrame(int alpha)
{
    unsigned char *screen;
    size_t pixel;
    short obj;

    if (!SaveFlightState())
        return;
    if (g_bPreviousPosesValid && alpha < 256)
        InterpolatePoses(alpha);
    g_bInterpolatedPass = 1;
    transform_objects_to_your_view();
    for (obj = 0; obj < SPACE_OBJECT_COUNT; obj++) {
        if (aeObjectClass[obj] != OBJECT_CLASS_FIXED_OBJECT)
            continue;
        asObjectScreenXList[obj] = g_asChildScreenX[obj];
        asObjectScreenYArray[obj] = g_asChildScreenY[obj];
        asObjDistancesTable[obj] = g_asChildDistance[obj];
    }
    reposition_fixed_child_objects();
    sort_object_depth();
    SdlBeginSpaceFrame(pScreenViewGeometry, cScreenViewMode,
                       bCockpitlessView > 0, cPrimaryViewBufferColour);
    draw_sorted_objects_to_buffer();
    if (nCameraViewMode == 0)
        overlay_head_up_display();
    if (nTrainSimActive != 0) {
        SetTextContext(&stHudMessageTextContext);
        DrawArcadeScorePanel(10, 10);
    }
    RestoreCockpitExplosionIfVisible();
    dump_buffer_to_screen();
    if (nCameraViewMode == 0)
        RestoreTransientCockpitGraphics();
    ClearViewport(&stSpaceBufferViewport, cPrimaryViewBufferColour);
    /* What the step drew over its view, as it drew it. */
    screen = stScreenView.pixels;
    for (pixel = 0; pixel < FLIGHT_SCREEN_PIXELS; pixel++) {
        if (g_abFlightOverlayMask[pixel])
            screen[pixel] = g_abFlightStepScreen[pixel];
    }
    g_bInterpolatedPass = 0;
    SdlPresentCurrentFrame(stScreenView.pixels);
    RestoreFlightState();
}

/* ThrottleFrameAndDrawFps, before waiting for the next step: frames until
   that step is due, each drawn at its expected display time. The last ends
   at the step itself, as the game would have presented it. */
void SdlPresentInterpolatedFlightFrames(long deadline, long interval)
{
    Uint32 start;
    Uint32 now;
    long elapsed;
    int alpha;
    int frames;

    if (!g_bFlightFrameDeferred)
        return;
    g_bFlightFrameDeferred = 0;
    frames = 0;
    for (;;) {
        now = SDL_GetTicks();
        if (frames > 0 && (long)(deadline - (long)now) <
                              (long)g_dwInterpolatedPresentMs)
            break;
        elapsed = (long)now + (long)g_dwInterpolatedPresentMs -
                  (deadline - interval);
        alpha = interval > 0 ? (int)(elapsed * 256 / interval) : 256;
        if (alpha < 0)
            alpha = 0;
        if (alpha > 256)
            alpha = 256;
        start = SDL_GetTicks();
        SdlDrawInterpolatedFlightFrame(alpha);
        g_dwInterpolatedPresentMs = SDL_GetTicks() - start;
        if (g_dwInterpolatedPresentMs < 1)
            g_dwInterpolatedPresentMs = 1;
        if (g_dwInterpolatedPresentMs > 50)
            g_dwInterpolatedPresentMs = 50;
        frames++;
        if (alpha >= 256)
            break;
    }
}
