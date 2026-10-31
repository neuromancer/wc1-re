/* SDL presentation of CODE_14's five-page InflightComputer.
 * The Mac map uses 32-bit world units and its own layout, not WC1 SetScale. */
#include "wc1.h"
#include "swc.h"
#include "swc_map.h"

typedef struct SwcNavArt {
    SDL_Texture *texture;
    SDL_Rect bounds;
} SwcNavArt;

enum SwcComputerPage {
    SWC_COMPUTER_NAV,
    SWC_COMPUTER_COMM,
    SWC_COMPUTER_GUNS,
    SWC_COMPUTER_WEAPONS,
    SWC_COMPUTER_EJECT,
    SWC_COMPUTER_PAGE_COUNT
};

/* InflightComputer retains DATA+0x466e between visits. */
static int swcComputerPage;

/* CODE_14 CheckPoint +0x1c04, with Q8-to-world conversion at its callers. */
static void SwcNavCheckPoint(SwcNavMap *map, FixedVector point)
{
    int x = point.x >> 8;
    int z = point.z >> 8;

    map->minX = SDL_min(map->minX, x);
    map->maxX = SDL_max(map->maxX, x);
    map->minZ = SDL_min(map->minZ, z);
    map->maxZ = SDL_max(map->maxZ, z);
}

/* CODE_14 FindTextPos +0x142c and BuildMap +0x14f8. DATA+0x272/+0x28a
 * contain width, height, layer origin and the extra marker offset. */
void SdlBuildSwcMap(SwcNavMap *map, const SwcTextFont *font, int navigation)
{
    static const int layouts[2][6] = {
        {125, 124, 40, 20, 54, 45}, {137, 138, 41, 43, 0, 0}
    };
    const int *layout = layouts[navigation != 0];
    int rowHeight = layout[1] / 17 + 1;
    unsigned char rows[17] = {0};
    MissionObjective *objective;
    MissionNavPoint *nav;
    MissionShipRecord *ship;
    SwcNavEntry *entry;
    int index;
    int navIndex;
    int slot;
    int record;
    int object;
    int width;
    int height;
    int centreX;
    int centreZ;
    int scale;
    int row;
    int distance;

    SDL_zero(*map);
    map->minX = map->maxX = (cMissionObjectiveCountValue > 0 ?
        aMissionObjectivesArray[0].position.x : aShipPositionsArray[0].x) >> 8;
    map->minZ = map->maxZ = (cMissionObjectiveCountValue > 0 ?
        aMissionObjectivesArray[0].position.z : aShipPositionsArray[0].z) >> 8;
    /* SetScale +0x1c8a includes every objective, even hidden objectives,
     * and refreshes mobile positions before calculating the bounds. */
    SwcNavCheckPoint(map, aShipPositionsArray[0]);
    for (index = 0; index < cMissionObjectiveCountValue; index++) {
        objective = &aMissionObjectivesArray[index];
        if (mobile_objective((short)index)) {
            object = find_ship_index(objective->index);
            if (object >= 0 && object < 10)
                objective->position = aShipPositionsArray[object];
        }
        SwcNavCheckPoint(map, objective->position);
    }
    for (navIndex = 0; navIndex < ACTIVE_MISSION_NAV_POINT_COUNT; navIndex++) {
        nav = &aMissionNavPoints[navIndex];
        if (nav->type == 0)
            break;
        for (slot = 0; slot < 10; slot++) {
            record = nav->missionShips[slot];
            if (record < 0 || record >= ACTIVE_MISSION_SHIP_COUNT)
                continue;
            ship = &aMissionShips[record];
            if (ship->type != OBJECT_TYPE_ASTEROID_FIELD && ship->type != OBJECT_TYPE_MINE_FIELD)
                continue;
            entry = &map->entries[map->count++];
            AddFixedVectors(&nav->position, &ship->position, &entry->position);
            SwcNavCheckPoint(map, entry->position);
            entry->objective = -1;
            entry->frame = ship->type == OBJECT_TYPE_ASTEROID_FIELD ? 3 : 4;
            entry->color = ship->type == OBJECT_TYPE_ASTEROID_FIELD ? 0xf5 : 0xf6;
            SDL_strlcpy(entry->text, ship->type == OBJECT_TYPE_ASTEROID_FIELD ?
                "Asteroids" : "Mines", sizeof(entry->text));
        }
    }
    for (index = 0; index < cMissionObjectiveCountValue; index++) {
        objective = &aMissionObjectivesArray[index];
        if (objective->type < 0 || objective->type > 4 || hidden_objective((short)index))
            continue;
        if (mobile_objective((short)index)) {
            if (objective_lost((short)index))
                continue;
            /* BuildMap +0x18ec calls find_ships_sphere (CODE_05 +0x6498),
             * whose active-nav preference and wave fallback match WC1. */
            navIndex = find_ships_sphere(objective->index);
            if (navIndex >= 0 && navIndex < ACTIVE_MISSION_NAV_POINT_COUNT) {
                nav = &aMissionNavPoints[navIndex];
                AddFixedVectors(&nav->position,
                    &aMissionShips[objective->index].position, &objective->position);
            }
        }
        entry = &map->entries[map->count++];
        entry->position = objective->position;
        entry->objective = index;
        entry->frame = objective->type < 2 ? objective->type : 2;
        if (objective->type == 0)
            entry->color = visited((short)index) ? 0xf8 : 0xf9;
        else
            entry->color = objective->type == 1 ? 0xfa : objective->type == 4 ? 0xfb : 0xfc;
        SDL_strlcpy(entry->text, objective->displayName, sizeof(entry->text));
    }
    if (navigation) {
        entry = &map->entries[map->count++];
        entry->position = aShipPositionsArray[0];
        entry->frame = 2;
        entry->color = 0xf8;
        entry->objective = -1;
        SDL_strlcpy(entry->text, (const char *)abMissionAuxData, sizeof(entry->text));
    }

    /* SetScale, ScaleX +0x1eaa and ScaleY +0x1f06. Preserve the Mac's
     * asymmetric 10,000-unit margin and its integer division order. */
    width = 10000 + map->maxX - map->minX;
    height = 10000 + map->maxZ - map->minZ;
    centreX = map->minX + width / 2;
    centreZ = map->maxZ - height / 2;
    scale = SDL_max(1, SDL_max(width / layout[0], height / layout[1]));
    for (index = 0; index < map->count; index++) {
        entry = &map->entries[index];
        entry->marker.x = layout[2] + layout[4] + layout[0] / 2 +
            ((entry->position.x >> 8) - centreX) / scale;
        entry->marker.y = layout[3] + layout[5] + layout[1] / 2 -
            ((entry->position.z >> 8) - centreZ) / scale;
        row = SDL_clamp((entry->marker.y - layout[3]) / rowHeight, 0, 16);
        if (rows[row]) {
            for (distance = 1; distance < 3; distance++) {
                if (row + distance <= 16 && !rows[row + distance]) {
                    row += distance;
                    break;
                }
                if (row - distance >= 0 && !rows[row - distance]) {
                    row -= distance;
                    break;
                }
            }
        }
        rows[row] = 1;
        entry->label.x = SDL_min(entry->marker.x,
            layout[2] + layout[4] + layout[0] - SdlSwcTextWidth(font, entry->text)) - 1;
        entry->label.y = layout[3] + row * rowHeight + 8;
    }
}

