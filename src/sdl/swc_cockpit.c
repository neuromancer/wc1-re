/* SDL presentation of the SWC Mac demo's Hornet instruments.
 * CODE_13 and the expanded DATA/0 tables provide the layout and sprite
 * selection. WC1 supplies flight state, speed readouts and scanner maths.
 * Mac QuickDraw text is replaced by a bundled, pre-rendered bitmap font. */
#include "wc1.h"
#include "swc.h"
#include "swc_hud_font.h"

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
    SWC_COCKPIT_INSTRUMENT_COUNT
};

/* Expanded DATA/0 +0x300, Hornet entry. Original words are BE32. */
const CockpitScannerGeometry stSwcCockpitScanner = {
    159, 171, 148, 160, 169, 181
};

static SwcCockpitShape swcCockpitShapes[SWC_COCKPIT_INSTRUMENT_COUNT];
static SDL_Renderer *swcCockpitRenderer;
static SDL_Texture *swcCockpitFont;
static SDL_Color swcCockpitColors[256];
static int swcCockpitDrawing;
static int swcCockpitDrawResult;

void SdlFreeSwcCockpit(void)
{
    size_t shape;
    uint32_t frame;

    for (shape = 0; shape < SDL_arraysize(swcCockpitShapes); shape++) {
        if (swcCockpitShapes[shape].frames != NULL) {
            for (frame = 0; frame < swcCockpitShapes[shape].count; frame++)
                SDL_DestroyTexture(swcCockpitShapes[shape].frames[frame].texture);
            SDL_free(swcCockpitShapes[shape].frames);
        }
    }
    memset(swcCockpitShapes, 0, sizeof(swcCockpitShapes));
    SDL_DestroyTexture(swcCockpitFont);
    swcCockpitFont = NULL;
    swcCockpitRenderer = NULL;
    swcCockpitDrawing = 0;
}

int SdlInitSwcCockpit(SDL_Renderer *renderer, const SwcCmf *cockpit,
                       const SwcCmf *space, const SDL_Color colors[256])
{
    /* DATA/0 +0x433e/+0x43e6 resource lists, and +0x420/+0x786/+0x78c
       frame maxima: energy 10, throttle 13, fuel 13 for the Hornet. */
    static const uint32_t chunks[] = {10, 15, 16, 17, 18, 31};
    static const uint32_t counts[] = {7, 11, 14, 14, 1, 13};
    SwcBuffer set = {0};
    SwcFrame frame;
    SwcCockpitShape *shape;
    SDL_Surface *fontSurface;
    uint32_t *row;
    size_t index;
    uint32_t image;
    int character;
    int x;
    int y;
    int result = -1;

    SdlFreeSwcCockpit();
    swcCockpitRenderer = renderer;
    memcpy(swcCockpitColors, colors, sizeof(swcCockpitColors));
    for (index = 0; index < SDL_arraysize(chunks); index++) {
        if (SwcCMGetChunk(index == SWC_SCANNER_CONTACTS ? space : cockpit,
                           index == SWC_SCANNER_CONTACTS ? "CKPT" : "PC00",
                           chunks[index], &set) != 0)
            goto done;
        shape = &swcCockpitShapes[index];
        if (SwcGetFrameCount(&set, &shape->count) != 0)
            goto done;
        if (shape->count != counts[index]) {
            SDL_SetError("Unexpected SWC Hornet instrument frame count in chunk %u",
                         chunks[index]);
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
            shape->frames[image].texture =
                SdlCreateSwcTexture(renderer, &set, image, colors);
            if (shape->frames[image].texture == NULL)
                goto done;
        }
        SDL_free(set.data);
        memset(&set, 0, sizeof(set));
    }

    fontSurface = SDL_CreateRGBSurfaceWithFormat(0, 16 * SWC_HUD_FONT_WIDTH,
        6 * SWC_HUD_FONT_HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);
    if (fontSurface == NULL)
        goto done;
    SDL_FillRect(fontSurface, NULL, 0);
    for (character = 0; character < 95; character++) {
        for (y = 0; y < SWC_HUD_FONT_HEIGHT; y++) {
            row = (uint32_t *)((uint8_t *)fontSurface->pixels +
                (character / 16 * SWC_HUD_FONT_HEIGHT + y) * fontSurface->pitch);
            for (x = 0; x < SWC_HUD_FONT_WIDTH; x++) {
                if (abSwcHudFont[character][y] & (0x80 >> x))
                    row[character % 16 * SWC_HUD_FONT_WIDTH + x] = UINT32_C(0xffffffff);
            }
        }
    }
    swcCockpitFont = SDL_CreateTextureFromSurface(renderer, fontSurface);
    SDL_FreeSurface(fontSurface);
    if (swcCockpitFont == NULL ||
        SDL_SetTextureBlendMode(swcCockpitFont, SDL_BLENDMODE_BLEND) != 0)
        goto done;
    result = 0;
done:
    SDL_free(set.data);
    return result;
}

