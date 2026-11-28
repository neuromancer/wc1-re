/* nav.c */
short NavMapPointInsideReservedArea(short area, short x, short y);    /* 0x0040D090 */
short NavMapLabelFits(short x, short y, short width, short height);   /* 0x0040D0E0 */
short NavMapLabelPositionAvailable(short x, short y,
                                   short width, short height);        /* 0x0040D120 */
void ResetNavMapReservedAreas(void);                                  /* 0x0040D1D0 */
void ReserveNavMapArea(short x, short y, short width, short height);  /* 0x0040D1E0 */
void ResetNavMapLabels(void);                                         /* 0x0040D240 */
short TryPlaceNavMapLabel(short x, short y, short width,
                          short force);                               /* 0x0040D250 */
void PlaceNavMapLabel(short x, short y, unsigned short colour,
                      const char *text);                             /* 0x0040D2C0 */
void AddUniqueObjectiveNavLabel(short x, short y,
                                unsigned short colour, const char *text,
                                short objective, short missionShip);  /* 0x0040D410 */
short IsPointInNavMapLabel(short label, short x, short y);            /* 0x0040D490 */
void DrawNavMapLabels(void);                                          /* 0x0040D540 */
void DrawNavRectangleMarker(short x, short y, short size, short shadow,
                            unsigned short colour, short reserve);    /* 0x0040D5A0 */
void DrawNavSquareOutline(Viewport *viewport, short x, short y,
                          unsigned short size, signed char colour);            /* 0x0040D640 */
void DrawNavSquareMarker(short x, short y, short size,
                         short shadow, short colour,
                         short reserve);                              /* 0x0040D680 */
void DrawNavTriangleOutline(Viewport *viewport, short x, short y,
                            short size, signed char colour);          /* 0x0040D740 */
void DrawNavTriangleMarker(short x, short y, short size,
                           short shadow, unsigned short colour,
                           short reserve);                            /* 0x0040D7D0 */
void DrawNavCrossMarker(short x, short y, short size,
                        short shadow, unsigned short colour,
                        short reserve);                               /* 0x0040D830 */
void SetScreenClipRect(unsigned short a, unsigned short b, unsigned short c, unsigned short d);/* 0x0040D8C0 */
void DrawNavHazardMarker(FixedVector navPosition, FixedVector offset,
                         short size, unsigned short markerColour,
                         unsigned short textColour,
                         const char *text);                           /* 0x0040D8F0 */
void DrawNavPlayerMarker(short colour, short reserve);                /* 0x0040D980 */
void BuildMap(short showPlayer);                                      /* 0x0040DA00 */
void UpdateInflightNavText(short showColon);                          /* 0x0040DDA0 */
void FormatNavCoordinates(unsigned char *out);                                 /* 0x0040DE70 */
void DrawSelectedNavLegendEntry(const char *text, short navPoint);    /* 0x0040DEA0 */
void DrawNavMapLegend(void);                                          /* 0x0040DEE0 */
char *nav_note(short objective);                                      /* 0x0040DF50 */
void DrawNavLocationReadout(const char *title, short showFlightData); /* 0x0040DF70 */
void BriefingMap_LoadShapes(void);                                    /* 0x0040E190 */
void BriefingMap_DisplayMap(void);                                      /* 0x0040E210 */
short SelectNavObjectiveAtPoint(short mouseX, short mouseY);          /* 0x0040E2B0 */
void CentreMouseOnCurrentNavObjective(void);                         /* 0x0040E3C0 */
void ShowConfedNavScan(void);                                        /* 0x0040E430 */
void InflightComputer(void);                                         /* 0x0040E480 */
