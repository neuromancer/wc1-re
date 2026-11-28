/* main.c */
short LogMemoryUsage(void);                                               /* 0x004272F0 */
unsigned int ShowMemoryStatusDebug(void);                                 /* 0x004273C0 */
void exit_squadron(const char *msg);                                    /* 0x00427370 */
unsigned int GetJoystickButtonEdge(unsigned int a, short b);                    /* 0x004274C0 */
#ifdef SDL_PORT
int GameMain(short argc, char **argv);
#else
int main(short argc, char **argv);                                      /* 0x004274E0 */
#endif
unsigned int GetScreenUpdateFlag(void);                                         /* 0x004279D0 */
unsigned int initialize_view_buffer(void);                                /* 0x00427A00 */
unsigned int dump_buffer_to_screen(void);                                 /* 0x00427A40 */
unsigned int clear_view_buffer(void);                                     /* 0x00427B00 */
unsigned int InitializeConversationViewport(void);                     /* 0x00427B20 */
unsigned int ResetScreenClipToFullHeight(void);                                         /* 0x00427BA0 */
unsigned int InitializeConversationText(void);                         /* 0x00427BC0 */
unsigned int RefreshMemoryStatusOverlay(void);                                         /* 0x00427C30 */
unsigned int Update_3Space(void);                                       /* 0x00427C50 */
unsigned int UpdateSpacePaletteFade(void);                              /* 0x00427CD0 */
unsigned int TriggerPlayerHitPaletteFlash(void);                      /* 0x00427C80 */
unsigned int FadeFlightPaletteEntry(short *entry);                      /* 0x00427CA0 */
unsigned int house_keep(void);                                          /* 0x00427D40 */
void init_player_input(void);                                           /* 0x00427DF0 */
void get_player_input(void);                                           /* 0x00427E40 */
int process_player_input(void);                                        /* 0x00427F20 */
unsigned int fire_players_lasers(void);                                /* 0x00428480 */
unsigned int players_flight_dynamics(void);                            /* 0x004284D0 */
unsigned int player_input(void);                                       /* 0x004285D0 */
unsigned int SelectNextExternalViewObject(void);                       /* 0x00428C90 */
unsigned int SelectPreviousExternalViewObject(void);                   /* 0x00428CD0 */
unsigned int HandleFleetOverviewInput(void);                           /* 0x00428D10 */
short MeasureMessageWidth(const char *text);                                /* 0x00428E70 */
void WaitForKeyAcknowledge(int mode);                                     /* 0x00428EA0 */
void ShowModalMessage(const char *format, ...);                           /* 0x00428F20 */
void ReportOutOfMemoryAndExit(const char *resource);                    /* 0x00428F80 */
void ShowOnScreenMessage(int flags, short duration,
                         const char *format, ...);                        /* 0x00428FA0 */
void ShowGamePausedBanner(short showBanner);                           /* 0x004290A0 */
void ShowVersionBanner(void);                                           /* 0x004290D0 */
void SetMessageDisplaySpeed(void);                                     /* 0x004290F0 */
void ReportFramesSkipped(short adjustment);                            /* 0x00429120 */
int HandleSpaceFlightControls(void);                                   /* 0x00429160 */
unsigned int Draw_3Space_Frame(void);                                  /* 0x00429DD0 */
void GetArcadeBonus(void);                                         /* 0x00429E30 */
void FigureArcadeTime(void);                                       /* 0x00429E70 */
void DrawArcadeScorePanel(short x, short y);                            /* 0x00429E90 */
void UpdateArcadeScoreDisplay(void);                                   /* 0x00429EE0 */
unsigned int RenderSpaceViewFrame(void);                               /* 0x00429FC0 */
unsigned int RefreshCockpitStatus(void);                                /* 0x0042A0C0 */
short GetShipDistanceToNavPoint(short ship, MissionNavPoint *navPoint); /* 0x0042A0E0 */
short FindNearestNavPoint(short ship);                               /* 0x0042A120 */
unsigned int ReleaseStaleNavTarget(void);                                     /* 0x0042A170 */
int RunSpaceFlight(short entryNavPoint);                               /* 0x0042A190 */
int calculate_damage_level(void);                                     /* 0x0042A520 */
void UpdateTrainSimMenuCursor(void);                                   /* 0x0042A610 */
void ResetMouseCursorFrame(void);                                  /* 0x0042A670 */
void UpdateRoomMenuCursor(void);                                   /* 0x0042A680 */
void __stdcall FadeViewportPaletteToColour(Viewport *viewport,
                                           unsigned short colour,
                                           short enabled);             /* 0x0042A700 */