/* CODE_01 MacDraw2 +0x0ef4 adds frame x/y to the draw origin. Unlike the
   centred ship images, these offsets place each gauge in the cockpit. */
static void SwcDrawInstrument(enum SwcCockpitInstrument instrument,
                               int frame, int x, int y)
{
    SwcCockpitShape *shape = &swcCockpitShapes[instrument];
    SDL_Rect destination;

    if (swcCockpitDrawResult != 0)
        return;
    if (frame < 0 || (uint32_t)frame >= shape->count) {
        swcCockpitDrawResult = SDL_SetError("Invalid SWC instrument frame %d", frame);
        return;
    }
    destination = shape->frames[frame].bounds;
    destination.x += x;
    destination.y += y;
    swcCockpitDrawResult = SDL_RenderCopy(swcCockpitRenderer,
        shape->frames[frame].texture, NULL, &destination);
}

static void SwcDrawCockpitText(int x, int baseline, unsigned char color,
                                const char *text)
{
    SDL_Rect source = {0, 0, SWC_HUD_FONT_WIDTH, SWC_HUD_FONT_HEIGHT};
    SDL_Rect destination = {x - SWC_HUD_FONT_BEARING,
        baseline - SWC_HUD_FONT_BASELINE, SWC_HUD_FONT_WIDTH, SWC_HUD_FONT_HEIGHT};
    SDL_Color ink = swcCockpitColors[color];
    unsigned char character;

    if (swcCockpitDrawResult != 0)
        return;
    swcCockpitDrawResult = SDL_SetTextureColorMod(swcCockpitFont, ink.r, ink.g, ink.b);
    if (swcCockpitDrawResult != 0)
        return;
    while (*text != 0 && destination.x < SWC_FRAME_WIDTH) {
        character = (unsigned char)*text++;
        if (character < 32 || character > 126)
            character = '?';
        character -= 32;
        source.x = character % 16 * SWC_HUD_FONT_WIDTH;
        source.y = character / 16 * SWC_HUD_FONT_HEIGHT;
        swcCockpitDrawResult = SDL_RenderCopy(swcCockpitRenderer,
            swcCockpitFont, &source, &destination);
        if (swcCockpitDrawResult != 0)
            return;
        destination.x += SWC_HUD_FONT_ADVANCE;
    }
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

static void SwcDrawCockpitScanner(void)
{
    FixedVector relative;
    FixedVector rotated;
    SphericalVector spherical;
    unsigned short color;
    short object;
    int frame;

    for (object = 1; object < 10; object++) {
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
        if (acShipTarget[0] == object) {
            if (!(nSpaceFrame & 1))
                continue;
            frame++;
        }
        SwcDrawInstrument(SWC_SCANNER_CONTACTS, frame,
                             asScannerObjectX[object], asScannerObjectY[object]);
    }
    if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCount) {
        set_objective_range(1);
        SwcDrawInstrument(SWC_SCANNER_CONTACTS, 0,
                             asScannerObjectX[10] - 3, asScannerObjectY[10] - 3);
    }
    /* CODE_13 draw_3d_scanner +0x285a, DATA/0 +0x390: grid over contacts. */
    SwcDrawInstrument(SWC_SCANNER_GRID, 0, 139, 151);
}

int SdlDrawSwcCockpit(void)
{
    const ObjectTypeData *type = &aObjectTypeData[aeObjectType[0]];
    TextContext *savedTextContext = pCurrentTextContext;
    int fuelCapacity;
    int frame;
    char text[64];

    swcCockpitDrawResult = 0;
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

    SwcDrawCockpitScanner();
    swcCockpitDrawing = 1;
    update_digital_readouts();
    swcCockpitDrawing = 0;
    SetTextContext(savedTextContext);
    if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCount) {
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
    return swcCockpitDrawResult;
}
