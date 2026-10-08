/* SDL presentation of the SWC Mac demo's Hornet instruments.
 * CODE_13 and the expanded DATA/0 tables provide the layout and sprite
 * selection. WC1 supplies flight state, speed readouts and scanner maths.
 * Mac QuickDraw text is replaced by a bundled, pre-rendered bitmap font. */
#include "wc1.h"
#include "swc.h"

#include <string.h>

typedef struct SwcCockpitFrame {
    SDL_Texture *texture;
    SDL_Rect bounds;
} SwcCockpitFrame;

typedef struct SwcCockpitShape {
    SwcCockpitFrame *frames;
    uint32_t count;
} SwcCockpitShape;

enum SwcCockpitInstrument {
    SWC_PLAYER_STATUS,
    SWC_WEAPON_ENERGY,
    SWC_FUEL,
    SWC_THROTTLE,
    SWC_SCANNER_GRID,
    SWC_SCANNER_CONTACTS,
    SWC_TARGET_SHIELDS,
    SWC_RETICLE,
    SWC_AUTOPILOT_LIGHT,
    SWC_DAMAGE_LIGHT,
    SWC_MISSILE_LIGHT,
    SWC_COCKPIT_DAMAGE,
    SWC_COCKPIT_SPARKS,
    SWC_COCKPIT_INSTRUMENT_COUNT
};

/* Expanded DATA/0 +0x300, Hornet entry. Original words are BE32. */
const CockpitScannerGeometry stSwcCockpitScanner = {
    159, 171, 148, 160, 169, 181
};

static SwcCockpitShape swcCockpitShapes[SWC_COCKPIT_INSTRUMENT_COUNT];
static SwcCockpitShape swcTargetShapes[OBJECT_TYPE_COUNT];
static SwcCockpitShape swcDeadVduShapes[2];
static const SwcCmf *swcCockpitCmf;
static SDL_Renderer *swcCockpitRenderer;
static SDL_Texture *swcCockpitFont;
static SDL_Color swcCockpitColors[256];
static int swcCockpitDrawing;
static int swcCockpitDrawResult;
static short swcMalfunctionTicks[9];
static int swcVduDead[2];
static int swcVduStaticFrame[2];
static int swcAutopilotLight;
static int swcMissileLight;
static int swcDamageLight;
static int swcScannerFrames[10];
static int swcScannerEnemy = -1;
static int swcHostileArrow = -1;
static int swcDamageComponent;
static int swcDamageDisplayTicks;
static int swcSparkFrame = -1;
static int swcSparkRegion;
static short swcSparkStartFrame;
static SDL_Point swcSparkPosition;
static int swcIttsActive;
static SDL_Point swcIttsPosition = {0x7fff, 0};

/* DATA/0 +0x7f2: radar, left VDU, right VDU. */
static const SDL_Point swcDamagePositions[3] = {
    {142, 153}, {33, 134}, {231, 132}
};

static void SwcFreeCockpitShape(SwcCockpitShape *shape)
{
    uint32_t frame;

    if (shape->frames != NULL) {
        for (frame = 0; frame < shape->count; frame++)
            SDL_DestroyTexture(shape->frames[frame].texture);
        SDL_free(shape->frames);
    }
    memset(shape, 0, sizeof(*shape));
}

