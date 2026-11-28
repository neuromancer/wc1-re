/* Super Wing Commander's cockpit (swc_cockpit.c). */
#ifndef SDL_PORT_SWC_COCKPIT_H
#define SDL_PORT_SWC_COCKPIT_H

#ifdef __cplusplus
extern "C" {
#endif

extern char szObjectiveStatusMessage[28];
short SdlSwcMessageShowing(void);
void SdlSwcTransmit(short object, short message);
void SdlSwcVocalizeMessage(int command);
void SdlSwcCommunicate(int choice);
short SdlSwcAutopilotValid(short showReason);
void SdlSwcFlagReached(short objective, short reached);
extern unsigned char bSwcWaitingForEscort; /* SWC DATA/A5+0x46ec */
short SdlSwcComponentMalfunction(char component);
void SdlSwcCockpitDamage(short region);
extern int anSwcWeaponNoticeTicks[2]; /* SWC DATA/A5+0x4848/+0x484c. */
extern int nSwcSpaceGlareTicks; /* SWC DATA/A5+0x8c62. */
extern int bSwcExplosionGlare; /* SWC DATA/A5+0x8c6a. */
void SdlDrawSwcCockpitReadout(signed char slot, const char *text);
struct CockpitScannerGeometry;
extern struct CockpitScannerGeometry stSwcCockpitScanner;

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_SWC_COCKPIT_H */
