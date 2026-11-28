/* winmain.c */
void SaveGamePalette(void);                                             /* 0x00401000 */
void RestoreGamePalette(void);                                          /* 0x00401020 */
short easy2see(short obj);                                            /* 0x00401040 */
void make_shard(short asteroid, FixedVector direction);               /* 0x004010C0 */
void remove_hazard(signed char hazard);                               /* 0x004011D0 */
void remove_all_hazards(void);                                        /* 0x00401210 */
short difficulty(void);                                               /* 0x00401250 */
short asteroid_velocity(void);                                       /* 0x00401270 */
void skew_randomly(short obj, short allowReverse);                    /* 0x00401290 */
void align(short *value, short quantum);                              /* 0x00401390 */
void init_hazard(short obj, FixedVector position, short moving);      /* 0x004013B0 */
short near_field(const HazardField *field, const FixedVector *point); /* 0x00401680 */
short within_field(const HazardField *field, const FixedVector *point); /* 0x004016A0 */
short try_far_spot(FixedVector *spot, short *moving);                 /* 0x004016C0 */
short rear_sphere(void);                                              /* 0x00401870 */
short ok_hazard_spot(short obj);                                      /* 0x00401890 */
short make_hazard(void);                                              /* 0x004018D0 */
void extra_hazard(short obj);                                        /* 0x00401930 */
void approach(short obj);                                            /* 0x00401950 */
void manage_hazard(short obj, short slot);                            /* 0x004019E0 */
void match_ship_to_eye(void);                                        /* 0x00401A60 */
void update_hazards(void);                                           /* 0x00401B30 */
void start_hazard_field(short region);                               /* 0x00401BC0 */
void add_hazard_field(enum ObjectType type, FixedVector center,
                      short radius, short density);                   /* 0x00401C00 */
void check_hazards(void);                                            /* 0x00401C60 */
void __stdcall WarpMouseTo(short x, short y);                        /* 0x00401CE0 */
void CheckLauncherAndConfig(void);                                   /* 0x00401D10 */
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous,
                   LPSTR commandLine, int showCommand);               /* 0x00401E30 */
void ShutdownGameWindow(void);                                       /* 0x00402070 */
void ShowNoticeMessageBox(const char *text);                            /* 0x004020E0 */
unsigned int AbortToDesktop(void);                                    /* 0x00402110 */
int CreateMainWindow(HINSTANCE instance, HINSTANCE previous,
                     int showCommand);                                /* 0x00402180 */
unsigned int PumpWindowMessages(void);                                /* 0x00402320 */
unsigned int GetF1KeyLatch(void);                                     /* 0x00402520 */
LRESULT CALLBACK MainWindowProc(HWND window, UINT message,
                                WPARAM wParam, LPARAM lParam);         /* 0x00402530 */
int __stdcall GetJoystickPosition(int *x, int *y,
                                  unsigned int *buttons, short joystick,
                                  unsigned short fallback);            /* 0x00402A20 */
short GetJoystickButtons(void);                                                   /* 0x00402AC0 */
void GetJoystickDevCaps(short joystick,
                        unsigned short *xMin, unsigned short *xMax,
                        unsigned short *yMin, unsigned short *yMax); /* 0x00402AE0 */
HINSTANCE GetApplicationInstance(void);                               /* 0x00402B80 */
HWND GetMainWindowHandle(void);                                       /* 0x00402B90 */
HDC GetMainWindowDeviceContext(void);                                 /* 0x00402BA0 */
void *AllocateGuardedMemory(unsigned int size);                        /* 0x00402BB0 */
void ReportHeapGuardCorruption(void *memory, int count, int overrun);     /* 0x00402CA0 */
void CheckAllGuardedAllocations(void);                                /* 0x00402D40 */
void FreeGuardedAllocation(void *memory);                                /* 0x00402DB0 */