static int SwcLoadCockpitShape(const SwcCmf *cmf, const char *type,
                               uint32_t chunk, uint32_t count,
                               SwcCockpitShape *shape)
{
    SwcBuffer set = {0};
    SwcFrame frame;
    uint32_t image;
    int result = -1;

    if (shape->frames != NULL)
        return 0;
    if (SwcCMGetChunk(cmf, type, chunk, &set) != 0)
        return -1;
    if (SwcGetFrameCount(&set, &shape->count) != 0)
        goto done;
    if (shape->count != count) {
        SDL_SetError("Unexpected SWC cockpit frame count in %s/%u", type, chunk);
        goto done;
    }
    shape->frames = SDL_calloc(shape->count, sizeof(*shape->frames));
    if (shape->frames == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    for (image = 0; image < shape->count; image++) {
        if (SwcGetFramePtr(&set, image, &frame) != 0)
            goto done;
        shape->frames[image].bounds = (SDL_Rect){
            frame.x, frame.y, frame.width, frame.height
        };
        if (frame.width == 0 || frame.height == 0)
            continue;
        shape->frames[image].texture =
            SdlCreateSwcTexture(swcCockpitRenderer, &set, image, swcCockpitColors);
        if (shape->frames[image].texture == NULL)
            goto done;
    }
    result = 0;
done:
    SDL_free(set.data);
    if (result != 0)
        SwcFreeCockpitShape(shape);
    return result;
}

void SdlFreeSwcCockpit(void)
{
    size_t shape;

    for (shape = 0; shape < SDL_arraysize(swcCockpitShapes); shape++)
        SwcFreeCockpitShape(&swcCockpitShapes[shape]);
    for (shape = 0; shape < SDL_arraysize(swcTargetShapes); shape++)
        SwcFreeCockpitShape(&swcTargetShapes[shape]);
    for (shape = 0; shape < SDL_arraysize(swcDeadVduShapes); shape++)
        SwcFreeCockpitShape(&swcDeadVduShapes[shape]);
    SDL_DestroyTexture(swcCockpitFont);
    swcCockpitFont = NULL;
    swcCockpitRenderer = NULL;
    swcCockpitCmf = NULL;
    swcCockpitDrawing = 0;
    memset(swcMalfunctionTicks, 0, sizeof(swcMalfunctionTicks));
    memset(swcVduDead, 0, sizeof(swcVduDead));
    memset(swcVduStaticFrame, 0, sizeof(swcVduStaticFrame));
    memset(swcScannerFrames, 0xff, sizeof(swcScannerFrames));
    swcAutopilotLight = swcMissileLight = swcDamageLight = 0;
    swcScannerEnemy = swcHostileArrow = -1;
    swcDamageComponent = swcDamageDisplayTicks = 0;
    swcSparkFrame = -1;
    swcIttsActive = 0;
    swcIttsPosition.x = 0x7fff;
}

int SdlInitSwcCockpit(SDL_Renderer *renderer, const SwcCmf *cockpit,
                       const SwcCmf *space, const SDL_Color colors[256])
{
    /* DATA/0 +0x433e/+0x43e6 resource lists, and +0x420/+0x786/+0x78c
       frame maxima: energy 10, throttle 13, fuel 13 for the Hornet. */
    static const uint32_t chunks[] = {10, 15, 16, 17, 18, 31, 9, 1, 12, 13, 14, 7, 6};
    static const uint32_t counts[] = {7, 11, 14, 14, 1, 13, 2, 14, 1, 1, 2, 3, 8};
    size_t index;
    int shared;
    int result = -1;

    SdlFreeSwcCockpit();
    swcCockpitRenderer = renderer;
    swcCockpitCmf = cockpit;
    memcpy(swcCockpitColors, colors, sizeof(swcCockpitColors));
    for (index = 0; index < SDL_arraysize(chunks); index++) {
        shared = index == SWC_SCANNER_CONTACTS || index == SWC_RETICLE ||
            index == SWC_COCKPIT_SPARKS;
        if (SwcLoadCockpitShape(shared ? space : cockpit, shared ? "CKPT" : "PC00",
                               chunks[index], counts[index], &swcCockpitShapes[index]) != 0)
            goto done;
    }
    /* CODE_03 init_vdus +0x0914: shared VDUS/1 and /2, four static frames. */
    for (index = 0; index < SDL_arraysize(swcDeadVduShapes); index++) {
        if (SwcLoadCockpitShape(space, "VDUS", (uint32_t)index + 1, 4,
                               &swcDeadVduShapes[index]) != 0)
            goto done;
    }
    swcCockpitFont = SdlCreateSwcFont(renderer);
    if (swcCockpitFont == NULL)
        goto done;
    result = 0;
done:
    return result;
}

/* CODE_01 MacDraw2 +0x0ef4 adds frame x/y to the draw origin. Unlike the
   centred ship images, these offsets place each gauge in the cockpit. */
static void SwcDrawCockpitShape(SwcCockpitShape *shape, int frame, int x, int y)
{
    SDL_Rect destination;

    if (swcCockpitDrawResult != 0)
        return;
    if (frame < 0 || (uint32_t)frame >= shape->count) {
        swcCockpitDrawResult = SDL_SetError("Invalid SWC instrument frame %d", frame);
        return;
    }
    destination = shape->frames[frame].bounds;
    if (shape->frames[frame].texture == NULL)
        return;
    destination.x += x;
    destination.y += y;
    swcCockpitDrawResult = SDL_RenderCopy(swcCockpitRenderer,
        shape->frames[frame].texture, NULL, &destination);
}

static void SwcDrawInstrument(enum SwcCockpitInstrument instrument,
                               int frame, int x, int y)
{
    SwcDrawCockpitShape(&swcCockpitShapes[instrument], frame, x, y);
}

static void SwcDrawCockpitText(int x, int baseline, unsigned char color,
                                const char *text)
{
    if (swcCockpitDrawResult != 0)
        return;
    swcCockpitDrawResult = SdlDrawSwcText(swcCockpitRenderer, swcCockpitFont,
        x, baseline, swcCockpitColors[color], text);
}

void SdlDrawSwcCockpitReadout(signed char slot, const char *text)
{
    char padded[16];

    /* Only the shared speed routine draws here. Other WC1 readout calls can
       occur while changing objectives, outside cockpit composition. */
    if (!swcCockpitDrawing || (slot != 2 && slot != 3))
        return;
    SDL_snprintf(padded, sizeof(padded), "%04d", SDL_atoi(text));
    /* CODE_13 update_digital_readouts +0x0660; DATA/0 +0x666/+0x696. */
    SwcDrawCockpitText(slot == 3 ? 96 : 205, 32, 0x84, padded);
}

static int SwcCockpitTarget(void)
{
    int target = acShipTarget[0];

    return target > 0 && target < 10 &&
        aeObjectClass[target] >= OBJECT_CLASS_SHIP &&
        aeSpecialManeuver[target] != SPECIAL_MANEUVER_UNKNOWN_9 ? target : -1;
}

/* CODE_13 itts_range_check +0x2bce. WC2's
   HasInRangeGunForTargetLead (cockpt.c, 0x43ce8f) has the same loop. */
static int SwcIttsRangeCheck(short targetRange)
{
    const ShipWeaponSlot *weapons = (ShipWeaponSlot *)&aShipWeapons[0][1];
    int remaining = (signed char)aShipWeapons[0][0];

    while (remaining-- > 0) {
        const ObjectTypeData *type;

        if (weapons[remaining].disabled)
            continue;
        type = &aObjectTypeData[weapons[remaining].type];
        if (type->objectClass == OBJECT_CLASS_PROJECTILE &&
            type->maximumVelocity * type->lifetime > targetRange)
            return 1;
    }
    return 0;
}

static void SwcUpdateItts(void)
{
    /* CODE_13 position_itts_crosshair +0x2c48, overlay +0x2f56.
       Reuse WC2 UpdateTargetLeadIndicator (0x43cf5a)'s velocity prediction,
       retaining SWC's forward cone, 160-pixel projection and sprite origin. */
    const ShipWeaponSlot *weapons = (ShipWeaponSlot *)&aShipWeapons[0][1];
    int target = SwcCockpitTarget();
    int weapon;
    int projectileSpeed = 100;
    int interceptFrames;
    int distance;
    FixedVector combinedVelocity;
    FixedVector projectileVelocity;
    FixedVector targetOffset;
    FixedVector interceptPoint;
    FixedVector relative;
    FixedVector eyeRelative;

    swcIttsPosition.x = 0x7fff;
    if (target == -1 || nTargetLockMode == 0 ||
        aeShipSide[target] == aeShipSide[0] ||
        aeObjectClass[target] >= OBJECT_CLASS_CAPITAL_SHIP ||
        acPlayerComponentDamage[5] >= 4) {
        swcIttsActive = 0;
        return;
    }
    if (!swcIttsActive) {
        if (!SwcIttsRangeCheck(distance_from_object(0, (short)target)))
            return;
        swcIttsActive = 1;
        set_global_message("ITTS engaged", 0xd4, 3);
    }
    weapon = (signed char)aShipWeapons[0][0];
    while (weapon-- > 0) {
        const ObjectTypeData *type;

        if (weapons[weapon].disabled)
            continue;
        type = &aObjectTypeData[weapons[weapon].type];
        if (type->objectClass == OBJECT_CLASS_PROJECTILE &&
            projectileSpeed < type->maximumVelocity)
            projectileSpeed = type->maximumVelocity;
    }
    vector_component_in_dir(&aShipVelocity[0], &aShipForwardVector[0],
                            &combinedVelocity);
    ScaleFixedVector(&aShipForwardVector[0], projectileSpeed << 8,
                     &projectileVelocity);
    AddFixedVectors(&projectileVelocity, &combinedVelocity, &combinedVelocity);
    projectileSpeed = (Vector_magnitude(&combinedVelocity) >> 8) + 1;
    interceptFrames = distance_from_object(0, (short)target) / projectileSpeed;
    zero_vector(&targetOffset);
    if (interceptFrames > 0)
        ScaleFixedVector(&aShipVelocity[target], interceptFrames << 8,
                         &targetOffset);
    AddFixedVectors(&aShipPosition[target], &targetOffset, &interceptPoint);
    if (!SwcIttsRangeCheck(distance_from_point(0, &interceptPoint))) {
        set_global_message("Target out of range", 0xd4, 3);
        swcIttsActive = 0;
        return;
    }
    ComputeVectorDelta(&aShipPosition[0], &interceptPoint, &relative);
    distance = Vector_magnitude(&relative);
    if (distance <= asObjectCollisionRadius[EYE_OBJECT] * 256)
        return;
    transform_to_objects_frame(&relative, &eyeRelative, EYE_OBJECT);
    if (eyeRelative.z < asObjectCollisionRadius[0] * 256 ||
        DivideFixed(eyeRelative.z, distance) < 0x94)
        return;
    swcIttsPosition.x = (int)(((int64_t)eyeRelative.x * 0xa000 /
                               eyeRelative.z) >> 8) + 0x91;
    swcIttsPosition.y = (int)(((int64_t)eyeRelative.y * 0xa000 /
                               eyeRelative.z) >> 8) + 0x55;
    if (swcIttsPosition.x < 10 || swcIttsPosition.x > 310 ||
        swcIttsPosition.y < 5 || swcIttsPosition.y > 230)
        swcIttsPosition.x = 0x7fff;
}

static void SwcUpdateCockpitScanner(void)
{
    FixedVector relative;
    FixedVector rotated;
    SphericalVector spherical;
    unsigned short color;
    short object;
    int frame;
    int target = SwcCockpitTarget();
    int enemyVisible = 0;
    int x;
    int y;

    memset(swcScannerFrames, 0xff, sizeof(swcScannerFrames));
    swcHostileArrow = -1;
    if (anCockpitDamageState[0])
        return;
    /* CODE_13 +0x285a retains the last hostile target, otherwise chooses
       the first enemy ship. Friendly targets never supply a threat arrow. */
    if (target != -1)
        swcScannerEnemy = target;
    if (swcScannerEnemy != -1 &&
        (aeObjectClass[swcScannerEnemy] < OBJECT_CLASS_SHIP ||
         aeSpecialManeuver[swcScannerEnemy] == SPECIAL_MANEUVER_UNKNOWN_9 ||
         aeShipSide[swcScannerEnemy] == aeShipSide[0]))
        swcScannerEnemy = -1;
    for (object = 1; object < 10; object++) {
        if (swcScannerEnemy == -1 && aeObjectClass[object] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[object] != SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[object] != aeShipSide[0])
            swcScannerEnemy = object;
        if (!get_color(object, &color))
            continue;
        /* WC1 get_color and SWC CODE_13 +0x27ac classify the same contacts.
           The Mac routine returns CKPT/31 frame indices instead of colors. */
        if (color == cRedColour)
            frame = 11;
        else if (color == cBlueColour)
            frame = 9;
        else if (color == cOrangeColour)
            frame = 5;
        else if (color == cDarkGreyColour)
            frame = 1;
        else if (color == cYellowColour)
            frame = 7;
        else
            frame = 3;
        /* CODE_13 +0x2940..+0x295c transforms each contact directly. The scene's
           projected-position cache can omit objects close to the player. */
        ComputeVectorDelta(&aShipPosition[0], &aShipPosition[object], &relative);
        transform_to_objects_frame(&relative, &rotated, EYE_OBJECT);
        rectangular_to_spherical(&rotated, &spherical);
        /* SWC's scanner cutoff is 30,000; WC1's draw_3d_scanner uses 60,000. */
        if (spherical.radius >= 30000 * 256)
            continue;
        rotational_pos_to_scanner_pos((signed char)object, &spherical);
        if (object == swcScannerEnemy)
            enemyVisible = 1;
        if (target == object) {
            if (!(nSpaceFrame & 1))
                continue;
            frame++;
        }
        swcScannerFrames[object] = frame;
    }
    if (enemyVisible) {
        x = asScannerObjectX[swcScannerEnemy];
        y = asScannerObjectY[swcScannerEnemy];
        if (y < 169)
            swcHostileArrow = 0;
        if (x > 163)
            swcHostileArrow = 2;
        if (y > 175)
            swcHostileArrow = 4;
        if (x < 157)
            swcHostileArrow = 6;
        if (x > 163 && y < 169)
            swcHostileArrow = 1;
        if (x > 163 && y > 175)
            swcHostileArrow = 3;
        if (x < 157 && y > 175)
            swcHostileArrow = 5;
        if (x < 157 && y < 169)
            swcHostileArrow = 7;
    }
}

static void SwcDrawCockpitScanner(void)
{
    int object;

    for (object = 1; object < 10; object++) {
        if (swcScannerFrames[object] != -1)
            SwcDrawInstrument(SWC_SCANNER_CONTACTS, swcScannerFrames[object],
                             asScannerObjectX[object], asScannerObjectY[object]);
    }
    if (nNavPointerObject != -1 && cCurrentObjective >= 0 &&
        cCurrentObjective < cMissionObjectiveCount) {
        SwcDrawInstrument(SWC_SCANNER_CONTACTS, 0,
                             asScannerObjectX[10] - 3, asScannerObjectY[10] - 3);
    }
    /* CODE_13 draw_3d_scanner +0x285a, DATA/0 +0x390: grid over contacts. */
    SwcDrawInstrument(SWC_SCANNER_GRID, 0, 139, 151);
}

short SdlSwcComponentMalfunction(char component)
{
    int index = (unsigned char)component;
    int damage;

    if (index >= (int)SDL_arraysize(swcMalfunctionTicks))
        return 0;
    /* CODE_13 malf +0x1a28: damage-squared test and a countdown.
       SWC Random3DO excludes the upper bound, unlike WC1 RandomInRange. */
    if (swcMalfunctionTicks[index] > 0) {
        swcMalfunctionTicks[index]--;
        return 1;
    }
    damage = acPlayerComponentDamage[index];
    if ((unsigned short)RandomInRange(0, 14) < damage * damage) {
        swcMalfunctionTicks[index] = RandomInRange(5, 9);
        return 1;
    }
    return 0;
}

void SdlSwcCockpitDamage(short region)
{
    /* CODE_13 cockpit_explosion +0x4222. A running spark sequence blocks
       another impact; an already damaged region can spark again later. */
    if (region < 0 || region >= (short)SDL_arraysize(swcDamagePositions) ||
        nCameraViewMode != 0 || nTrainSimActive != 0 || swcSparkFrame != -1)
        return;
    anCockpitDamageState[region] = 1;
    swcSparkRegion = region;
    swcSparkPosition = swcDamagePositions[region];
    swcSparkFrame = 0;
    swcSparkStartFrame = nSpaceFrame;
}

void SdlUpdateSwcCockpit(void)
{
    int vdu;
    int mode;
    int count;
    int offset;

    /* These are simulation updates, never render-frame random draws. */
    if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCount)
        set_objective_range(1);
    if (nSpaceFrame % 7 == 0) {
        swcAutopilotLight = cCurrentObjective >= 0 &&
            cCurrentObjective < cMissionObjectiveCount && auto_pilot_valid(0);
        swcMissileLight = missile_on_tail(0);
        swcDamageLight = calculate_damage_level() >= 3 &&
            aasShipShield[0][0] + aasShipShield[0][1] < 10;
    }
    for (vdu = 0; vdu < 2; vdu++) {
        swcVduDead[vdu] = 0;
        if (anCockpitDamageState[vdu + 1])
            continue;
        mode = get_mode((short)vdu);
        swcVduDead[vdu] = mode == 0 ||
            (vdu == 0 && mode == 1 && malf(3)) ||
            (vdu == 1 && mode == 3 && malf(5));
        if (swcVduDead[vdu])
            swcVduStaticFrame[vdu] = RandomInRange(0, 2);
    }
    /* CODE_13 show_weapon_disp +0x0fc8 cycles nine damaged components. */
    if (!anCockpitDamageState[1] && !swcVduDead[0] && get_mode(0) == 1 &&
        --swcDamageDisplayTicks <= 0) {
        swcDamageDisplayTicks = 30;
        for (count = 0; count < 9; count++) {
            swcDamageComponent = (swcDamageComponent + 1) % 9;
            if (acPlayerComponentDamage[swcDamageComponent] != 0)
                break;
        }
    }
    SwcUpdateCockpitScanner();
    SwcUpdateItts();
    /* CODE_13 explosion_draw +0x40a0: eight frames, occasional repeat
       around the damaged panel. Frame zero is retained for the impact tick. */
    if (swcSparkFrame >= 0 && swcSparkStartFrame != nSpaceFrame &&
        ++swcSparkFrame >= 8) {
        if ((unsigned short)RandomInRange(0, 9) < 3) {
            swcSparkFrame = 0;
            offset = RandomInRange(0, 9);
            swcSparkPosition.x = swcDamagePositions[swcSparkRegion].x +
                RandomInRange(0, 19) - offset;
            offset = RandomInRange(0, 6);
            swcSparkPosition.y = swcDamagePositions[swcSparkRegion].y +
                RandomInRange(0, 14) - offset;
        } else {
            swcSparkFrame = -1;
        }
    }
}

