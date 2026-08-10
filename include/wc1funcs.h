/*
 *  Prototypes for every reimplemented function, in address order.
 *
 *  Address order is link order, so this list doubles as the reconstruction
 *  inventory: see src/map for the same list with the original addresses.
 */
#ifndef WC1_FUNCS_H
#define WC1_FUNCS_H

void RestoreGamePalette(void);                                          /* 0x00401020 */
void remove_hazard(signed char hazard);                               /* 0x004011D0 */
void remove_all_hazards(void);                                        /* 0x00401210 */
short difficulty(void);                                               /* 0x00401250 */
short asteroid_velocity(void);                                       /* 0x00401270 */
void skew_randomly(short obj, short allowReverse);                    /* 0x00401290 */
short align(short *value, short quantum);                             /* 0x00401390 */
void init_hazard(short obj, FixedVector position, short moving);      /* 0x004013B0 */
int near_field(const HazardField *field, const FixedVector *point);   /* 0x00401680 */
int within_field(const HazardField *field, const FixedVector *point); /* 0x004016A0 */
int try_far_spot(FixedVector *spot, short *moving);                   /* 0x004016C0 */
short rear_sphere(void);                                              /* 0x00401870 */
int ok_hazard_spot(short obj);                                        /* 0x00401890 */
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
void WarpMouseTo(short x, short y);                                  /* 0x00401CE0 */
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
int __stdcall GetJoystickPosition(unsigned int *x, unsigned int *y,
                                  unsigned int *buttons, short joystick,
                                  unsigned int fallback);              /* 0x00402A20 */
short GetJoystickButtons(void);                                                   /* 0x00402AC0 */
void GetJoystickDevCaps(short joystick, short *xMin, short *xMax,
                        short *yMin, short *yMax);                    /* 0x00402AE0 */
HINSTANCE GetApplicationInstance(void);                               /* 0x00402B80 */
void *AllocateGuardedMemory(unsigned int size);                        /* 0x00402BB0 */
void ReportHeapGuardCorruption(void *memory, int count, int overrun);     /* 0x00402CA0 */
void CheckAllGuardedAllocations(void);                                /* 0x00402D40 */
void FreeGuardedAllocation(void *memory);                                /* 0x00402DB0 */
void SetMousePosition(int x, int y);                                    /* 0x00402E80 */
unsigned int PollKeyboardState(void);                                 /* 0x00402EA0 */
int GetShiftKeyState(void);                                             /* 0x00403060 */
int GetControlKeyState(void);                                           /* 0x00403070 */
unsigned int GetKeyboardModifiers(void);                                     /* 0x00403080 */
unsigned int GetGameClockTicks(void);                                   /* 0x00403090 */
void InitGameClockEpoch(void);                                    /* 0x004030B0 */
void WriteDebugString(const char *s);                                   /* 0x004030D0 */
void SetMousePositionDuplicate(int x, int y);                                 /* 0x004030E0 */
FontWorkspace **AllocateFontWorkspace(short fontIndex);                /* 0x00403100 */
char *LocateStreamsDirOnDisc(void);                                  /* 0x004031C0 */
char FindCdRomDriveByVolumeLabel(const char *label,
                                 const char *directory);              /* 0x00403290 */
int SetCurrentDirOnDrive(char drive, const char *directory);          /* 0x004033E0 */
int PromptInsertCorrectCd(void);                                      /* 0x00403450 */
short OpenDataFileOrDie(const char *path);                                     /* 0x004034D0 */
void __stdcall CloseDataFile(unsigned int fd);                             /* 0x00403500 */
short __stdcall WriteDataFileAtOffset(unsigned int fd, int offset,
                                      unsigned int length,
                                      const void *data);                    /* 0x00403520 */
short __stdcall CreateDataFile(const char *path);                          /* 0x004035C0 */
int __stdcall ReadDataFileAtOffset(unsigned int fd, int offset,
                                   unsigned int length, void *data);        /* 0x00403610 */
int __stdcall SeekDataFile(unsigned int fd, int offset,
                           unsigned int origin);                            /* 0x004036B0 */
int MeasureScaledIntroTextWidth(const char *text, short scale);            /* 0x00403710 */
int DrawCenteredScaledIntroText(const char *text, short centreX,
                                short baselineY, short scale);             /* 0x004037A0 */
short GetLineLength(const char *text);                                  /* 0x00403890 */
void print_subtitle(Viewport *viewport, short colour,
                    const char *text);                                  /* 0x00403920 */
int advance_canned_sequence(short obj);                                 /* 0x00403A80 */
void update_canned_sequence(short obj);                                 /* 0x00403B70 */
void __stdcall SplitGameClockTicks(unsigned char *parts);                  /* 0x00403C40 */
void MonoDebug_install(void);                                           /* 0x00403C90 */
void MonoDebug_remove(void);                                            /* 0x00403D60 */
void SoundDebugPrintf(const char *fmt, ...);                          /* 0x00403DB0 */
void MonoDebug_print(const char *text);                                  /* 0x00403DE0 */
void ReadPerformanceCounter(LARGE_INTEGER *p);                           /* 0x00403E30 */
void __stdcall ResetStringBuilder(TextContext *context);               /* 0x00403E40 */
void visit_the_cinema(int view, int obj, short frames);                  /* 0x00403E50 */
unsigned int player_wingman(short obj);                                  /* 0x00403EE0 */
void set_speed(short obj, short speed);                                  /* 0x00403F10 */
void auto_position(short obj, short *formationSlot);                     /* 0x00403F40 */
void auto_pilot_sequence(void);                                          /* 0x00404050 */
void SetShipAiScratchWord(unsigned short v);                                 /* 0x004060A0 */
void SelectNewShipAiBehavior(short ship);                               /* 0x004060B0 */
void ShipAiState42(short ship, short target);                           /* 0x004060D0 */
void ShipAiRoutine01(short ship);                                       /* 0x004061E0 */
void ShipAiState25(short ship);                                         /* 0x00406C70 */
void Mtail_fire(short ship, short target);                              /* 0x00406D20 */
void Mzip_past(short ship, short target);                               /* 0x00406D80 */
void Mstrafe_enemy(short ship, short target);                           /* 0x00406FB0 */
void Mbest_strafe(short ship, short target);                            /* 0x00407030 */
void Mstrafe_n_roll(short ship, short target);                          /* 0x004070D0 */
void general_zig(short ship, unsigned int target, short pitch);         /* 0x00407270 */
void Mzig_zag(short ship, unsigned int target);                         /* 0x00407350 */
void Mzig_zag_pitch(short ship, unsigned int target);                   /* 0x00407370 */
void ShipAiState02(short ship, short target);                           /* 0x00407450 */
void ShipAiState44(short ship);                                         /* 0x00407560 */
void ShipAiState27(short ship, short target);                           /* 0x00407580 */
void ShipAiState21(short ship);                                         /* 0x004075A0 */
void ShipAiStateNoOp(void);                                               /* 0x004075B0 */
void ShipAiState03(short ship);                                         /* 0x004075C0 */
void perform_maneuver(short obj);                                       /* 0x004075D0 */
short __stdcall GetShapeFrameExtent(short x, short y,
                                    unsigned char *shape, short frame,
                                    short extent);                       /* 0x00407710 */
void cruise_home(short obj);                                            /* 0x00409760 */
void fail(short obj);                                                   /* 0x004098C0 */
void coming_home(short obj);                                            /* 0x004098D0 */
void run_away(short obj);                                               /* 0x004099C0 */
short check_engage_target(short obj);                                   /* 0x00409AC0 */
short check_destroy_target(short obj);                                  /* 0x00409B10 */
void maneuvering(short obj, short newTarget);                           /* 0x00409C20 */
void formation_burst(short obj);                                        /* 0x00409C50 */
void disallow_engage(void);                                             /* 0x00409CE0 */
void allow_engage(void);                                                /* 0x00409CF0 */
void try2allow_engage(int pilotLevel);                                  /* 0x00409D10 */
void imperial_formation(short obj);                                     /* 0x00409D60 */
void formation_break(short obj);                                        /* 0x00409F00 */
void imperial_wingman(short obj);                                       /* 0x00409F80 */
void kilrathi_wingman(short obj);                                       /* 0x0040A030 */
void wingman_mission(short obj);                                        /* 0x0040A130 */
short dist_from_home(short obj);                                        /* 0x0040A160 */
short scan_and_lock(short obj, int scanRange,
                    enum ShipTactic newTactic);                         /* 0x0040A180 */
