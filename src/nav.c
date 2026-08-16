/*
 *  Nav map, location readouts and the virtual screen.
 *
 *  Address range 0x40d000-0x40ffff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: DrawNav* family; string band 0x4687AC-0x4688F4.
 */
#include "wc1.h"

short g_nNavMapCoordinateScaling_00468660 = 0;
short g_nNavMapScale_00468664 = 1;
NavMapObjectiveStyle g_aNavMapObjectiveStyles_00468668[10] = {
    { 1, 2, &DAT_004699b4, &g_cDefaultTextColour_004699cc,
      &g_cDefaultTextColour_004699cc },
    { 3, 2, &DAT_0046999c, &g_cViewportClearColour_004699a0,
      &g_cDefaultTextColour_004699cc },
    { 4, 2, &DAT_004699c8, &DAT_004699c8,
      &g_cDefaultTextColour_004699cc },
    { 2, 3, &DAT_004699c8, &g_cDefaultTextColour_004699cc,
      &g_cDefaultTextColour_004699cc },
    { 2, 3, &DAT_004699ac, &DAT_004699ac,
      &g_cDefaultTextColour_004699cc },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 }
};
unsigned char *g_pNavMapShape_00468708 = 0;
short g_nNavMapLabelCount_0046870c = 0;
short g_nNavMapReservedAreaCount_00468710 = 0;
char g_szCampaignSector_00468718[16] = "Vega XR-231.3";
char *g_apszShipMissionTypeNames_00468728[11] = {
    g_szMissionPatrol_00468758,
    g_szMissionEscort_00468760,
    g_szMissionStrike_00468768,
    g_szMissionDefend_00468770,
    g_szMissionWingman_00468778,
    g_szMissionFlee_00468780,
    g_szMissionGotoWarp_00468788,
    g_szMissionWarpArriveError_00468794,
    g_szMissionCannedSequenceError_00468798,
    g_szMissionRendezvous_0046879c,
    g_szMissionComeHomeError_004687a8
};
int g_bInflightComputerActive_00468754 = 0;
char g_szMissionPatrol_00468758[8] = "Patrol";
char g_szMissionEscort_00468760[8] = "Escort";
char g_szMissionStrike_00468768[8] = "Strike";
char g_szMissionDefend_00468770[8] = "Defend";
char g_szMissionWingman_00468778[8] = "Wingman";
char g_szMissionFlee_00468780[8] = "Flee";
char g_szMissionGotoWarp_00468788[12] = "Goto Warp";
char g_szMissionWarpArriveError_00468794[4] = "err";
char g_szMissionCannedSequenceError_00468798[4] = "err";
char g_szMissionRendezvous_0046879c[12] = "Rendezvous";
char g_szMissionComeHomeError_004687a8[4] = "err";
char g_szNavLabelTextFormat_004687ac[12] = "%X%Y%F%s";
char g_szNavAsteroids_004687b8[12] = "Asteroids";
char g_szNavMines_004687c4[8] = "Mines";
char g_szInflightTimeFieldFormat_004687cc[8] = "%02d";
char g_szStandardTimeFormat_004687d4[24] =
    "%X%YStandard time %s";
char g_szStandardTimeBlank_004687ec[4] = " ";
char g_szInflightTimeSuffixFormat_004687f0[8] = "%02d  ";
char g_szStandardTimeColon_004687f8[4] = ":";
char g_szNavLegendNewline_004687fc[4] = "\n";
char g_szNavMissionFlightPath_00468800[20] = "MISSION FLIGHT PATH";
char g_szNavHomeBase_00468814[12] = "HOME BASE";
char g_szNavBlankLine_00468820[4] = "\n";
char g_szNavTitleFormat_00468824[8] = "%s\n\n";
char g_szNavSectorFormat_0046882c[12] = "Sector: %s\n";
char g_szNavSystemFormat_00468838[16] = "System: %s\n\n";
char g_szNavMissionFormat_00468848[8] = "* %s *\n";
char g_szNavShipFormat_00468850[8] = "* %s *\n";
char g_szNavNotesHeading_00468858[8] = "\nNotes\n";
char g_szNavNoteFormat_00468860[4] = "%s\n";
char g_szNavLocationFormat_00468864[48] =
    "%X%Y                         Location: %d.%d.%d";
char g_szNavViewportName_00468894[8] = "VSCREEN";
char g_szBriefingNavMapTitle_0046889c[20] = "Briefing Nav Map";
char g_szConfedNavScan_004688b0[16] = "ConFed Nav Scan";
volatile short g_nNearHeapActive_004688c0 = 0;
volatile short g_nNearHeapMaxDescriptors_004688c4 = 0x80;
volatile int g_nNearHeapRelocationBytes_004688c8 = 0;
int DAT_004688cc = 0;
int DAT_004688d0 = 0;
int DAT_004688d4 = 0;
int DAT_004688d8 = 0;
short DAT_004688dc = 0;
int DAT_004688e0 = 0;
short DAT_004688e4 = -1;
short DAT_004688e8 = -1;
unsigned short DAT_004688ec = 1;
int DAT_004688f0 = 0;
char *g_pszIntroOpeningText_00468910 =
    "In the distant future,\n"
    "mankind is locked in a deadly war...";
int g_nIntroCreditCount_00468a30 = 11;
char *g_apszIntroCredits_00468a38[20] = {
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

TitleMenuRegion g_aTitleMenuRegions_00468a88[5] = {
    { 1, 49, 48, 283, 99 },
    { 1, 49, 91, 283, 149 },
    { 1, 49, 134, 283, 149 },
    { 1, 49, 177, 283, 209 },
    { -1, 0, 0, 0, 0 }
};

PacketResourceDescriptor g_aIntroResourceDescriptors_00468ac0[3] = {
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_EXPLOSION1].shapeSet,
      3, 2 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet,
      3, 5 },
    { 0, 0, 0 }
};

int g_bTitleMenuSceneInitialized_00468ad8 = 0;

/* Function start: 0x40D090 */
short NavMapPointInsideReservedArea(short area, short x, short y)
{
    ShortRect *rectangle;

    rectangle = &g_aNavMapExclusionRects_00475f48[area];
    return rectangle->left <= x && x <= rectangle->right &&
           rectangle->top <= y && y <= rectangle->bottom;
}

/* Function start: 0x40D0E0 */
short NavMapLabelFits(short x, short y, short width, short height)
{
    return x > 0 && y > 0 && x + width < 150 && y + height < 135;
}

/* Function start: 0x40D120 */
short NavMapLabelPositionAvailable(short x, short y,
                                   short width, short height)
{
    short available;
    short area;
    short checkX;
    short checkY;

    available = NavMapLabelFits(x, y, width, height);
    if (available == 1) {
        area = 0;
        while (area < g_nNavMapReservedAreaCount_00468710 &&
               available != 0) {
            checkX = x;
            while (checkX < x + width && available != 0) {
                checkY = y;
                while (checkY < y + height && available != 0) {
                    available = (short)((unsigned short)
                        NavMapPointInsideReservedArea(area, checkX, checkY) == 0);
                    checkY++;
                }
                checkX++;
            }
            area++;
        }
    }
    return available;
}

/* Function start: 0x40D1D0 */
void ResetNavMapReservedAreas(void)
{
    g_nNavMapReservedAreaCount_00468710 = 0;
}

/* Function start: 0x40D1E0 */
void ReserveNavMapArea(short x, short y, short width, short height)
{
    ShortRect *rectangle;
    short area;

    area = g_nNavMapReservedAreaCount_00468710;
    rectangle = &g_aNavMapExclusionRects_00475f48[area];
    rectangle->left = x;
    rectangle->top = y;
    rectangle->right = (short)(x + width);
    rectangle->bottom = (short)(y + height);
    area++;
    g_nNavMapReservedAreaCount_00468710 = area;
}

/* Function start: 0x40D240 */
void ResetNavMapLabels(void)
{
    g_nNavMapLabelCount_0046870c = 0;
}

/* Function start: 0x40D250 */
short TryPlaceNavMapLabel(short x, short y, short width, short force)
{
    short placed;

    placed = 0;
    if (NavMapLabelPositionAvailable(x, y, width, 6) != 0 ||
        (force != 0 && NavMapLabelFits(x, y, width, 6) != 0)) {
        g_aNavMapLabels_00475e80[g_nNavMapLabelCount_0046870c].x = x;
        g_aNavMapLabels_00475e80[g_nNavMapLabelCount_0046870c].y = y;
        placed = 1;
    }
    return placed;
}