/* InflightComputer uses set_new_objective's flight-path index, which is
 * distinct from a marker's objective index. Bound the search if all hide. */
static void SwcSelectNavObjective(int direction, int objective)
{
    int index = cCurrentNavPtIdx;
    int previousIndex = index;
    int count = cMissionObjectiveCountValue;
    int attempt;

    for (attempt = 0; attempt < count; attempt++) {
        if (objective >= 0)
            index = attempt;
        else
            index = (index + direction + count) % count;
        if (abFlightPathList[index] < 0 || abFlightPathList[index] >= count ||
            (objective >= 0 && abFlightPathList[index] != objective))
            continue;
        if (set_new_objective((short)index))
            return;
    }
    cCurrentNavPtIdx = (signed char)previousIndex;
}

static int SwcLoadNavArt(SDL_Renderer *renderer, const SwcCmf *cmf,
                          const SDL_Color colors[256], int id, int frame, SwcNavArt *art)
{
    SwcBuffer set = {0};
    SwcFrame shape;
    int result = -1;

    if (SwcCMGetChunk(cmf, "CKPT", (uint32_t)id, &set) == 0 &&
        SwcGetFramePtr(&set, (uint32_t)frame, &shape) == 0) {
        art->bounds = (SDL_Rect){shape.x, shape.y, shape.width, shape.height};
        /* CKPT/3's comm/gun/weapon backgrounds are deliberately empty. */
        if (shape.width == 0 && shape.height == 0) {
            result = 0;
        } else {
            art->texture = SdlCreateSwcTexture(renderer, &set, (uint32_t)frame, colors);
            if (art->texture != NULL)
                result = 0;
        }
    }
    SDL_free(set.data);
    return result;
}