void patrol_area(short obj);                                            /* 0x0040A1C0 */
void kilrathi_patrol(short obj);                                        /* 0x0040A360 */
void imperial_wingleader(short obj);                                    /* 0x0040A400 */
void cruise_to_destination(short obj);                                  /* 0x0040A410 */
void prepare_for_jump(short obj);                                       /* 0x0040A540 */
void accelerate_and_jump(short obj);                                    /* 0x0040A630 */
void reach_warp(short obj);                                             /* 0x0040A670 */
void warp_arrival(short obj);                                           /* 0x0040A710 */
void return_to_buddy(short obj, short buddy);                           /* 0x0040A740 */
void escort_buddy(short obj, short buddy);                              /* 0x0040A7A0 */
void escort_mission(short obj);                                         /* 0x0040A7D0 */
void check_goal(short obj);                                             /* 0x0040A900 */
void streak_toward(short obj, short goal, short range);                 /* 0x0040A940 */
void approach_and_engage(short obj, short goal);                        /* 0x0040A9B0 */
void strike_mission(short obj);                                         /* 0x0040AAC0 */
void return_to_master(short obj, short master);                         /* 0x0040ABB0 */
void defend_mission(short obj);                                         /* 0x0040AC00 */
void rendezvous_mission(short obj);                                     /* 0x0040AD80 */
void ship_intelligence(short obj);                                      /* 0x0040AE80 */
void orbit_sphere(short obj);                                           /* 0x0040AF70 */
void tanker_intelligence(short obj);                                    /* 0x0040B010 */
void destroyer_intelligence(short obj);                                 /* 0x0040B0C0 */
void stationary_intelligence(short obj);                                /* 0x0040B110 */
void capital_ship_intelligence(short obj);                              /* 0x0040B140 */
void futurion_intelligence(short obj);                                  /* 0x0040B320 */
void mine_intelligence(short obj);                                      /* 0x0040B3A0 */
void heat_seeking_missile_intelligence(short obj);                      /* 0x0040B430 */
void FF_missile_intelligence(short obj);                                /* 0x0040B570 */
unsigned int GetShipSlotState(short i);                             /* 0x0040B700 */
void release_capital_ship_shapes(enum ObjectType type);                /* 0x0040B990 */
void load_object_resources(enum ObjectType type, short slot);          /* 0x0040B9F0 */
void release_object_resources(short slot);                              /* 0x0040BC70 */
void free_nav_object_resources(void);                                  /* 0x0040BE20 */
void remove_nav_point_objects(void);                                   /* 0x0040BEA0 */
short find_free_object_resource_slot(void);                             /* 0x0040BEC0 */
int object_resources_loaded(enum ObjectType type);                      /* 0x0040BEF0 */
int nav_point_uses_object_type(const MissionNavPoint *navPoint,
                               enum ObjectType type);                   /* 0x0040BF20 */
void cache_nav_point_resources(MissionNavPoint *navPoint);              /* 0x0040BF50 */
void EnterNavPoint(short navPoint);                                     /* 0x0040BFF0 */
unsigned int GetObjectVisible(void);                                        /* 0x0040C350 */
void place_ship_near_player_until_valid(short obj, int minimum,
                                        short maximum);                 /* 0x0040C360 */
void initialize_mission_ship(short obj, short missionShip,
                             signed char navPoint);                     /* 0x0040C5E0 */
unsigned int IsInitialMissionShip(short missionShip);                  /* 0x0040C740 */
short spawn_mission_ship(short missionShip, short navPoint);            /* 0x0040C800 */
short __stdcall SampleBothJoysticks(InputDeviceSample *samples,
                                    unsigned int fallback);             /* 0x0040CAA0 */
int __stdcall SampleJoystickDevice(InputDeviceSample *samples,
                                   short joystick,
                                   unsigned int fallback);              /* 0x0040CAE0 */
void SetNavCursorIndex(unsigned short v);                                /* 0x0040CBB0 */
void ObjectDrawHook(short *p);                                          /* 0x0040CBC0 */
void ClearNavHazardFlag(void);                                           /* 0x0040D1D0 */
void ClearNavLegendFlag(void);                                                 /* 0x0040D240 */
void SetScreenClipRect(unsigned short a, unsigned short b, unsigned short c, unsigned short d);/* 0x0040D8C0 */
void FormatNavCoordinates(unsigned char *out);                                 /* 0x0040DE70 */
char *GetNavNameSkippingMarker(short i);                                         /* 0x0040DF50 */
void NudgeObjectX(short i, short dx, short dy);                 /* 0x0040EFE0 */
void __stdcall free_viewport(Viewport *viewport);                      /* 0x0040F940 */
unsigned short GetPaletteReadyUnused(void);                                    /* 0x0040FA30 */
void DrawTitleLogo(short distance, short y);                            /* 0x0040FA40 */
void UpdateTitleMenuCursor(void);                                      /* 0x0040FB10 */
int Title_Sequence(void);                                               /* 0x0040FB70 */
void CalibrateJoystickInteractive(short logicalFile, short section,
                                   short flags, short mode);            /* 0x004102B0 */
void WaitForJoystickButtonRelease(void);                               /* 0x004106C0 */
void WaitForJoystickButtonPress(void);                                 /* 0x00410700 */
void rotate_eye_to_goal(void);                                         /* 0x00410A30 */
short GetVectorMagnitude(const FixedVector *vector);                   /* 0x00410AD0 */
void set_eye_direction_and_position(void);                             /* 0x00410AF0 */
unsigned int force_view(int view, int obj);                            /* 0x004117B0 */
unsigned int new_view(int view, int obj);                              /* 0x004117D0 */
unsigned int start_dust(short obj, FixedVector origin,
                        short forwardDistance, int rightOffset,
                        int upOffset);                                 /* 0x00411EC0 */
unsigned int generate_stars(void);                                    /* 0x00411FE0 */
unsigned int update_star_field(void);                                 /* 0x00412100 */
unsigned short count_down(short obj);                                   /* 0x00412410 */
void house_keep_objects(void);                                         /* 0x00412430 */
void update_objects_in_space(void);                                    /* 0x00412820 */
void rotate_object(short obj);                                         /* 0x00412920 */
void accelerate_and_move_object(short obj);                            /* 0x004129A0 */
void animate_shape(short obj);                                         /* 0x00412CD0 */
void animate_object(short obj);                                        /* 0x00412E30 */
void object_intelligence(short obj);                                   /* 0x00413880 */
void EmitTextString(void (__stdcall *writer)(short), const char *text); /* 0x00413A10 */
void FormatTextTokens(void (__stdcall *writer)(short),
                      const char *format, unsigned long *arguments);    /* 0x00413A40 */
void DrawFormattedText(const char *format, ...);                        /* 0x00413C40 */
void FormatTextBufferFromStart(const char *format, ...);                /* 0x00413C70 */
void AppendFormattedText(const char *format, ...);                      /* 0x00413CB0 */
void FatalErrorAndExit(const char *format, ...);                       /* 0x00413CE0 */
unsigned short IsCockpitExplosionActive(void);                         /* 0x00413D20 */
unsigned int GetSeriesRecordField(char slot, int rec);                       /* 0x00413F70 */
short DrawHudMessageSlot(HudMessageSlot *slot);                        /* 0x004140A0 */
void ClearHudMessageSlot(HudMessageSlot *slot);                        /* 0x00414180 */
void ClearHudMessageIfMatching(HudMessageSlot *slot, char *text);      /* 0x004141B0 */
void ClearHudGunReadouts(void);                                             /* 0x004141D0 */
void SetHudMessageSlot(HudMessageSlot *slot, TextContext *context,
                       short x, short y, char *text,
                       unsigned short colour,
                       signed char flashCount);                        /* 0x004141F0 */
void UpdateMessage(HudMessageSlot *slot);                              /* 0x00414240 */
void set_global_message(char *text, unsigned short colour,
                        int flashCount);                               /* 0x00414270 */
void CockpitMessage(char *text, unsigned short colour,
                    int flashCount);                                  /* 0x004142B0 */
void remove_message(char *text);                                      /* 0x004142E0 */
short KilrathiShipWithinRange(short obj, short range);                 /* 0x00414300 */
short CanEngageAutopilot(short showReason);                            /* 0x00414380 */
void *ClearHudTargetVectors(void);                                            /* 0x00414410 */
unsigned short get_mode(short i);                                     /* 0x004147E0 */
void set_mode(short i, int state);                                    /* 0x00414800 */
int GetVduModeStackDepth(short i);                                    /* 0x00414890 */
void push_mode(short i, int state);                                   /* 0x004148A0 */
void pop_mode(short i);                                               /* 0x004148E0 */
void set_new_vdu(short vdu);                                          /* 0x00414910 */
void ClearMessageSlot(short i);                                          /* 0x004149C0 */
void ClearAutopilotFlag(void);                                              /* 0x004149E0 */
int IsAutopilotEngaged(void);                                              /* 0x004149F0 */
unsigned short SetAutopilotFlag(unsigned short v);                        /* 0x00414A10 */
void RefreshAutopilotHud(void);                                             /* 0x00414A20 */
void PlayTargetLockSfx(void);                                           /* 0x00414AD0 */
void PlayShieldHitSfx(void);                            /* 0x00414AE0 */
int malf(char component);                                             /* 0x00414AF0 */
unsigned short vdu_malf(short vdu, short sound);                       /* 0x00414B20 */
void ShowComponentHitHudMessage(char *text, unsigned short colour,
                                signed char flashCount);               /* 0x00414B70 */