/* Function start: 0x40D2C0 */
void PlaceNavMapLabel(short x, short y, unsigned short colour,
                      const char *text)
{
    short width;
    short offset;
    short force;

    width = (short)(strlen(text) * 4 + 2);
    g_aNavMapLabels_00475e80[
        g_nNavMapLabelCount_0046870c].colour = colour;
    g_aNavMapLabels_00475e80[
        g_nNavMapLabelCount_0046870c].text = text;
    offset = -1;
    do {
        offset++;
        if (TryPlaceNavMapLabel((short)(x + offset + 4), y,
                                width, 0) != 0)
            break;
        force = offset == 12;
        if (TryPlaceNavMapLabel((short)(x - width / 2),
                                (short)(y + offset + 5),
                                width, force) != 0)
            break;
        if (TryPlaceNavMapLabel((short)(x - offset - width - 3), y,
                                width, force) != 0)
            break;
        if (TryPlaceNavMapLabel(x, (short)(y + offset + 5),
                                width, force) != 0)
            break;
        if (TryPlaceNavMapLabel((short)(x - width / 2),
                                (short)(y - offset - 9),
                                width, force) != 0)
            break;
    } while (offset != 12);
    ReserveNavMapArea(
        g_aNavMapLabels_00475e80[g_nNavMapLabelCount_0046870c].x,
        g_aNavMapLabels_00475e80[g_nNavMapLabelCount_0046870c].y,
        width, 6);
    g_nNavMapLabelCount_0046870c++;
}

/* Function start: 0x40D410 */
void AddUniqueObjectiveNavLabel(short x, short y,
                                unsigned short colour, const char *text,
                                short objective, short missionShip)
{
    short previous;

    if (missionShip == -1) {
        PlaceNavMapLabel(x, y, colour, text);
        return;
    }
    previous = 0;
    while (previous < objective) {
        if (g_aMissionObjectives_0059dac0[previous].index == missionShip)
            break;
        previous++;
    }
    if (previous < objective)
        return;
    PlaceNavMapLabel(x, y, colour, text);
}

/* Function start: 0x40D490 */
short IsPointInNavMapLabel(short labelIndex, short x, short y)
{
    if (g_aNavMapLabels_00475e80[labelIndex].x <= x &&
        (unsigned int)x <=
            (unsigned int)(g_aNavMapLabels_00475e80[labelIndex].x +
                           strlen(g_aNavMapLabels_00475e80[
                               labelIndex].text) * 4) &&
        g_aNavMapLabels_00475e80[labelIndex].y <= y &&
        y <= g_aNavMapLabels_00475e80[labelIndex].y + 6)
        return 1;
    return 0;
}

/* Function start: 0x40D540 */
void DrawNavMapLabels(void)
{
    short label;

    label = 0;
    while (label < (short)g_nNavMapLabelCount_0046870c) {
        DrawFormattedText(g_szNavLabelTextFormat_004687ac,
                          g_aNavMapLabels_00475e80[label].x,
                          g_aNavMapLabels_00475e80[label].y,
                          g_aNavMapLabels_00475e80[label].colour,
                          g_aNavMapLabels_00475e80[label].text);
        label++;
    }
}

/* Function start: 0x40D5A0 */
void DrawNavRectangleMarker(short x, short y, short size, short shadow,
                            unsigned short colour, short reserve)
{
    if (shadow == 0)
        DrawViewportEllipse(g_stNavLabelTextContext_005a8180.viewport,
                            x, y, size, (short)((size * 7) / 8),
                            (short)colour);
    else
        DrawViewportEllipseShadow(
            g_stNavLabelTextContext_005a8180.viewport,
            x, y, size, (short)((size * 7) / 8), (short)colour);
    if (reserve != 0 && size < 5)
        ReserveNavMapArea((short)(x - size), (short)(y - size),
                          (short)(size * 2 + 1),
                          (short)(size * 2 + 1));
}

/* Function start: 0x40D640 */
void DrawNavSquareOutline(Viewport *viewport, short x, short y,
                          short size, signed char colour)
{
    DrawViewportBorder(viewport, (short)(x - size), (short)(y - size),
                       (short)(x + size), (short)(y + size),
                       (short)colour);
}

/* Function start: 0x40D680 */
void DrawNavSquareMarker(short x, short y, short size, short shadow,
                         short colour, short reserve)
{
    if (size == 0) {
        DrawViewportPixel(g_stNavLabelTextContext_005a8180.viewport,
                          x, y, colour);
        DrawViewportPixel(g_stNavLabelTextContext_005a8180.viewport,
                          (short)(x + 1), y, colour);
        DrawViewportPixel(g_stNavLabelTextContext_005a8180.viewport,
                          x, (short)(y + 1), colour);
        DrawViewportPixel(g_stNavLabelTextContext_005a8180.viewport,
                          (short)(x + 1),
                          (short)(y + 1), colour);
    } else {
        DrawNavSquareOutline(g_stNavLabelTextContext_005a8180.viewport,
                             x, y, size,
                             (signed char)colour);
    }
    if (reserve != 0 && size < 5)
        ReserveNavMapArea((short)(x - size), (short)(y - size),
                          (short)(size * 2 + 1),
                          (short)(size * 2 + 1));
    (void)shadow;
}

/* Function start: 0x40D740 */
void DrawNavTriangleOutline(Viewport *viewport, short x, short y,
                            short size, signed char colour)
{
    DrawViewportLine(viewport, x, (short)(y - size),
                     (short)(x + size), (short)(y + size), colour);
    DrawViewportLine(viewport, (short)(x + size), (short)(y + size),
                     (short)(x - size), (short)(y + size), colour);
    DrawViewportLine(viewport, (short)(x - size), (short)(y + size),
                     x, (short)(y - size), colour);
}

/* Function start: 0x40D7D0 */
void DrawNavTriangleMarker(short x, short y, short size, short shadow,
                           unsigned short colour, short reserve)
{
    DrawNavTriangleOutline(g_stNavLabelTextContext_005a8180.viewport,
                           x, y, size,
                           (signed char)colour);
    if (reserve != 0 && size < 5)
        ReserveNavMapArea((short)(x - size), (short)(y - size),
                          (short)(size * 2 + 1),
                          (short)(size * 2 + 1));
    (void)shadow;
}

/* Function start: 0x40D830 */
void DrawNavCrossMarker(short x, short y, short size, short shadow,
                        unsigned short colour, short reserve)
{
    DrawViewportLine(g_stNavLabelTextContext_005a8180.viewport,
                     (short)(x - size), (short)(y - size),
                     (short)(x + size), (short)(y + size),
                     (short)colour);
    DrawViewportLine(g_stNavLabelTextContext_005a8180.viewport,
                     (short)(x - size), (short)(y + size),
                     (short)(x + size), (short)(y - size),
                     (short)colour);
    if (reserve != 0 && size < 5)
        ReserveNavMapArea((short)(x - size), (short)(y - size),
                          (short)(size * 2 + 1),
                          (short)(size * 2 + 1));
    (void)shadow;
}

/* Function start: 0x40D8C0 */
void SetScreenClipRect(unsigned short a, unsigned short b,
                       unsigned short c, unsigned short d)
{
    SetRectBounds(&DAT_005a76b0, a, b, c, d);
}

/* Function start: 0x40D8F0 */
void DrawNavHazardMarker(FixedVector navPosition, FixedVector offset,
                         short size, unsigned short markerColour,
                         unsigned short textColour, const char *text)
{
    short x;
    short y;

    AddFixedVectors(&navPosition, &offset, &navPosition);
    ScaleNavMapMarkerSize(&size);
    nav_getxy(&x, &y, navPosition.x, navPosition.z);
    DrawNavRectangleMarker(x, y, size, 0, markerColour, 1);
    PlaceNavMapLabel(x, y, textColour, text);
}

/* Function start: 0x40D980 */
void DrawNavPlayerMarker(short colour, short reserve)
{
    short x;
    short y;

    nav_getxy(&x, &y, g_aShipPosition_0059c490[0].x,
              g_aShipPosition_0059c490[0].z);
    x = (short)(x + g_stNavLabelTextContext_005a8180.viewport->left);
    y = (short)(y + g_stNavLabelTextContext_005a8180.viewport->top);
    DrawViewportPixel(g_stNavLabelTextContext_005a8180.viewport,
                      x, y, colour);
    DrawNavSquareMarker(x, y, 0, 0, colour, reserve);
}

