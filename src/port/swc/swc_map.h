/* Shared SDL presentation of CODE_14 BuildMap's two Mac layouts. */
#ifndef SDL_SWC_MAP_H
#define SDL_SWC_MAP_H

typedef struct SwcNavEntry {
    FixedVector position;
    SDL_Point marker;
    SDL_Point label;
    char text[32];
    int frame;
    int color;
    int objective;
} SwcNavEntry;

typedef struct SwcNavMap {
    SwcNavEntry entries[ACTIVE_MISSION_NAV_POINT_COUNT * 10 + MISSION_OBJECTIVE_COUNT + 1];
    int count;
    int minX;
    int maxX;
    int minZ;
    int maxZ;
} SwcNavMap;

void SdlBuildSwcMap(SwcNavMap *map, const SwcTextFont *font, int navigation);

#endif
