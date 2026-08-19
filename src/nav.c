/*
 *  Nav map, location readouts and the virtual screen.
 *
 *  Address range 40d000-40ffff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: DrawNav* family; string band 4687AC-4688F4.
 */
#include "wc1.h"

short nNavMapCoordinateScaling = 0; /* 0x00468660 */
short nNavMapScale = 1; /* 0x00468664 */
NavMapObjectiveStyle aNavMapObjectiveStyles[10] = { /* 0x00468668 */
    { 1, 2, &cPrimaryTextColour, &cDefaultTextColour,
      &cDefaultTextColour },
    { 3, 2, &cBlackColour, &cViewportClearColour,
      &cDefaultTextColour },
    { 4, 2, &cMagentaColour, &cMagentaColour,
      &cDefaultTextColour },
    { 2, 3, &cMagentaColour, &cDefaultTextColour,
      &cDefaultTextColour },
    { 2, 3, &cRedColour, &cRedColour,
      &cDefaultTextColour },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 }
};
unsigned char *pNavMapShape = 0; /* 0x00468708 */
short nNavMapLabelCount = 0; /* 0x0046870c */
short nNavMapReservedAreaCount = 0; /* 0x00468710 */
char szCampaignSector[16] = "Vega XR-231.3"; /* 0x00468718 */
char *apszShipMissionTypeNames[11] = { /* 0x00468728 */
    szMissionPatrol,
    szMissionEscort,
    szMissionStrike,
    szMissionDefend,
    szMissionWingman,
    szMissionFlee,
    szMissionGotoWarp,
    szMissionWarpArriveError,
    szMissionCannedSequenceError,
    szMissionRendezvous,
    szMissionComeHomeError
};
int bInflightComputerActive = 0; /* 0x00468754 */
char szMissionPatrol[8] = "Patrol"; /* 0x00468758 */
char szMissionEscort[8] = "Escort"; /* 0x00468760 */
char szMissionStrike[8] = "Strike"; /* 0x00468768 */
char szMissionDefend[8] = "Defend"; /* 0x00468770 */
char szMissionWingman[8] = "Wingman"; /* 0x00468778 */
char szMissionFlee[8] = "Flee"; /* 0x00468780 */
char szMissionGotoWarp[12] = "Goto Warp"; /* 0x00468788 */
char szMissionWarpArriveError[4] = "err"; /* 0x00468794 */
char szMissionCannedSequenceError[4] = "err"; /* 0x00468798 */
char szMissionRendezvous[12] = "Rendezvous"; /* 0x0046879c */
char szMissionComeHomeError[4] = "err"; /* 0x004687a8 */
char szNavLabelTextFormat[12] = "%X%Y%F%s"; /* 0x004687ac */
char szNavAsteroids[12] = "Asteroids"; /* 0x004687b8 */
char szNavMines[8] = "Mines"; /* 0x004687c4 */
char szInflightTimeFieldFormat[8] = "%02d"; /* 0x004687cc */
char szStandardTimeFormat[24] = /* 0x004687d4 */
    "%X%YStandard time %s";
char szStandardTimeBlank[4] = " "; /* 0x004687ec */
char szInflightTimeSuffixFormat[8] = "%02d  "; /* 0x004687f0 */
char szStandardTimeColon[4] = ":"; /* 0x004687f8 */
char szNavLegendNewline[4] = "\n"; /* 0x004687fc */
char szNavMissionFlightPath[20] = "MISSION FLIGHT PATH"; /* 0x00468800 */
char szNavHomeBase[12] = "HOME BASE"; /* 0x00468814 */
char szNavBlankLine[4] = "\n"; /* 0x00468820 */
char szNavTitleFormat[8] = "%s\n\n"; /* 0x00468824 */
char szNavSectorFormat[12] = "Sector: %s\n"; /* 0x0046882c */
char szNavSystemFormat[16] = "System: %s\n\n"; /* 0x00468838 */
char szNavMissionFormat[8] = "* %s *\n"; /* 0x00468848 */
char szNavShipFormat[8] = "* %s *\n"; /* 0x00468850 */
char szNavNotesHeading[8] = "\nNotes\n"; /* 0x00468858 */
char szNavNoteFormat[4] = "%s\n"; /* 0x00468860 */
char szNavLocationFormat[48] = /* 0x00468864 */
    "%X%Y                         Location: %d.%d.%d";
char szNavViewportName[8] = "VSCREEN"; /* 0x00468894 */
char szBriefingNavMapTitle[20] = "Briefing Nav Map"; /* 0x0046889c */
char szConfedNavScan[16] = "ConFed Nav Scan"; /* 0x004688b0 */
volatile short nNearHeapActive = 0; /* 0x004688c0 */
volatile short nNearHeapMaxDescriptors = 0x80; /* 0x004688c4 */
volatile int nNearHeapRelocationBytes = 0; /* 0x004688c8 */
int bOfficeVisitPending = 0; /* 0x004688cc */
int bPromotionPending = 0; /* 0x004688d0 */
int bPlayerEjectedThisMission = 0; /* 0x004688d4 */
int bPlayerShipTypeChanged = 0; /* 0x004688d8 */
short nPreviousPlayerShipType = 0; /* 0x004688dc */
int bCampaignStartupMode = 0; /* 0x004688e0 */
short nPendingMedalIndex = -1; /* 0x004688e4 */
short nPostSeriesSequence = -1; /* 0x004688e8 */
unsigned short bSeriesFailed = 1; /* 0x004688ec */
int bCampaignActive = 0; /* 0x004688f0 */
char *pszIntroOpeningText = /* 0x00468910 */
    "In the distant future,\n"
    "mankind is locked in a deadly war...";