static void SwcDrawTargetStatus(void)
{
    int target = SwcCockpitTarget();
    enum ObjectType objectType;
    const ObjectTypeData *type;
    SwcCockpitShape *shape;
    char chunk[5];
    int capital;

    if (target == -1 || swcCockpitDrawResult != 0)
        return;
    objectType = aeObjectType[target];
    type = &aObjectTypeData[objectType];
    capital = type->objectClass == OBJECT_CLASS_CAPITAL_SHIP;
    SDL_snprintf(chunk, sizeof(chunk), "%s%02d", capital ? "SH" : "ST", objectType);
    shape = &swcTargetShapes[objectType];
    /* CODE_03 cockpit resource loading: fighter STnn/2, capital SHnn/38.
       CODE_13 show_target_disp +0x1270; DATA/0 +0x756 origin (230,136). */
    swcCockpitDrawResult = SwcLoadCockpitShape(swcCockpitCmf, chunk,
                                               capital ? 38 : 2, 5, shape);
    SwcDrawCockpitShape(shape, 4, 230, 136);
    if (nSpaceFrame % 10 < 5) {
        if (aasShipArmor[target][0] < type->armorFront / 2)
            SwcDrawCockpitShape(shape, 0, 230, 136);
        if (aasShipArmor[target][3] < type->armorRight / 2)
            SwcDrawCockpitShape(shape, 1, 230, 136);
        if (aasShipArmor[target][2] < type->armorLeft / 2)
            SwcDrawCockpitShape(shape, 2, 230, 136);
        if (aasShipArmor[target][1] < type->armorRear / 2)
            SwcDrawCockpitShape(shape, 3, 230, 136);
    }
    if (type->shieldFore > 0 && aasShipShield[target][0] * 11 / type->shieldFore > 0)
        SwcDrawInstrument(SWC_TARGET_SHIELDS, 0, 0, 0);
    if (type->shieldAft > 0 && aasShipShield[target][1] * 11 / type->shieldAft > 0)
        SwcDrawInstrument(SWC_TARGET_SHIELDS, 1, 0, 0);
}