/* Function start: 0x40DA00 */
void BuildMap(short showPlayer)
{
    MissionNavPoint *navPoint;
    MissionShipRecord *missionShip;
    MissionObjective *objective;
    const NavMapObjectiveStyle *style;
    unsigned short labelColour;
    short missionShipIndex;
    short objectiveIndex;
    short slot;
    short x;
    short y;

    SetScreenClipRect(1, 1, 153, 138);
    DrawSpriteDefault(&DAT_005a76b0, 1, 1, g_pNavMapShape_00468708, 0);
    SetScreenClipRect(2, 2, 152, 137);
    g_stNavLabelTextContext_005a8180.viewport = &DAT_005a76b0;
    g_stNavLabelTextContext_005a8180.text = g_szDefaultTextBuffer_005a7590;
    InitializeTextContextFromFont(&g_stNavLabelTextContext_005a8180,
                                  2, DAT_004699b4, -1);
    g_stNavLabelTextContext_005a8180.alignment = 0;
    SetTextContext(&g_stNavLabelTextContext_005a8180);
    ResetNavMapLabels();
    ResetNavMapReservedAreas();
    SetScale();

    for (navPoint = g_aMissionNavPoints_0046c2f0;
         navPoint->type != 0;
         navPoint++) {
        for (slot = 0; slot < 10; slot++) {
            missionShipIndex = navPoint->missionShips[slot];
            if (missionShipIndex != -1) {
                missionShip = &g_aMissionShips_0046c948[missionShipIndex];
                if (missionShip->type == OBJECT_TYPE_ASTEROID_FIELD) {
                    DrawNavHazardMarker(navPoint->position,
                                        missionShip->position,
                                        missionShip->speed,
                                        DAT_004699d4, DAT_004699d4,
                                        g_szNavAsteroids_004687b8);
                } else if (missionShip->type == OBJECT_TYPE_MINE_FIELD) {
                    DrawNavHazardMarker(navPoint->position,
                                        missionShip->position,
                                        missionShip->speed,
                                        DAT_004699ac, DAT_004699ac,
                                        g_szNavMines_004687c4);
                }
            }
        }
    }

    for (objectiveIndex = 0;
         objectiveIndex < (short)g_cMissionObjectiveCount_0059c46a;
         objectiveIndex++) {
        objective = &g_aMissionObjectives_0059dac0[objectiveIndex];
        if (mobile_objective(objectiveIndex) == 0 ||
            (g_aMissionShips_0046c948[
                 (signed char)objective->index].state == 0 &&
             achieved(objectiveIndex) == 0)) {
            ScaleNavMapCoordinates(&x, &y,
                                   objective->mapX, objective->mapY);
            if (hidden_objective(objectiveIndex) == 0) {
                style = &g_aNavMapObjectiveStyles_00468668[
                    objective->type];
                if (visited(objectiveIndex) == 0)
                    DrawViewportPixel(&DAT_005a76b0, x, y,
                                      *style->unvisitedColour);
                switch (style->markerType) {
                case 1:
                    DrawNavSquareMarker(x, y, style->markerSize, 0,
                                        *style->markerColour, 1);
                    break;
                case 2:
                    DrawNavRectangleMarker(x, y, style->markerSize, 0,
                                           *style->markerColour, 1);
                    break;
                case 3:
                    DrawNavTriangleMarker(x, y, style->markerSize, 0,
                                          *style->markerColour, 1);
                    break;
                case 4:
                    DrawNavCrossMarker(x, y, style->markerSize, 0,
                                       *style->markerColour, 1);
                    break;
                }
                if (g_cCurrentObjective_0046c020 == objectiveIndex)
                    labelColour = DAT_004699a8;
                else
                    labelColour = *style->labelColour;
                g_awNavObjectiveLabelIndex_005a8130[objectiveIndex] =
                    g_nNavMapLabelCount_0046870c;
                AddUniqueObjectiveNavLabel(
                    x, y, labelColour, objective_name(objectiveIndex),
                    objectiveIndex, (short)objective->index);
            }
        }
    }
    if (showPlayer != 0) {
        DrawNavPlayerMarker(g_cViewportClearColour_004699a0, 1);
        nav_getxy(&x, &y, g_aShipPosition_0059c490[0].x,
                  g_aShipPosition_0059c490[0].z);
        PlaceNavMapLabel(
            x, y, DAT_004699c4,
            g_stCampaignState_0059ca50.currentPilot->callsign);
    }
    DrawNavMapLabels();
    SetScreenClipRect(0, 0, 259, 155);
}

/* Function start: 0x40DDA0 */
void UpdateInflightNavText(short showColon)
{
    char time[12];
    short cursorX;

    SetTextContext(&g_stNavMapTextContext_005a8160);
    sprintf(time, g_szInflightTimeFieldFormat_004687cc,
            (int)((signed char *)g_pElapsedCampaignDate_005a86ac)[0]);
    DrawFormattedText(
        g_szStandardTimeFormat_004687d4,
        (int)(short)(g_stNavMapTextContext_005a8160.viewport->left + 150),
        (int)(short)(g_stNavMapTextContext_005a8160.viewport->top + 140),
        time);
    cursorX = g_stNavMapTextContext_005a8160.cursorX;
    if (showColon == 0) {
        DrawFormattedText(g_szStandardTimeBlank_004687ec);
    } else {
        g_stNavMapTextContext_005a8160.cursorX =
            (short)(g_stNavMapTextContext_005a8160.cursorX + 4);
    }
    sprintf(time, g_szInflightTimeSuffixFormat_004687f0,
            (int)((signed char *)g_pElapsedCampaignDate_005a86ac)[1]);
    DrawFormattedText(time);
    g_stNavMapTextContext_005a8160.cursorX = cursorX;
    if (showColon != 0)
        DrawFormattedText(g_szStandardTimeColon_004687f8);
}

/* Function start: 0x40DE70 */
void FormatNavCoordinates(unsigned char *out)
{
    unsigned char tmp[4];

    SplitGameClockTicks(tmp);
    out[1] = tmp[2];
    out[0] = tmp[3];
}

/* Function start: 0x40DEA0 */
void DrawSelectedNavLegendEntry(const char *text, short navPoint)
{
    if ((short)g_cCurrentNavPointIndex_0059c86c == navPoint) {
        DrawNavTextLine(0, (unsigned short)DAT_004699a8, text);
        DrawNavTextLine(0, (unsigned short)DAT_004699a8,
                        g_szNavLegendNewline_004687fc);
    }
}

/* Function start: 0x40DEE0 */
void DrawNavMapLegend(void)
{
    short objective;

    objective = 0;
    while (objective < (short)g_cMissionObjectiveCount_0059c46a) {
        if (visited(objective) == 0 && hidden_objective(objective) == 0)
            break;
        objective++;
    }
    SetTextCursor((unsigned short)DAT_005a76b0.left, 120);
    DrawSelectedNavLegendEntry(g_szNavMissionFlightPath_00468800,
                               objective);
    DrawSelectedNavLegendEntry(
        g_szNavHomeBase_00468814,
        (short)((short)g_cMissionObjectiveCount_0059c46a - 1));
}

/* Function start: 0x40DF50 */
char *nav_note(short objective)
{
    char *note;

    note = g_aMissionObjectives_0059dac0[objective].name;
    if (*note == '?')
        note++;
    return note;
}

