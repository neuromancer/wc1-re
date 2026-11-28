/* Super Wing Commander's flight (swc_flight.c). */
#ifndef SDL_PORT_SWC_FLIGHT_H
#define SDL_PORT_SWC_FLIGHT_H

#ifdef __cplusplus
extern "C" {
#endif

int SdlRunSwcMission(const char *missionPath, int cockpitless, int fullscreen,
                     int credits);
int SdlSwcFlightActive(void);
void SdlUpdateSwcFlightCamera(void);
int SdlSwcVisitCinema(int view, int obj, short frames);
extern int nSwcCinemaResult; /* SDL presentation: 0 complete, 1 quit, -1 error. */
/* Mac loadout quantities live beside WC1's unchanged seven-byte slots. */
extern const unsigned char aSwcInitialWeaponAmmo[22][10];
extern unsigned char aSwcWeaponAmmo[10][10];

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_SWC_FLIGHT_H */