static void SwcDrawDeadDisplay(int vdu)
{
    /* CODE_13 vdu_polygon +0x0b7a; DATA/0 +0x426/+0x5a6. MacDrawcel2
       maps the full image onto this quad, ignoring sprite x/y offsets. */
    static const SDL_FPoint polygons[2][4] = {
        {{33, 134}, {90, 132}, {90, 188}, {33, 189}},
        {{231, 132}, {288, 134}, {288, 189}, {231, 188}}
    };
    static const SDL_FPoint uv[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    static const int indices[6] = {0, 1, 2, 0, 2, 3};
    SDL_Vertex vertices[4];
    int corner;

    if (swcCockpitDrawResult != 0)
        return;
    for (corner = 0; corner < 4; corner++) {
        vertices[corner].position = polygons[vdu][corner];
        vertices[corner].color = (SDL_Color){255, 255, 255, 255};
        vertices[corner].tex_coord = uv[corner];
    }
    swcCockpitDrawResult = SDL_RenderGeometry(swcCockpitRenderer,
        swcDeadVduShapes[vdu].frames[swcVduStaticFrame[vdu]].texture,
        vertices, 4, indices, 6);
}

static void SwcDrawPlayerStatus(void)
{
    const ObjectTypeData *type = &aObjectTypeData[aeObjectType[0]];

    /* CODE_13 show_weapon_disp +0x0e54: hull silhouette, flashing armor
       below half strength, and fore/aft shield presence at 1/11 strength. */
    SwcDrawInstrument(SWC_PLAYER_STATUS, 2, 0, 0);
    if (nSpaceFrame % 10 < 5) {
        if (aasShipArmor[0][0] < type->armorFront / 2)
            SwcDrawInstrument(SWC_PLAYER_STATUS, 3, 0, 0);
        if (aasShipArmor[0][3] < type->armorRight / 2)
            SwcDrawInstrument(SWC_PLAYER_STATUS, 4, 0, 0);
        if (aasShipArmor[0][2] < type->armorLeft / 2)
            SwcDrawInstrument(SWC_PLAYER_STATUS, 5, 0, 0);
        if (aasShipArmor[0][1] < type->armorRear / 2)
            SwcDrawInstrument(SWC_PLAYER_STATUS, 6, 0, 0);
    }
    if (type->shieldFore > 0 && aasShipShield[0][0] * 11 / type->shieldFore > 0)
        SwcDrawInstrument(SWC_PLAYER_STATUS, 0, 0, 0);
    if (type->shieldAft > 0 && aasShipShield[0][1] * 11 / type->shieldAft > 0)
        SwcDrawInstrument(SWC_PLAYER_STATUS, 1, 0, 0);
}

int SdlDrawSwcCockpit(void)
{
    const ObjectTypeData *type = &aObjectTypeData[aeObjectType[0]];
    TextContext *savedTextContext = pCurrentTextContext;
    int fuelCapacity;
    int frame;

    swcCockpitDrawResult = 0;
    /* CODE_13 update_lights +0x0408, DATA/0 +0x3d8. */
    if (swcAutopilotLight)
        SwcDrawInstrument(SWC_AUTOPILOT_LIGHT, 0, 0, 0);
    if (swcMissileLight)
        SwcDrawInstrument(SWC_MISSILE_LIGHT, 0, 0, 0);
    if (swcDamageLight && nSpaceFrame % 30 != 0)
        SwcDrawInstrument(SWC_DAMAGE_LIGHT, 0, 156, 45);
    if (!anCockpitDamageState[0])
        SwcDrawCockpitScanner();
    /* CODE_13 update_VDUs +0x0aa8. Physical damage suppresses its VDU;
       mode zero or a component malfunction replaces the live instruments.
       Navigation, damage and info pages are empty in the original demo. */
    if (!anCockpitDamageState[1]) {
        if (get_mode(0) == 0 || swcVduDead[0])
            SwcDrawDeadDisplay(0);
        else if (get_mode(0) == 1)
            SwcDrawPlayerStatus();
    }
    if (!anCockpitDamageState[2]) {
        if (get_mode(1) == 0 || swcVduDead[1])
            SwcDrawDeadDisplay(1);
        else if (get_mode(1) == 3)
            SwcDrawTargetStatus();
    }
    /* CODE_13 update_bars +0x0520. The original uses separate animated
       shape sets for energy, commanded throttle and fuel, not WC1's bars. */
    frame = SDL_clamp(asShipWeaponEnergy[0] * 10 / 100, 0, 10);
    SwcDrawInstrument(SWC_WEAPON_ENERGY, frame, 0, 0);
    frame = asShipMaximumSpeed[0] > 0 ?
        (anShipSpeed[0] >> 8) * 13 / asShipMaximumSpeed[0] : 0;
    SwcDrawInstrument(SWC_THROTTLE, SDL_clamp(frame, 0, 13), 0, 0);
    /* WC1 stores the 32-bit ship fuel capacity across these two packed
       words. memcpy avoids its reference build's unaligned int load. */
    memcpy(&fuelCapacity, &type->lifetime, sizeof(fuelCapacity));
    frame = anShipFuel[0] < 0 || fuelCapacity <= 0 ? 0 :
        (int)((int64_t)anShipFuel[0] * 13 / fuelCapacity) + 1;
    SwcDrawInstrument(SWC_FUEL, SDL_clamp(frame, 0, 13), 0, 0);
    swcCockpitDrawing = 1;
    update_digital_readouts();
    swcCockpitDrawing = 0;
    SetTextContext(savedTextContext);
    return swcCockpitDrawResult;
}

int SdlDrawSwcCockpitDamage(void)
{
    int region;

    swcCockpitDrawResult = 0;
    /* CODE_13 place_damage_on_cockpit +0x41ae and explosion_draw +0x40a0
       run after instruments and text; damaged panels remain visible. */
    for (region = 0; region < 3; region++) {
        if (anCockpitDamageState[region])
            SwcDrawInstrument(SWC_COCKPIT_DAMAGE, region,
                swcDamagePositions[region].x, swcDamagePositions[region].y);
    }
    if (swcSparkFrame >= 0)
        SwcDrawInstrument(SWC_COCKPIT_SPARKS, swcSparkFrame,
            swcSparkPosition.x - 20, swcSparkPosition.y - 20);
    return swcCockpitDrawResult;
}

int SdlDrawSwcSpaceHud(const SDL_Rect *targetBounds)
{
    int target = SwcCockpitTarget();
    int object = nNavPointerObject;
    int index;
    int reticle = 0;

    swcCockpitDrawResult = 0;
    /* CODE_13 overlay_head_up_display +0x2f56: scene center (160,100),
       normal reticle at (center.x-16, center.y-4), including frame offsets.
       update_cockpit draws this layer before the opaque cockpit artwork. */
    if (swcIttsActive && target != -1 && nTargetLockMode != 0 &&
        swcIttsPosition.x != 0x7fff) {
        SwcDrawInstrument(SWC_RETICLE, 5, swcIttsPosition.x, swcIttsPosition.y);
        if (swcIttsPosition.x + 20 >= 155 && swcIttsPosition.x + 10 <= 163 &&
            swcIttsPosition.y + 20 >= 107 && swcIttsPosition.y + 10 <= 115)
            reticle = 4;
    }
    SwcDrawInstrument(SWC_RETICLE, reticle, 144, 96);
    if (!anCockpitDamageState[0] && swcHostileArrow != -1)
        SwcDrawInstrument(SWC_RETICLE, 6 + swcHostileArrow, 144, 96);
    if (object >= 0 && asObjectScreenX[object] != (short)0x8001) {
        const SDL_Rect *bounds = &swcCockpitShapes[SWC_RETICLE].frames[3].bounds;

        /* The shared nav pointer is a centered space object, not an
           instrument placed by MacDraw2's sprite-origin offsets. */
        SwcDrawInstrument(SWC_RETICLE, 3,
            160 + asObjectScreenX[object] - bounds->x - bounds->w / 2,
            100 + asObjectScreenY[object] - bounds->y - bounds->h / 2);
    }
    if (target != -1 && targetBounds != NULL && swcCockpitDrawResult == 0) {
        SDL_Color color = swcCockpitColors[nTargetLockCountdown == 0 ? 0x81 :
            aeShipSide[target] == aeShipSide[0] ? 0xef : 0xd4];
        int left = targetBounds->x - 3;
        int top = targetBounds->y - 3;
        int right = targetBounds->x + targetBounds->w + 3;
        int bottom = targetBounds->y + targetBounds->h + 3;
        SDL_Point corners[4][3] = {
            {{left, top + 5}, {left, top}, {left + 5, top}},
            {{right - 5, top}, {right, top}, {right, top + 5}},
            {{right, bottom - 5}, {right, bottom}, {right - 5, bottom}},
            {{left + 5, bottom}, {left, bottom}, {left, bottom - 5}}
        };

        /* SDL corners follow the rotated sprite bounds. Original CODE_13
           draw_target_box uses MacScale1 bounds and CKPT/36 artwork. */
        swcCockpitDrawResult = SDL_SetRenderDrawColor(swcCockpitRenderer,
            color.r, color.g, color.b, 255);
        for (index = 0; index < 4 && swcCockpitDrawResult == 0; index++)
            swcCockpitDrawResult = SDL_RenderDrawLines(swcCockpitRenderer,
                                                       corners[index], 3);
    }
    return swcCockpitDrawResult;
}

int SdlDrawSwcHud(void)
{
    /* Original short names and status origin: DATA/0 +0x8ee/+0x8d6/+0x726.
       Keep status visible in this host, with ammunition added to the name. */
    static const char *const guns[] = {"Lasers", "Neutron", "Mass Drv"};
    static const char *const weapons[] = {
        "DF Missile", "HS Missile", "FF Missile", "IR Missile", "Torpedo", "Mine"
    };
    const ShipWeaponSlot *loadout = (ShipWeaponSlot *)&aShipWeapons[0][1];
    enum ObjectType weapon = (enum ObjectType)-1;
    int target = SwcCockpitTarget();
    int gun = (int)eSelectedGunType;
    int index;
    int ammunition = 0;
    int range;
    int baseline = 84;
    int damage = SDL_clamp(acPlayerComponentDamage[swcDamageComponent], 0, 4);
    char text[64];

    swcCockpitDrawResult = 0;
    if (damage != 0) {
        /* DATA/0 +0x886/+0x89a/+0x8ae, original status names and colours. */
        static const unsigned char colors[5] = {0xc2, 0x84, 0x9f, 0xd4, 0xb0};
        static const char *const conditions[5] = {"Ok", "Light", "Moderate", "Heavy", "Destroyed"};
        static const char *const components[9] = {
            "Ion drv", "Pwr plant", "Shld gen'r", "Computer", "InterCom",
            "Targetting", "Accel", "Ejector", "Repair"
        };

        SDL_snprintf(text, sizeof(text), "%s %s", components[swcDamageComponent], conditions[damage]);
        SwcDrawCockpitText(51, baseline, colors[damage], text);
        baseline += 11;
    }
    SwcDrawCockpitText(51, baseline, 0x81, "GUNS: ");
    SwcDrawCockpitText(81, baseline, 0xb0,
        gun == -1 ? "NONE" : (gun & 0x80) ? "FULL" :
        gun >= OBJECT_TYPE_LASER_CANNON && gun <= OBJECT_TYPE_MASS_DRIVER_CANNON ?
            guns[gun - OBJECT_TYPE_LASER_CANNON] : "NONE");
    if (nSelectedReleaseWeaponIndex >= 0 &&
        nSelectedReleaseWeaponIndex < (signed char)aShipWeapons[0][0]) {
        weapon = loadout[nSelectedReleaseWeaponIndex].type;
        for (index = 0; index < (signed char)aShipWeapons[0][0]; index++) {
            if (loadout[index].type == weapon)
                ammunition++;
        }
    }
    baseline += 11;
    SwcDrawCockpitText(51, baseline, 0x81, "WEAP: ");
    if (weapon >= OBJECT_TYPE_DUMB_FIRE_MISSILE && weapon <= OBJECT_TYPE_SPACE_MINE)
        SDL_snprintf(text, sizeof(text), "%s x%d",
                     weapons[weapon - OBJECT_TYPE_DUMB_FIRE_MISSILE], ammunition);
    else
        SDL_strlcpy(text, "NONE", sizeof(text));
    SwcDrawCockpitText(81, baseline, 0xb0, text);
    if (weapon == OBJECT_TYPE_HEAT_SEEKING_MISSILE ||
        weapon == OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE)
        SwcDrawCockpitText(51, baseline + 11, 0x81, nTargetLockCountdown == 0 ? "LOCKED" :
            nTargetLockCountdown > 0 ? "LOCKING" : "NO LOCK");

    if (target != -1) {
        /* CODE_13 digital readouts use the target's surface range, as WC1 does. */
        SwcDrawCockpitText(200, 110, 0x81, "Target: ");
        SwcDrawCockpitText(240, 110, 0x84, aObjectTypeData[aeObjectType[target]].displayName);
        range = distance_from_object(0, (short)target);
        SwcDrawCockpitText(200, 121, 0x81, "Range: ");
        SDL_snprintf(text, sizeof(text), "%d", range);
        SwcDrawCockpitText(235, 121, 0x84, range < 32000 ? text : "FAR");
    } else if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCount) {
        set_objective_range(0);
        /* Original destination/range layer at DATA/0 +0x6f6, including FAR
           at 32,000. Name visibility and distance remain WC1 core logic. */
        SwcDrawCockpitText(200, 94, 0x81, "Dest: ");
        SwcDrawCockpitText(230, 94, 0x84, objective_name((short)cCurrentObjective));
        /* CODE_06 tprintf +0x2214 advances by font size + 2 (9 + 2). */
        SwcDrawCockpitText(200, 105, 0x81, "Range: ");
        if (nCurrentObjectiveRange < 32000)
            SDL_snprintf(text, sizeof(text), "%d", nCurrentObjectiveRange);
        else
            SDL_strlcpy(text, "FAR", sizeof(text));
        SwcDrawCockpitText(235, 105, 0x84, text);
    }
    /* WC1 owns lock countdowns and message lifetimes; only their drawing is
       replaced. These host message positions also work without the cockpit. */
    for (index = 0; index < 2; index++) {
        const HudMessageSlot *slot = &aHudMessageSlots[index];

        if (slot->text != NULL && slot->flashCount != 0 && slot->drawColour != cBlackColour)
            SwcDrawCockpitText(8, 214 + 11 * index, 0x81, slot->text);
    }
    if (bPlayerDestroyed || nArcadeState == 4)
        SwcDrawCockpitText(90, 65, 0xd4, "SHIP DESTROYED - ESC TO EXIT");
    return swcCockpitDrawResult;
}