/* Function start: 0x40DF70 */
void DrawNavLocationReadout(const char *title, short showFlightData)
{
    enum ShipMissionType playerMissionType;

    ClearViewport(&DAT_005a76b0, DAT_0046999c);
    SetScreenClipRect(155, 2, 259, 155);
    g_stNavMapTextContext_005a8160.viewport = &DAT_005a76b0;
    g_stNavMapTextContext_005a8160.text = g_szDefaultTextBuffer_005a7590;
    InitializeTextContextFromFont(&g_stNavMapTextContext_005a8160,
                                  1, DAT_004699b4, DAT_0046999c);
    g_stNavMapTextContext_005a8160.alignment = 0;
    g_stNavMapTextContext_005a8160.cursorX = 0;
    g_stNavMapTextContext_005a8160.cursorY = 0;
    SetTextContext(&g_stNavMapTextContext_005a8160);
    DrawNavTextLine(0, g_cDefaultTextColour_004699cc,
                    g_szNavBlankLine_00468820);
    DrawNavTextLine(2, g_cDefaultTextColour_004699cc,
                    g_szNavTitleFormat_00468824, title);
    DrawNavTextLine(0, g_cDefaultTextColour_004699cc,
                    g_szNavSectorFormat_0046882c,
                    g_szCampaignSector_00468718);
    DrawNavTextLine(0, g_cDefaultTextColour_004699cc,
                    g_szNavSystemFormat_00468838,
                    g_abSeriesAuxData_005a8240);
    DrawNavTextLine(2, g_cDefaultTextColour_004699cc,
                    g_szNavMissionFormat_00468848,
                    g_abMissionAuxData_005a8210);
    playerMissionType = g_aMissionShips_0046c948[
        g_nPlayerMissionShipIndex_005a8694].missionType;
    DrawNavTextLine(2, g_cDefaultTextColour_004699cc,
                    g_szNavShipFormat_00468850,
                    g_apszShipMissionTypeNames_00468728[
                        playerMissionType]);
    DrawNavTextLine(2, g_cDefaultTextColour_004699cc,
                    g_szNavNotesHeading_00468858);
    DrawNavTextLine(0, g_cDefaultTextColour_004699cc,
                    g_szNavNoteFormat_00468860,
                    nav_note((short)g_cCurrentObjective_0046c020));
    if (showFlightData != 0)
        DrawNavMapLegend();
    BuildMap(showFlightData);
    if (showFlightData != 0) {
        SetScreenClipRect(0, 0, 259, 155);
        SetTextContext(&g_stNavMapTextContext_005a8160);
        DrawNavTextLine(0, g_cDefaultTextColour_004699cc,
                        g_szNavLocationFormat_00468864,
                        8, 142,
                        g_aShipPosition_0059c490[0].x,
                        g_aShipPosition_0059c490[0].y,
                        g_aShipPosition_0059c490[0].z);
    }
    CopyViewportContents(&DAT_005a76b0, &DAT_005a6ba0);
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x40E190 */
void BriefingMap_LoadShapes(void)
{
    short objective;

    g_pNavMapShape_00468708 =
        FetchDiskPacketRetrying(8, 2, 0);
    SetScreenClipRect(0, 0, 259, 155);
    if (AllocateViewport(&DAT_005a76b0, (short)DAT_0046999c, 0) == 0)
        ReportOutOfMemoryAndExit(g_szNavViewportName_00468894);
    objective = 0;
    while (objective < (short)g_cMissionObjectiveCount_0059c46a) {
        LocateMobileObjective(objective);
        objective++;
    }
}

/* Function start: 0x40E210 */
void BriefingMap_DisplayMap(void)
{
    Viewport savedViewport;

    savedViewport = DAT_005a76b0;
    free_viewport(&DAT_005a76b0);
    BriefingMap_LoadShapes();
    DAT_005a6ba0.top = 4;
    DrawNavLocationReadout(g_szBriefingNavMapTitle_0046889c, 0);
    free_viewport(&DAT_005a76b0);
    ReleasePacketHandle(g_pNavMapShape_00468708);
    g_pNavMapShape_00468708 = 0;
    ReleaseTextFont(2);
    ReleaseTextFont(1);
    DAT_005a76b0 = savedViewport;
    AllocateViewport(&DAT_005a76b0, (short)DAT_0046999c, 0);
}

/* Function start: 0x40E2B0 */
short SelectNavObjectiveAtPoint(short mouseX, short mouseY)
{
    short mapX;
    short mapY;
    short oldNavPoint;
    short selected;
    short pathIndex;
    signed char objective;

    oldNavPoint = (short)g_cCurrentNavPointIndex_0059c86c;
    mouseX = (short)(mouseX - 30);
    mouseY = (short)(mouseY - 22);
    pathIndex = 0;
    selected = 0;
    objective = g_abFlightPath_0059c000[pathIndex];
    while (objective != -1) {
        if (hidden_objective((short)objective) == 0) {
            ScaleNavMapCoordinates(
                &mapX, &mapY,
                g_aMissionObjectives_0059dac0[objective].mapX,
                g_aMissionObjectives_0059dac0[objective].mapY);
            if ((short)(abs((int)mouseX - mapX) +
                        abs((int)mouseY - mapY)) < 6 ||
                IsPointInNavMapLabel(
                    (short)g_awNavObjectiveLabelIndex_005a8130[pathIndex],
                    mouseX, mouseY) != 0) {
                selected = 1;
                set_new_objective(pathIndex);
                if (pathIndex == oldNavPoint)
                    return selected;
            }
        }
        pathIndex++;
        objective = g_abFlightPath_0059c000[pathIndex];
    }
    return selected;
}

/* Function start: 0x40E3C0 */
void CentreMouseOnCurrentNavObjective(void)
{
    short x;
    short y;
    signed char objective;

    objective = g_abFlightPath_0059c000[
        g_cCurrentNavPointIndex_0059c86c];
    ScaleNavMapCoordinates(
        &x, &y,
        g_aMissionObjectives_0059dac0[objective].mapX,
        g_aMissionObjectives_0059dac0[objective].mapY);
    x = (short)(x + 30);
    y = (short)(y + 22);
    LeaveAllocationScope();
    WarpMouseTo(x, y);
    EnterAllocationScope();
}

/* Function start: 0x40E430 */
void ShowConfedNavScan(void)
{
    SetRectBounds(&DAT_005a6ba0, 30, 22, 289, 177);
    LeaveAllocationScope();
    DrawNavLocationReadout(g_szConfedNavScan_004688b0, 1);
    EnterAllocationScope();
    SetRectBounds(&DAT_005a6ba0, 0, 0, 319, 199);
}

/* Function start: 0x40E480 */
void InflightComputer(void)
{
    short savedNavPoint;
    short done;
    short hasObjectives;
    short savedInputMode;
    short objective;
    short displayedNavPoint;
    short eventType;
    int frame;
    unsigned char markerColour;
    unsigned char *background;
    InputEventState event;
    Viewport pointerViewport;
    unsigned int savedInputState[7];

    savedNavPoint = (short)g_cCurrentNavPointIndex_0059c86c;
    done = 0;
    hasObjectives = 0;
    displayedNavPoint = savedNavPoint;
    g_bInflightComputerActive_00468754 = 1;
    memcpy(savedInputState, (const void *)&g_stMouseCursorState_0059ab10,
           sizeof(savedInputState));

    if (message_showing() != 0)
        EndCommMenu();
    GetScreenUpdateFlag();
    g_cScreenViewportMode_0059a9f2 = -1;
    background = FetchDiskPacketRetrying(8, 1, 0);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    DrawSpriteDefault(&DAT_005a6ba0, 0, 0, background, 0);
    ReleasePacketHandle(background);

    objective = 0;
    BriefingMap_LoadShapes();
    ShowConfedNavScan();
    if (g_cMissionObjectiveCount_0059c46a > 0) {
        do {
            if (hidden_objective(objective) == 0)
                hasObjectives = 1;
            objective++;
        } while (objective <
                 (short)g_cMissionObjectiveCount_0059c46a);
    }

    if (hasObjectives == 0) {
        SetEventManagerPump(PollJoystickButtonEvents);
        WaitForInputKey();
        SetFrameTimerAndWait(20);
        SetEventManagerPump(get_player_input);
    } else {
        pointerViewport = DAT_005a6ba0;
        SetRectBounds(&pointerViewport, 32, 24, 182, 159);
        savedInputMode = (short)(signed char)g_bInputMode_0059a848;
        g_stMouseCursorState_0059ab10.viewport = &pointerViewport;
        g_bInputMode_0059a848 = 1;
        SetEventManagerPump(PollMenuInputDevices);
        EventManagerHook(ResetMouseCursorFrame);
        g_nMenuInputRepeatDelay_005a8208 = 6;
        EnterAllocationScope();
        CentreMouseOnCurrentNavObjective();

        do {
            if (displayedNavPoint !=
                (short)g_cCurrentNavPointIndex_0059c86c) {
                displayedNavPoint =
                    (short)g_cCurrentNavPointIndex_0059c86c;
                PlaySfxWaveFileByNumber(0x19, -1, 0);
                ShowConfedNavScan();
            }
            SetRectBounds(&DAT_005a6ba0, 32, 24, 289, 177);
            SetMouseCursorShape(g_stMouseCursorState_0059ab10.shape, 0);
            FormatNavCoordinates(
                (unsigned char *)g_pElapsedCampaignDate_005a86ac);
            g_stNavLabelTextContext_005a8180.viewport = &DAT_005a6ba0;
            frame = DAT_0059ab54 / 15;
            markerColour = DAT_004699c0;
            if (frame % 2 != 0)
                markerColour = g_cViewportClearColour_004699a0;
            DrawNavPlayerMarker(markerColour, 0);
            g_stNavMapTextContext_005a8160.viewport = &DAT_005a6ba0;
            UpdateInflightNavText((short)((frame / 4) % 2));
            SetRectBounds(&DAT_005a6ba0, 0, 0, 319, 199);

            eventType = PollInputEvent(&event, 0xff);
            switch (eventType) {
            case 2:
            case 10:
                done = 1;
                break;
            case 3:
            case 5:
                if ((short)event.value == 0x1c ||
                    (short)event.value == 0x39) {
                    done = 1;
                } else if ((short)event.value == 0x31) {
                    cycle_next_objective();
                    CentreMouseOnCurrentNavObjective();
                } else {
                    MoveMenuPointerFromKeyboard(&event);
                }
                break;
            }
            SelectNavObjectiveAtPoint(g_stMouseCursorState_0059ab10.x,
                                      g_stMouseCursorState_0059ab10.y);
            DIBslam();
            DIBslamReal();
        } while (done == 0 && DAT_0059ab58 == 0);

        if (DAT_0059ab58 != 0) {
            DAT_0059ab58 = 0;
            g_cCurrentNavPointIndex_0059c86c =
                (signed char)savedNavPoint;
            set_new_objective(savedNavPoint);
        }
        free_viewport(&DAT_005a76b0);
        LeaveAllocationScope();
        EventManagerHook(0);
        SetEventManagerPump(get_player_input);
        g_bInputMode_0059a848 = (unsigned char)savedInputMode;
    }

    ReleasePacketHandle(g_pNavMapShape_00468708);
    SetTextContext(&g_stDefaultTextContext_005a7740);
    PlaySfxWaveFileByNumber(0x19, -1, 0);
    memcpy((void *)&g_stMouseCursorState_0059ab10, savedInputState,
           sizeof(savedInputState));
    WarpMouseTo(((short *)savedInputState)[0],
                ((short *)savedInputState)[1]);
    if (DAT_0046a008 == 0) {
        force_view(0, 0);
    } else {
        GetScreenUpdateFlag();
        SetViewportRect(&DAT_005a7510, 0, 0,
                        (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                        (unsigned short)(g_nScreenHeight_0046daa8 - 1));
        initialize_view_buffer();
        force_view(0, 0);
        DAT_0046a008 = 1;
        GetScreenUpdateFlag();
        SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
        initialize_view_buffer();
    }
    g_bInflightComputerActive_00468754 = 0;
}

/* Function start: 0x40E890 */
unsigned short MergeAdjacentNearHeapBlocks(int descriptorAddress)
{
    NearHeapBlock *block;

    block = DosNearPtrToFar(descriptorAddress);
    if ((block->sizeAndFlags & 0x80000000) == 0 &&
        (block[1].sizeAndFlags & 0x80000000) == 0 &&
        block->address + (block->sizeAndFlags & 0xfffff) ==
            block[1].address) {
        block[1].address = block->address;
        block[1].sizeAndFlags += block->sizeAndFlags & 0xfffff;
        while (descriptorAddress > g_nNearHeapFirstDescriptor_005a8124) {
            descriptorAddress -= 8;
            block = DosNearPtrToFar(descriptorAddress);
            block[1].address = block->address;
            block[1].sizeAndFlags = block->sizeAndFlags;
        }
        g_nNearHeapFirstDescriptor_005a8124 += 8;
        return 1;
    }
    return 0;
}

/* Function start: 0x40E900 */
int ReleaseNearHeapBlock(int descriptorAddress)
{
    NearHeapBlock *block;
    int nextDescriptorAddress;

    block = DosNearPtrToFar(descriptorAddress);
    block->sizeAndFlags &= 0x7fffffff;
    nextDescriptorAddress = descriptorAddress + 8;
    if (nextDescriptorAddress <
            g_nNearHeapBase_005a8120 + g_nNearHeapSize_005a811c &&
        MergeAdjacentNearHeapBlocks(descriptorAddress) != 0)
        descriptorAddress = nextDescriptorAddress;
    if (descriptorAddress > g_nNearHeapFirstDescriptor_005a8124)
        MergeAdjacentNearHeapBlocks(descriptorAddress - 8);
    return descriptorAddress;
}

/* Function start: 0x40E950 */
void PurgeNearHeapBlocks(unsigned short flags)
{
    NearHeapBlock *block;
    int descriptorAddress;
    int descriptorBytes;

    if (g_nNearHeapActive_004688c0 != 0) {
        if ((flags & 0x10) != 0) {
            descriptorAddress = g_nNearHeapBase_005a8120;
            descriptorAddress += g_nNearHeapSize_005a811c;
            descriptorAddress -= 8;
            g_nNearHeapFirstDescriptor_005a8124 = descriptorAddress;
            block = DosNearPtrToFar(descriptorAddress);
            block->address = g_nNearHeapBase_005a8120;
            descriptorBytes = g_nNearHeapMaxDescriptors_004688c4 * 8;
            block->sizeAndFlags =
                g_nNearHeapSize_005a811c - descriptorBytes;
            return;
        }
        descriptorAddress = g_nNearHeapBase_005a8120 +
                            g_nNearHeapSize_005a811c - 8;
        for (; descriptorAddress >= g_nNearHeapFirstDescriptor_005a8124;
             descriptorAddress -= 8) {
            block = DosNearPtrToFar(descriptorAddress);
            if ((block->sizeAndFlags & 0x40000000) == 0)
                descriptorAddress = ReleaseNearHeapBlock(descriptorAddress);
        }
    }
}

/* Function start: 0x40E9E0 */
unsigned short InitializeNearHeap(void)
{
    short initialSize;
    int adjustedSize;

    if (g_nNearHeapActive_004688c0 == 0) {
        initialSize = (short)GetNavRangeSentinel();
        g_pNearHeapAllocation_005a8128 = 0;
        g_nNearHeapSize_005a811c = initialSize;
        if (g_nNearHeapMaxDescriptors_004688c4 * 8 <
            g_nNearHeapSize_005a811c) {
            g_pNearHeapAllocation_005a8128 =
                AllocateTaggedMemory(g_nNearHeapSize_005a811c, 0);
            if (g_pNearHeapAllocation_005a8128 != 0) {
                g_nNearHeapActive_004688c0++;
                g_nNearHeapBase_005a8120 =
                    DosFarPtrToNear(g_pNearHeapAllocation_005a8128);
                if (*(unsigned short *)0x00400013 == 0x270) {
                    g_nNearHeapRelocationBytes_004688c8 =
                        0x9c000 - g_nNearHeapSize_005a811c -
                        g_nNearHeapBase_005a8120;
                    adjustedSize =
                        0x98000 - g_nNearHeapRelocationBytes_004688c8;
                    adjustedSize -= g_nNearHeapBase_005a8120;
                    g_nNearHeapSize_005a811c = adjustedSize;
                    if (g_nNearHeapMaxDescriptors_004688c4 * 8 <
                        g_nNearHeapSize_005a811c) {
                        DosMemcpy(
                            DosNearPtrToFar(
                                g_nNearHeapBase_005a8120 +
                                g_nNearHeapSize_005a811c),
                            DosNearPtrToFar(
                                0x9c000 -
                                g_nNearHeapRelocationBytes_004688c8),
                            g_nNearHeapRelocationBytes_004688c8);
                    } else {
                        FreeIfNotNull(g_pNearHeapAllocation_005a8128);
                        g_nNearHeapActive_004688c0 = 0;
                    }
                }
                if (g_nNearHeapActive_004688c0 != 0)
                    PurgeNearHeapBlocks(0x10);
            }
        }
    }
    return g_nNearHeapActive_004688c0;
}

/* Function start: 0x40EB70 */
void *AllocateNearHeapBlockFromEnd(int size, unsigned short flags)
{
    NearHeapBlock *block;
    NearHeapBlock *lastBlock;
    int descriptorAddress;
    int shiftAddress;
    int allocationAddress;
    int blockSize;
    unsigned int allocationFlags;
    unsigned short alignment;

    if (size <= 0)
        return 0;
    allocationFlags = 0x80000000;
    alignment = flags & 3;
    if (alignment == 1) {
        allocationFlags = 0x90000000;
        size++;
    } else if (alignment == 2) {
        allocationFlags = 0xa0000000;
        size += 0xf;
    }
    if ((flags & 0x10) != 0)
        allocationFlags |= 0x40000000;

    allocationAddress = 0;
    descriptorAddress =
        g_nNearHeapBase_005a8120 + g_nNearHeapSize_005a811c - 8;
    for (; descriptorAddress >= g_nNearHeapFirstDescriptor_005a8124;
         descriptorAddress -= 8) {
        block = DosNearPtrToFar(descriptorAddress);
        blockSize = block->sizeAndFlags & 0xfffff;
        if ((block->sizeAndFlags & 0x80000000) != 0)
            continue;
        if ((int)blockSize < size)
            continue;
        if ((int)blockSize > size) {
            if (g_nNearHeapBase_005a8120 -
                    g_nNearHeapMaxDescriptors_004688c4 * 8 +
                    g_nNearHeapSize_005a811c >=
                g_nNearHeapFirstDescriptor_005a8124) {
                lastBlock = DosNearPtrToFar(
                    g_nNearHeapBase_005a8120 +
                    g_nNearHeapSize_005a811c - 8);
                if ((lastBlock->sizeAndFlags & 0x80000000) == 0) {
                    blockSize = lastBlock->sizeAndFlags & 0xfffff;
                    if (lastBlock->address + blockSize ==
                            g_nNearHeapFirstDescriptor_005a8124 &&
                        (int)blockSize > 8) {
                        lastBlock->sizeAndFlags -= 8;
                        g_nNearHeapMaxDescriptors_004688c4++;
                    }
                }
            }

            if (g_nNearHeapBase_005a8120 -
                    g_nNearHeapMaxDescriptors_004688c4 * 8 +
                    g_nNearHeapSize_005a811c >=
                g_nNearHeapFirstDescriptor_005a8124)
                continue;

            g_nNearHeapFirstDescriptor_005a8124 -= 8;
            shiftAddress = g_nNearHeapFirstDescriptor_005a8124;
            for (; descriptorAddress > shiftAddress; shiftAddress += 8) {
                block = DosNearPtrToFar(shiftAddress);
                block->address = block[1].address;
                block->sizeAndFlags = block[1].sizeAndFlags;
            }
            descriptorAddress -= 8;
            block = DosNearPtrToFar(descriptorAddress);
            blockSize = block->sizeAndFlags - size;
            block->sizeAndFlags = blockSize;
            block[1].address += blockSize & 0xffffff;
            blockSize = allocationFlags + size;
            block[1].sizeAndFlags = blockSize;
            allocationAddress = block[1].address;
            break;
        }
        block->sizeAndFlags |= allocationFlags;
        allocationAddress = block->address;
        break;
    }

    if (allocationAddress != 0) {
        if (alignment == 1)
            allocationAddress = (allocationAddress + 1) & 0xfffffffe;
        else if (alignment == 2)
            allocationAddress = (allocationAddress + 0xf) & 0xfffffff0;
    }
    return DosNearPtrToFar(allocationAddress);
}

/* Function start: 0x40ED30 */
void *AllocateNearHeapBlockByFlags(int size, unsigned short flags)
{
    NearHeapBlock *block;
    NearHeapBlock *lastBlock;
    int descriptorAddress;
    int shiftAddress;
    int allocationAddress;
    unsigned int blockSize;
    unsigned int allocationFlags;
    unsigned short alignment;

    if ((flags & 0x20) != 0)
        return AllocateNearHeapBlockFromEnd(size, flags);
    if (size <= 0)
        return 0;
    allocationFlags = 0x80000000;
    alignment = flags & 3;
    if (alignment == 1) {
        allocationFlags = 0x90000000;
        size++;
    } else if (alignment == 2) {
        allocationFlags = 0xa0000000;
        size += 0xf;
    }
    if ((flags & 0x10) != 0)
        allocationFlags |= 0x40000000;

    allocationAddress = 0;
    descriptorAddress = g_nNearHeapFirstDescriptor_005a8124;
    for (; descriptorAddress <
               g_nNearHeapBase_005a8120 + g_nNearHeapSize_005a811c;
         descriptorAddress += 8) {
        block = DosNearPtrToFar(descriptorAddress);
        blockSize = block->sizeAndFlags & 0xfffff;
        if ((block->sizeAndFlags & 0x80000000) != 0)
            continue;
        if ((int)blockSize < size)
            continue;
        if ((int)blockSize > size) {
            if (g_nNearHeapBase_005a8120 -
                    g_nNearHeapMaxDescriptors_004688c4 * 8 +
                    g_nNearHeapSize_005a811c >=
                g_nNearHeapFirstDescriptor_005a8124) {
                lastBlock = DosNearPtrToFar(
                    g_nNearHeapBase_005a8120 +
                    g_nNearHeapSize_005a811c - 8);
                if ((lastBlock->sizeAndFlags & 0x80000000) == 0) {
                    blockSize = lastBlock->sizeAndFlags & 0xfffff;
                    if (lastBlock->address + blockSize ==
                            g_nNearHeapFirstDescriptor_005a8124 &&
                        (int)blockSize > 8) {
                        lastBlock->sizeAndFlags -= 8;
                        g_nNearHeapMaxDescriptors_004688c4++;
                    }
                }
            }

            if (g_nNearHeapBase_005a8120 -
                    g_nNearHeapMaxDescriptors_004688c4 * 8 +
                    g_nNearHeapSize_005a811c >=
                g_nNearHeapFirstDescriptor_005a8124)
                continue;

            g_nNearHeapFirstDescriptor_005a8124 -= 8;
            shiftAddress = g_nNearHeapFirstDescriptor_005a8124;
            for (; descriptorAddress > shiftAddress; shiftAddress += 8) {
                block = DosNearPtrToFar(shiftAddress);
                block->address = block[1].address;
                block->sizeAndFlags = block[1].sizeAndFlags;
            }
            descriptorAddress -= 8;
            block = DosNearPtrToFar(descriptorAddress);
            block->sizeAndFlags = allocationFlags + size;
            block[1].address += size;
            block[1].sizeAndFlags -= size;
            allocationAddress = block->address;
            break;
        }
        block->sizeAndFlags |= allocationFlags;
        allocationAddress = block->address;
        break;
    }

    if (allocationAddress != 0) {
        if (alignment == 1)
            allocationAddress = (allocationAddress + 1) & 0xfffffffe;
        else if (alignment == 2)
            allocationAddress = (allocationAddress + 0xf) & 0xfffffff0;
    }
    return DosNearPtrToFar(allocationAddress);
}

/* Function start: 0x40EFE0 */
void add_statistics(short pilot, short missions, short kills)
{
    g_apWingmanPilots_00598a30[pilot]->missions += missions;
    g_apWingmanPilots_00598a30[pilot]->kills += kills;
}

/* Function start: 0x40F010 */
void PostMission(void)
{
    short oldKills;
    short pilot;
    short missions;
    short kills;

    oldKills = g_stCampaignState_0059ca50.currentPilot->kills;
    if (oldKills < 5 && oldKills + g_nPlayerKillCount_005a7c9c > 4)
        g_stCampaignState_0059ca50.badges[
            CAMPAIGN_BADGE_FIVE_KILLS] = 1;
    else if (oldKills < 25 &&
             oldKills + g_nPlayerKillCount_005a7c9c > 24)
        g_stCampaignState_0059ca50.badges[
            CAMPAIGN_BADGE_TWENTY_FIVE_KILLS] = 1;

    if (g_stCampaignState_0059ca50.badges[
            CAMPAIGN_BADGE_SHIP_TYPE_BASE +
            g_stCampaignState_0059ca50.playerShipType] == 0)
        g_stCampaignState_0059ca50.badges[
            CAMPAIGN_BADGE_SHIP_TYPE_BASE +
            g_stCampaignState_0059ca50.playerShipType] = 1;

    g_stCampaignState_0059ca50.currentPilot->missions++;
    switch (g_stCampaignState_0059ca50.currentPilot->missions) {
    case 1:
        g_stCampaignState_0059ca50.badges[
            CAMPAIGN_BADGE_FIRST_MISSION] = 1;
        /* The retail switch deliberately falls through. */
    case 5:
        g_stCampaignState_0059ca50.badges[
            CAMPAIGN_BADGE_FIVE_MISSIONS] = 1;
        break;
    case 10:
        g_stCampaignState_0059ca50.badges[
            CAMPAIGN_BADGE_TEN_MISSIONS] = 1;
        break;
    case 15:
        g_stCampaignState_0059ca50.badges[
            CAMPAIGN_BADGE_FIFTEEN_MISSIONS] = 1;
        break;
    }

    g_stCampaignState_0059ca50.currentPilot->kills +=
        g_nPlayerKillCount_005a7c9c;
    if (oldKills / 5 <
        g_stCampaignState_0059ca50.currentPilot->kills / 5)
        g_stCampaignState_0059ca50.promotionScore++;

    for (pilot = 0; pilot < 8; pilot++) {
        if (g_nYourWingman_0046c04c == -1 ||
            g_acShipRating_0059cd80[g_nYourWingman_0046c04c] != pilot) {
            if (g_stCampaignState_0059ca50.personalityDeathMission[
                    pilot] != 0) {
                continue;
            }
            missions = RandomInRange(0, 2);
            if (missions == 0)
                kills = 0;
            else
                kills = RandomInRange(
                    0, g_nPlayerKillCount_005a7c9c);
        } else {
            missions = 1;
            kills = g_nWingmanKillCount_005a7cb8;
        }
        add_statistics(pilot, missions, kills);
    }
}

/* Function start: 0x40F190 */
int FullMissionScore(void)
{
    signed char *scores;
    short objective;
    short score;

    score = 0;
    scores = (signed char *)(g_pMissionCampaignData_005988bc +
        (int)g_stCampaignState_0059ca50.currentMission * 0x14 +
        (int)g_stCampaignState_0059ca50.currentSeries * 0x5a - 0x50);
    objective = 0;
    do {
        score = (short)(score + scores[objective + 4]);
        objective++;
    } while (objective < 16);
    return score;
}

/* Function start: 0x40F1E0 */
int PlayersMissionScore(void)
{
    signed char *scores;
    short objective;
    short score;

    scores = (signed char *)(g_pMissionCampaignData_005988bc +
        (int)g_stCampaignState_0059ca50.currentSeries * 0x5a +
        (int)g_stCampaignState_0059ca50.currentMission * 0x14 - 0x50);
    score = 0;
    for (objective = 0; objective < 16; objective++) {
        if (achieved(objective) != 0)
            score = (short)(score + scores[objective + 4]);
    }
    return score;
}

/* Function start: 0x40F240 */
unsigned int UpdateSeries(void)
{
    unsigned char *seriesData;
    short *medalData;
    short fullScore;
    short playerScore;
    int failed;

    g_stSavedCampaignDate_0046e188 = *g_pCurrentCampaignDate_005a86a8;
    seriesData = g_pMissionCampaignData_005988bc +
        (int)g_stCampaignState_0059ca50.currentSeries * 0x5a - 0x5a;
    medalData = (short *)(g_pMissionCampaignData_005988bc +
        (int)g_stCampaignState_0059ca50.currentSeries * 0x5a +
        (int)g_stCampaignState_0059ca50.currentMission * 0x14 - 0x50);

    fullScore = (short)FullMissionScore();
    playerScore = (short)PlayersMissionScore();
    if (playerScore == fullScore)
        g_stCampaignState_0059ca50.promotionScore++;
    g_stCampaignState_0059ca50.seriesScore = (short)(
        g_stCampaignState_0059ca50.seriesScore + playerScore);
    g_stCampaignState_0059ca50.currentMission++;

    if (g_stCampaignState_0059ca50.currentMission >=
        (signed char)seriesData[2]) {
        DAT_004688dc =
            (short)g_stCampaignState_0059ca50.playerShipType;
        DAT_004688e8 = (short)(signed char)seriesData[5];
        g_stCampaignState_0059ca50.seriesHistory[
            g_stCampaignState_0059ca50.seriesHistoryCount] =
            g_stCampaignState_0059ca50.currentSeries;
        g_stCampaignState_0059ca50.seriesHistoryCount++;
        failed = g_stCampaignState_0059ca50.seriesScore <
            *(short *)(seriesData + 3);
        if (failed != 0) {
            g_stCampaignState_0059ca50.currentSeries =
                (signed char)seriesData[8];
            g_stCampaignState_0059ca50.playerShipType =
                (enum ObjectType)(signed char)seriesData[9];
        } else {
            g_stCampaignState_0059ca50.currentSeries =
                (signed char)seriesData[6];
            g_stCampaignState_0059ca50.playerShipType =
                (enum ObjectType)(signed char)seriesData[7];
        }
        DAT_004688ec = (unsigned short)failed;
        if (DAT_004688dc !=
            (short)g_stCampaignState_0059ca50.playerShipType) {
            DAT_004688d8 = 1;
            DAT_004688cc = 1;
        }
        g_stCampaignState_0059ca50.seriesScore = 0;
        g_stCampaignState_0059ca50.currentMission = 0;
        if ((signed char)g_pMissionCampaignData_005988bc[
                (int)g_stCampaignState_0059ca50.currentSeries *
                0x5a + 5] ==
                DAT_004688e8 &&
            DAT_004688e8 < 0x40)
            DAT_004688e8 = -1;
    }

    if (g_nWingmanKilledThisMission_005a7cb4 != 0)
        g_nMissionMedalScore_005a8116 =
            MaxShort(0, (short)(
                g_stCampaignState_0059ca50.missionScore - 15));
    if (medalData[1] <= g_nMissionMedalScore_005a8116 &&
        DAT_004688e4 == -1) {
        g_stSavedCampaignDate_0046e188 =
            *g_pCurrentCampaignDate_005a86a8;
        DAT_004688e4 = medalData[0];
    }
    return 0;
}

/* Function start: 0x40F3F0 */
unsigned int MoveNewCampaign(void)
{
    short days;

    if (g_stCampaignState_0059ca50.currentMission != 0)
        days = RandomInRange(0, 1);
    else
        days = (short)(RandomInRange(0, 1) + 5);
    g_pCurrentCampaignDate_005a86a8->day =
        (short)(g_pCurrentCampaignDate_005a86a8->day + days);
    if (g_pCurrentCampaignDate_005a86a8->day >= 366) {
        g_pCurrentCampaignDate_005a86a8->day =
            (short)(g_pCurrentCampaignDate_005a86a8->day - 365);
        g_pCurrentCampaignDate_005a86a8->year++;
    }
    return 0;
}

/* Function start: 0x40F440 */
unsigned int StartNewCampaign(short campaign)
{
    DAT_004688f0 = 1;
    ResetCampaignData();
    DAT_004688e0 = 1;
    RunTrainSim();
    g_stCampaignState_0059ca50.campaignIndex = campaign;
    g_nCampaignDataSet_005a8118 = campaign;
    DAT_004688e0 = 0;
    LoadPacketIntoBuffer(g_asCampaignPilotFiles_00469450[campaign], 1,
                         g_pMissionCampaignData_005988bc);
    DAT_00470510 = 0;
    DAT_005a8114 = -1;
    return 0;
}

/* Function start: 0x40F4B0 */
short GameFlow(void)
{
    short roomSelection;
    int barracksSelection;
    short launchMission;
    short flownSeries;
    short flownMission;
    short nextSeries;
    short nextMission;
    int flightResult;

    launchMission = 0;
    FrameStartHook(0);
    if (DAT_005a8114 != -1) {
        g_stCampaignState_0059ca50.campaignIndex = DAT_005a8114;
        g_nCampaignDataSet_005a8118 = DAT_005a8114;
    }

    DAT_0046505c = 0;
    do {
        roomSelection = 0;
        DAT_004688d4 = 0;
        DAT_004688e8 = -1;
        DAT_004688d0 = 0;
        DAT_004688e4 = -1;
        DAT_004688cc = 0;
        DAT_004688d8 = 0;
        if (DAT_004688e0 == 0)
            roomSelection = RecRoom();
        DAT_00470510 = 0;
        if (roomSelection == 5) {
            RunTrainSim();
        } else {
            barracksSelection = BarracksScreen();
            DAT_004688e0 = 0;
            if (barracksSelection == 6)
                return 0;
            if (barracksSelection == 7)
                launchMission++;
        }
        PumpWindowMessages();
    } while (launchMission == 0);

    DAT_0046505c = 1;
    g_nDebriefingPersonality_00465c80 = *(short *)(
        g_pMissionCampaignData_005988bc +
        (short)g_stCampaignState_0059ca50.currentSeries * 0x5a - 0x5a);
    Briefing((short)g_stCampaignState_0059ca50.currentSeries,
             (short)g_stCampaignState_0059ca50.currentMission);
    PlayScrambleHangarScene();
    g_stCampaignState_0059ca50.playerShipType =
        g_aMissionShips_0046c948[
            g_nPlayerMissionShipIndex_005a8694].type;
    scramble();
    flownSeries = (short)g_stCampaignState_0059ca50.currentSeries;
    flownMission = (short)g_stCampaignState_0059ca50.currentMission;
    init_mission(flownSeries, flownMission);
    LaunchPlayerShip();
    flightResult = RunSpaceFlight(-1);

    switch (flightResult) {
    case 1:
        free_cockpit();
        ShowCarrierLaunchSequence(
            (signed char)g_nPlayerCollisionObject_0046c050);
        g_nArcadeState_00469fb0 = 0;
        g_nPlayerCollisionObject_0046c050 = -1;
        free_3Space();
        flightResult = calculate_damage_level();
        landing((signed char)flightResult);
        break;
    case 2:
        ejection_sequence();
        check_stranded();
        if (g_nArcadeState_00469fb0 == 3)
            stranded_sequence();
        free_3Space();
        if (g_nArcadeState_00469fb0 == 3)
            return 0;
        g_nArcadeState_00469fb0 = 0;
        DAT_004688d4 = 1;
        g_stCampaignState_0059ca50.promotionScore = MaxShort(
            0, (short)(g_stCampaignState_0059ca50.promotionScore - 1));
        g_stCampaignState_0059ca50.elapsedDate.year++;
        if (g_stCampaignState_0059ca50.elapsedDate.year == 1)
            DAT_004688e4 = 3;
        DAT_004688cc = 1;
        break;
    case 3:
        stranded_sequence();
        free_3Space();
        return 0;
    case 4:
        death_sequence();
        free_3Space();
        funeral_sequence(1);
        DAT_004688f0 = 0;
        return 0;
    default:
        free_cockpit();
        free_all_slots();
        free_3Space();
        return 0;
    }

    PostMission();
    UpdateSeries();
    nextSeries = (short)g_stCampaignState_0059ca50.currentSeries;
    nextMission = (short)g_stCampaignState_0059ca50.currentMission;
    g_stCampaignState_0059ca50.currentSeries = (signed char)flownSeries;
    g_stCampaignState_0059ca50.currentMission = (signed char)flownMission;

    if (DAT_004688d4 == 0) {
        if ((unsigned short)RandomInRange(0, 5) +
                g_stCampaignState_0059ca50.promotionScore > 7) {
            g_stCampaignState_0059ca50.promotionScore = 0;
            if (g_nCampaignDataSet_005a8118 == 0) {
                DAT_004688d0 =
                    g_stCampaignState_0059ca50.currentPilot->rank < 3;
            } else if (g_nCampaignDataSet_005a8118 > 0) {
                DAT_004688d0 =
                    g_stCampaignState_0059ca50.currentPilot->rank < 4;
            } else {
                DAT_004688d0 = 0;
            }
            DAT_004688cc =
                DAT_004688cc != 0 || DAT_004688d0 != 0;
        }
    }

    DeBriefing(flownSeries, flownMission);
    if (DAT_004688d0 != 0)
        g_stCampaignState_0059ca50.currentPilot->rank++;

    if (nextSeries == -1) {
        if (DAT_004688e4 != -1)
            AwardCampaignMedal(DAT_004688e4);

        if (DAT_004688e8 == -1) {
            flightResult = 0;
        } else if (DAT_004688e8 == 0x40) {
            ShowCampaignVictorySequence();
            flightResult = 1;
        } else if (DAT_004688e8 == 0x41) {
            ShowTigerClawEscapeScene();
            flightResult = 0;
        } else {
            ShowMeanwhileTransition(DAT_004688e8, (short)DAT_004688ec);
            flightResult = DAT_004688ec >= 1;
        }
        ShowTheEndScreen((short)flightResult);
        DAT_004688f0 = 0;
        return 0;
    }

    if (g_nWingmanKilledThisMission_005a7cb4 != 0)
        funeral_sequence(0);
    if (DAT_004688cc == 1)
        Office();
    if (DAT_004688e4 != -1) {
        AwardCampaignMedal(DAT_004688e4);
        DAT_004688e4 = -1;
    }
    if (DAT_004688e8 != -1)
        ShowMeanwhileTransition(DAT_004688e8, (short)DAT_004688ec);
    g_stCampaignState_0059ca50.currentSeries = (signed char)nextSeries;
    g_stCampaignState_0059ca50.currentMission = (signed char)nextMission;
    MoveNewCampaign();
    AddRandomTrainSimHighScores();
    DAT_00470510 = 1;
    return 1;
}

/* Function start: 0x40F940 */
void __stdcall free_viewport(Viewport *viewport)
{
    unsigned char *allocation;
    int i;
    int oldCount;

    g_nFreeViewportCalls_005a8110++;
    allocation = viewport->allocation;
    if (allocation != 0) {
        i = 0;
        while (i < g_nViewportAllocationCount_005a7f0c) {
            if (g_apViewportAllocations_005a7f10[i] == allocation) {
                oldCount = g_nViewportAllocationCount_005a7f0c;
                g_nViewportAllocationCount_005a7f0c--;
                g_apViewportAllocations_005a7f10[i] =
                    g_apViewportAllocations_005a7f10[oldCount];
                break;
            }
            i++;
        }
        if (viewport->rowOffsets != 0) {
            ReleasePacketHandle(viewport->rowOffsets);
            viewport->rowOffsets = 0;
        }
        if (DAT_0046b168 != 0x13)
            printf("free_viewport not mcga\n");
        ReleasePacketHandle(allocation);
        viewport->pixels = 0;
        viewport->allocation = 0;
        if (DAT_005a6ba0.pixels == allocation)
            DAT_005a6ba0.pixels = 0;
        if (DAT_005a6ba0.allocation == allocation)
            DAT_005a6ba0.allocation = 0;
        if (DAT_005a76b0.pixels == allocation)
            DAT_005a76b0.pixels = 0;
        if (DAT_005a76b0.allocation == allocation)
            DAT_005a76b0.allocation = 0;
    }
}

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
    GetTransformedShapeBounds(&DAT_005a7510,
                              (short)(g_nScreenWidth_0046daa4 >> 1), y,
                              g_pTitleShape_005a7f08, 1, 0, scale, 0,
                              bounds);
#ifdef WC1_SDL
    if (!Wc1SdlRecordSpaceSprite(
            &DAT_005a7510, (short)(bounds[0] - 1), y,
            g_pTitleShape_005a7f08, 0, 0, scale, 0))
#endif
        DrawSpriteScaled(&DAT_005a7510, (short)(bounds[0] - 1), y,
                         g_pTitleShape_005a7f08, 0, 0, scale, 0);
#ifdef WC1_SDL
    if (!Wc1SdlRecordSpaceSprite(
            &DAT_005a7510,
            (short)(g_nScreenWidth_0046daa4 >> 1), y,
            g_pTitleShape_005a7f08, 1, 0, scale, 0))
#endif
        DrawSpriteScaled(&DAT_005a7510,
                         (short)(g_nScreenWidth_0046daa4 >> 1), y,
                         g_pTitleShape_005a7f08, 1, 0, scale, 0);
#ifdef WC1_SDL
    if (!Wc1SdlRecordSpaceSprite(
            &DAT_005a7510, bounds[2], y,
            g_pTitleShape_005a7f08, 2, 0, scale, 0))
#endif
        DrawSpriteScaled(&DAT_005a7510, bounds[2], y,
                         g_pTitleShape_005a7f08, 2, 0, scale, 0);
}

/* Function start: 0x40FB10 */
void UpdateTitleMenuCursor(void)
{
    short frame;
    short mouseX;
    short mouseY;
    TitleMenuRegion *region;

    frame = 0;
    mouseX = g_stHostMouseState_0059af70.x;
    mouseY = g_stHostMouseState_0059af70.y;
    region = g_aTitleMenuRegions_00468a88;
    while (region->frame != -1) {
        if (IsPointInRect(mouseX, mouseY,
                          &region->left) != 0)
            frame = region->frame;
        region++;
    }
    SetMouseCursorShape(g_stMouseCursorState_0059ab10.shape, frame);
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
    if (DAT_0046506c != 0)
        g_nIntroCreditCount_00468a30 += 9;
    if (DAT_0059ab58 == 0) {
        PreloadMusicTrackHook(0x17);
        SetEventManagerPump(PollJoystickButtonEvents);
        g_bIntroSceneResourcesActive_00469d48 = 0;
        init_3Space_objects(0);
        g_nCannedSceneMode_00469fac = 2;
        g_pIntroFont_005a8960 =
            FetchDiskPacketRetrying(9, 1, 0);
        g_nSceneResourceBudget_005a7ce4 = 0x3e8000;
        g_nSceneResourceBudget_005a7ce4 = LoadPacketResourceList(
            g_aIntroResourceDescriptors_00468ac0, 0, 0x3e8000);
        g_aObjectTypeData_00466458[OBJECT_TYPE_DEBRIS_WING].shapeSet =
            g_aObjectTypeData_00466458[
                OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
        ClearInputKeyStatePreservingModifiers();
        FlushInputEvents();
        DAT_0059ab58 = 0;

        while (state == 0) {
            PumpWindowMessages();
            missionShip = 32;
            do {
                g_aMissionShips_0046c948[missionShip].state = 0;
                missionShip++;
            } while (missionShip < 46);
            titleDistance = 200;
            remove_all_hazards();
            g_bIntroSecondaryScene_0046c024 = 0;
            set_up_action_sphere(16);
            g_pTitleShape_005a7f08 =
                FetchDiskPacketRetrying(9, 0, 0);
            spacetrack(0x17, 2, 1);
            initialize_scripted_view(g_asIntroCameraSequence_0046c090);
            DAT_00469fb4 = 1;

            frame = 0;
            do {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    print_subtitle(&DAT_005a7510, 0x32,
                                   g_pszIntroOpeningText_00468910);
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
                                  (short)(g_nViewCenterY_0059a854 - 6));
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
            FreePacketAndClear(&g_pTitleShape_005a7f08, 0);
            if (state != 0)
                break;

            ScaleFixedVector(&g_aShipForwardVector_0059bce0[61], 0x9600,
                             &g_aShipVelocity_0059c010[61]);
            set_up_action_sphere(17);
            g_bIntroSecondaryScene_0046c024 = 1;
            g_anObjectPitchRotation_0059b2a0[0] = 0;
            g_anObjectYawRotation_0059ce80[0] = 0;
            g_anObjectRollRotation_0059d7e0[0] = 0;
            start_hazard_field(0);

            credit = 0;
            for (; credit < g_nIntroCreditCount_00468a30; credit++) {
                frame = 0;
                do {
                    Update_3Space();
                    if (Draw_3Space_Frame() != 0) {
                        print_subtitle(&DAT_005a7510, 0x32,
                            g_apszIntroCredits_00468a38[credit]);
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
        ReleasePacketHandle(g_pIntroFont_005a8960);
        ReleasePacketHandle(g_pTitleShape_005a7f08);
        FreeShapeSet(g_aIntroResourceDescriptors_00468ac0, 0);
        g_aObjectTypeData_00466458[OBJECT_TYPE_DEBRIS_WING].shapeSet =
            g_aObjectTypeData_00466458[
                OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
        free_all_slots();
        free_3Space();
        g_bIntroSecondaryScene_0046c024 = 0;
        g_nCannedSceneMode_00469fac = 0;
        g_bScriptedView_0046a8d4 = 0;
        g_bIntroSceneResourcesActive_00469d48 = 1;
        ReleaseMusicTrackHook(0x17);
    }

    DAT_0059ab58 = 0;
    if (g_bTitleMenuSceneInitialized_00468ad8 == 0) {
        SceneEnterHook();
        g_bTitleMenuSceneInitialized_00468ad8 = 1;
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
    if (optionCount < 4) {
        short *fillWord;
        unsigned int fillCount;

        fillWord = &menuOptions[optionCount];
        fillCount = 4 - optionCount;
        do {
            *fillWord++ = -1;
            fillCount--;
        } while (fillCount != 0);
    }

    menuIndex = 0;
    do {
        if (menuOptions[menuIndex] == -1) {
            g_aTitleMenuRegions_00468a88[menuIndex].frame = -1;
        } else {
            g_aTitleMenuRegions_00468a88[menuIndex].frame = 1;
            if (menuOptions[menuIndex] < 3) {
                GetShapeFrameBounds(
                    &g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].top,
                    menuShape, menuOptions[menuIndex]);
            } else {
                GetShapeFrameBounds(
                    &g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].top,
                    alternateMenuShape, 0);
            }
        }
        menuIndex++;
    } while (menuIndex < 4);

    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    menuIndex = 0;
    do {
        if (menuOptions[menuIndex] != -1) {
            if (menuOptions[menuIndex] < 3) {
                DrawSpriteDefault(&DAT_005a6ba0,
                    g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].top,
                    menuShape, menuOptions[menuIndex]);
            } else {
                DrawSpriteDefault(&DAT_005a6ba0,
                    g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].top,
                    alternateMenuShape, 0);
            }
        }
        menuIndex++;
    } while (menuIndex < 4);
    DIBslam();
    DIBslamReal();

    g_stMouseCursorState_0059ab10.viewport = &DAT_005a6ba0;
    SetEventManagerPump(PollMenuInputDevices);
    g_nMenuInputRepeatDelay_005a8208 = 6;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    g_bInputMode_0059a848 = 1;
    DAT_0046505c = 0;
    while (state == 0) {
        selectedIndex = -1;
        activate = 0;
        UpdateTitleMenuCursor();
        eventType = PollInputEvent(&event, 0xff);
        if (eventType == 2) {
            activate = 1;
        } else if (eventType == 3 || eventType == 5) {
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
        }
        if (activate != 0) {
            if (selectedIndex == -1)
                selectedIndex = FindMenuRegionAtPoint(
                    event.x, event.y, g_aTitleMenuRegions_00468a88);
            if (selectedIndex < 0 || selectedIndex > 3)
                state = 0;
            else
                state = (signed char)(menuOptions[selectedIndex] + 1);
        }
        DIBslam();
        DIBslamReal();
    }

    DAT_0046505c = 1;
    ClearDebugPauseFlags();
    ReleasePacketHandle(menuShape);
    ReleasePacketHandle(alternateMenuShape);
    SetEventManagerPump(0);
    EventManagerHook(0);
    LeaveAllocationScope();
    FadeViewportPaletteToColour(&DAT_005a6ba0, DAT_0046999c, 1);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    DIBslam();
    DIBslamReal();
    RestoreGamePalette();
    return state - 1;
}