static int SwcDrawNavArt(SDL_Renderer *renderer, const SwcNavArt *art, int x, int y)
{
    SDL_Rect destination = art->bounds;

    destination.x += x;
    destination.y += y;
    if (art->texture == NULL)
        return 0;
    return SDL_RenderCopy(renderer, art->texture, NULL, &destination);
}

/* UpdateInflightNavText +0x273a: size 9 Helvetica, 11-pixel newline,
 * white body text, yellow headings and the selected objective's note. */
static int SwcDrawNavText(SDL_Renderer *renderer, const SwcTextFont *font,
                          const SDL_Color colors[256])
{
    const char *text[] = {"ConFed Nav", "Scan", "", "SECTOR:", "XR-231.3",
        "SYSTEM:", (const char *)abSeriesAuxData, "", (const char *)abMissionAuxData, ""};
    SDL_Rect clip = {178, 43, 80, 138};
    int role = aMissionShips[stCurrentMissionHeader.playerMissionShip].missionType;
    int index;
    int result = -1;

    if (role >= 0 && role < 11)
        text[9] = apszShipMissionTypeNames[role];
    if (SDL_RenderSetClipRect(renderer, &clip) != 0)
        return -1;
    for (index = 0; index < (int)SDL_arraysize(text); index++) {
        if (SdlDrawSwcProportionalText(renderer, font, 178, 52 + index * 11,
                colors[index == 3 || index == 5 ? 0xf9 : 1], text[index]) != 0)
            goto done;
    }
    clip = (SDL_Rect){41, 43, 217, 145};
    if (SDL_RenderSetClipRect(renderer, &clip) != 0)
        goto done;
    if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCountValue &&
        SdlDrawSwcProportionalText(renderer, font, 45, 179, colors[1],
            nav_note(cCurrentObjective)) != 0)
        goto done;
    result = 0;
done:
    if (SDL_RenderSetClipRect(renderer, NULL) != 0)
        result = -1;
    return result;
}

/* CODE_14 UpdateCommStatus +0x290e. Its recipient list includes the
 * wingman, friendly carriers and the current hostile fighter. The command
 * order and conditions differ from WC1's cockpit communication menu. */
static int SwcBuildComputerComms(int recipient, int options[10], const char *names[10])
{
    /* DATA/0 +0x2c70: original computer menu strings. */
    static const char *const commands[] = {
        "Never mind", "Attack my target!", "Help me out here", "Return to base",
        "TAUNT ENEMY", "", "", "Break and attack", "Keep formation",
        "Form on my wing", "Keep radio silence", "Broadcast freely", "Request Landing"
    };
    int wingman = nYourWingman;
    int target = acShipTargetTable[0];
    int count = 0;
    int object;
    int rating;

    if (recipient < 0) {
        if (wingman > 0 && wingman < 10 && aeObjectClass[wingman] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[wingman] != SPECIAL_MANEUVER_UNKNOWN_9) {
            rating = acShipRating[wingman];
            if (rating >= 0 && rating < 8 && apWingmanPilotsList[rating] != NULL) {
                options[count] = wingman;
                names[count++] = apWingmanPilotsList[rating]->callsign;
            }
        }
        for (object = 1; object < 10; object++) {
            if (aeShipSide[object] != SIDE_IMPERIAL ||
                aeObjectClass[object] < OBJECT_CLASS_SHIP ||
                aeSpecialManeuvers[object] == SPECIAL_MANEUVER_UNKNOWN_9 ||
                (count > 0 && options[0] == object))
                continue;
            if ((object == wingman && aeObjectClass[object] == OBJECT_CLASS_SHIP) ||
                aeObjectTypeList[object] == OBJECT_TYPE_TIGERS_CLAW) {
                options[count] = object;
                names[count++] = aObjectTypeData[aeObjectTypeList[object]].displayName;
            }
        }
        if (target > 0 && target < 10 && aeShipSide[target] == SIDE_KILRATHI &&
            aeObjectClass[target] == OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[target] != SPECIAL_MANEUVER_UNKNOWN_9) {
            options[count] = target;
            names[count++] = "ENEMY TARGET";
        }
        return count;
    }
    if (recipient == wingman) {
        if (aeShipObjectiveList[recipient] == OBJECTIVE_HOLD_FORMATION && any_enemy(0, 14000))
            options[count++] = 7;
        options[count++] = nAutoEngageTimer == -1 ? 9 : 8;
        options[count++] = bRadioSilence ? 11 : 10;
        options[count++] = 3;
    }
    if (aeShipSide[recipient] == aeShipSide[0]) {
        if (aeObjectTypeList[recipient] == OBJECT_TYPE_TIGERS_CLAW && !bLandingAuthorized)
            options[count++] = 12;
        if (target > 0 && target < 10 && !unactive((short)target) &&
            aeShipSide[target] == SIDE_KILRATHI)
            options[count++] = 1;
        if (evaluate_damage(0) < 50 && any_enemy(0, 14000))
            options[count++] = 2;
    }
    if (aeShipSide[recipient] == SIDE_KILRATHI)
        options[count++] = 4;
    options[count++] = 0;
    for (object = 0; object < count; object++)
        names[object] = commands[options[object]];
    return count;
}