int damage_your_component(char component, char amount, char maximum); /* 0x00414BF0 */
void RemovePlayerReleaseWeapon(signed char weapon);                  /* 0x00414CB0 */
void InputFilterHook(void);                                            /* 0x00415040 */
short sighted(short objective);                                       /* 0x00415050 */
short visited(short objective);                                       /* 0x00415070 */
short achieved(short objective);                                      /* 0x00415090 */
void flag_objective(short objective, unsigned char flags);            /* 0x004150B0 */
void set_next_destination(void);                                       /* 0x004153D0 */
unsigned int CheckForShipQueuedToCurrentNavPoint(void);                   /* 0x004154C0 */
unsigned int GetShipAiScratch(void);                                     /* 0x00415510 */
void flag_reached(short objective, short reached);                     /* 0x00415530 */
void ClearWeaponHardpoints(void);                                            /* 0x00415A70 */
void BeginMissileLockWarning(unsigned short v);                                 /* 0x00415FC0 */
unsigned int GetHudMessageSlot(unsigned short v);                         /* 0x00415FF0 */
void EndMissileLockWarning(void);                                             /* 0x00416010 */
void SetRectBounds(int p, unsigned short a, unsigned short b, unsigned short c, unsigned short d);/* 0x00416220 */
short GetRectHeight(int p);                                             /* 0x00416250 */
void print_message_text(char *text, unsigned char colour);             /* 0x00416260 */
void ShowHudTextLine(char *s, unsigned char b);                          /* 0x00416460 */
void SetHudTextColour(short v);                                              /* 0x00416480 */
void draw_target_box(unsigned short colour, signed char object,
                     short solid, short drawLockMarker, short padding,
                     ShortRect *savedBounds);                         /* 0x004164B0 */
void ReleaseCurrentTargetLock(void);                                                 /* 0x004168A0 */
void RestoreCockpitExplosionIfVisible(void);                           /* 0x00416C90 */
unsigned int RestoreTransientCockpitGraphics(void);                    /* 0x00416CB0 */
void SetHudMessageText(char *text, unsigned short colour,
                       unsigned short duration);                       /* 0x00416DE0 */
void malf_noise(short vdu, int effect, unsigned int colour,
                short sound, short refresh);                           /* 0x00416E20 */
void determine_pilot_hand(void);                                      /* 0x004171D0 */
void DrawPilotHandFrame(void);                                       /* 0x00417260 */
void animate_pilot(void);                                             /* 0x004173C0 */
void ResetPilotHandAnimation(void);                                   /* 0x004173F0 */
void send_message(short obj, signed char message);                      /* 0x00417420 */
void clear_cockpit_damage(void);                                      /* 0x00417610 */
void RestoreCockpitExplosionBackground(void);                         /* 0x00417760 */
void ShowDamageMessage(short a);                                       /* 0x00417B10 */
void PlayMissileLaunchSfx(void);                            /* 0x00417F00 */
short __stdcall MeasureTextPixelWidthClamped(const char *text);         /* 0x00418080 */
unsigned short GetMusicDriverPresent(void);                                    /* 0x00418130 */
short get_ship_max_velocity(short obj);                                /* 0x004181C0 */
short recalc_max_velocity(short ship);                                /* 0x00418210 */
void drain_fuel(short ship, short amount);                            /* 0x00418280 */
void damage_ion_drive(short ship, short amount,
                      short maximum);                                 /* 0x004182B0 */
int GetShipAccelerationRate(short ship);                          /* 0x004182F0 */
void point_at(short obj, FixedVector point);                      /* 0x00418330 */
void look_at(FixedVector point);                                 /* 0x004183A0 */
void position_relative(FixedVector *position, FixedVector direction,
                       short distance);                          /* 0x004183D0 */
void position_relative_ijk(FixedVector *position, short obj,
                           short right, short up, short forward); /* 0x00418420 */
short FixedToShortSaturating(int value);                           /* 0x004184C0 */
int MinInt(int a, int b);                                        /* 0x004184E0 */
int MaxInt(int a, int b);                                        /* 0x004184F0 */
int AbsInt(int v);                                               /* 0x00418500 */
unsigned short SignShort(short v);                                  /* 0x00418520 */
unsigned int SignFixed(int v);                                      /* 0x00418540 */
short WrapDegrees(short degrees);                                     /* 0x00418560 */
int equ_vector(const FixedVector *left, const FixedVector *right);     /* 0x00418590 */
void zero_vector(FixedVector *vector);                                /* 0x004185F0 */
void ZeroVectorPtr(int *p);                                             /* 0x00418600 */
void AddFixedVectors(const FixedVector *left, const FixedVector *right,
                     FixedVector *sum);                               /* 0x00418620 */
void SubtractFixedVectors(FixedVector *left, FixedVector *right,
                          FixedVector *difference);                    /* 0x00418650 */
void ComputeVectorDelta(FixedVector *from, FixedVector *to,
                        FixedVector *delta);                            /* 0x00418680 */
void ScaleFixedVector(FixedVector *vector, int scale,
                      FixedVector *result);                            /* 0x004186B0 */
void divide_vector(FixedVector *vector, int divisor,
                   FixedVector *result);                              /* 0x00418700 */
short ChooseRandomSignedMagnitude(short minimum, short maximum,
                                  int allowNegative);                  /* 0x00418750 */
void MakeRandomVectorFixed(short minimum, short maximum,
                           FixedVector *vector);                       /* 0x00418780 */
void FillFixedVectorWithRandomComponents(short limit,
                                         FixedVector *vector);          /* 0x004187E0 */
void rectangular_to_spherical(const FixedVector *rectangular,
                              SphericalVector *spherical);       /* 0x00418890 */
int dot_product(const FixedVector *left, const FixedVector *right);    /* 0x004189E0 */
void vector_cross_product(const FixedVector *left,
                          const FixedVector *right,
                          FixedVector *product);                 /* 0x00418A80 */
short NormalizeFixedVector(FixedVector *vector);                      /* 0x00418B10 */
int vector_length_in_dir(const FixedVector *vector,
                         const FixedVector *direction);          /* 0x00418B60 */
void vector_component_in_dir(const FixedVector *vector,
                             const FixedVector *direction,
                             FixedVector *component);            /* 0x00418BB0 */
void rotate_about_i(short angle, FixedVector *j,
                    FixedVector *k);                            /* 0x00418BE0 */
void rotate_about_j(short angle, FixedVector *i,
                    FixedVector *k);                            /* 0x00418D00 */
void rotate_about_k(short angle, FixedVector *i,
                    FixedVector *j);                            /* 0x00418E40 */
void init_ijk(short obj);                                       /* 0x00418F60 */
void copy_frame(short source, short destination);                    /* 0x00418FD0 */
void fix_objects_ijk(short obj);                                 /* 0x00419050 */
void transform_to_objects_frame(const FixedVector *source,
                                FixedVector *destination,
                                short obj);                           /* 0x004190B0 */
void alter_pitch(short angle, short obj);                        /* 0x00419110 */
void alter_yaw(short angle, short obj);                          /* 0x00419150 */
void alter_roll(short angle, short obj);                         /* 0x00419190 */
short distance_between_points(const FixedVector *from,
                              const FixedVector *to);                  /* 0x004191D0 */
short distance_from_point(short obj, const FixedVector *point);       /* 0x00419210 */
short distance_from_object(short obj, short other);                   /* 0x00419260 */
void get_facing_range_from_point(short obj, const FixedVector *point);/* 0x00419290 */
void get_facing_range_from_object(short obj, short other);            /* 0x00419310 */
void ship_vs_point(short obj, const FixedVector *point);              /* 0x00419390 */
void ship_vs_ship(short obj, short other);                             /* 0x004193B0 */
short facing_to_object(short obj, short other);                       /* 0x004193D0 */
short match_roll_orientation(short obj, short reference);             /* 0x00419440 */
int set_ship_rotation_goals(short obj, short reference,
                            const FixedVector *direction,
                            short *yawGoal, short *pitchGoal);         /* 0x004194D0 */
void point_ship(short obj, short reference,
                const FixedVector *direction);                        /* 0x00419620 */
void point_ship_at_point(short obj, const FixedVector *point);        /* 0x00419660 */
void point_ship_at_object(short obj, short other);                    /* 0x004196A0 */
void point_capital_ship_at_object(short obj, short other);            /* 0x004196C0 */
void point_ship_behind_object(short obj, short other);                /* 0x00419710 */
void point_ship_below_object(short obj, short other);                 /* 0x00419790 */
void point_perpendicular_to_point(short obj,
                                 const FixedVector *point);           /* 0x00419810 */