int nIntroCreditCount = 11; /* 0x00468a30 */
char *apszIntroCredits[20] = { /* 0x00468a38 */
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

TitleMenuRegion aTitleMenuRegions[5] = { /* 0x00468a88 */
    { 1, 49, 48, 283, 99 },
    { 1, 49, 91, 283, 149 },
    { 1, 49, 134, 283, 149 },
    { 1, 49, 177, 283, 209 },
    { -1, 0, 0, 0, 0 }
};

PacketResourceDescriptor aIntroResourceDescriptors[3] = { /* 0x00468ac0 */
    { &aObjectTypeData[OBJECT_TYPE_EXPLOSION1].shapeSet,
      3, 2 },
    { &aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet,
      3, 5 },
    { 0, 0, 0 }
};

int bTitleMenuSceneInitialized = 0; /* 0x00468ad8 */

/* Function start: 0x40D090 */
short NavMapPointInsideReservedArea(short area, short x, short y)
{
    ShortRect *rectangle;

    rectangle = &aNavMapExclusionRects[area];
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
        while (area < nNavMapReservedAreaCount &&
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
    nNavMapReservedAreaCount = 0;
}

/* Function start: 0x40D1E0 */
void ReserveNavMapArea(short x, short y, short width, short height)
{
    ShortRect *rectangle;
    short area;

    area = nNavMapReservedAreaCount;
    rectangle = &aNavMapExclusionRects[area];
    rectangle->left = x;
    rectangle->top = y;
    rectangle->right = (short)(x + width);
    rectangle->bottom = (short)(y + height);
    area++;
    nNavMapReservedAreaCount = area;
}

/* Function start: 0x40D240 */
void ResetNavMapLabels(void)
{
    nNavMapLabelCount = 0;
}

/* Function start: 0x40D250 */
short TryPlaceNavMapLabel(short x, short y, short width, short force)
{
    short placed;

    placed = 0;
    if (NavMapLabelPositionAvailable(x, y, width, 6) != 0 ||
        (force != 0 && NavMapLabelFits(x, y, width, 6) != 0)) {
        aNavMapLabels[nNavMapLabelCount].x = x;
        aNavMapLabels[nNavMapLabelCount].y = y;
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
    aNavMapLabels[
        nNavMapLabelCount].colour = colour;
    aNavMapLabels[
        nNavMapLabelCount].text = text;
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
        aNavMapLabels[nNavMapLabelCount].x,
        aNavMapLabels[nNavMapLabelCount].y,
        width, 6);
    nNavMapLabelCount++;
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
        if (aMissionObjectives[previous].index == missionShip)
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
    if (aNavMapLabels[labelIndex].x <= x &&
        (unsigned int)x <=
            (unsigned int)(aNavMapLabels[labelIndex].x +
                           strlen(aNavMapLabels[
                               labelIndex].text) * 4) &&
        aNavMapLabels[labelIndex].y <= y &&
        y <= aNavMapLabels[labelIndex].y + 6)
        return 1;
    return 0;
}

/* Function start: 0x40D540 */
void DrawNavMapLabels(void)
{
    short label;

    label = 0;
    while (label < (short)nNavMapLabelCount) {
        DrawFormattedText(szNavLabelTextFormat,
                          aNavMapLabels[label].x,
                          aNavMapLabels[label].y,
                          aNavMapLabels[label].colour,
                          aNavMapLabels[label].text);
        label++;
    }
}

/* Function start: 0x40D5A0 */
void DrawNavRectangleMarker(short x, short y, short size, short shadow,
                            unsigned short colour, short reserve)
{
    if (shadow == 0)
        DrawViewportEllipse(stNavLabelTextContext.viewport,
                            x, y, size, (short)((size * 7) / 8),
                            (short)colour);
    else
        DrawViewportEllipseShadow(
            stNavLabelTextContext.viewport,
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
        DrawViewportPixel(stNavLabelTextContext.viewport,
                          x, y, colour);
        DrawViewportPixel(stNavLabelTextContext.viewport,
                          (short)(x + 1), y, colour);
        DrawViewportPixel(stNavLabelTextContext.viewport,
                          x, (short)(y + 1), colour);
        DrawViewportPixel(stNavLabelTextContext.viewport,
                          (short)(x + 1),
                          (short)(y + 1), colour);
    } else {
        DrawNavSquareOutline(stNavLabelTextContext.viewport,
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
    DrawNavTriangleOutline(stNavLabelTextContext.viewport,
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
    DrawViewportLine(stNavLabelTextContext.viewport,
                     (short)(x - size), (short)(y - size),
                     (short)(x + size), (short)(y + size),
                     (short)colour);
    DrawViewportLine(stNavLabelTextContext.viewport,
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
    SetRectBounds(&stSceneBuffer, a, b, c, d);
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

    nav_getxy(&x, &y, aShipPosition[0].x,
              aShipPosition[0].z);
    x = (short)(x + stNavLabelTextContext.viewport->left);
    y = (short)(y + stNavLabelTextContext.viewport->top);
    DrawViewportPixel(stNavLabelTextContext.viewport,
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
    DrawSpriteDefault(&stSceneBuffer, 1, 1, pNavMapShape, 0);
    SetScreenClipRect(2, 2, 152, 137);
    stNavLabelTextContext.viewport = &stSceneBuffer;
    stNavLabelTextContext.text = szDefaultTextBuffer;
    InitializeTextContextFromFont(&stNavLabelTextContext,
                                  2, cPrimaryTextColour, -1);
    stNavLabelTextContext.alignment = 0;
    SetTextContext(&stNavLabelTextContext);
    ResetNavMapLabels();
    ResetNavMapReservedAreas();
    SetScale();

    for (navPoint = aMissionNavPoints;
         navPoint->type != 0;
         navPoint++) {
        for (slot = 0; slot < 10; slot++) {
            missionShipIndex = navPoint->missionShips[slot];
            if (missionShipIndex != -1) {
                missionShip = &aMissionShips[missionShipIndex];
                if (missionShip->type == OBJECT_TYPE_ASTEROID_FIELD) {
                    DrawNavHazardMarker(navPoint->position,
                                        missionShip->position,
                                        missionShip->speed,
                                        cAsteroidColour, cAsteroidColour,
                                        szNavAsteroids);
                } else if (missionShip->type == OBJECT_TYPE_MINE_FIELD) {
                    DrawNavHazardMarker(navPoint->position,
                                        missionShip->position,
                                        missionShip->speed,
                                        cRedColour, cRedColour,
                                        szNavMines);
                }
            }
        }
    }

    for (objectiveIndex = 0;
         objectiveIndex < (short)cMissionObjectiveCount;
         objectiveIndex++) {
        objective = &aMissionObjectives[objectiveIndex];
        if (mobile_objective(objectiveIndex) == 0 ||
            (aMissionShips[
                 (signed char)objective->index].state == 0 &&
             achieved(objectiveIndex) == 0)) {
            ScaleNavMapCoordinates(&x, &y,
                                   objective->mapX, objective->mapY);
            if (hidden_objective(objectiveIndex) == 0) {
                style = &aNavMapObjectiveStyles[
                    objective->type];
                if (visited(objectiveIndex) == 0)
                    DrawViewportPixel(&stSceneBuffer, x, y,
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
                if (cCurrentObjective == objectiveIndex)
                    labelColour = cYellowColour;
                else
                    labelColour = *style->labelColour;
                awNavObjectiveLabelIndex[objectiveIndex] =
                    nNavMapLabelCount;
                AddUniqueObjectiveNavLabel(
                    x, y, labelColour, objective_name(objectiveIndex),
                    objectiveIndex, (short)objective->index);
            }
        }
    }
    if (showPlayer != 0) {
        DrawNavPlayerMarker(cViewportClearColour, 1);
        nav_getxy(&x, &y, aShipPosition[0].x,
                  aShipPosition[0].z);
        PlaceNavMapLabel(
            x, y, cLightGreyColour,
            stCampaignState.currentPilot->callsign);
    }
    DrawNavMapLabels();
    SetScreenClipRect(0, 0, 259, 155);
}

/* Function start: 0x40DDA0 */
void UpdateInflightNavText(short showColon)
{
    char time[12];
    short cursorX;

    SetTextContext(&stNavMapTextContext);
    sprintf(time, szInflightTimeFieldFormat,
            (int)((signed char *)pElapsedCampaignDate)[0]);
    DrawFormattedText(
        szStandardTimeFormat,
        (int)(short)(stNavMapTextContext.viewport->left + 150),
        (int)(short)(stNavMapTextContext.viewport->top + 140),
        time);
    cursorX = stNavMapTextContext.cursorX;
    if (showColon == 0) {
        DrawFormattedText(szStandardTimeBlank);
    } else {
        stNavMapTextContext.cursorX =
            (short)(stNavMapTextContext.cursorX + 4);
    }
    sprintf(time, szInflightTimeSuffixFormat,
            (int)((signed char *)pElapsedCampaignDate)[1]);
    DrawFormattedText(time);
    stNavMapTextContext.cursorX = cursorX;
    if (showColon != 0)
        DrawFormattedText(szStandardTimeColon);
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
    if ((short)cCurrentNavPointIndex == navPoint) {
        DrawNavTextLine(0, (unsigned short)cYellowColour, text);
        DrawNavTextLine(0, (unsigned short)cYellowColour,
                        szNavLegendNewline);
    }
}

/* Function start: 0x40DEE0 */
void DrawNavMapLegend(void)
{
    short objective;

    objective = 0;
    while (objective < (short)cMissionObjectiveCount) {
        if (visited(objective) == 0 && hidden_objective(objective) == 0)
            break;
        objective++;
    }
    SetTextCursor((unsigned short)stSceneBuffer.left, 120);
    DrawSelectedNavLegendEntry(szNavMissionFlightPath,
                               objective);
    DrawSelectedNavLegendEntry(
        szNavHomeBase,
        (short)((short)cMissionObjectiveCount - 1));
}

/* Function start: 0x40DF50 */
char *nav_note(short objective)
{
    char *note;

    note = aMissionObjectives[objective].name;
    if (*note == '?')
        note++;
    return note;
}

/* Function start: 0x40DF70 */
void DrawNavLocationReadout(const char *title, short showFlightData)
{
    enum ShipMissionType playerMissionType;

    ClearViewport(&stSceneBuffer, cBlackColour);
    SetScreenClipRect(155, 2, 259, 155);
    stNavMapTextContext.viewport = &stSceneBuffer;
    stNavMapTextContext.text = szDefaultTextBuffer;
    InitializeTextContextFromFont(&stNavMapTextContext,
                                  1, cPrimaryTextColour, cBlackColour);
    stNavMapTextContext.alignment = 0;
    stNavMapTextContext.cursorX = 0;
    stNavMapTextContext.cursorY = 0;
    SetTextContext(&stNavMapTextContext);
    DrawNavTextLine(0, cDefaultTextColour,
                    szNavBlankLine);
    DrawNavTextLine(2, cDefaultTextColour,
                    szNavTitleFormat, title);
    DrawNavTextLine(0, cDefaultTextColour,
                    szNavSectorFormat,
                    szCampaignSector);
    DrawNavTextLine(0, cDefaultTextColour,
                    szNavSystemFormat,
                    abSeriesAuxData);
    DrawNavTextLine(2, cDefaultTextColour,
                    szNavMissionFormat,
                    abMissionAuxData);
    playerMissionType = aMissionShips[
        nPlayerMissionShipIndex].missionType;
    DrawNavTextLine(2, cDefaultTextColour,
                    szNavShipFormat,
                    apszShipMissionTypeNames[
                        playerMissionType]);
    DrawNavTextLine(2, cDefaultTextColour,
                    szNavNotesHeading);
    DrawNavTextLine(0, cDefaultTextColour,
                    szNavNoteFormat,
                    nav_note((short)cCurrentObjective));
    if (showFlightData != 0)
        DrawNavMapLegend();
    BuildMap(showFlightData);
    if (showFlightData != 0) {
        SetScreenClipRect(0, 0, 259, 155);
        SetTextContext(&stNavMapTextContext);
        DrawNavTextLine(0, cDefaultTextColour,
                        szNavLocationFormat,
                        8, 142,
                        aShipPosition[0].x,
                        aShipPosition[0].y,
                        aShipPosition[0].z);
    }
    CopyViewportContents(&stSceneBuffer, &stScreen);
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x40E190 */
void BriefingMap_LoadShapes(void)
{
    short objective;

    pNavMapShape =
        FetchDiskPacketRetrying(8, 2, 0);
    SetScreenClipRect(0, 0, 259, 155);
    if (AllocateViewport(&stSceneBuffer,
                         (short)cBlackColour, 0) == 0)
        ReportOutOfMemoryAndExit(szNavViewportName);
    objective = 0;
    while (objective < (short)cMissionObjectiveCount) {
        LocateMobileObjective(objective);
        objective++;
    }
}

/* Function start: 0x40E210 */
void BriefingMap_DisplayMap(void)
{
    Viewport savedViewport;

    savedViewport = stSceneBuffer;
    free_viewport(&stSceneBuffer);
    BriefingMap_LoadShapes();
    stScreen.top = 4;
    DrawNavLocationReadout(szBriefingNavMapTitle, 0);
    free_viewport(&stSceneBuffer);
    ReleasePacketHandle(pNavMapShape);
    pNavMapShape = 0;
    ReleaseTextFont(2);
    ReleaseTextFont(1);
    stSceneBuffer = savedViewport;
    AllocateViewport(&stSceneBuffer, (short)cBlackColour, 0);
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

    oldNavPoint = (short)cCurrentNavPointIndex;
    mouseX = (short)(mouseX - 30);
    mouseY = (short)(mouseY - 22);
    pathIndex = 0;
    selected = 0;
    objective = abFlightPath[pathIndex];
    while (objective != -1) {
        if (hidden_objective((short)objective) == 0) {
            ScaleNavMapCoordinates(
                &mapX, &mapY,
                aMissionObjectives[objective].mapX,
                aMissionObjectives[objective].mapY);
            if ((short)(abs((int)mouseX - mapX) +
                        abs((int)mouseY - mapY)) < 6 ||
                IsPointInNavMapLabel(
                    (short)awNavObjectiveLabelIndex[pathIndex],
                    mouseX, mouseY) != 0) {
                selected = 1;
                set_new_objective(pathIndex);
                if (pathIndex == oldNavPoint)
                    return selected;
            }
        }
        pathIndex++;
        objective = abFlightPath[pathIndex];
    }
    return selected;
}

/* Function start: 0x40E3C0 */
void CentreMouseOnCurrentNavObjective(void)
{
    short x;
    short y;
    signed char objective;

    objective = abFlightPath[
        cCurrentNavPointIndex];
    ScaleNavMapCoordinates(
        &x, &y,
        aMissionObjectives[objective].mapX,
        aMissionObjectives[objective].mapY);
    x = (short)(x + 30);
    y = (short)(y + 22);
    LeaveAllocationScope();
    WarpMouseTo(x, y);
    EnterAllocationScope();
}

/* Function start: 0x40E430 */
void ShowConfedNavScan(void)
{
    SetRectBounds(&stScreen, 30, 22, 289, 177);
    LeaveAllocationScope();
    DrawNavLocationReadout(szConfedNavScan, 1);
    EnterAllocationScope();
    SetRectBounds(&stScreen, 0, 0, 319, 199);
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
#ifdef WC1_SDL
    MouseCursorState savedInputState;
#else
    unsigned int savedInputState[7];
#endif

    savedNavPoint = (short)cCurrentNavPointIndex;
    done = 0;
    hasObjectives = 0;
    displayedNavPoint = savedNavPoint;
    bInflightComputerActive = 1;
#ifdef WC1_SDL
    memcpy(&savedInputState,
           (const void *)&stMouseCursorState,
           sizeof(savedInputState));
#else
    memcpy(savedInputState, (const void *)&stMouseCursorState,
           sizeof(savedInputState));
#endif

    if (message_showing() != 0)
        EndCommMenu();
    GetScreenUpdateFlag();
    cScreenViewportMode = -1;
    background = FetchDiskPacketRetrying(8, 1, 0);
    ClearViewport(&stScreen, cBlackColour);
    DrawSpriteDefault(&stScreen, 0, 0, background, 0);
    ReleasePacketHandle(background);

    objective = 0;
    BriefingMap_LoadShapes();
    ShowConfedNavScan();
    if (cMissionObjectiveCount > 0) {
        do {
            if (hidden_objective(objective) == 0)
                hasObjectives = 1;
            objective++;
        } while (objective <
                 (short)cMissionObjectiveCount);
    }

    if (hasObjectives == 0) {
        SetEventManagerPump(PollJoystickButtonEvents);
        WaitForInputKey();
        SetFrameTimerAndWait(20);
        SetEventManagerPump(get_player_input);
    } else {
        pointerViewport = stScreen;
        SetRectBounds(&pointerViewport, 32, 24, 182, 159);
        savedInputMode = (short)(signed char)bInputMode;
        stMouseCursorState.viewport = &pointerViewport;
        bInputMode = 1;
        SetEventManagerPump(PollMenuInputDevices);
        EventManagerHook(ResetMouseCursorFrame);
        nMenuInputRepeatDelay = 6;
        EnterAllocationScope();
        CentreMouseOnCurrentNavObjective();

        do {
            if (displayedNavPoint !=
                (short)cCurrentNavPointIndex) {
                displayedNavPoint =
                    (short)cCurrentNavPointIndex;
                PlaySfxWaveFileByNumber(0x19, -1, 0);
                ShowConfedNavScan();
            }
            SetRectBounds(&stScreen, 32, 24, 289, 177);
            SetMouseCursorShape(stMouseCursorState.shape, 0);
            FormatNavCoordinates(
                (unsigned char *)pElapsedCampaignDate);
            stNavLabelTextContext.viewport = &stScreen;
            frame = nTickCount60Hz / 15;
            markerColour = cDarkGreyColour;
            if (frame % 2 != 0)
                markerColour = cViewportClearColour;
            DrawNavPlayerMarker(markerColour, 0);
            stNavMapTextContext.viewport = &stScreen;
            UpdateInflightNavText((short)((frame / 4) % 2));
            SetRectBounds(&stScreen, 0, 0, 319, 199);

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
            SelectNavObjectiveAtPoint(stMouseCursorState.x,
                                      stMouseCursorState.y);
            DIBslam();
            DIBslamReal();
        } while (done == 0 && bEscapePressed == 0);

        if (bEscapePressed != 0) {
            bEscapePressed = 0;
            cCurrentNavPointIndex =
                (signed char)savedNavPoint;
            set_new_objective(savedNavPoint);
        }
        free_viewport(&stSceneBuffer);
        LeaveAllocationScope();
        EventManagerHook(0);
        SetEventManagerPump(get_player_input);
        bInputMode = (unsigned char)savedInputMode;
    }

    ReleasePacketHandle(pNavMapShape);
    SetTextContext(&stDefaultTextContext);
    PlaySfxWaveFileByNumber(0x19, -1, 0);
#ifdef WC1_SDL
    memcpy((void *)&stMouseCursorState,
           &savedInputState, sizeof(savedInputState));
    WarpMouseTo(savedInputState.x, savedInputState.y);
#else
    memcpy((void *)&stMouseCursorState, savedInputState,
           sizeof(savedInputState));
    WarpMouseTo(((short *)savedInputState)[0],
                ((short *)savedInputState)[1]);
#endif
    if (bCockpitlessView == 0) {
        force_view(0, 0);
    } else {
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBuffer, 0, 0,
                        (unsigned short)(nScreenWidth - 1),
                        (unsigned short)(nScreenHeight - 1));
        initialize_view_buffer();
        force_view(0, 0);
        bCockpitlessView = 1;
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
        initialize_view_buffer();
    }
    bInflightComputerActive = 0;
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
        while (descriptorAddress > nNearHeapFirstDescriptor) {
            descriptorAddress -= 8;
            block = DosNearPtrToFar(descriptorAddress);
            block[1].address = block->address;
            block[1].sizeAndFlags = block->sizeAndFlags;
        }
        nNearHeapFirstDescriptor += 8;
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
            nNearHeapBase + nNearHeapSize &&
        MergeAdjacentNearHeapBlocks(descriptorAddress) != 0)
        descriptorAddress = nextDescriptorAddress;
    if (descriptorAddress > nNearHeapFirstDescriptor)
        MergeAdjacentNearHeapBlocks(descriptorAddress - 8);
    return descriptorAddress;
}

/* Function start: 0x40E950 */
void PurgeNearHeapBlocks(unsigned short flags)
{
    NearHeapBlock *block;
    int descriptorAddress;
    int descriptorBytes;

    if (nNearHeapActive != 0) {
        if ((flags & 0x10) != 0) {
            descriptorAddress = nNearHeapBase;
            descriptorAddress += nNearHeapSize;
            descriptorAddress -= 8;
            nNearHeapFirstDescriptor = descriptorAddress;
            block = DosNearPtrToFar(descriptorAddress);
            block->address = nNearHeapBase;
            descriptorBytes = nNearHeapMaxDescriptors * 8;
            block->sizeAndFlags =
                nNearHeapSize - descriptorBytes;
            return;
        }
        descriptorAddress = nNearHeapBase +
                            nNearHeapSize - 8;
        for (; descriptorAddress >= nNearHeapFirstDescriptor;
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

    if (nNearHeapActive == 0) {
        initialSize = (short)GetNavRangeSentinel();
        pNearHeapAllocation = 0;
        nNearHeapSize = initialSize;
        if (nNearHeapMaxDescriptors * 8 <
            nNearHeapSize) {
            pNearHeapAllocation =
                AllocateTaggedMemory(nNearHeapSize, 0);
            if (pNearHeapAllocation != 0) {
                nNearHeapActive++;
                nNearHeapBase =
                    DosFarPtrToNear(pNearHeapAllocation);
                if (*(unsigned short *)0x00400013 == 0x270) {
                    nNearHeapRelocationBytes =
                        0x9c000 - nNearHeapSize -
                        nNearHeapBase;
                    adjustedSize =
                        0x98000 - nNearHeapRelocationBytes;
                    adjustedSize -= nNearHeapBase;
                    nNearHeapSize = adjustedSize;
                    if (nNearHeapMaxDescriptors * 8 <
                        nNearHeapSize) {
                        DosMemcpy(
                            DosNearPtrToFar(
                                nNearHeapBase +
                                nNearHeapSize),
                            DosNearPtrToFar(
                                0x9c000 -
                                nNearHeapRelocationBytes),
                            nNearHeapRelocationBytes);
                    } else {
                        FreeIfNotNull(pNearHeapAllocation);
                        nNearHeapActive = 0;
                    }
                }
                if (nNearHeapActive != 0)
                    PurgeNearHeapBlocks(0x10);
            }
        }
    }
    return nNearHeapActive;
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
        nNearHeapBase + nNearHeapSize - 8;
    for (; descriptorAddress >= nNearHeapFirstDescriptor;
         descriptorAddress -= 8) {
        block = DosNearPtrToFar(descriptorAddress);
        blockSize = block->sizeAndFlags & 0xfffff;
        if ((block->sizeAndFlags & 0x80000000) != 0)
            continue;
        if ((int)blockSize < size)
            continue;
        if ((int)blockSize > size) {
            if (nNearHeapBase -
                    nNearHeapMaxDescriptors * 8 +
                    nNearHeapSize >=
                nNearHeapFirstDescriptor) {
                lastBlock = DosNearPtrToFar(
                    nNearHeapBase +
                    nNearHeapSize - 8);
                if ((lastBlock->sizeAndFlags & 0x80000000) == 0) {
                    blockSize = lastBlock->sizeAndFlags & 0xfffff;
                    if (lastBlock->address + blockSize ==
                            nNearHeapFirstDescriptor &&
                        (int)blockSize > 8) {
                        lastBlock->sizeAndFlags -= 8;
                        nNearHeapMaxDescriptors++;
                    }
                }
            }

            if (nNearHeapBase -
                    nNearHeapMaxDescriptors * 8 +
                    nNearHeapSize >=
                nNearHeapFirstDescriptor)
                continue;

            nNearHeapFirstDescriptor -= 8;
            shiftAddress = nNearHeapFirstDescriptor;
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
    descriptorAddress = nNearHeapFirstDescriptor;
    for (; descriptorAddress <
               nNearHeapBase + nNearHeapSize;
         descriptorAddress += 8) {
        block = DosNearPtrToFar(descriptorAddress);
        blockSize = block->sizeAndFlags & 0xfffff;
        if ((block->sizeAndFlags & 0x80000000) != 0)
            continue;
        if ((int)blockSize < size)
            continue;
        if ((int)blockSize > size) {
            if (nNearHeapBase -
                    nNearHeapMaxDescriptors * 8 +
                    nNearHeapSize >=
                nNearHeapFirstDescriptor) {
                lastBlock = DosNearPtrToFar(
                    nNearHeapBase +
                    nNearHeapSize - 8);
                if ((lastBlock->sizeAndFlags & 0x80000000) == 0) {
                    blockSize = lastBlock->sizeAndFlags & 0xfffff;
                    if (lastBlock->address + blockSize ==
                            nNearHeapFirstDescriptor &&
                        (int)blockSize > 8) {
                        lastBlock->sizeAndFlags -= 8;
                        nNearHeapMaxDescriptors++;
                    }
                }
            }

            if (nNearHeapBase -
                    nNearHeapMaxDescriptors * 8 +
                    nNearHeapSize >=
                nNearHeapFirstDescriptor)
                continue;

            nNearHeapFirstDescriptor -= 8;
            shiftAddress = nNearHeapFirstDescriptor;
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
    apWingmanPilots[pilot]->missions += missions;
    apWingmanPilots[pilot]->kills += kills;
}

/* Function start: 0x40F010 */
void PostMission(void)
{
    short oldKills;
    short pilot;
    short missions;
    short kills;

    oldKills = stCampaignState.currentPilot->kills;
    if (oldKills < 5 && oldKills + nPlayerKillCount > 4)
        stCampaignState.badges[
            CAMPAIGN_BADGE_FIVE_KILLS] = 1;
    else if (oldKills < 25 &&
             oldKills + nPlayerKillCount > 24)
        stCampaignState.badges[
            CAMPAIGN_BADGE_TWENTY_FIVE_KILLS] = 1;

    if (stCampaignState.badges[
            CAMPAIGN_BADGE_SHIP_TYPE_BASE +
            stCampaignState.playerShipType] == 0)
        stCampaignState.badges[
            CAMPAIGN_BADGE_SHIP_TYPE_BASE +
            stCampaignState.playerShipType] = 1;

    stCampaignState.currentPilot->missions++;
    switch (stCampaignState.currentPilot->missions) {
    case 1:
        stCampaignState.badges[
            CAMPAIGN_BADGE_FIRST_MISSION] = 1;
        /* The retail switch deliberately falls through. */
    case 5:
        stCampaignState.badges[
            CAMPAIGN_BADGE_FIVE_MISSIONS] = 1;
        break;
    case 10:
        stCampaignState.badges[
            CAMPAIGN_BADGE_TEN_MISSIONS] = 1;
        break;
    case 15:
        stCampaignState.badges[
            CAMPAIGN_BADGE_FIFTEEN_MISSIONS] = 1;
        break;
    }

    stCampaignState.currentPilot->kills +=
        nPlayerKillCount;
    if (oldKills / 5 <
        stCampaignState.currentPilot->kills / 5)
        stCampaignState.promotionScore++;

    for (pilot = 0; pilot < 8; pilot++) {
        if (nYourWingman == -1 ||
            acShipRating[nYourWingman] != pilot) {
            if (stCampaignState.personalityDeathMission[
                    pilot] != 0) {
                continue;
            }
            missions = RandomInRange(0, 2);
            if (missions == 0)
                kills = 0;
            else
                kills = RandomInRange(
                    0, nPlayerKillCount);
        } else {
            missions = 1;
            kills = nWingmanKillCount;
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
    scores = (signed char *)(pMissionCampaignData +
        (int)stCampaignState.currentMission * 0x14 +
        (int)stCampaignState.currentSeries * 0x5a - 0x50);
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

    scores = (signed char *)(pMissionCampaignData +
        (int)stCampaignState.currentSeries * 0x5a +
        (int)stCampaignState.currentMission * 0x14 - 0x50);
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

    stSavedCampaignDate = *pCurrentCampaignDate;
    seriesData = pMissionCampaignData +
        (int)stCampaignState.currentSeries * 0x5a - 0x5a;
    medalData = (short *)(pMissionCampaignData +
        (int)stCampaignState.currentSeries * 0x5a +
        (int)stCampaignState.currentMission * 0x14 - 0x50);

    fullScore = (short)FullMissionScore();
    playerScore = (short)PlayersMissionScore();
    if (playerScore == fullScore)
        stCampaignState.promotionScore++;
    stCampaignState.seriesScore = (short)(
        stCampaignState.seriesScore + playerScore);
    stCampaignState.currentMission++;

    if (stCampaignState.currentMission >=
        (signed char)seriesData[2]) {
        nPreviousPlayerShipType =
            (short)stCampaignState.playerShipType;
        nPostSeriesSequence =
            (short)(signed char)seriesData[5];
        stCampaignState.seriesHistory[
            stCampaignState.seriesHistoryCount] =
            stCampaignState.currentSeries;
        stCampaignState.seriesHistoryCount++;
        failed = stCampaignState.seriesScore <
            *(short *)(seriesData + 3);
        if (failed != 0) {
            stCampaignState.currentSeries =
                (signed char)seriesData[8];
            stCampaignState.playerShipType =
                (enum ObjectType)(signed char)seriesData[9];
        } else {
            stCampaignState.currentSeries =
                (signed char)seriesData[6];
            stCampaignState.playerShipType =
                (enum ObjectType)(signed char)seriesData[7];
        }
        bSeriesFailed = (unsigned short)failed;
        if (nPreviousPlayerShipType !=
            (short)stCampaignState.playerShipType) {
            bPlayerShipTypeChanged = 1;
            bOfficeVisitPending = 1;
        }
        stCampaignState.seriesScore = 0;
        stCampaignState.currentMission = 0;
        if ((signed char)pMissionCampaignData[
                (int)stCampaignState.currentSeries *
                0x5a + 5] ==
                nPostSeriesSequence &&
            nPostSeriesSequence < 0x40)
            nPostSeriesSequence = -1;
    }

    if (nWingmanKilledThisMission != 0)
        nMissionMedalScore =
            MaxShort(0, (short)(
                stCampaignState.missionScore - 15));
    if (medalData[1] <= nMissionMedalScore &&
        nPendingMedalIndex == -1) {
        stSavedCampaignDate =
            *pCurrentCampaignDate;
        nPendingMedalIndex = medalData[0];
    }
    return 0;
}

/* Function start: 0x40F3F0 */
unsigned int MoveNewCampaign(void)
{
    short days;

    if (stCampaignState.currentMission != 0)
        days = RandomInRange(0, 1);
    else
        days = (short)(RandomInRange(0, 1) + 5);
    pCurrentCampaignDate->day =
        (short)(pCurrentCampaignDate->day + days);
    if (pCurrentCampaignDate->day >= 366) {
        pCurrentCampaignDate->day =
            (short)(pCurrentCampaignDate->day - 365);
        pCurrentCampaignDate->year++;
    }
    return 0;
}

/* Function start: 0x40F440 */
unsigned int StartNewCampaign(short campaign)
{
    bCampaignActive = 1;
    ResetCampaignData();
    bCampaignStartupMode = 1;
    RunTrainSim();
    stCampaignState.campaignIndex = campaign;
    nCampaignDataSet = campaign;
    bCampaignStartupMode = 0;
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 1,
                         pMissionCampaignData);
    bPanRoomTransition = 0;
    nPendingCampaignIndex = -1;
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
    if (nPendingCampaignIndex != -1) {
        stCampaignState.campaignIndex = nPendingCampaignIndex;
        nCampaignDataSet = nPendingCampaignIndex;
    }

    bKeyEventQueueEnabled = 0;
    do {
        roomSelection = 0;
        bPlayerEjectedThisMission = 0;
        nPostSeriesSequence = -1;
        bPromotionPending = 0;
        nPendingMedalIndex = -1;
        bOfficeVisitPending = 0;
        bPlayerShipTypeChanged = 0;
        if (bCampaignStartupMode == 0)
            roomSelection = RecRoom();
        bPanRoomTransition = 0;
        if (roomSelection == 5) {
            RunTrainSim();
        } else {
            barracksSelection = BarracksScreen();
            bCampaignStartupMode = 0;
            if (barracksSelection == 6)
                return 0;
            if (barracksSelection == 7)
                launchMission++;
        }
        PumpWindowMessages();
    } while (launchMission == 0);

    bKeyEventQueueEnabled = 1;
    nDebriefingPersonality = *(short *)(
        pMissionCampaignData +
        (short)stCampaignState.currentSeries * 0x5a - 0x5a);
    Briefing((short)stCampaignState.currentSeries,
             (short)stCampaignState.currentMission);
    PlayScrambleHangarScene();
    stCampaignState.playerShipType =
        aMissionShips[
            nPlayerMissionShipIndex].type;
    scramble();
    flownSeries = (short)stCampaignState.currentSeries;
    flownMission = (short)stCampaignState.currentMission;
    init_mission(flownSeries, flownMission);
    LaunchPlayerShip();
    flightResult = RunSpaceFlight(-1);

    switch (flightResult) {
    case 1:
        free_cockpit();
        ShowCarrierLaunchSequence(
            (signed char)nPlayerCollisionObject);
        nArcadeState = 0;
        nPlayerCollisionObject = -1;
        free_3Space();
        flightResult = calculate_damage_level();
        landing((signed char)flightResult);
        break;
    case 2:
        ejection_sequence();
        check_stranded();
        if (nArcadeState == 3)
            stranded_sequence();
        free_3Space();
        if (nArcadeState == 3)
            return 0;
        nArcadeState = 0;
        bPlayerEjectedThisMission = 1;
        stCampaignState.promotionScore = MaxShort(
            0, (short)(stCampaignState.promotionScore - 1));
        stCampaignState.elapsedDate.year++;
        if (stCampaignState.elapsedDate.year == 1)
            nPendingMedalIndex = 3;
        bOfficeVisitPending = 1;
        break;
    case 3:
        stranded_sequence();
        free_3Space();
        return 0;
    case 4:
        death_sequence();
        free_3Space();
        funeral_sequence(1);
        bCampaignActive = 0;
        return 0;
    default:
        free_cockpit();
        free_all_slots();
        free_3Space();
        return 0;
    }

    PostMission();
    UpdateSeries();
    nextSeries = (short)stCampaignState.currentSeries;
    nextMission = (short)stCampaignState.currentMission;
    stCampaignState.currentSeries = (signed char)flownSeries;
    stCampaignState.currentMission = (signed char)flownMission;

    if (bPlayerEjectedThisMission == 0) {
        if ((unsigned short)RandomInRange(0, 5) +
                stCampaignState.promotionScore > 7) {
            stCampaignState.promotionScore = 0;
            if (nCampaignDataSet == 0) {
                bPromotionPending =
                    stCampaignState.currentPilot->rank < 3;
            } else if (nCampaignDataSet > 0) {
                bPromotionPending =
                    stCampaignState.currentPilot->rank < 4;
            } else {
                bPromotionPending = 0;
            }
            bOfficeVisitPending =
                bOfficeVisitPending != 0 ||
                bPromotionPending != 0;
        }
    }

    DeBriefing(flownSeries, flownMission);
    if (bPromotionPending != 0)
        stCampaignState.currentPilot->rank++;

    if (nextSeries == -1) {
        if (nPendingMedalIndex != -1)
            AwardCampaignMedal(nPendingMedalIndex);

        if (nPostSeriesSequence == -1) {
            flightResult = 0;
        } else if (nPostSeriesSequence == 0x40) {
            ShowCampaignVictorySequence();
            flightResult = 1;
        } else if (nPostSeriesSequence == 0x41) {
            ShowTigerClawEscapeScene();
            flightResult = 0;
        } else {
            ShowMeanwhileTransition(nPostSeriesSequence,
                                    (short)bSeriesFailed);
            flightResult = bSeriesFailed >= 1;
        }
        ShowTheEndScreen((short)flightResult);
        bCampaignActive = 0;
        return 0;
    }

    if (nWingmanKilledThisMission != 0)
        funeral_sequence(0);
    if (bOfficeVisitPending == 1)
        Office();
    if (nPendingMedalIndex != -1) {
        AwardCampaignMedal(nPendingMedalIndex);
        nPendingMedalIndex = -1;
    }
    if (nPostSeriesSequence != -1)
        ShowMeanwhileTransition(nPostSeriesSequence,
                                (short)bSeriesFailed);
    stCampaignState.currentSeries = (signed char)nextSeries;
    stCampaignState.currentMission = (signed char)nextMission;
    MoveNewCampaign();
    AddRandomTrainSimHighScores();
    bPanRoomTransition = 1;
    return 1;
}

/* Function start: 0x40F940 */
void __stdcall free_viewport(Viewport *viewport)
{
    unsigned char *allocation;
    int i;
    int oldCount;

    nFreeViewportCalls++;
    allocation = viewport->allocation;
    if (allocation != 0) {
        i = 0;
        while (i < nViewportAllocationCount) {
            if (apViewportAllocations[i] == allocation) {
                oldCount = nViewportAllocationCount;
                nViewportAllocationCount--;
                apViewportAllocations[i] =
                    apViewportAllocations[oldCount];
                break;
            }
            i++;
        }
        if (viewport->rowOffsets != 0) {
            ReleasePacketHandle(viewport->rowOffsets);
            viewport->rowOffsets = 0;
        }
        if (nVideoMode != 0x13)
            printf("free_viewport not mcga\n");
        ReleasePacketHandle(allocation);
        viewport->pixels = 0;
        viewport->allocation = 0;
        if (stScreen.pixels == allocation)
            stScreen.pixels = 0;
        if (stScreen.allocation == allocation)
            stScreen.allocation = 0;
        if (stSceneBuffer.pixels == allocation)
            stSceneBuffer.pixels = 0;
        if (stSceneBuffer.allocation == allocation)
            stSceneBuffer.allocation = 0;
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
    GetTransformedShapeBounds(&stSpaceBuffer,
                              (short)(nScreenWidth >> 1), y,
                              pTitleShape, 1, 0, scale, 0,
                              bounds);
#ifdef WC1_SDL
    if (!Wc1SdlRecordSpaceSprite(
            &stSpaceBuffer, (short)(bounds[0] - 1), y,
            pTitleShape, 0, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBuffer, (short)(bounds[0] - 1), y,
                         pTitleShape, 0, 0, scale, 0);
#ifdef WC1_SDL
    if (!Wc1SdlRecordSpaceSprite(
            &stSpaceBuffer,
            (short)(nScreenWidth >> 1), y,
            pTitleShape, 1, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBuffer,
                         (short)(nScreenWidth >> 1), y,
                         pTitleShape, 1, 0, scale, 0);
#ifdef WC1_SDL
    if (!Wc1SdlRecordSpaceSprite(
            &stSpaceBuffer, bounds[2], y,
            pTitleShape, 2, 0, scale, 0))
#endif
        DrawSpriteScaled(&stSpaceBuffer, bounds[2], y,
                         pTitleShape, 2, 0, scale, 0);
}

/* Function start: 0x40FB10 */
void UpdateTitleMenuCursor(void)
{
    short frame;
    short mouseX;
    short mouseY;
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
    if (bEscapePressed == 0) {
        PreloadMusicTrackHook(0x17);
        SetEventManagerPump(PollJoystickButtonEvents);
        bIntroSceneResourcesActive = 0;
        init_3Space_objects(0);
        nCannedSceneMode = 2;
        pIntroFont =
            FetchDiskPacketRetrying(9, 1, 0);
        nSceneResourceBudget = 0x3e8000;
        nSceneResourceBudget = LoadPacketResourceList(
            aIntroResourceDescriptors, 0, 0x3e8000);
        aObjectTypeData[OBJECT_TYPE_DEBRIS_WING].shapeSet =
            aObjectTypeData[
                OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
        ClearInputKeyStatePreservingModifiers();
        FlushInputEvents();
        bEscapePressed = 0;

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
                    print_subtitle(&stSpaceBuffer, 0x32,
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
                                  (short)(nViewCenterY - 6));
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

            ScaleFixedVector(&aShipForwardVector[61], 0x9600,
                             &aShipVelocity[61]);
            set_up_action_sphere(17);
            bIntroSecondaryScene = 1;
            anObjectPitchRotation[0] = 0;
            anObjectYawRotation[0] = 0;
            anObjectRollRotation[0] = 0;
            start_hazard_field(0);

            credit = 0;
            for (; credit < nIntroCreditCount; credit++) {
                frame = 0;
                do {
                    Update_3Space();
                    if (Draw_3Space_Frame() != 0) {
                        print_subtitle(&stSpaceBuffer, 0x32,
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

    bEscapePressed = 0;
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
            aTitleMenuRegions[menuIndex].frame = -1;
        } else {
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
        }
        menuIndex++;
    } while (menuIndex < 4);

    ClearViewport(&stScreen, cBlackColour);
    menuIndex = 0;
    do {
        if (menuOptions[menuIndex] != -1) {
            if (menuOptions[menuIndex] < 3) {
                DrawSpriteDefault(&stScreen,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    menuShape, menuOptions[menuIndex]);
            } else {
                DrawSpriteDefault(&stScreen,
                    aTitleMenuRegions[menuIndex].left,
                    aTitleMenuRegions[menuIndex].top,
                    alternateMenuShape, 0);
            }
        }
        menuIndex++;
    } while (menuIndex < 4);
    DIBslam();
    DIBslamReal();

    stMouseCursorState.viewport = &stScreen;
    SetEventManagerPump(PollMenuInputDevices);
    nMenuInputRepeatDelay = 6;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    bInputMode = 1;
    bKeyEventQueueEnabled = 0;
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
                    event.x, event.y, aTitleMenuRegions);
            if (selectedIndex < 0 || selectedIndex > 3)
                state = 0;
            else
                state = (signed char)(menuOptions[selectedIndex] + 1);
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
    FadeViewportPaletteToColour(&stScreen, cBlackColour, 1);
    ClearViewport(&stScreen, cBlackColour);
    DIBslam();
    DIBslamReal();
    RestoreGamePalette();
    return state - 1;
}