/* UpdateCommStatus and print_message_text +0x289a: font 10 with twelve
 * pixel rows at text-layer origin (41,43); font 9 for the bottom prompt. */
static int SwcDrawComputerComms(SDL_Renderer *renderer, const SwcTextFont *font,
                                  const SwcTextFont *smallFont, const SDL_Color colors[256],
                                  int recipient, const char *const names[10], int count, int selected)
{
    char text[96];
    int index;

    if (SdlDrawSwcProportionalText(renderer, font, 41, 52, colors[0xf9],
            " VID-COM SYSTEM") != 0 ||
        SdlDrawSwcProportionalText(renderer, font, 41, 64, colors[1],
            recipient < 0 ? " Send message to?" : " Send message?") != 0 ||
        SdlDrawSwcProportionalText(renderer, font, 41, 88, colors[0xf9], " SELECT") != 0 ||
        SdlDrawSwcProportionalText(renderer, smallFont, 42, 179, colors[1],
            recipient < 0 ? "Press <Enter> to select." : "Press <Enter> to send message.") != 0)
        return -1;
    if (count == 0)
        return SdlDrawSwcProportionalText(renderer, font, 41, 100, colors[0xb0], " NONE");
    for (index = 0; index < count; index++) {
        SDL_snprintf(text, sizeof(text), " %d. %s", index + 1, names[index]);
        if (SdlDrawSwcProportionalText(renderer, font, 41, 100 + index * 12,
                colors[index == selected ? 1 : 0xb0], text) != 0)
            return -1;
    }
    return 0;
}

/* InflightComputer +0x3136..+0x3498 draws CKPT/12 and the installed
 * CKPT/9 weapons at DATA+0x30's centers/scales. Selected mounts blink. */
static int SwcDrawComputerLoadout(SDL_Renderer *renderer, SDL_Texture *canvas,
                                    SwcRasterImage weapons[10], const SwcTextFont *font,
                                    const SwcTextFont *smallFont, const SDL_Color colors[256],
                                    int page, Uint32 ticks)
{
    const ShipWeaponSlot *slots = (const ShipWeaponSlot *)&aShipWeaponsTable[0][1];
    const short *placement;
    char text[96];
    int count = aShipWeaponsTable[0][0];
    int index;
    int type;
    int row = 0;

    if (count < 0 || count > 10)
        return SDL_SetError("Invalid SWC computer loadout count");
    if (SdlDrawSwcProportionalText(renderer, smallFont, 46, 172, colors[0x81],
            page == SWC_COMPUTER_GUNS ? "Press Tab or -> to select new GUN." :
                                       "Press Tab or -> to select new WEAPON.") != 0)
        return -1;
    for (index = 0; index < count; index++) {
        type = slots[index].type;
        if (type < OBJECT_TYPE_LASER_CANNON || type > OBJECT_TYPE_SPACE_MINE ||
            slots[index].hardpoint < 0 || slots[index].hardpoint >= 40)
            return SDL_SetError("Unsupported SWC computer weapon mount");
        if ((type < OBJECT_TYPE_DUMB_FIRE_MISSILE) != (page == SWC_COMPUTER_GUNS))
            continue;
        if (page == SWC_COMPUTER_GUNS)
            SDL_snprintf(text, sizeof(text), "%s", aObjectTypeData[type].displayName);
        else
            SDL_snprintf(text, sizeof(text), "%s (%d)", aObjectTypeData[type].displayName,
                         aSwcWeaponAmmo[0][index]);
        if (SdlDrawSwcProportionalText(renderer, font, 141, 63 + row * 12,
                colors[slots[index].disabled ? 0xb0 : 0x81], text) != 0)
            return -1;
        row++;
        if ((!slots[index].disabled && ticks % 20 < 10) ||
            weapons[type - OBJECT_TYPE_LASER_CANNON].pixels == NULL)
            continue;
        placement = aSwcComputerWeaponLayout[slots[index].hardpoint];
        if (SdlDrawSwcRotatedImage(renderer, canvas, &weapons[type - OBJECT_TYPE_LASER_CANNON],
                colors, placement[0], placement[1], 0, placement[2], 0, NULL) != 0)
            return -1;
    }
    return 0;
}