void point_perpendicular(short obj, short other);                     /* 0x00419850 */
void point_parallel(short obj, short other);                           /* 0x00419870 */
void NormalizeAndScaleVector(FixedVector *vector, int scale);         /* 0x00419950 */
void SetVectorFixedPoint(unsigned int *p, short v);                           /* 0x00419970 */
unsigned int IsPointWithinRange(FixedVector *from, FixedVector *to,
                                short range);                          /* 0x00419990 */
void position_child(short parent, short hardpoint,
                    FixedVector *position);                       /* 0x00419A70 */
void child_object(short hardpoint, short child, short parent);    /* 0x00419B40 */
short get_ship_slot(void);                                        /* 0x00419B70 */
short find_vacant_3d_object(void);                                /* 0x00419BA0 */
void remove_object(short obj);                                        /* 0x00419BD0 */
void ClampVectorTo30(short *p);                                           /* 0x0041A0F0 */
void ClampTo30(short *p);                                           /* 0x0041A110 */
void transform_objects_to_your_view(void);                             /* 0x0041A1D0 */
void set_background_objects_rotation(short obj,
                                     FixedVector *direction);    /* 0x0041A530 */
void get_right_shape(short obj, const FixedVector *direction);    /* 0x0041A610 */
short InitializeModalTextPanel(ModalTextPanel *panel, short fontIndex,
                               unsigned int topLeft,
                               unsigned int bottomRight,
                               short clearColour,
                               unsigned char backgroundColour,
                               short borderColour);                    /* 0x0041A9D0 */
void DrawModalTextPanel(ModalTextPanel *panel, short x, short y,
                        unsigned char alignment,
                        const char *format, ...);                      /* 0x0041AAE0 */
void RestoreModalTextPanel(ModalTextPanel *panel);                     /* 0x0041AB60 */
short ShowModalTextPanel(short fontIndex, const char *format, ...);    /* 0x0041AB90 */
void ReleaseModalTextPanel(void);                                      /* 0x0041AD10 */
short AnySavedGames(void);                                             /* 0x0041AD50 */
DWORD WINAPI DebugOverlayWorkerProc(void *parameter);                  /* 0x0041C960 */
LRESULT CALLBACK DebugKeyboardHookProc(int code, WPARAM key,
                                       LPARAM flags);                  /* 0x0041CA60 */
void DebugOverlayPrintf(DebugOverlayConsole *console,
                        const char *format, ...);                      /* 0x0041CAB0 */
short MinShort(short a, short b);                                       /* 0x0041D0C0 */
short MaxShort(short a, short b);                                       /* 0x0041D0E0 */
void FreePacketAndClear(int *p, int releaseFlags);                      /* 0x0041D100 */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              unsigned short flags);                    /* 0x0041D2E0 */
unsigned int InitializeTextContextFromFont(TextContext *context,
                                           short fontIndex,
                                           unsigned char colour,
                                           signed char background);    /* 0x0041D510 */
unsigned int DrawTextAt(TextContext *context, short x, short y,
                        char *text, unsigned char alignment);           /* 0x0041D5F0 */
unsigned int GetZeroUnused(void);                                        /* 0x0041DA00 */
short CheckEscaped(void);                                               /* 0x0041DA10 */
short WaitForInputKey(void);                                         /* 0x0041DAA0 */
void MoveMenuPointerFromKeyboard(InputEventState *event);               /* 0x0041DC70 */
void WaitForStreamIdle(void);                                       /* 0x0041DEB0 */
short initialize_object(short obj, enum ObjectType type,
                        short owner);                                  /* 0x0041DEE0 */
short borrow_dust(void);                                             /* 0x0041DF40 */
short new_object(enum ObjectType type, short owner);                 /* 0x0041DF70 */
short initialize_ship(enum ObjectType type, short owner);            /* 0x0041DFA0 */
short any_selected(unsigned char *loadout,
                   enum ObjectClass objectClass);                    /* 0x0041DFE0 */
unsigned int remove_weapon(short obj, short weapon);                 /* 0x0041E040 */
void set_objects_data(short obj, enum ObjectType type,
                      short owner);                                  /* 0x0041E120 */
void match_rotation_goal(short *rotation, short *goal,
                         short totalError, short rate);                 /* 0x0041E400 */
void rotate_object_to_goal(short obj);                                  /* 0x0041E520 */
unsigned int AdjustShipSpeed(short ship, int delta);              /* 0x0041E710 */
unsigned int ApproachShipSpeed(short ship, int targetSpeed);      /* 0x0041E750 */
unsigned int steady_object(short ship);                                 /* 0x0041E7C0 */
short real_velocity(short obj);                                       /* 0x0041E7F0 */
unsigned int fix_velocity(short obj);                                 /* 0x0041E820 */
unsigned int sort_viable_target_list(void);                           /* 0x0041E860 */
int Create_ship_hit_debris(short obj);                                /* 0x0041ECE0 */
void check_next_wave(void);                                          /* 0x0041F7C0 */
void Create_explosion_debris(short obj);                              /* 0x0041F800 */
short ShipExplosion(short obj);                                      /* 0x0041FBC0 */
short Explosion(short obj);                                          /* 0x0041FCD0 */
int explode(short attacker, short victim);                            /* 0x00420040 */
void fire_capital_weapon(short obj, short target);                    /* 0x004202D0 */
int fire_turrets(short obj);                                          /* 0x00420AA0 */
int fire_weapon(short obj, short weapon);                              /* 0x00420C20 */
short fire_fixed_projectile_weapon(short obj);                         /* 0x00421220 */
int drop_mine(short obj, signed char weapon, enum ObjectType type,
              short lifetime);                                       /* 0x004212A0 */
void fire_afterburner(short obj, short time);                          /* 0x00421350 */
short find_weapon(short obj, enum ObjectType weaponType);              /* 0x00421100 */
unsigned int ReportShieldHit(void);                                       /* 0x0041F5D0 */
short RandomlyNegate(short v);                                           /* 0x004208C0 */
unsigned int fire_super_brake(short ship);                              /* 0x004213B0 */
short flip_angle(short ship, short angle);                              /* 0x004213D0 */
void place_exhaust_on_ships(void);                                      /* 0x00421430 */
void reposition_fixed_child_objects(void);                             /* 0x004215E0 */
unsigned int housekeep_power_plant_and_fuel(short ship);                /* 0x00421760 */
unsigned int replenish_shields(short ship);                            /* 0x00421780 */
unsigned int replenish_weapon_energy_bank(short ship);                 /* 0x00421830 */
unsigned int EMShutDown(void);                                         /* 0x00421A40 */
unsigned int InitializeEventManagerResources(void);                    /* 0x00421A60 */
unsigned int EMStartUp(void);                                          /* 0x00421AB0 */
unsigned int LoadOriginFxDrivers(void);                                 /* 0x00421B10 */
unsigned int InitializeGameTextContexts(void);                          /* 0x00421D80 */
unsigned int GetFxDriverInitResult(void);                                      /* 0x00421FE0 */
unsigned int GetMessagePumpResult(void);                                      /* 0x00421FF0 */
unsigned int GetFxDriverStatus(void);                                      /* 0x00422000 */
int ace_status(short ace, unsigned char bits);                       /* 0x00422010 */
void unflag_ace(short ace, unsigned char bits);                       /* 0x00422030 */
void flag_ace(short ace, unsigned char bits);                         /* 0x00422050 */
void kill_ace(short ace);                                             /* 0x00422060 */
void ace_greeting(short obj);                                         /* 0x00422090 */
void prepare_ace(short ace);                                          /* 0x004220D0 */
short signed_random(short range);                                     /* 0x004220F0 */
int alert_flag(short ship, unsigned int bits);                        /* 0x00422110 */
unsigned int HasSpeechBuffer(void);                                      /* 0x00422130 */
unsigned int set_alert(short obj, unsigned int bits);                  /* 0x00422140 */
void clear_alert(short obj);                                          /* 0x00422160 */
void start_collision_alert(short obj, short other);                   /* 0x00422180 */
void try2end_collision_alert(short obj);                              /* 0x004221E0 */
short normal_speed(short obj);                                         /* 0x00422220 */
short real_crash_time(short obj, short other);                         /* 0x00422260 */
void clear_crash_cache(void);                                         /* 0x00422440 */
short crash_time(short obj, short other);                             /* 0x00422460 */
short detect_collisions(short obj);                                   /* 0x004224F0 */
unsigned int unactive(short ship);                                      /* 0x00422560 */
int are_alive(short obj);                                             /* 0x00422590 */
void trim_goals(short obj, short amount);                             /* 0x004225C0 */
int report_kilrathi_rout(int mode);                                   /* 0x00422640 */
short find_ship_index(short missionShip);                              /* 0x00422710 */
int try2rout(short obj);                                              /* 0x00422780 */
unsigned char no_goal(short ship);                                     /* 0x00422830 */
int being_tailed(short obj, short other);                             /* 0x00422860 */
int any_enemy_tail(short obj);                                        /* 0x004228A0 */
short detect_enemy_tail(short obj);                                    /* 0x00422930 */
int is_ship_tailing_player_target(short obj);                         /* 0x004229B0 */
int missile_on_tail(short obj);                                       /* 0x004229F0 */
short select_weighted_value(short *choices);                          /* 0x00422A30 */
unsigned int build_squad_list(short leader);                          /* 0x00422A70 */
unsigned int find_squad_center(FixedVector *center);                  /* 0x00422AC0 */
unsigned int init_formation_burst(short obj);                          /* 0x00422B30 */
unsigned int reset_mission_type(short obj,
                                enum ShipMissionType missionType);     /* 0x00422BE0 */
