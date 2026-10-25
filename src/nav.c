/*
 * Navigation map, objective labels, and the in-flight computer.
 * Placement and boundary evidence are recorded in docs/ORDER.md.
 */
#define WC1_NAV_SOURCE
#include "wc1.h"
#ifndef SDL_PORT
#include <dos.h>
#endif
#include <float.h>

short nNavMapCoordinateScaling = 0; /* 0x00468660 */
short nNavMapScale = 1; /* 0x00468664 */
/* 0x00468668 */
static NavMapObjectiveStyle aNavMapObjectiveStyles[10] = {
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
static unsigned char *pNavMapShape = 0; /* 0x00468708 */
static short nNavMapLabelCount = 0; /* 0x0046870c */
static short nNavMapReservedAreaCount = 0; /* 0x00468710 */
static char szCampaignSector[16] = "Vega XR-231.3"; /* 0x00468718 */
#ifndef SDL_PORT
static
#endif
/* 0x00468728 */
char *apszShipMissionTypeNames[11] = {
    "Patrol",
    "Escort",
    "Strike",
    "Defend",
    "Wingman",
    "Flee",
    "Goto Warp",
    "err",
    "err",
    "Rendezvous",
    "err"
};
int bInflightComputerActive = 0; /* 0x00468754 */

/* 0x00475e80 */
static NavMapLabel aNavMapLabels[20];
/* 0x00475f48 */
static ShortRect aNavMapExclusionRects[21];

short nNavMapCenterPositionY; /* 0x005a817c */
TextContext stMapNavigationLabelTextContext; /* 0x005a8180 */
short nCurrentActiveInputDevice; /* 0x005a819c */
int nJoystickVertDeadZoneValue; /* 0x005a81a0 */
int nJoystickHorizontalDeadZone; /* 0x005a81a4 */
int nJoystickUpScale; /* 0x005a81a8 */
int nCurrentJoystickLeftScale; /* 0x005a81ac */
int nStickMaxX; /* 0x005a81b0 */
int nStickMaxY; /* 0x005a81b4 */
int nStickMinimumPositionX; /* 0x005a81b8 */
int nStickMinimumPositionY; /* 0x005a81bc */
int nCurrentJoystickRawCoordX; /* 0x005a81c0 */
int nCurrentJoystickRawCoordY; /* 0x005a81c4 */
int nJoystickVerticalRange; /* 0x005a81c8 */
int nJoystickHorizontalRangeValue; /* 0x005a81cc */
int nJoystickRightScale; /* 0x005a81d0 */
int nJoystickDownScaleValue; /* 0x005a81d4 */
int nJoystickCentreY; /* 0x005a81d8 */
int nJoystickCentreX; /* 0x005a81dc */
int nCurrentJoystickFailureValue; /* 0x005a81e0 */
InputDeviceSample aInputDevSamples[2]; /* 0x005a81f0 */

/* Function start: 0x40D090 */
short NavMapPointInsideReservedArea(short area, short x, short y)
{
    return aNavMapExclusionRects[area].left <= x &&
           x <= aNavMapExclusionRects[area].right &&
           aNavMapExclusionRects[area].top <= y &&
           y <= aNavMapExclusionRects[area].bottom;
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
    short area;

    area = nNavMapReservedAreaCount;
    aNavMapExclusionRects[area].left = x;
    aNavMapExclusionRects[area].top = y;
    aNavMapExclusionRects[area].right = (short)(x + width);
    aNavMapExclusionRects[area].bottom = (short)(y + height);
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

    width = (short)(strlen(text) * 4 + 2);
    aNavMapLabels[nNavMapLabelCount].colour = colour;
    aNavMapLabels[nNavMapLabelCount].text = text;
    offset = -1;
    do {
        offset++;
        if (TryPlaceNavMapLabel((short)(x + offset + 4), y,
                                width, 0) != 0)
            break;
        if (TryPlaceNavMapLabel((short)(x - width / 2),
                                (short)(y + offset + 5),
                                width, offset == 12) != 0)
            break;
        if (TryPlaceNavMapLabel((short)(x - offset - width - 3), y,
                                width, offset == 12) != 0)
            break;
        if (TryPlaceNavMapLabel(x, (short)(y + offset + 5),
                                width, offset == 12) != 0)
            break;
        if (TryPlaceNavMapLabel((short)(x - width / 2),
                                (short)(y - offset - 9),
                                width, offset == 12) != 0)
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
        if (aMissionObjectivesArray[previous].index == missionShip)
            break;
        previous++;
    }
    if (previous < objective)
        return;
    else
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

/* Function start: 0x40D500 */
/* Nothing in the shipped executable calls this. Returns the index of the
   last-placed nav map label containing the point, or -1. */
short FindNavMapLabelAtPoint(short x, short y)
{
    short label;

    label = nNavMapLabelCount;
    while (--label > -1) {
        if (IsPointInNavMapLabel(label, x, y))
            break;
    }
    return label;
}

/* Function start: 0x40D540 */
void DrawNavMapLabels(void)
{
    short label;

    label = 0;
    while (label < (short)nNavMapLabelCount) {
        DrawFormattedText("%X%Y%F%s",
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
        DrawViewportEllipse(stMapNavigationLabelTextContext.viewport,
                            x, y, size, (short)((size * 7) / 8),
                            (short)colour);
    else
        DrawViewportEllipseShadow(
            stMapNavigationLabelTextContext.viewport,
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
        DrawViewportPixel(stMapNavigationLabelTextContext.viewport,
                          x, y, colour);
        DrawViewportPixel(stMapNavigationLabelTextContext.viewport,
                          (short)(x + 1), y, colour);
        DrawViewportPixel(stMapNavigationLabelTextContext.viewport,
                          x, (short)(y + 1), colour);
        DrawViewportPixel(stMapNavigationLabelTextContext.viewport,
                          (short)(x + 1),
                          (short)(y + 1), colour);
    } else {
        DrawNavSquareOutline(stMapNavigationLabelTextContext.viewport,
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
    DrawNavTriangleOutline(stMapNavigationLabelTextContext.viewport,
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
    DrawViewportLine(stMapNavigationLabelTextContext.viewport,
                     (short)(x - size), (short)(y - size),
                     (short)(x + size), (short)(y + size),
                     (short)colour);
    DrawViewportLine(stMapNavigationLabelTextContext.viewport,
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

    nav_getxy(&x, &y, aShipPositionsArray[0].x,
              aShipPositionsArray[0].z);
    x = (short)(x + stMapNavigationLabelTextContext.viewport->left);
    y = (short)(y + stMapNavigationLabelTextContext.viewport->top);
    DrawViewportPixel(stMapNavigationLabelTextContext.viewport,
                      x, y, colour);
    DrawNavSquareMarker(x, y, 0, 0, colour, reserve);
}

/* Function start: 0x40DA00 */
void BuildMap(short showPlayer)
{
    MissionNavPoint *navPoint;
    MissionShipRecord *missionShip;
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
    stMapNavigationLabelTextContext.viewport = &stSceneBuffer;
    stMapNavigationLabelTextContext.text = szDefaultTxtBuf;
    InitializeTextContextFromFont(&stMapNavigationLabelTextContext,
                                  2, cPrimaryTextColour, -1);
    stMapNavigationLabelTextContext.alignment = 0;
    SetTextContext(&stMapNavigationLabelTextContext);
    navPoint = aMissionNavPoints;
    ResetNavMapLabels();
    ResetNavMapReservedAreas();
    SetScale();

    for (; navPoint->type != 0;
         navPoint++) {
        for (slot = 0; slot < 10; slot++) {
            missionShipIndex = navPoint->missionShips[slot];
            if (missionShipIndex != -1) {
                missionShip = &aMissionShips[missionShipIndex];
                switch (missionShip->type) {
                case OBJECT_TYPE_ASTEROID_FIELD:
                    DrawNavHazardMarker(navPoint->position,
                                        missionShip->position,
                                        missionShip->speed,
                                        cAsteroidColour, cAsteroidColour,
                                        "Asteroids");
                    break;
                case OBJECT_TYPE_MINE_FIELD:
                    DrawNavHazardMarker(navPoint->position,
                                        missionShip->position,
                                        missionShip->speed,
                                        cRedColour, cRedColour,
                                        "Mines");
                    break;
                }
            }
        }
    }

    for (objectiveIndex = 0;
         objectiveIndex < (short)cMissionObjectiveCountValue;
         objectiveIndex++) {
        if (mobile_objective(objectiveIndex) == 0 ||
            (aMissionShips[
                 (signed char)aMissionObjectivesArray[objectiveIndex].index]
                     .state == 0 &&
             achieved(objectiveIndex) == 0)) {
            ScaleNavMapCoordinates(&x, &y,
                                   aMissionObjectivesArray[objectiveIndex].mapX,
                                   aMissionObjectivesArray[objectiveIndex].mapY);
            if (hidden_objective(objectiveIndex) == 0) {
                style = &aNavMapObjectiveStyles[
                    aMissionObjectivesArray[objectiveIndex].type];
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
                awMapNavObjectiveLabelIndex[objectiveIndex] =
                    nNavMapLabelCount;
                AddUniqueObjectiveNavLabel(
                    x, y, labelColour, objective_name(objectiveIndex),
                    objectiveIndex,
                    (short)aMissionObjectivesArray[objectiveIndex].index);
            }
        }
    }
    if (showPlayer != 0) {
        DrawNavPlayerMarker(cViewportClearColour, 1);
        nav_getxy(&x, &y, aShipPositionsArray[0].x,
                  aShipPositionsArray[0].z);
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

    SetTextContext(&stCurrentNavMapTxtCtx);
    sprintf(time, "%02d",
            (int)((signed char *)pElapsedCampaignDatePtr)[0]);
    DrawFormattedText(
        "%X%YStandard time %s",
        (int)(short)(stCurrentNavMapTxtCtx.viewport->left + 150),
        (int)(short)(stCurrentNavMapTxtCtx.viewport->top + 140),
        time);
    cursorX = stCurrentNavMapTxtCtx.cursorX;
    if (showColon == 0) {
        DrawFormattedText(" ");
    } else {
        stCurrentNavMapTxtCtx.cursorX =
            (short)(stCurrentNavMapTxtCtx.cursorX + 4);
    }
    sprintf(time, "%02d  ",
            (int)((signed char *)pElapsedCampaignDatePtr)[1]);
    DrawFormattedText(time);
    stCurrentNavMapTxtCtx.cursorX = cursorX;
    if (showColon != 0)
        DrawFormattedText(":");
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
    if ((short)cCurrentNavPtIdx == navPoint) {
        DrawNavTextLine(0, (unsigned short)cYellowColour, text);
        DrawNavTextLine(0, (unsigned short)cYellowColour,
                        "\n");
    }
}

/* Function start: 0x40DEE0 */
void DrawNavMapLegend(void)
{
    short objective;

    objective = 0;
    while (objective < (short)cMissionObjectiveCountValue) {
        if (visited(objective) == 0 && hidden_objective(objective) == 0)
            break;
        objective++;
    }
    SetTextCursor((unsigned short)stSceneBuffer.left, 120);
    DrawSelectedNavLegendEntry("MISSION FLIGHT PATH",
                               objective);
    DrawSelectedNavLegendEntry(
        "HOME BASE",
        (short)((short)cMissionObjectiveCountValue - 1));
}

/* Function start: 0x40DF50 */
char *nav_note(short objective)
{
    char *note;

    note = aMissionObjectivesArray[objective].name;
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
    stCurrentNavMapTxtCtx.viewport = &stSceneBuffer;
    stCurrentNavMapTxtCtx.text = szDefaultTxtBuf;
    InitializeTextContextFromFont(&stCurrentNavMapTxtCtx,
                                  1, cPrimaryTextColour, cBlackColour);
    stCurrentNavMapTxtCtx.alignment = 0;
    stCurrentNavMapTxtCtx.cursorX = 0;
    stCurrentNavMapTxtCtx.cursorY = 0;
    SetTextContext(&stCurrentNavMapTxtCtx);
    DrawNavTextLine(0, cDefaultTextColour,
                    "\n");
    DrawNavTextLine(2, cDefaultTextColour,
                    "%s\n\n", title);
    DrawNavTextLine(0, cDefaultTextColour,
                    "Sector: %s\n",
                    szCampaignSector);
    DrawNavTextLine(0, cDefaultTextColour,
                    "System: %s\n\n",
                    abSeriesAuxData);
    DrawNavTextLine(2, cDefaultTextColour,
                    "* %s *\n",
                    abMissionAuxData);
    playerMissionType = aMissionShips[
        stCurrentMissionHeader.playerMissionShip].missionType;
    DrawNavTextLine(2, cDefaultTextColour,
                    "* %s *\n",
                    apszShipMissionTypeNames[
                        playerMissionType]);
    DrawNavTextLine(2, cDefaultTextColour,
                    "\nNotes\n");
    DrawNavTextLine(0, cDefaultTextColour,
                    "%s\n",
                    nav_note((short)cCurrentObjective));
    if (showFlightData != 0)
        DrawNavMapLegend();
    BuildMap(showFlightData);
    if (showFlightData != 0) {
        SetScreenClipRect(0, 0, 259, 155);
        SetTextContext(&stCurrentNavMapTxtCtx);
        DrawNavTextLine(0, cDefaultTextColour,
                        "%X%Y                         Location: %d.%d.%d",
                        8, 142,
                        aShipPositionsArray[0].x,
                        aShipPositionsArray[0].y,
                        aShipPositionsArray[0].z);
    }
    CopyViewportContents(&stSceneBuffer, &stScreenView);
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
        ReportOutOfMemoryAndExit("VSCREEN");
    objective = 0;
    while (objective < (short)cMissionObjectiveCountValue) {
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
    stScreenView.top = 4;
    DrawNavLocationReadout("Briefing Nav Map", 0);
    free_viewport(&stSceneBuffer);
    ReleasePacketHandle(pNavMapShape);
    ReleaseTextFont(2);
    ReleaseTextFont(1);
    stSceneBuffer = savedViewport;
    AllocateViewport(&stSceneBuffer, (short)cBlackColour, 0);
}

/* Function start: 0x40E2B0 */
short SelectNavObjectiveAtPoint(short mouseX, short mouseY)
{
    short x;
    short pathIndex;
    short y;
    short mapY;
    short mapX;
    short oldNavPoint;
    short selected;

    oldNavPoint = (short)cCurrentNavPtIdx;
    x = (short)(*(volatile short *)&mouseX - 30);
    pathIndex = 0;
    y = (short)(*(volatile short *)&mouseY - 22);
    selected = 0;
    while (abFlightPathList[pathIndex] != -1) {
        if (hidden_objective((short)abFlightPathList[pathIndex]) == 0) {
            ScaleNavMapCoordinates(
                &mapX, &mapY,
                aMissionObjectivesArray[abFlightPathList[pathIndex]].mapX,
                aMissionObjectivesArray[abFlightPathList[pathIndex]].mapY);
            if ((short)(abs((int)x - mapX) +
                        abs((int)y - mapY)) < 6 ||
                IsPointInNavMapLabel(
                    (short)awMapNavObjectiveLabelIndex[pathIndex],
                    x, y) != 0) {
                selected = 1;
                set_new_objective(pathIndex);
                if (pathIndex == oldNavPoint)
                    return selected;
            }
        }
        pathIndex++;
    }
    return selected;
}

/* Function start: 0x40E3C0 */
void CentreMouseOnCurrentNavObjective(void)
{
    short x;
    short y;
    signed char objective;

    objective = abFlightPathList[
        cCurrentNavPtIdx];
    ScaleNavMapCoordinates(
        &x, &y,
        aMissionObjectivesArray[objective].mapX,
        aMissionObjectivesArray[objective].mapY);
    x = (short)(x + 30);
    y = (short)(y + 22);
    LeaveAllocationScope();
    WarpMouseTo(x, y);
    EnterAllocationScope();
}

/* Function start: 0x40E430 */
void ShowConfedNavScan(void)
{
    SetRectBounds(&stScreenView, 30, 22, 289, 177);
    LeaveAllocationScope();
    DrawNavLocationReadout("ConFed Nav Scan", 1);
    EnterAllocationScope();
    SetRectBounds(&stScreenView, 0, 0, 319, 199);
}

/* Function start: 0x40E480 */
void InflightComputer(void)
{
    short done;
    short hasObjectives;
    volatile short savedInputMode;
    short objective;
    short displayedNavPoint;
    short eventType;
    short savedNavPoint;
    int frame;
    unsigned char *background;
    InputEventState event;
    Viewport pointerViewport;
#ifdef SDL_PORT
    MouseCursorState savedInputState;
#else
    unsigned int savedInputState[7];
#endif

    savedNavPoint = (short)cCurrentNavPtIdx;
    done = 0;
    hasObjectives = 0;
    displayedNavPoint = savedNavPoint;
    bInflightComputerActive = 1;
#ifdef SDL_PORT
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
    cScreenViewMode = -1;
    background = FetchDiskPacketRetrying(8, 1, 0);
    ClearViewport(&stScreenView, cBlackColour);
    DrawSpriteDefault(&stScreenView, 0, 0, background, 0);
    ReleasePacketHandle(background);

    objective = 0;
    BriefingMap_LoadShapes();
    ShowConfedNavScan();
    if (cMissionObjectiveCountValue > 0) {
        do {
            if (hidden_objective(objective) == 0)
                hasObjectives = 1;
            objective++;
        } while (objective <
                 (short)cMissionObjectiveCountValue);
    }

    if (hasObjectives == 0) {
        SetEventManagerPump(PollJoystickButtonEvents);
        WaitForInputKey();
        SetFrameTimerAndWait(20);
        SetEventManagerPump(get_player_input);
    } else {
        pointerViewport = stScreenView;
        SetRectBounds(&pointerViewport, 32, 24, 182, 159);
        stMouseCursorState.viewport = &pointerViewport;
        savedInputMode = (short)(signed char)bInputModeState;
        bInputModeState = 1;
        SetEventManagerPump(PollMenuInputDevices);
        EventManagerHook(ResetMouseCursorFrame);
        nMenuInputRepeatDelay = 6;
        EnterAllocationScope();
        CentreMouseOnCurrentNavObjective();

        do {
            if (displayedNavPoint !=
                (short)cCurrentNavPtIdx) {
                displayedNavPoint =
                    (short)cCurrentNavPtIdx;
                PlaySfxWaveFileByNumber(0x19, -1, 0);
                ShowConfedNavScan();
            }
            SetRectBounds(&stScreenView, 32, 24, 289, 177);
            SetMouseCursorShape(stMouseCursorState.shape, 0);
            FormatNavCoordinates(
                (unsigned char *)pElapsedCampaignDatePtr);
            stMapNavigationLabelTextContext.viewport = &stScreenView;
            frame = (int)nTickCount60Hz / 15;
            DrawNavPlayerMarker(frame % 2 != 0 ? cViewportClearColour
                                               : cDarkGreyColour, 0);
            stCurrentNavMapTxtCtx.viewport = &stScreenView;
            UpdateInflightNavText((short)((frame / 4) % 2));
            SetRectBounds(&stScreenView, 0, 0, 319, 199);

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
        } while (done == 0 && bEscapePressedFlag == 0);

        if (bEscapePressedFlag != 0) {
            bEscapePressedFlag = 0;
            cCurrentNavPtIdx =
                (signed char)savedNavPoint;
            set_new_objective(cCurrentNavPtIdx);
        }
        free_viewport(&stSceneBuffer);
        LeaveAllocationScope();
        EventManagerHook(0);
        SetEventManagerPump(get_player_input);
        bInputModeState = (unsigned char)savedInputMode;
    }

    ReleasePacketHandle(pNavMapShape);
    SetTextContext(&stDefaultTxtContext);
    PlaySfxWaveFileByNumber(0x19, -1, 0);
#ifdef SDL_PORT
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
        SetViewportRect(&stSpaceBufferViewport, 0, 0,
                        (unsigned short)(nScreenWidth - 1),
                        (unsigned short)(nScreenHeight - 1));
        initialize_view_buffer();
        force_view(0, 0);
        bCockpitlessView = 1;
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
        initialize_view_buffer();
    }
    bInflightComputerActive = 0;
}