/* CODE_14 InflightComputer +0x2dd2. Simulation stays suspended; SDL owns
 * input, cursor lifetime and redraw pacing. Enter/click actions complete on
 * release so a send/eject/close gesture cannot fire a weapon on return. */
int SdlShowSwcComputer(SDL_Window *window, SDL_Renderer *renderer, const char *resourceFork)
{
    /* DATA/0 +0x0210; the surrounding button art is part of CKPT/2. */
    static const int tabY[SWC_COMPUTER_PAGE_COUNT] = {65, 88, 106, 125, 155};
    SwcCmf cmf = {0};
    SwcBuffer weaponSet = {0};
    SwcNavArt frame = {0};
    SwcNavArt backgrounds[6] = {{0}};
    SwcNavArt tabs[SWC_COMPUTER_PAGE_COUNT] = {{0}};
    SwcNavArt guards[15] = {{0}};
    SwcNavArt markers[5] = {{0}};
    SwcNavArt ship = {0};
    SwcRasterImage weapons[10] = {{0}};
    SwcTextFont labelFont = {0};
    SwcTextFont textFont = {0};
    SwcFrame shape;
    SwcNavMap map;
    SwcNavEntry *entry;
    SDL_Color colors[256];
    SDL_Color markerColors[256];
    SDL_Texture *canvas = NULL;
    SDL_Event event;
    SDL_Rect savedClip;
    SDL_Rect clip = {41, 43, 217, 145};
    SDL_Point click;
    SDL_Point press = {0, 0};
    SDL_Rect hit;
    SDL_bool clipped = SDL_RenderIsClipEnabled(renderer);
    SDL_bool grabbed = SDL_GetWindowMouseGrab(window);
    SDL_Keycode pendingKey = SDLK_UNKNOWN;
    int savedCursor = SDL_ShowCursor(SDL_QUERY);
    int focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    int page = swcComputerPage;
    int previousPage;
    int mouseDown = 0;
    int mousePage = -1;
    int mouseRecipient = -1;
    int recipient = -1;
    int selected = 0;
    int options[10];
    const char *names[10];
    int count = 0;
    int activate;
    int close;
    int step;
    int guard = 0;
    int shipFrame = (int)aeObjectTypeList[0];
    int index;
    int command;
    int result = SWC_MOVIE_ERROR;
    Uint32 previous;
    Uint32 current;
    Uint32 elapsed = 0;
    Uint32 ticks = 0;
    char path[PATH_MAX];
    char failure[256];

    SDL_RenderGetClipRect(renderer, &savedClip);
    if (shipFrame == OBJECT_TYPE_SALTHI)
        shipFrame = 5;
    if (shipFrame < 0 || shipFrame > 5) {
        SDL_SetError("Unsupported SWC computer ship silhouette");
        goto done;
    }
    if (!SdlResolvePath("CMFs/ComputerInflight.CMF", path, sizeof(path)) ||
        SwcCMOpen(path, &cmf) != 0 || SwcReadPalette(resourceFork, 250, colors) != 0 ||
        SdlCreateSwcTextFont(renderer, &labelFont, 10) != 0 ||
        SdlCreateSwcTextFont(renderer, &textFont, 9) != 0 ||
        SwcLoadNavArt(renderer, &cmf, colors, 2, 0, &frame) != 0 ||
        SwcLoadNavArt(renderer, &cmf, colors, 12, shipFrame, &ship) != 0 ||
        SwcCMGetChunk(&cmf, "CKPT", 9, &weaponSet) != 0)
        goto done;
    for (index = 0; index < 256; index++)
        markerColors[index] = (SDL_Color){255, 255, 255, 255};
    for (index = 0; index < (int)SDL_arraysize(backgrounds); index++) {
        if (SwcLoadNavArt(renderer, &cmf, colors, 3, index, &backgrounds[index]) != 0)
            goto done;
    }
    for (index = 0; index < SWC_COMPUTER_PAGE_COUNT; index++) {
        if (SwcLoadNavArt(renderer, &cmf, colors, 4, index, &tabs[index]) != 0 ||
            SwcLoadNavArt(renderer, &cmf, markerColors, 10, index, &markers[index]) != 0)
            goto done;
    }
    for (index = 0; index < (int)SDL_arraysize(guards); index++) {
        if (SwcLoadNavArt(renderer, &cmf, colors, 13, index, &guards[index]) != 0)
            goto done;
    }
    for (index = 0; index < (int)SDL_arraysize(weapons); index++) {
        if (SwcGetFramePtr(&weaponSet, (uint32_t)index, &shape) != 0)
            goto done;
        if (shape.width == 0 && shape.height == 0)
            continue;
        if (SdlLoadSwcRasterImage(&weaponSet, (uint32_t)index, &weapons[index]) != 0)
            goto done;
    }
    canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, SWC_FRAME_WIDTH, SWC_FRAME_HEIGHT);
    if (canvas == NULL || SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND) != 0)
        goto done;
    SdlBuildSwcMap(&map, &labelFont, 1);
    if (SDL_RenderSetClipRect(renderer, NULL) != 0)
        goto done;
    SDL_SetWindowMouseGrab(window, SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);
    SdlPauseSwcSound(1);
    previous = SDL_GetTicks();
    for (;;) {
        current = SDL_GetTicks();
        if (focused)
            elapsed += SDL_min(current - previous, 250);
        previous = current;
        while (elapsed >= 50) {
            elapsed -= 50;
            ticks++;
            /* +0x2ef2..+0x2f52 opens the guard every five updates, then
             * alternates frames 13/14. Leaving Eject closes it again. */
            if (ticks % 5 == 0) {
                if (page == SWC_COMPUTER_EJECT)
                    guard += guard < 14 ? 1 : -1;
                else if (guard > 0)
                    guard--;
            }
        }
        while (SDL_PollEvent(&event)) {
            if (SdlHandleSwcEvent(window, &event, 0))
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
                    mouseDown = 0;
                    pendingKey = SDLK_UNKNOWN;
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                           event.window.event == SDL_WINDOWEVENT_RESTORED) {
                    focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
                    previous = SDL_GetTicks();
                }
            }
            if (!focused)
                continue;
            previousPage = page;
            activate = close = step = 0;
            if (page == SWC_COMPUTER_COMM) {
                count = SwcBuildComputerComms(recipient, options, names);
                selected = count > 0 ? SDL_clamp(selected, 0, count - 1) : -1;
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                switch (event.key.keysym.sym) {
                case SDLK_m:
                case SDLK_ESCAPE:
                case SDLK_BACKSPACE:
                case SDLK_SPACE:
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    pendingKey = event.key.keysym.sym;
                    break;
                case SDLK_UP:
                    page = SDL_max(0, page - 1);
                    break;
                case SDLK_DOWN:
                    page = SDL_min(SWC_COMPUTER_PAGE_COUNT - 1, page + 1);
                    break;
                case SDLK_LEFT:
                    if (page == SWC_COMPUTER_NAV || page == SWC_COMPUTER_COMM)
                        step = -1;
                    break;
                case SDLK_RIGHT:
                    step = 1;
                    break;
                case SDLK_TAB:
                    step = (event.key.keysym.mod & KMOD_SHIFT) &&
                        (page == SWC_COMPUTER_NAV || page == SWC_COMPUTER_COMM) ? -1 : 1;
                    break;
                case SDLK_n:
                    if (page == SWC_COMPUTER_NAV)
                        step = 1;
                    break;
                default:
                    if (page == SWC_COMPUTER_COMM && event.key.keysym.sym >= SDLK_1 &&
                        event.key.keysym.sym <= SDLK_9 &&
                        event.key.keysym.sym - SDLK_1 < count) {
                        selected = (int)(event.key.keysym.sym - SDLK_1);
                        pendingKey = event.key.keysym.sym;
                    }
                    break;
                }
            }
            if (event.type == SDL_KEYUP && pendingKey != SDLK_UNKNOWN &&
                event.key.keysym.sym == pendingKey) {
                if (pendingKey == SDLK_BACKSPACE && page == SWC_COMPUTER_COMM && recipient >= 0) {
                    recipient = -1;
                    selected = 0;
                } else if (pendingKey == SDLK_RETURN || pendingKey == SDLK_KP_ENTER ||
                           (pendingKey >= SDLK_1 && pendingKey <= SDLK_9)) {
                    activate = 1;
                } else {
                    close = 1;
                }
                pendingKey = SDLK_UNKNOWN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                mouseDown = 1;
                mousePage = page;
                mouseRecipient = recipient;
                press = (SDL_Point){event.button.x, event.button.y};
            }
            if (event.type == SDL_MOUSEMOTION && page == SWC_COMPUTER_COMM) {
                click = (SDL_Point){event.motion.x, event.motion.y};
                for (index = 0; index < count; index++) {
                    hit = (SDL_Rect){41, 90 + index * 12, 217, 12};
                    if (SDL_PointInRect(&click, &hit)) {
                        selected = index;
                        break;
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                click = (SDL_Point){event.button.x, event.button.y};
                if (mouseDown && mousePage == page && mouseRecipient == recipient) {
                    for (index = 0; index < SWC_COMPUTER_PAGE_COUNT; index++) {
                        hit = (SDL_Rect){262, tabY[index], 35, 18};
                        if (SDL_PointInRect(&click, &hit) && SDL_PointInRect(&press, &hit)) {
                            page = index;
                            break;
                        }
                    }
                    if (page == previousPage && index == SWC_COMPUTER_PAGE_COUNT) {
                        if (page == SWC_COMPUTER_NAV) {
                            for (index = 0; index < map.count; index++) {
                                entry = &map.entries[index];
                                if (entry->objective < 0)
                                    continue;
                                hit = (SDL_Rect){entry->marker.x - 3, entry->marker.y - 3, 15, 15};
                                if (!SDL_PointInRect(&click, &hit)) {
                                    hit = (SDL_Rect){entry->label.x, entry->label.y - 9,
                                        SdlSwcTextWidth(&labelFont, entry->text), 11};
                                }
                                if (SDL_PointInRect(&click, &hit) && SDL_PointInRect(&press, &hit)) {
                                    SwcSelectNavObjective(0, entry->objective);
                                    break;
                                }
                            }
                        } else if (page == SWC_COMPUTER_COMM) {
                            for (index = 0; index < count; index++) {
                                hit = (SDL_Rect){41, 90 + index * 12, 217, 12};
                                if (SDL_PointInRect(&click, &hit) && SDL_PointInRect(&press, &hit)) {
                                    selected = index;
                                    activate = 1;
                                    break;
                                }
                            }
                        } else if (page == SWC_COMPUTER_GUNS || page == SWC_COMPUTER_WEAPONS) {
                            if (SDL_PointInRect(&click, &clip) && SDL_PointInRect(&press, &clip))
                                step = 1;
                        } else if (page == SWC_COMPUTER_EJECT) {
                            hit = (SDL_Rect){263, 179, 33, 13};
                            if (SDL_PointInRect(&click, &hit) && SDL_PointInRect(&press, &hit))
                                activate = 1;
                        }
                    }
                }
                mouseDown = 0;
            }
            if (page != previousPage) {
                mouseDown = 0;
                pendingKey = SDLK_UNKNOWN;
            }
            if (close || (activate && page == SWC_COMPUTER_NAV)) {
                result = SWC_MOVIE_FINISHED;
                goto done;
            }
            if (step != 0) {
                if (page == SWC_COMPUTER_NAV)
                    SwcSelectNavObjective(step, -1);
                else if (page == SWC_COMPUTER_COMM && count > 0)
                    selected = SDL_clamp(selected + step, 0, count - 1);
                else if (page == SWC_COMPUTER_GUNS)
                    select_new_gun();
                else if (page == SWC_COMPUTER_WEAPONS) {
                    select_new_release_weapon((enum ObjectType)-1);
                    SdlSwcLockOff();
                }
            }
            if (activate && page == SWC_COMPUTER_COMM && selected >= 0 && selected < count) {
                if (recipient < 0) {
                    recipient = options[selected];
                    selected = 0;
                    mouseDown = 0;
                    pendingKey = SDLK_UNKNOWN;
                } else {
                    command = options[selected];
                    SdlSwcVocalizeMessage(command);
                    request(0, (short)recipient, (short)command);
                    result = SWC_MOVIE_FINISHED;
                    goto done;
                }
            }
            if (activate && page == SWC_COMPUTER_EJECT && acPlayerComponentDamageList[7] < 4) {
                /* +0x34e6..+0x351c: selection alone never ejects. Activation
                 * uses the same exclusive Random3DO bound as Ctrl+E. */
                if (acPlayerComponentDamageList[7] <= 1 ||
                    RandomBelowOrEqual((short)(acPlayerComponentDamageList[7] - 1)) == 0)
                    nArcadeState = 2;
                else
                    SdlPlaySwcSound(25, 127);
                result = SWC_MOVIE_FINISHED;
                goto done;
            }
        }
        if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0 ||
            SDL_RenderClear(renderer) != 0 ||
            SwcDrawNavArt(renderer, &frame, 0, 0) != 0 ||
            SwcDrawNavArt(renderer, &backgrounds[page == SWC_COMPUTER_EJECT ?
                4 + (ticks % 20 >= 10) : page], 41, 43) != 0 ||
            SwcDrawNavArt(renderer, &tabs[page], 262, tabY[page]) != 0 ||
            SwcDrawNavArt(renderer, &guards[guard], 263, 179) != 0 ||
            SDL_RenderSetClipRect(renderer, &clip) != 0)
            goto done;
        if (page == SWC_COMPUTER_NAV) {
            for (index = 0; index < map.count; index++) {
                entry = &map.entries[index];
                if (entry->objective < 0 || entry->objective != cCurrentObjective || ticks % 6 < 3) {
                    if (SDL_SetTextureColorMod(markers[entry->frame].texture,
                            colors[entry->color].r, colors[entry->color].g, colors[entry->color].b) != 0 ||
                        SwcDrawNavArt(renderer, &markers[entry->frame],
                            entry->marker.x, entry->marker.y) != 0)
                        goto done;
                }
            }
            for (index = 0; index < map.count; index++) {
                entry = &map.entries[index];
                if (SdlDrawSwcProportionalText(renderer, &labelFont,
                        entry->label.x, entry->label.y, colors[entry->color], entry->text) != 0)
                    goto done;
            }
            if (SwcDrawNavText(renderer, &textFont, colors) != 0)
                goto done;
        } else if (page == SWC_COMPUTER_COMM) {
            count = SwcBuildComputerComms(recipient, options, names);
            selected = count > 0 ? SDL_clamp(selected, 0, count - 1) : -1;
            if (SwcDrawComputerComms(renderer, &labelFont, &textFont, colors,
                    recipient, names, count, selected) != 0)
                goto done;
        } else if (page == SWC_COMPUTER_GUNS || page == SWC_COMPUTER_WEAPONS) {
            if (SwcDrawNavArt(renderer, &ship, 41, 43) != 0 ||
                SwcDrawComputerLoadout(renderer, canvas, weapons, &labelFont, &textFont,
                    colors, page, ticks) != 0)
                goto done;
        }
        if (SDL_RenderSetClipRect(renderer, NULL) != 0)
            goto done;
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
done:
    if (result == SWC_MOVIE_ERROR)
        SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
    else
        swcComputerPage = page;
    SDL_RenderSetClipRect(renderer, clipped ? &savedClip : NULL);
    focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    SDL_SetWindowMouseGrab(window, focused ? grabbed : SDL_FALSE);
    SDL_ShowCursor(focused && savedCursor == SDL_DISABLE ? SDL_DISABLE : SDL_ENABLE);
    SdlPauseSwcSound(!focused);
    SDL_DestroyTexture(labelFont.texture);
    SDL_DestroyTexture(textFont.texture);
    SDL_DestroyTexture(canvas);
    SDL_DestroyTexture(frame.texture);
    SDL_DestroyTexture(ship.texture);
    for (index = 0; index < (int)SDL_arraysize(backgrounds); index++)
        SDL_DestroyTexture(backgrounds[index].texture);
    for (index = 0; index < SWC_COMPUTER_PAGE_COUNT; index++)
        SDL_DestroyTexture(tabs[index].texture);
    for (index = 0; index < (int)SDL_arraysize(guards); index++)
        SDL_DestroyTexture(guards[index].texture);
    for (index = 0; index < (int)SDL_arraysize(markers); index++)
        SDL_DestroyTexture(markers[index].texture);
    for (index = 0; index < (int)SDL_arraysize(weapons); index++)
        SdlFreeSwcRasterImage(&weapons[index]);
    SDL_free(weaponSet.data);
    SwcCMClose(&cmf);
    if (result == SWC_MOVIE_ERROR)
        SDL_SetError("%s", failure);
    return result;
}