unsigned int change_mission_type(short obj,
                                 enum ShipMissionType missionType);    /* 0x00422C30 */
unsigned int reset_objective(short ship, enum ShipObjective objective); /* 0x00422C70 */
unsigned int alter_objective(short ship, enum ShipObjective objective); /* 0x00422CA0 */
unsigned int reset_tactic(short ship, enum ShipTactic tactic);          /* 0x00422CD0 */
unsigned int alter_tactic(short ship, enum ShipTactic tactic);          /* 0x00422D00 */
void reset_maneuver(short ship, short maneuver);                      /* 0x00422D30 */
void try2reset_maneuver(short obj, short maneuver);                    /* 0x00422D60 */
unsigned int set_special(short ship, enum SpecialManeuver special);     /* 0x00422D90 */
unsigned int approach_zero_speed(short ship);                          /* 0x00422DD0 */
unsigned int approach_min_speed(short obj);                            /* 0x00422DF0 */
unsigned int approach_half_speed(short obj);                           /* 0x00422E10 */
unsigned int approach_cruise_speed(short ship);                         /* 0x00422E50 */
unsigned int approach_full_speed(short ship);                           /* 0x00422E80 */
unsigned int approach_ship_speed(short obj, short other);             /* 0x00422EA0 */
unsigned int get_front_spot(short obj, unsigned short distance,
                            FixedVector *point);                       /* 0x00422EC0 */
unsigned int get_rear_spot(short obj, unsigned short distance,
                           FixedVector *point);                        /* 0x00422F10 */
unsigned int close_behind(short range);                               /* 0x00422F60 */
short scan_for_enemy(short obj, unsigned short range);                  /* 0x00422F80 */
int any_enemy(short obj, short range);                                  /* 0x00423070 */
short nearest_enemy_range(short obj);                                 /* 0x004230F0 */
void fire_when_ready(short obj, short aimed);                         /* 0x00423210 */
unsigned int ships_within_range(short obj, short other, short range); /* 0x00423260 */
int attacker_in_range(short obj, short range);                          /* 0x004232B0 */
int in_danger(short obj);                                               /* 0x00423350 */
unsigned int target_within_range(short obj);                           /* 0x00423400 */
short build_target_list(short obj, short range);                       /* 0x00423440 */
int select_safe_target(short obj);                                    /* 0x004234C0 */
void inherit_leader_mission(short obj);                               /* 0x00423530 */
void inherit_leader(short obj);                                         /* 0x004235B0 */
unsigned int dead_ship(short missionShip);                            /* 0x00423610 */
int gone_ship(short missionShip);                                       /* 0x00423640 */
short skill_rating(short obj);                                        /* 0x00423670 */
int skill_check(short obj, short difficulty);                         /* 0x004236B0 */
short find_ships_sphere(short missionShip);                           /* 0x004236F0 */
int locate_ship(short missionShip, FixedVector *point);               /* 0x00423780 */
unsigned int get_follow_point(short obj, FixedVector *point);         /* 0x00423820 */
unsigned int get_first_follow_point(short obj, FixedVector *point);   /* 0x00423930 */
int hostile_sphere(short obj, short navPoint);                        /* 0x00423970 */
int abandoned(short obj, short other);                                  /* 0x004239D0 */
void engage(short obj, short target,
            enum ShipObjective objective);                              /* 0x00423A50 */
int target_valid(short obj);                                            /* 0x00423AC0 */
unsigned int triumph(short obj);                                      /* 0x00423B00 */
short find_ratio(short inputMinimum, short inputMaximum, short input,
                 short outputMinimum, short outputMaximum);          /* 0x00423BA0 */
short evaluate_damage(short obj);                                       /* 0x00423C00 */
short mine_available(short obj);                                      /* 0x00423CD0 */
int ReleasePacketResourceList(PacketResourceDescriptor *resources,
                              short releaseFlags);                     /* 0x00423D50 */
int LoadPacketResourceList(PacketResourceDescriptor *resources,
                           short flags, int availableBytes);           /* 0x00423D80 */
unsigned int initialize_cockpit(signed char mode);                     /* 0x00423E90 */
void init_constellation(short scene);                                  /* 0x004243E0 */
void free_constellation(void);                                         /* 0x00424490 */
void init_3Space_objects(short scene);                                 /* 0x00424A80 */
void load_common_3Space_objects(void);                                 /* 0x00424B00 */
void remove_all_3d_objects(void);                                      /* 0x00424B80 */
void free_3Space(void);                                                /* 0x00424BA0 */
void free_3Space_objects(void);                                        /* 0x00424BE0 */
unsigned int ResetSceneFlags(void);                                      /* 0x00424C60 */
unsigned int PreloadMusicTrackHook(short track);                       /* 0x00424CE0 */
unsigned int ReleaseMusicTrackHook(short track);                       /* 0x00424CF0 */
void SceneEnterHook(void);                                            /* 0x00425AF0 */
void CreateDebugOverlayConsole(HINSTANCE module, HWND window,
                               short columns, short rows);             /* 0x00425B00 */
void DestroyGlobalDebugOverlayConsole(void);                           /* 0x00425B90 */
void SystemDebugPrintf(const char *format, ...);                           /* 0x00425BB0 */
char PumpMessagesDuringWait(void);                                        /* 0x00425BC0 */
unsigned char TakeDebugStepFlag(void);                                     /* 0x00425BD0 */
void ResetDiskPromptTimer(void);                                             /* 0x00425BE0 */
void DiskPromptDrawHook(void);                                             /* 0x00425C10 */
void ClearDebugPauseFlags(void);                                              /* 0x00425C20 */
unsigned int GetDebugKeyState(unsigned int *p);                        /* 0x00425C30 */
unsigned char *GetHighScoreEntry(short i);                           /* 0x00425DF0 */
unsigned int GetHighScoreValue(short i);                                      /* 0x00425E20 */
void SetHighScoreEntry(short i, unsigned char b, unsigned int v);    /* 0x00425E30 */
void ClearHighScoreTable(short v);                                    /* 0x00425ED0 */
int IsHighScoreSlotUsed(short i);                                      /* 0x00425FE0 */
void LoadSceneBackdrop(char n);                                           /* 0x00426C50 */
short LogMemoryUsage(void);                                               /* 0x004272F0 */
unsigned int ShowMemoryStatusDebug(void);                                 /* 0x004273C0 */
void exit_squadron(const char *msg);                                    /* 0x00427370 */
unsigned int GetJoystickButtonEdge(unsigned int a, short b);                    /* 0x004274C0 */
int main(short argc, char **argv);                                      /* 0x004274E0 */
unsigned int GetScreenUpdateFlag(void);                                         /* 0x004279D0 */
unsigned int initialize_view_buffer(void);                                /* 0x00427A00 */
unsigned int dump_buffer_to_screen(void);                                 /* 0x00427A40 */
unsigned int clear_view_buffer(void);                                     /* 0x00427B00 */
unsigned int ResetScreenClipToFullHeight(void);                                         /* 0x00427BA0 */
unsigned int RefreshMemoryStatusOverlay(void);                                         /* 0x00427C30 */
void Update_3Space(void);                                               /* 0x00427C50 */
unsigned int UpdateSpacePaletteFade(void);                              /* 0x00427CD0 */
unsigned int SetDefaultCommDelay(void);                                  /* 0x00427C80 */
void house_keep(void);                                                  /* 0x00427D40 */
short MeasureMessageWidth(const char *text);                                /* 0x00428E70 */
void WaitForKeyAcknowledge(int mode);                                     /* 0x00428EA0 */
void ShowModalMessage(const char *format, ...);                           /* 0x00428F20 */
void ReportOutOfMemoryAndExit(void);                                    /* 0x00428F80 */
void ShowOnScreenMessage(int flags, short duration,
                         const char *format, ...);                        /* 0x00428FA0 */
