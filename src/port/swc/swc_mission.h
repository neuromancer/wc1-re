/* Super Wing Commander's mission data (swc_mission.c). */
#ifndef SDL_PORT_SWC_MISSION_H
#define SDL_PORT_SWC_MISSION_H

#ifdef __cplusplus
extern "C" {
#endif

int SdlFindSwcMissionData(char *path, unsigned long capacity);
int SdlFindSwcResources(const char *directory, char *path, unsigned long capacity);
int SdlLoadSwcMissionData(const char *path, short series, short mission);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_SWC_MISSION_H */