void ShowVersionBanner(void);                                           /* 0x004290D0 */
unsigned int Draw_3Space_Frame(void);                                  /* 0x00429DD0 */
void ComputeArcadeWaveBonus(void);                                     /* 0x00429E30 */
void ComputeArcadeTimeBonus(void);                                                 /* 0x00429E70 */
void DrawArcadeScorePanel(short x, short y);                            /* 0x00429E90 */
void UpdateArcadeScoreDisplay(void);                                   /* 0x00429EE0 */
unsigned int RenderSpaceViewFrame(void);                               /* 0x00429FC0 */
void RefreshCockpitStatus(void);                                                 /* 0x0042A0C0 */
short GetShipDistanceToNavPoint(short ship, MissionNavPoint *navPoint); /* 0x0042A0E0 */
short FindNearestNavPoint(short ship);                               /* 0x0042A120 */
unsigned int ReleaseStaleNavTarget(void);                                     /* 0x0042A170 */
void RedrawCommWindow(void);                                       /* 0x0042A670 */
void __stdcall FadeViewportPaletteToColour(Viewport *viewport,
                                           unsigned short colour,
                                           short enabled);             /* 0x0042A700 */
short find_objective(int type, short index);                         /* 0x0042A8F0 */
void arrive_from_warp(short obj);                                      /* 0x0042A950 */
unsigned int unwarp(short obj);                                       /* 0x0042AA10 */
unsigned int warp(short obj);                                         /* 0x0042AAF0 */
int drop_player_mine(short obj);                                      /* 0x0042ABD0 */
unsigned int personality_killed(short personality);                   /* 0x0042AC50 */
void clean_up_cockpit(void);                                          /* 0x0042ACC0 */
short find_next_gun(short obj, enum ObjectType currentGun);           /* 0x0042AD00 */
int select_guns(short obj, short selectedGun);                         /* 0x0042ADA0 */
unsigned int select_new_gun(void);                                    /* 0x0042AE10 */
unsigned int select_new_release_weapon(enum ObjectType preferredType); /* 0x0042AE50 */
void WaitForDebugStep(void);                                          /* 0x0042AFA0 */
void CALLBACK FrameTimerCallback(UINT timerId, UINT message, DWORD user,
                                 DWORD first, DWORD second);       /* 0x0042AFB0 */
void SetMultimediaTimerCallback(int period);                       /* 0x0042AFC0 */
void *PacketLoad(const char *filename, short section, void *destination,
                 unsigned short flags);                               /* 0x0042B050 */
void ServiceAudioStream(void);                                        /* 0x0042B1B0 */
WaveTableEntry *AllocateWaveTableEntry(void);                         /* 0x0042B1F0 */
WaveTableEntry *FindWaveTableEntryByName(const char *name);           /* 0x0042B240 */
void RemoveWaveTableEntry(WaveTableEntry *entry);                     /* 0x0042B290 */
void FreeWaveTable(void);                                           /* 0x0042B300 */
ActiveSoundEntry *AllocateActiveSoundEntry(void);                     /* 0x0042B340 */
void RemoveActiveSoundEntry(ActiveSoundEntry *entry);                 /* 0x0042B390 */
ActiveSoundEntry *FindActiveSoundEntryBySample(IxSample *sample);    /* 0x0042B3F0 */
void ReleaseFinishedSoundEntries(void);                              /* 0x0042B410 */
void StopSoundsUsingWave(const char *name);                          /* 0x0042B450 */
void playWAVE(unsigned char *filename, int looping, int volume);     /* 0x0042B4A0 */
void stop_all_sounds(void);                                         /* 0x0042B640 */
void ServiceSoundSystem(void);                                         /* 0x0042B7D0 */
void SetSoundEffectsVolume(int volume);                               /* 0x0042B7E0 */
void LoadVolumeSettingsFromRegistry(void);                            /* 0x0042B870 */
void FxDriverShutdownHook(void);                                            /* 0x0042C410 */
unsigned int LoadInstallDat(void);                                     /* 0x0042C660 */
unsigned short GetJoystickPresentUnused(void);                                    /* 0x0042CDA0 */
unsigned int parse_view_script(void);                                  /* 0x0042CDB0 */
unsigned int update_scripted_view(void);                               /* 0x0042D1C0 */
void initialize_scripted_view(const short *script);                    /* 0x0042D230 */
unsigned int GetVictoryScreenId(void);                                     /* 0x0042D270 */
void CloseDataFileByHandle(unsigned short *p);                                  /* 0x0042D870 */
short GetTargetColourIndex(void);                                                /* 0x0042DB70 */
void LogDisplayMode(const char *mode);                                 /* 0x0042E020 */
unsigned short __stdcall AllocateViewport(Viewport *viewport,
                                          short clearColour,
                                          short flags);                /* 0x0042E090 */
void FadeMusic(void);                                                  /* 0x0042E320 */
void SetMusicOn(void);                                               /* 0x0042E330 */
void StopMusic(void);                                                   /* 0x0042E350 */
void SetMusBreakpt(void);                                               /* 0x0042E380 */
void PaletteFadeHook(void);                                            /* 0x0042E390 */
void FlushSoundEffect(void);                                               /* 0x0042E3A0 */
void FlushSoundEffects(void);                                               /* 0x0042E3C0 */
void SceneLeaveHook(void);                                            /* 0x0042E3E0 */
unsigned int StartMusicTrack(short track, short mode,
                             short enabled);                           /* 0x0042E880 */
void StopMusicUnlessSuppressed(void);                                        /* 0x0042E8B0 */
unsigned short GetMusicMode(void);                                /* 0x0042E8D0 */
void ResetSoundState(void);                                             /* 0x0042EE80 */
void ResetSoundStateForScene(void);                                               /* 0x0042EEA0 */
void ResetSoundStateForFlight(void);                                               /* 0x0042EEB0 */
void EnableMusicForScene(void);                                               /* 0x0042EEE0 */
unsigned int SoundFxTick(void);                                       /* 0x0042EF00 */
void FlushSoundEffectsAndLog(void);                                               /* 0x0042EF10 */
void sound_effect(void);                                                /* 0x0042EF20 */
void PlaySfxWaveFileByNumber(int soundNumber, int sourceObject,
                             int looping);                            /* 0x0042EF30 */
int IsShipQueuedOrderDefend(short i);                                      /* 0x0042F1F0 */
unsigned short GetConversationState(void);                                    /* 0x0042F730 */
void InitFullScreenViewport(int *vp, short arg);                                    /* 0x0042F7E0 */
void FrameStartHook(void);                                            /* 0x0042F930 */
unsigned short IsSoundHardwarePresent(void);                                     /* 0x0042F940 */
void MessagePumpHook(void);                                           /* 0x0042F950 */
int PushMemoryStackFrame(int memory, int offset);                       /* 0x0042F960 */
int MapPacketHandleToBlock(int handle);                               /* 0x0042FA20 */
void *AllocateTaggedMemory(unsigned int size, unsigned short flags);   /* 0x0042FA90 */
void ReleasePacketHandle(int handle);                                 /* 0x0042FAE0 */
unsigned int GetFixedOneMillionThunk(void);                  /* 0x0042FB20 */
unsigned int GetFixedOneMillionThunkAlt(void);               /* 0x0042FB30 */
short __stdcall UpdateInputDeviceTransitions(short raw);               /* 0x00430710 */
void PollJoystickButtonEvents(void);                                   /* 0x00430840 */
void PollMenuInputDevices(void);                                       /* 0x00430920 */
short StepMenuSelection(short v, int flag);                                 /* 0x00430BC0 */
int IsCommMenuIdle(void);                                              /* 0x00430CA0 */
void AppendCommMenuChoice(char *text, short command);                    /* 0x00430CB0 */
void SendCommMenuChoice(short i);                                          /* 0x00430D30 */
void OpenCommMenuForTarget(unsigned int a, int b);                         /* 0x00430D50 */
int IsEjectPromptActive(void);                                                /* 0x00430D80 */
short GetPendingMenuAction(void);                                            /* 0x00430DA0 */
void SetPendingMenuAction(unsigned char v);                                 /* 0x00430DB0 */
void RequestEjectPrompt(void);                                               /* 0x00430DC0 */
int HasNoLockedTarget(void);                                              /* 0x00430E10 */
int IsWingmanIdle(void);                                              /* 0x00430E30 */
unsigned short IsCommMenuAvailable(void);                                     /* 0x00430E50 */
void RequestCommMenu(unsigned char v);                                       /* 0x00430E70 */
void FreeCommDisplayResources(void);                                  /* 0x00431410 */
void EndCommSessionWithWingman(void);                                  /* 0x00431470 */
void EndCommMenu(void);                                              /* 0x004314C0 */
void ShowCentredPrompt(char *text, unsigned short arg);                       /* 0x004314F0 */
void ShutdownVideoHook(void);                                               /* 0x004318F0 */
short ReadCalibratedJoystick(void);                                    /* 0x00431D20 */
void ThrottleFrameAndDrawFps(HDC dc);                                       /* 0x00431F00 */
void SetSpaceFlightFrameTiming(void);                                  /* 0x004320E0 */
void SetCinematicFrameTiming(void);                                    /* 0x00432110 */
void DIBerror(const char *tag, int hr);                                 /* 0x00432140 */
void DIBpositionWindow(void);                                           /* 0x00432230 */
void DIBreInstall(void);                                                /* 0x004322B0 */
void DIBinstall(HWND window);                                           /* 0x00432310 */
int DIBcascade(int mode, int *reportedResult);                          /* 0x00432410 */
void DIBunInstall(void);                                              /* 0x00432680 */
void DIBmakeDIB(void);                                                  /* 0x004326E0 */
void DIBdestroyDIB(void);                                             /* 0x004328A0 */
void DIBslam(void);                                               /* 0x00432960 */
void DIBslamReal(void);                                           /* 0x00432970 */
void DIBupdate(int left, int top, int right, int bottom);               /* 0x00432C60 */
unsigned char *GetDIBPixelBuffer(void);                                  /* 0x00432DE0 */
HWND GetDIBWindowHandle(void);                                           /* 0x00432E00 */
void CachePaletteEntryFromWords(short index, unsigned short *rgb);        /* 0x00432E30 */
void DIBramPalette(void);                                                 /* 0x00432EA0 */
void DIBsetPalette(short index, short *rgb);                              /* 0x00432F10 */
void GetPaletteEntryAsWords(short i, unsigned short *rgb);                /* 0x00433020 */
void DIBwholePaletteFromTriplets(unsigned char *palette);             /* 0x00433060 */
void DIBwholePaletteFromWords(unsigned short *palette);                /* 0x00433120 */
void DIBwaitForVerticalBlank(void);                                      /* 0x004331E0 */
char *DirectDrawResultToText(int result);                              /* 0x004331F0 */
void show_info_disp(void);                                             /* 0x00433910 */
unsigned int steer_away_from_object(short obj, short other,
                                    short amount);                    /* 0x00433AC0 */
void steer_away_from_predicted_object(short obj, short other,
                                      short predictionTicks,
                                      short amount);                  /* 0x00433B90 */
void prevent_collision(short obj);                                    /* 0x00433C80 */
int handle_collisions(short obj);                                     /* 0x00433D90 */
int regulate_turn(short obj);                                         /* 0x00433DE0 */
unsigned int select_target(short obj);                                  /* 0x00433E50 */
unsigned int veer_random(short obj, short amount);                      /* 0x00433EC0 */
unsigned int offset_location(short obj, const ShortVector *offset,
                             FixedVector *location);                  /* 0x00433F50 */
unsigned int compute_formation_destination(short leader,
                                           const ShortVector *offset,
                                           FixedVector *destination); /* 0x00433FF0 */
unsigned int control_speed(short obj, unsigned short range,
                           int desiredSpeed);                         /* 0x00434040 */
unsigned int chase_location(short obj, const FixedVector *destination,
                            short reference);                         /* 0x004340F0 */
unsigned int goto_location(short obj,
                           const FixedVector *destination);           /* 0x004342C0 */
unsigned int goto_formation(short obj, const FixedVector *destination,
                            short leader);                            /* 0x00434360 */
unsigned int maintain_formation(short obj);                             /* 0x004344E0 */
unsigned int reset_stress(short obj);                                  /* 0x00434550 */
short stress_morale(short obj);                                        /* 0x004345D0 */
enum ShipManeuver any_defense(short obj);                              /* 0x004345F0 */
enum ShipManeuver pick_regular_maneuver(short obj, int event);         /* 0x00434630 */
enum ShipManeuver pick_from_list(const ManeuverChoice *choice,
                                 short obj);                           /* 0x00434800 */
enum ShipManeuver pick_kilrathi_maneuver(short obj, int event);        /* 0x004348A0 */
unsigned int process_maneuver_node(short obj, int event);              /* 0x00434900 */
void handle_stress(short obj, int event);                              /* 0x00434980 */
void intelligence_events(short obj);                                    /* 0x00434A80 */
unsigned int chase_speed(short obj, short range);                      /* 0x00434C70 */
short RandomBelow(short n);                                           /* 0x00434CD0 */
void SeedRandomFromClock(void);                                               /* 0x00434CF0 */
short __stdcall RandomInRange(short lo, short hi);                      /* 0x00434D20 */
short RandomBelowOrEqual(short n);                                      /* 0x00434D50 */
long MultiplyFixed(int left, int right);                              /* 0x00434D80 */
long DivideFixed(int numerator, int denominator);                     /* 0x00434DB0 */
long SinFixed(short degrees);                                    /* 0x00434E00 */
long CosFixed(short degrees);                                    /* 0x00434E30 */
long ArcCosFixed(int value);                                      /* 0x00434E90 */
long FloatToLongPassThrough(void);                                             /* 0x00434EC0 */
long ComputeFixedVectorMagnitude(const FixedVector *vector);         /* 0x00434F20 */
void __stdcall SetTextCursor(unsigned short a, unsigned short b);   /* 0x00434F70 */
void __stdcall SetTextContext(TextContext *context);                  /* 0x00434FA0 */
void WaitForVerticalBlankThunk(void);                                  /* 0x00434FB0 */
unsigned int IdentityHandle(unsigned int v);                             /* 0x00434FC0 */
void __stdcall SetWholePaletteFromTriplets(unsigned char *palette);               /* 0x00434FD0 */
unsigned short ReadWord(unsigned short *p);                        /* 0x00434FE0 */
unsigned short GetFontCharWidth(char i);                                     /* 0x00434FF0 */
void ReleaseVideoResourcesHook(void);                                           /* 0x00435010 */
short __stdcall GetShapeFrameBounds(short *bounds, short x, short y,
                                    unsigned char *shape, short frame); /* 0x00435020 */
short __stdcall IsPointInRect(short x, short y, const short *rect);       /* 0x00435090 */
void SplitPackedPoint(unsigned int packed, short *p);                      /* 0x004350D0 */
void DrawTextString(char *text);                                      /* 0x004350F0 */
void __stdcall DrawTextCharacter(char character);                    /* 0x00435290 */
void __stdcall AppendTextCharacter(char character);                  /* 0x00435310 */
void ResetTextCursor(void);                                           /* 0x004353F0 */
unsigned int DosFarPtrToNear(unsigned int v);                             /* 0x00435410 */
unsigned int DosNearPtrToFar(unsigned int v);                             /* 0x00435420 */
char *__stdcall DosStrrchr(char *s, short c);                       /* 0x00435430 */
char *__stdcall DosStrchr(char *s, short c);                        /* 0x00435450 */
char *__stdcall DosStrcpy(char *dst, const char *src);               /* 0x00435470 */
short DosStrlen(const char *s);                                   /* 0x004354D0 */
void DosMemcpy(void *dst, const void *src, size_t n);               /* 0x004354F0 */
unsigned short GetEventManagerStatus(void);                            /* 0x00435550 */
void __stdcall RegisterEventManagerShutdown(void (*fn)(void));         /* 0x00435560 */
short __stdcall InitializeEventManager(short period,
                                       void (*initialize)(void),
                                       void *configuration);           /* 0x00435570 */
void ShutdownEventManager(void);                                      /* 0x00435590 */
unsigned short __stdcall ConfigureEventManagerPointer(
    unsigned char *shape, short frame);                               /* 0x004355A0 */
void __stdcall EventManagerHook(short mode);                           /* 0x004355C0 */
void __stdcall SetEventManagerPump(void (*pump)(void));                /* 0x004355D0 */
void TranslatePolledInputEvent(unsigned short type,
                               unsigned int value);                   /* 0x004355F0 */
void QueueInputEventAtCursor(unsigned int type, short primaryButton,
                             short secondaryButton);                   /* 0x004356A0 */
InputEvent *AllocateInputEvent(void);                                  /* 0x004356E0 */
void ReleaseInputEvent(InputEvent *event);                             /* 0x00435760 */
void QueueInputEvent(unsigned short type, unsigned short x,
                     unsigned short y, unsigned short value,
                     int primaryButton, int secondaryButton,
                     unsigned int timestamp);                          /* 0x00435790 */
void ReleaseInputEventQueue(void);                                    /* 0x004358B0 */
void RetainInputEventsOfType(short type);                              /* 0x004358E0 */
void RemoveInputEvent(InputEvent *event);                              /* 0x00435940 */
short __stdcall GetNextInputEvent(InputEventState *event);             /* 0x004359C0 */
short __stdcall PollInputEvent(InputEventState *event, short filter);  /* 0x00435CC0 */
short __stdcall PeekInputEvent(InputEventState *event, short type);    /* 0x00435CE0 */
short __stdcall IsInputEventQueued(short type);                        /* 0x00435D80 */
void FlushInputEvents(void);                                          /* 0x00435DB0 */
unsigned int ResetAllocationDepth(void);                                          /* 0x00435DC0 */
void CheckCursor(void);                                                /* 0x00435DE0 */
void CaptureMouseCursorBackground(void);                           /* 0x00435E20 */
void DrawMouseCursor(void);                                        /* 0x00435EF0 */
void RestoreMouseCursorBackground(void);                           /* 0x00435FA0 */
void RefreshMouseCursorDisplay(void);                              /* 0x00436060 */
void EnterAllocationScope(void);                                                 /* 0x004360D0 */
void LeaveAllocationScope(void);                                                 /* 0x004360E0 */
void __stdcall SetMouseCursorShape(unsigned char *shape, short frame); /* 0x004360F0 */
void SetMouseHomePosition(short x, short y);                                  /* 0x00436160 */
void __stdcall ApplyPackedMousePosition(ShortPoint point);             /* 0x00436190 */
void SetFrameTimerPeriod(short period);                                   /* 0x004361E0 */
void SetFrameTimerAndWait(short period);                                       /* 0x004361F0 */
void SetFrameTimerPeriodDirect(short p);                      /* 0x00436210 */
void WaitForFrameTick(void);                                          /* 0x00436230 */
int IsFrameTickElapsed(void);                                            /* 0x00436240 */
unsigned short GetSoundHardwareFlag(void);                                    /* 0x00436260 */
void TimerResetHook(void);                                           /* 0x004362A0 */
unsigned int GetVideoReleaseResult(void);                                      /* 0x004362B0 */
unsigned short IdentityWord(unsigned short v);                         /* 0x004362C0 */
void TimerStopHook(void);                                           /* 0x004362D0 */
unsigned int GetFixedOneMillion(void);                                 /* 0x004362E0 */
unsigned int GetFixedOneMillionAlt(void);                                 /* 0x004362F0 */
void ClearInputKeyStatePreservingModifiers(void);                     /* 0x004363A0 */
void ClearInputKeyState(void);                                        /* 0x004363E0 */
void SetInputKeyState(int scanCode, unsigned char pressed);            /* 0x00436420 */
void sort_object_depth(void);                                          /* 0x00436460 */
void draw_sorted_objects_to_buffer(void);                              /* 0x00436520 */
void intro_drawbackgroundships(void);                                  /* 0x00436650 */
void set_up_screen_viewport(signed char mode);                         /* 0x00436740 */
void MouseIdleHook(void);                                           /* 0x004368C0 */
unsigned short GetNavRangeSentinel(void);                                   /* 0x004368E0 */
unsigned short GetOriginalFreeMemory(void);                                   /* 0x004368F0 */
void StartupHook(void);                                           /* 0x00436900 */
unsigned int JoystickEdgeHook(int button);                         /* 0x00436910 */
void FreeIfNotNull(void *p);                                            /* 0x00436950 */
unsigned int GetStartupErrorCode(void);                                     /* 0x00436970 */
void ShutdownHook(void);                                           /* 0x00436980 */
unsigned short GetDiskRetryCount(void);                                   /* 0x004369A0 */
unsigned short GetDiskChangeCount(void);                                   /* 0x004369B0 */
unsigned short GetShutdownErrorCode(void);                                   /* 0x004369C0 */
void VideoReleaseHook(void);                                           /* 0x004369D0 */
void ExitCleanupHook(void);                                           /* 0x004369E0 */
unsigned int IsVectorWithinRange(FixedVector *vector, short range);    /* 0x00436A00 */
unsigned int shrink_vector(FixedVector *vector);                       /* 0x00436A30 */
unsigned int shrink(int *component);                                   /* 0x00436A70 */
void __stdcall FillGraphicSuffix(char *path, short number,
                                 short digits);                        /* 0x00436C70 */
void __stdcall ConvertChar_Int(char *text, short number,
                              short digits);                          /* 0x00436CB0 */
unsigned int BeginBriefingScene(void);                                         /* 0x00438B90 */
void __stdcall SetViewportRect(Viewport *viewport, unsigned short left,
                               unsigned short top, unsigned short right,
                               unsigned short bottom);                /* 0x00439400 */
unsigned int DrawClippedLine(RasterClip *clip, int x1, int y1, int x2, int y2,
                             int mode, int colour);                    /* 0x00439E39 */
void SetPaletteTranslationTable(const unsigned char *translation);    /* 0x0043AE3F */
void ClearSaveSlotFlag(void);                                            /* 0x0043F690 */
int IsSaveSlotFree(void);                                            /* 0x0043F6A0 */
void ClearLoadSlotFlag(void);                                            /* 0x0043F720 */
void SelectSaveSlot(short i);                                        /* 0x0043F730 */
short FindMenuRegionAtPoint(short x, short y,
                            const TitleMenuRegion *regions);           /* 0x0043F7C0 */
void CheckHeapBlockSignature(int p);                                              /* 0x004408A0 */
unsigned int GetHeapBlockSize(int p);                              /* 0x004408C0 */
unsigned int SignExtendClipCoord(unsigned short v);                         /* 0x00440BE0 */
void ValidateViewportBounds(Viewport *viewport, RasterSurface *surface,
                            RasterClip *clip);                         /* 0x00440C00 */
void ClipViewportToScreen(Viewport *viewport);                         /* 0x00440CF0 */
void SetSolidColourTranslation(unsigned char colour);                 /* 0x00440D10 */
void DrawSpriteTransformed(Viewport *viewport, int x, int y,
                           unsigned char *shape, int frame,
                           int angle, int scaleX, int scaleY,
                           int flip, int blendMode);                /* 0x00440FE0 */
void RasterLineHook(void *marker);                                    /* 0x00441140 */
void DrawFontGlyph(char character, TextContext *context, int height,
                   int width, int y);                                 /* 0x00441150 */
void __stdcall GetPaletteEntry(short index, unsigned short *rgb);         /* 0x004413C0 */
void __stdcall SetPaletteEntry(short index, short *rgb);                  /* 0x004413E0 */
void DrawSpriteDefault(Viewport *viewport, short x, short y,
                       unsigned char *shape, short frame);          /* 0x00441400 */
void CaptureSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, unsigned char *shape,
                             short frame);                          /* 0x00441450 */
void RestoreSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, unsigned char *shape,
                             short frame);                          /* 0x00441740 */
void DrawSolidColourSprite(Viewport *viewport, short x, short y,
                           unsigned char *shape, short frame,
                           unsigned char colour);                    /* 0x00441A40 */
void CopyViewportContents(Viewport *source, Viewport *destination);    /* 0x00441A90 */
void ClearViewport(Viewport *viewport, short colour);                  /* 0x00441AE0 */
void DrawViewportLine(Viewport *viewport, short x1, short y1,
                      short x2, short y2, short colour);             /* 0x00441BA0 */
void DrawFilledViewportRect(Viewport *viewport, short left, short top,
                            short right, short bottom,
                            short colour);                           /* 0x00441C70 */
void DrawViewportBorder(Viewport *viewport, short left, short top,
                        short right, short bottom,
                        short colour);                                /* 0x00441CF0 */
void DrawSpriteScaled(Viewport *viewport, short x, short y,
                      unsigned char *shape, short frame, short angle,
                      short scale, short flip);                       /* 0x00441FC0 */
void DrawSolidColourSpriteScaled(Viewport *viewport, short x, short y,
                                 unsigned char *shape, short frame,
                                 short angle, short scale, short flip,
                                 unsigned char colour);                /* 0x00442000 */
int GetTransformedShapeBounds(Viewport *viewport, short x, short y,
                              unsigned char *shape, short frame,
                              short angle, short scale, int flip,
                              short *bounds);                         /* 0x00442050 */
void fizzle_fade(Viewport *source, Viewport *destination,
                 const short *geometry);                            /* 0x00442200 */
void snow_viewport(Viewport *viewport, int effect,
                   unsigned int colour);                              /* 0x00442300 */
void UpdateStreamerStoppedFlag(void);                                    /* 0x00442330 */
void Streamer_stop(void);                                                /* 0x00442460 */
void SetMusicStreamVolume(unsigned short volume);                     /* 0x00442590 */
int ReadCheaterFlagFromRegistry(void);                                /* 0x00442600 */
void ix_system_service_sounds(void);                                     /* 0x004472A7 */
void ix_system_shutdown(void);                                            /* 0x004473F3 */
void ix_system_set_master_volume(unsigned short volume);              /* 0x0044745B */
void ix_system_delete_all_samples(void);                                  /* 0x00447548 */
void ix_system_delete_all_sounds(void);                                   /* 0x00447610 */
void ix_system_delete_sound(IxSound *sound);                              /* 0x004475C7 */
void __fastcall ix_sound_release(IxSound *sound);                    /* 0x0044801E */
void __fastcall ix_sound_stop(IxSound *sound);                       /* 0x004480CF */
int __fastcall ix_sound_is_playing(IxSound *sound);                  /* 0x00448678 */
void ix_streamer_set_volume(unsigned short volume);                   /* 0x004435BE */

#endif /* WC1_FUNCS_H */
