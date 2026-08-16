/*
 *  Prototypes for every reimplemented function, in address order.
 *
 *  Address order is link order, so this list doubles as the reconstruction
 *  inventory: see src/map for the same list with the original addresses.
 */
#ifndef WC1_FUNCS_H
#define WC1_FUNCS_H

void SaveGamePalette(void);                                             /* 0x417550 */
void RestoreGamePalette(void);                                          /* 0x464021 */
short easy2see(short obj);                                            /* WC2 unmapped */
void make_shard(short asteroid, FixedVector direction);               /* 0x4176D2 */
void remove_hazard(signed char hazard);                               /* 0x457587 */
void remove_all_hazards(void);                                        /* 0x4576AB */
short difficulty(void);                                               /* 0x417AD7 */
short asteroid_velocity(void);                                       /* 0x417916 */
void skew_randomly(short obj, short allowReverse);                    /* 0x417941 */
void align(short *value, short quantum);                              /* 0x433AB0 */
void init_hazard(short obj, FixedVector position, short moving);      /* 0x417AF9 */
int near_field(const HazardField *field, const FixedVector *point);   /* WC2 unmapped */
short within_field(const HazardField *field, const FixedVector *point); /* 0x440831 */
short try_far_spot(FixedVector *spot, short *moving);                 /* 0x417EFF */
short rear_sphere(void);                                              /* 0x418175 */
int ok_hazard_spot(short obj);                                        /* WC2 unmapped */
short make_hazard(void);                                              /* 0x464C4B */
void extra_hazard(short obj);                                        /* WC2 unmapped */
void approach(short obj);                                            /* 0x4182B6 */
void manage_hazard(short obj, short slot);                            /* 0x41836E */
void match_ship_to_eye(void);                                        /* WC2 unmapped */
void update_hazards(void);                                           /* 0x4184F6 */
void start_hazard_field(short region);                               /* WC2 unmapped */
void add_hazard_field(enum ObjectType type, FixedVector center,
                      short radius, short density);                   /* WC2 unmapped */
void check_hazards(void);                                            /* 0x41787A */
void __stdcall WarpMouseTo(short x, short y);                        /* 0x4199F0 */
void CheckLauncherAndConfig(void);                                   /* 0x453C95 */
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous,
                   LPSTR commandLine, int showCommand);               /* 0x453E60 */
void ShutdownGameWindow(void);                                       /* 0x454103 */
void ShowNoticeMessageBox(const char *text);                            /* 0x45418B */
unsigned int AbortToDesktop(void);                                    /* 0x45422D */
int CreateMainWindow(HINSTANCE instance, HINSTANCE previous,
                     int showCommand);                                /* 0x4542B7 */
unsigned int PumpWindowMessages(void);                                /* 0x45445A */
unsigned int GetF1KeyLatch(void);                                     /* 0x473CBA */
LRESULT CALLBACK MainWindowProc(HWND window, UINT message,
                                WPARAM wParam, LPARAM lParam);         /* 0x454625 */
int __stdcall GetJoystickPosition(unsigned int *x, unsigned int *y,
                                  unsigned int *buttons, short joystick,
                                  unsigned int fallback);              /* 0x416690 */
short GetJoystickButtons(void);                                                   /* WC2 unmapped */
void GetJoystickDevCaps(short joystick, short *xMin, short *xMax,
                        short *yMin, short *yMax);                    /* 0x45536F */
HINSTANCE GetApplicationInstance(void);                               /* 0x455427 */
HWND GetMainWindowHandle(void);                                       /* 0x45543C */
HDC GetMainWindowDeviceContext(void);                                 /* 0x455451 */
void *AllocateGuardedMemory(unsigned int size);                        /* 0x455466 */
void ReportHeapGuardCorruption(void *memory, int count, int overrun);     /* 0x455565 */
void CheckAllGuardedAllocations(void);                                /* 0x462890 */
void FreeGuardedAllocation(void *memory);                                /* 0x4138A8 */
void SetMousePosition(int x, int y);                                    /* 0x423C60 */
unsigned int PollKeyboardState(void);                                 /* 0x455AC8 */
int GetShiftKeyState(void);                                             /* 0x455E8F */
int GetControlKeyState(void);                                           /* 0x455EBF */
unsigned int GetKeyboardModifiers(void);                                     /* 0x455EAA */
unsigned int GetGameClockTicks(void);                                   /* 0x455EF5 */
void InitGameClockEpoch(void);                                    /* 0x455F23 */
void WriteDebugString(const char *s);                                   /* 0x455697 */
void SetMousePositionDuplicate(int x, int y);                                 /* WC2 unmapped */
FontWorkspace **AllocateFontWorkspace(short fontIndex);                /* 0x455715 */
void FreeFontWorkspace(FontWorkspace **workspace);                    /* 0x42EE4F */
char *LocateStreamsDirOnDisc(void);                                  /* 0x456123 */
char FindCdRomDriveByVolumeLabel(const char *label,
                                 const char *directory);              /* 0x456236 */
int SetCurrentDirOnDrive(char drive, const char *directory);          /* 0x4563A3 */
int PromptInsertCorrectCd(void);                                      /* 0x456443 */
short __stdcall OpenDataFileOrDie(const char *path);                           /* 0x45A010 */
void __stdcall CloseDataFile(unsigned short fd);                           /* 0x45A04A */
short __stdcall WriteDataFileAtOffset(unsigned short fd, int offset,
                                      unsigned int length,
                                      const void *data);                    /* 0x45A06C */
short __stdcall CreateDataFile(const char *path);                          /* 0x45A12D */
int __stdcall ReadDataFileAtOffset(unsigned short fd, int offset,
                                   unsigned int length, void *data);        /* 0x45A19B */
int __stdcall SeekDataFile(unsigned short fd, int offset,
                           unsigned int origin);                            /* 0x45A259 */
int MeasureScaledIntroTextWidth(const char *text, short scale);            /* 0x41687F */
int DrawCenteredScaledIntroText(const char *text, short centreX,
                                short baselineY, short scale);             /* 0x4087FF */
short GetLineLength(const char *text);                                  /* 0x42C0A2 */
int print_subtitle(Viewport *viewport, short colour,
                    const char *text);                                  /* 0x42C0F1 */
int advance_canned_sequence(short obj);                                 /* 0x4207B0 */
unsigned int update_canned_sequence(short obj);                        /* 0x438711 */
void __stdcall SplitGameClockTicks(unsigned char *parts);                  /* 0x437760 */
void MonoDebug_install(void);                                           /* 0x4377F0 */
void MonoDebug_remove(void);                                            /* 0x4378D9 */
void SoundDebugPrintf(const char *fmt, ...);                          /* 0x42F0B3 */
void MonoDebug_print(const char *text);                                  /* 0x437983 */
void ReadPerformanceCounter(LARGE_INTEGER *p);                           /* 0x4379D4 */
void __stdcall ResetStringBuilder(TextContext *context);               /* 0x40A27A */
void visit_the_cinema(int view, int obj, short frames);                  /* 0x4228B0 */
unsigned int player_wingman(short obj);                                  /* 0x422953 */
void set_speed(short obj, short speed);                                  /* 0x422990 */
void auto_position(short obj, short *formationSlot);                     /* 0x4229B9 */
void auto_pilot_sequence(void);                                          /* 0x422B1C */
unsigned short __stdcall LoadPaletteTripletsFile(const char *path);     /* 0x459BC8 */
unsigned int ejection_sequence(void);                                    /* 0x44EBCA */
void stranded_sequence(void);                                            /* 0x4251F2 */
unsigned int ParseFaceAnimation(char *text, short *commands);           /* WC2 unmapped */
unsigned int ParseMouthAnimation(char *text, short *commands);          /* 0x42ABCE */
char *AddPCName(const char *text);                                      /* 0x452930 */
unsigned int LoadFace(short face);                                      /* 0x4099A8 */
unsigned int LongTalk(unsigned char *talker, char *text,
                      short *mouthCommands, short *faceCommands,
                      short duration);                                 /* WC2 unmapped */
unsigned int CloseTalk(unsigned char *talker, short mouthFrame,
                       short faceFrame);                               /* 0x4098F2 */
unsigned int Briefing(short series, short mission);                    /* 0x4416D5 */
unsigned int DeBriefing(short series, short mission);                  /* 0x424D4D */
unsigned int Office(void);                                             /* 0x409B80 */
unsigned int LoadBriefingData(short series, short mission);             /* 0x40C1DA */
unsigned int LoadMissionData(short series, short mission);              /* 0x4156A7 */
unsigned int UpdateMap(char *text, short duration);                     /* WC2 unmapped */
unsigned int CloseLook(unsigned char *shape, short shot,
                       short *animation, char *text, short duration,
                       short unused);                                  /* WC2 unmapped */
void SetShipAiScratchWord(unsigned short v);                                 /* 0x440606 */
void maneuver_complete(short ship);                                     /* 0x44061B */
void Mline_up_drop(short ship, short target);                            /* 0x440642 */
void Mwabble(short ship);                                                /* 0x4406BA */
void advance(short ship);                                               /* 0x439559 */
void ShipAiState35(short ship, short target);                            /* WC2 unmapped */
void Mfull_ahead(short ship);                                           /* 0x44151B */
void Mchill(short ship, short target);                                  /* WC2 unmapped */
void Mdrop_a_mine(short ship);                                          /* 0x441837 */
void Mthink(short ship);                                                /* 0x433E8C */
void Mtight_loop(short ship);                                           /* 0x44086B */
void Mhard_break(short ship);                                           /* 0x440967 */
void Msit_n_spin(short ship, short target);                             /* 0x440A93 */
void Mturn_n_spin(short ship, short target);                            /* 0x440D0B */
void Mburnout(short ship, short target);                                /* 0x440E46 */
void Mkickit(short ship);                                               /* WC2 unmapped */
void Mturn_n_kick(short ship);                                          /* 0x44145A */
void Mroll_over(short ship);                                            /* WC2 unmapped */
void Mhard_turn(short ship);                                            /* 0x441146 */
void Mfish_hook(short ship, short target);                              /* 0x4411E1 */
void Mtry2tail(short ship, short target);                               /* 0x4410C0 */
void Msplit_left(short ship);                                           /* WC2 unmapped */
void Msplit_right(short ship);                                          /* WC2 unmapped */
void Mgloat(short ship);                                                /* 0x4415DC */
void Mtail_fire(short ship, short target);                              /* WC2 unmapped */
void Mzip_past(short ship, short target);                               /* 0x4418B2 */
void Mtarget_missile(short ship, short target);                         /* 0x441C01 */
void Mram_missile(short ship, short target);                            /* 0x44199B */
void Mbuzz_debris(short ship);                                          /* 0x441A13 */
void Mstrafe_enemy(short ship, short target);                           /* 0x45BF30 */
void Mbest_strafe(short ship, short target);                            /* 0x441BC1 */
void Msit_n_fire(short ship, short target);                             /* 0x441D00 */
void Mstrafe_n_roll(short ship, short target);                          /* 0x441D71 */
void Mkill_missile(short ship, short target);                           /* 0x441DB3 */
void Msuicide_run(short ship, short target);                            /* 0x441E9C */
void Mget_distance(short ship, short target);                           /* 0x441EC8 */
void general_zig(short ship, unsigned int target, short pitch);         /* 0x441F86 */
void Mzig_zag(short ship, unsigned int target);                         /* 0x44210C */
void Mzig_zag_pitch(short ship, unsigned int target);                   /* 0x442129 */
void Mcorkscrew(short ship);                                            /* 0x442146 */
void Mveer_away(short ship, short target);                              /* 0x442252 */
void ShipAiState44(short ship);                                         /* 0x4423E2 */
void Mtarget_laser(short ship, short target);                           /* WC2 unmapped */
void Mrout_me(short ship);                                              /* WC2 unmapped */
void Mnone(void);                                                       /* WC2 unmapped */
void Mreset(short ship);                                                /* 0x4424CD */
void perform_maneuver(short obj);                                       /* 0x4424E4 */
short __stdcall GetShapeFrameExtent(short x, short y,
                                    unsigned char *shape, short frame,
                                    short extent);                       /* 0x447170 */
unsigned int AnimateScrambleWalk(short ticks);                          /* WC2 unmapped */
unsigned int PlayScrambleHangarScene(void);                            /* 0x460650 */
unsigned int DrawScrambleActor(short actorIndex);                      /* WC2 unmapped */
unsigned int ConfigureScrambleActor(short x, short y, short deltaX,
                                    short deltaY, unsigned char *shape,
                                    short scale, short angle,
                                    signed char flip,
                                    short actorIndex);                 /* WC2 unmapped */
unsigned int DrawScrambleFrame(void);                                  /* 0x42C7BC */
unsigned int scramble(void);                                           /* 0x429423 */
unsigned int landing(signed char damageLevel);                         /* 0x42EF12 */
unsigned int funeral_player(void);                                     /* WC2 unmapped */
unsigned int funeral_wingman(char *text, short duration);              /* WC2 unmapped */
unsigned int funeral_sequence(int playerFuneral);                      /* 0x459D74 */
unsigned int RunAnimationDemoLoop(signed char animation);             /* 0x409C1A */
void cruise_home(short obj);                                            /* 0x442770 */
void fail(short obj);                                                   /* WC2 unmapped */
void coming_home(short obj);                                            /* 0x43A8EA */
void run_away(short obj);                                               /* 0x442B78 */
short check_engage_target(short obj);                                   /* 0x442CC3 */
short check_destroy_target(short obj);                                  /* 0x442D58 */
void maneuvering(short obj, short newTarget);                           /* 0x442EFC */
void formation_burst(short obj);                                        /* 0x442F2D */
void disallow_engage(void);                                             /* 0x442FE9 */
void allow_engage(void);                                                /* 0x442FFD */
void try2allow_engage(int pilotLevel);                                  /* 0x44301A */
void imperial_formation(short obj);                                     /* 0x443095 */
void formation_break(short obj);                                        /* 0x4432EB */
void imperial_wingman(short obj);                                       /* 0x4433B1 */
void kilrathi_wingman(short obj);                                       /* 0x443499 */
void wingman_mission(short obj);                                        /* WC2 unmapped */
short dist_from_home(short obj);                                        /* 0x44366D */
short scan_and_lock(short obj, int scanRange,
                    enum ShipTactic newTactic);                         /* 0x443698 */
void patrol_area(short obj);                                            /* 0x443707 */
void kilrathi_patrol(short obj);                                        /* 0x442455 */
void imperial_wingleader(short obj);                                    /* WC2 unmapped */
void cruise_to_destination(short obj);                                  /* 0x443A08 */
void prepare_for_jump(short obj);                                       /* 0x443C45 */
void accelerate_and_jump(short obj);                                    /* 0x42D4C1 */
void reach_warp(short obj);                                             /* 0x443DBD */
void warp_arrival(short obj);                                           /* 0x443EE9 */
void return_to_buddy(short obj, short buddy);                           /* 0x443F28 */
void escort_buddy(short obj, short buddy);                              /* 0x443FA2 */
void escort_mission(short obj);                                         /* 0x443FE4 */
void check_goal(short obj);                                             /* 0x444199 */
void streak_toward(short obj, short goal, short range);                 /* 0x4441E5 */
void approach_and_engage(short obj, short goal);                        /* 0x444291 */
void strike_mission(short obj);                                         /* 0x4429C0 */
void return_to_master(short obj, short master);                         /* 0x4446C9 */
void defend_mission(short obj);                                         /* 0x44472C */
void rendezvous_mission(short obj);                                     /* 0x444966 */
void ship_intelligence(short obj);                                      /* 0x444ADC */
void orbit_sphere(short obj);                                           /* 0x444C80 */
void tanker_intelligence(short obj);                                    /* 0x444D5C */
void destroyer_intelligence(short obj);                                 /* 0x444E51 */
void stationary_intelligence(short obj);                                /* 0x42A876 */
void capital_ship_intelligence(short obj);                              /* 0x441742 */
void futurion_intelligence(short obj);                                  /* 0x445D9A */
void mine_intelligence(short obj);                                      /* 0x446474 */
void heat_seeking_missile_intelligence(short obj);                      /* 0x445E68 */
void FF_missile_intelligence(short obj);                                /* 0x446133 */
void set_sphere_point(const MissionShipRecord *record,
                      FixedVector *position);                           /* 0x44B200 */
unsigned int is_alive(int pilot);                                       /* 0x44B239 */
unsigned int check_futurion(short i);                                   /* 0x44B257 */
unsigned int init_mission(short series, short mission);                 /* 0x44B2E2 */
void prepare_mission(void);                                            /* WC2 unmapped */
int release_all_capital_ship_shapes(void);                             /* 0x456988 */
int release_capital_ship_shapes(enum ObjectType type);                 /* WC2 unmapped */
int load_ship(enum ObjectType type, short slot);                       /* 0x444420 */
int free_ship(short slot);                                             /* 0x44BA73 */
int free_all_slots(void);                                              /* 0x4175AD */
int load_all_slots(void);                                              /* 0x44CCE1 */
void remove_nav_point_objects(void);                                   /* 0x44CDCF */
short get_shape_slot(void);                                            /* 0x43B258 */
int shape_loaded(enum ObjectType type);                                /* 0x43A1EB */
int shape_needed(const MissionNavPoint *navPoint,
                 enum ObjectType type);                                /* 0x43A251 */
int new_sphere_shapes(MissionNavPoint *navPoint);                      /* 0x428EDC */
int set_up_action_sphere(short navPoint);                              /* 0x440F76 */
void free_pilot_talk(short personality);                               /* WC2 unmapped */
void get_pilot_talk(short personality);                                /* 0x434043 */
unsigned int init_personalities(void);                                 /* WC2 unmapped */
unsigned int room_for_me(short obj, short minimum);                    /* WC2 unmapped */
void approve_xyz(short obj, short minimum, short maximum);             /* 0x44D81F */
void set_up_next_wave(void);                                           /* 0x44D888 */
unsigned int sub_int_vector(const ShortVector *left,
                            const ShortVector *right,
                            ShortVector *difference);                  /* 0x448D1A */
unsigned int set_formation_position(short obj,
                                    const MissionShipRecord *record);  /* 0x44DA2F */
void Set_up_ship_info(short obj, short missionShip,
                      signed char navPoint);                           /* 0x44DB7E */
unsigned int is_team_member(short missionShip);                        /* 0x4380FD */
unsigned int find_next_ship_turn_slot(short obj);                     /* 0x43AA3F */
short init_ship(short missionShip, short navPoint);                    /* 0x44E187 */
unsigned int init_intelligence_data(short obj);                       /* 0x44E44F */
short __stdcall SampleBothJoysticks(InputDeviceSample *samples,
                                    unsigned int fallback);             /* 0x44F1F0 */
int __stdcall SampleJoystickDevice(InputDeviceSample *samples,
                                   short joystick,
                                   unsigned int fallback);              /* 0x4388F5 */
void SampleActiveJoystickDevice(void);                                 /* 0x461922 */
void DrawNavTextLine(unsigned char alignment, unsigned short colour,
                     const char *format, ...);                         /* 0x44FA70 */
void SetNavMapCoordinateScaling(short enabled);                       /* 0x44FADA */
void ScaleNavMapMarkerSize(short *size);                              /* 0x44760F */
void ScaleNavMapCoordinates(short *x, short *y,
                            short mapX, short mapY);                  /* 0x44FB19 */
void nav_getxy(short *x, short *y, int worldX, int worldZ);           /* 0x44FB68 */
void CheckPoint(short x, short y);                                    /* 0x44FBC8 */
void IncludeNavMapWorldPoint(int worldX, int worldZ);                  /* 0x44FC37 */
void SetScale(void);                                                   /* 0x44FC6D */
void Build_objective_list(void);                                       /* 0x44FE9F */
short NavMapPointInsideReservedArea(short area, short x, short y);    /* WC2 unmapped */
short NavMapLabelFits(short x, short y, short width, short height);   /* 0x4501B2 */
short NavMapLabelPositionAvailable(short x, short y,
                                   short width, short height);        /* 0x450210 */
void ResetNavMapReservedAreas(void);                                  /* WC2 unmapped */
void ReserveNavMapArea(short x, short y, short width, short height);  /* 0x43A29E */
void ResetNavMapLabels(void);                                         /* WC2 unmapped */
short TryPlaceNavMapLabel(short x, short y, short width,
                          short force);                               /* 0x4503BB */
void PlaceNavMapLabel(short x, short y, unsigned short colour,
                      const char *text);                             /* 0x450458 */
void AddUniqueObjectiveNavLabel(short x, short y,
                                unsigned short colour, const char *text,
                                short objective, short missionShip);  /* 0x4506F8 */
short IsPointInNavMapLabel(short label, short x, short y);            /* 0x4507DA */
void DrawNavMapLabels(void);                                          /* 0x45087F */
void DrawNavRectangleMarker(short x, short y, short size, short shadow,
                            unsigned short colour, short reserve);    /* 0x450900 */
void DrawNavSquareOutline(Viewport *viewport, short x, short y,
                          short size, signed char colour);            /* 0x43C570 */
void DrawNavSquareMarker(short x, short y, short size,
                         short shadow, short colour,
                         short reserve);                              /* 0x450A12 */
void DrawNavTriangleOutline(Viewport *viewport, short x, short y,
                            short size, signed char colour);          /* 0x450E98 */
void DrawNavTriangleMarker(short x, short y, short size,
                           short shadow, unsigned short colour,
                           short reserve);                            /* 0x450BBA */
void DrawNavCrossMarker(short x, short y, short size,
                        short shadow, unsigned short colour,
                        short reserve);                               /* 0x450C2A */
void SetScreenClipRect(unsigned short a, unsigned short b, unsigned short c, unsigned short d);/* 0x450DF6 */
void DrawNavHazardMarker(FixedVector navPosition, FixedVector offset,
                         short size, unsigned short markerColour,
                         unsigned short textColour,
                         const char *text);                           /* 0x450E1E */
void DrawNavPlayerMarker(short colour, short reserve);                /* 0x450F1E */
void BuildMap(short showPlayer);                                      /* 0x450FE2 */
void UpdateInflightNavText(short showColon);                          /* 0x45153A */
void FormatNavCoordinates(unsigned char *out);                                 /* 0x451637 */
void DrawSelectedNavLegendEntry(const char *text, short navPoint);    /* 0x418D14 */
void DrawNavMapLegend(void);                                          /* 0x4516C0 */
char *nav_note(short objective);                                      /* 0x451756 */
void DrawNavLocationReadout(const char *title, short showFlightData); /* 0x45180F */
void BriefingMap_LoadShapes(void);                                    /* 0x451AD3 */
void BriefingMap_DisplayMap(void);                                      /* 0x451B70 */
short SelectNavObjectiveAtPoint(short mouseX, short mouseY);          /* 0x451C5C */
void CentreMouseOnCurrentNavObjective(void);                         /* 0x451DDB */
void ShowConfedNavScan(void);                                        /* 0x451E57 */
void InflightComputer(void);                                         /* 0x451FAA */
unsigned short MergeAdjacentNearHeapBlocks(int descriptorAddress);   /* 0x4397D5 */
int ReleaseNearHeapBlock(int descriptorAddress);                     /* 0x420874 */
void PurgeNearHeapBlocks(unsigned short flags);                       /* 0x4208FC */
unsigned short InitializeNearHeap(void);                              /* 0x4209C2 */
void *AllocateNearHeapBlockFromEnd(int size, unsigned short flags);   /* 0x420B88 */
void *AllocateNearHeapBlockByFlags(int size, unsigned short flags);   /* 0x420E16 */
void add_statistics(short pilot, short missions, short kills);         /* WC2 unmapped */
void PostMission(void);                                                 /* 0x42BB70 */
int FullMissionScore(void);                                             /* WC2 unmapped */
int PlayersMissionScore(void);                                          /* WC2 unmapped */
unsigned int UpdateSeries(void);                                        /* 0x4651B7 */
unsigned int MoveNewCampaign(void);                                     /* WC2 unmapped */
unsigned int StartNewCampaign(short campaign);                         /* 0x4407DA */
short GameFlow(void);                                                   /* 0x46591A */
void __stdcall free_viewport(Viewport *viewport);                      /* 0x4471B0 */
unsigned short GetPaletteReadyUnused(void);                                    /* WC2 unmapped */
void DrawTitleLogo(short distance, short y);                            /* WC2 unmapped */
void UpdateTitleMenuCursor(void);                                      /* WC2 unmapped */
int Title_Sequence(void);                                               /* 0x42F1FD */
void CalibrateJoystickInteractive();                                  /* 0x418770 */
void WaitForJoystickButtonRelease(void);                               /* WC2 unmapped */
void WaitForJoystickButtonPress(void);                                 /* WC2 unmapped */
void SetFleetOverviewView(int initializeCockpit);                     /* 0x44ADE0 */
unsigned int rotate_eye_to_goal(void);                                 /* 0x41F2C5 */
short GetVectorMagnitude(const FixedVector *vector);                   /* 0x419CDC */
unsigned int set_eye_direction_and_position(void);                     /* 0x419D01 */
unsigned int force_view(int view, short obj);                          /* 0x41AD51 */
unsigned int new_view(int view, short obj);                            /* 0x41AE40 */
unsigned int start_dust(short obj, FixedVector origin,
                        short forwardDistance, int rightOffset,
                        int upOffset);                                 /* 0x41B62E */
unsigned int generate_stars(void);                                    /* 0x41B762 */
unsigned int update_star_field(void);                                 /* 0x41B8D6 */
short count_down(short obj);                                          /* 0x44804A */
unsigned int house_keep_objects(void);                                 /* 0x41BD81 */
unsigned int update_objects_in_space(void);                            /* 0x41C7ED */
unsigned int rotate_object(short obj);                                 /* 0x41CB3C */
unsigned int accelerate_and_move_object(short obj);                    /* 0x41CC0A */
unsigned int animate_shape(short obj);                                 /* 0x41D07B */
unsigned int animate_object(short obj);                                /* 0x41D2DA */
unsigned int hit_asteroid(short asteroid, short destructionChance);    /* 0x41D4C7 */
int object_collision(short obj);                                      /* 0x445087 */
unsigned int object_intelligence(short obj);                           /* 0x41E276 */
void EmitTextString(void (__stdcall *writer)(int), const char *text);   /* 0x40F040 */
#ifdef WC1_SDL
void FormatTextTokens(void (__stdcall *writer)(int),
                      const char *format, va_list *arguments);         /* 0x420378 */
#else
void FormatTextTokens(void (__stdcall *writer)(int),
                      const char *format, unsigned long *arguments);    /* 0x420378 */
#endif
void DrawFormattedText(const char *format, ...);                        /* 0x461D02 */
void FormatTextBufferFromStart(const char *format, ...);                /* 0x40D6B0 */
void AppendFormattedText(const char *format, ...);                      /* 0x4206F2 */
void FatalErrorAndExit(const char *format, ...);                       /* 0x4380B0 */
unsigned short IsCockpitExplosionActive(void);                         /* WC2 unmapped */
void EraseCockpitReadoutRegion(Viewport *viewport, short left,
                               short top, short right, short bottom,
                               short colour);                         /* 0x438129 */
void vdu_polygon(signed char bar, short percent);                     /* 0x438194 */
unsigned int InitializeCockpitReadout(signed char slot,
                                      TextContext *context);          /* 0x438454 */
void DrawCockpitReadout(signed char slot, const char *text);          /* 0x4384AD */
void EraseCockpitReadoutAtPosition(signed char slot, short left,
                                   short top);                        /* WC2 unmapped */
short DrawHudMessageSlot(HudMessageSlot *slot);                        /* 0x4385B3 */
void ClearHudMessageSlot(HudMessageSlot *slot);                        /* 0x4387FA */
void ClearHudMessageIfMatching(HudMessageSlot *slot,
                               const char *text);                     /* 0x4387A7 */
void ClearHudGunReadouts(void);                                             /* WC2 unmapped */
void SetHudMessageSlot(HudMessageSlot *slot, TextContext *context,
                       short x, short y, const char *text,
                       unsigned short colour,
                       signed char flashCount);                        /* 0x40A51D */
void UpdateMessage(HudMessageSlot *slot);                              /* 0x438864 */
void set_global_message(const char *text, unsigned short colour,
                        int flashCount);                               /* WC2 unmapped */
void CockpitMessage(const char *text, unsigned short colour,
                    int flashCount);                                  /* WC2 unmapped */
void remove_message(const char *text);                                /* 0x43893F */
short kilrathi_near(short obj, short range);                           /* 0x43895F */
short auto_pilot_valid(short showReason);                              /* 0x438ADD */
void *reset_cockpit(void);                                            /* 0x4213FE */
unsigned int SetCockpitLightBlink(signed char light, short interval); /* 0x438D30 */
void draw_cockpit_lights(void);                                      /* 0x438DAD */
void update_lights(void);                                            /* 0x438F62 */
void update_bars(void);                                              /* 0x439076 */
short get_mode(short i);                                              /* 0x439264 */
void set_mode(short i, int state);                                    /* 0x443D6E */
unsigned short SetVduModeIfChanged(short i, int state);               /* 0x464BFE */
int GetVduModeStackDepth(short i);                                    /* 0x43934D */
void push_mode(short i, int state);                                   /* 0x4393B3 */
void pop_mode(short i);                                               /* 0x4388B6 */
void set_new_vdu(short vdu);                                          /* 0x4393E1 */
short update_vid_disp(short vdu);                                     /* 0x447544 */
void InvalidateVduMode(short i);                                          /* 0x439500 */
void clear_message_time(void);                                              /* 0x43C5B0 */
short message_showing(void);                                          /* 0x43952E */
unsigned short set_message_time(unsigned short v);                        /* 0x46EEBD */
void check_message(void);                                             /* 0x439588 */
unsigned int update_digital_readouts(void);                          /* 0x4396BF */
void PlayTargetLockSfx(void);                                           /* 0x446CC2 */
void malf_sound(void);                                                  /* 0x439753 */
short malf(char component);                                           /* 0x439789 */
unsigned short vdu_malf(short vdu, short sound);                       /* 0x421BFC */
void ShowComponentHitHudMessage(const char *text, unsigned short colour,
                                short flashCount);                     /* 0x43984F */
int damage_your_component(char component, char amount, char maximum); /* 0x4398CB */
void RemovePlayerReleaseWeapon(signed char weapon);                  /* 0x4399C6 */
void fire_computer_graphic_missile(void);                            /* 0x439A5A */
void show_weapon_disp(void);                                         /* 0x439BB7 */
void update_status_text(void);                                        /* WC2 unmapped */
short sighted(short objective);                                       /* WC2 unmapped */
short visited(short objective);                                       /* WC2 unmapped */
short achieved(short objective);                                      /* 0x40AA37 */
void flag_objective(short objective, unsigned char flags);            /* WC2 unmapped */
void DrawCalculatingLabel(void);                                      /* 0x43A2CD */
const char *objective_name(short objective);                          /* 0x43A374 */
void show_navigation_disp(void);                                      /* 0x43A3F8 */
short hidden_objective(short objective);                              /* 0x43A474 */
int set_new_objective(short pathIndex);                               /* 0x43A75E */
short cycle_next_objective(void);                                     /* 0x43A851 */
void set_next_destination(void);                                       /* WC2 unmapped */
short LocateMobileObjective(short objective);                         /* 0x43A9D7 */
unsigned int someone_coming(void);                                     /* WC2 unmapped */
unsigned int escorting_a_ship(void);                                   /* 0x43AAC3 */
void flag_reached(short objective, short reached);                     /* 0x43AAFF */
void check_sighting(short objective, short range, short object);      /* 0x43AD61 */
void check_visit(short objective, short range);                       /* 0x43ADC4 */
void update_objective_location(short objective);                     /* 0x43AE2F */
unsigned int objective_lost(short objective);                         /* WC2 unmapped */
void check_objectives(void);                                          /* 0x43B089 */
void rotational_pos_to_scanner_pos(signed char object,
                                   const SphericalVector *position);  /* 0x43B0EB */
short mobile_objective(short objective);                              /* 0x450CF8 */
void ResetScannerContacts(void);                                     /* WC2 unmapped */
void clear_head_up_display(void);                                    /* 0x43B29D */
unsigned int set_objective_range(short showOnScanner);                /* 0x43B4CF */
short get_color(short object, unsigned short *colour);                /* 0x450623 */
unsigned int draw_3d_scanner(void);                                   /* 0x460DFE */
void start_lock(unsigned short v);                                    /* 0x43BE1B */
unsigned short starting_lock(unsigned short v);                       /* 0x43BE55 */
void lock_off(void);                                                   /* 0x43BE8D */
short CheckTargetLockMalfunction(void);                               /* 0x43BEFF */
short decrement_lock_time(short screenX);                             /* 0x43BF67 */
void target_locking(signed char target);                              /* 0x43C048 */
void SetRectBounds(Viewport *viewport, unsigned short left,
                   unsigned short top, unsigned short right,
                   unsigned short bottom);                           /* 0x43C30D */
short GetRectHeight(const Viewport *viewport);                        /* WC2 unmapped */
void print_message_text(char *text, unsigned short colour);            /* 0x433AD0 */
void ShowHudTextLine(char *s, unsigned short b);                       /* WC2 unmapped */
void SetHudTextColour(short v);                                              /* 0x465EA9 */
void draw_target_box(unsigned short colour, signed char object,
                     short solid, short drawLockMarker, short padding,
                     ShortRect *savedBounds);                         /* 0x43C6D8 */
void remove_nav_pointer(void);                                        /* 0x443E8C */
void draw_nav_pointer(void);                                          /* 0x43CBFD */
unsigned int overlay_head_up_display(void);                           /* 0x43D386 */
void RestoreCockpitExplosionIfVisible(void);                           /* WC2 unmapped */
unsigned int RestoreTransientCockpitGraphics(void);                    /* 0x4083E6 */
void SetHudMessageText(char *text, unsigned short colour,
                       unsigned short duration);                       /* 0x43D956 */
void malf_noise(short vdu, int effect, unsigned short colour,
                short sound, short refresh);                           /* 0x43D9DA */
void build_your_target_list(short *hasEnemy);                         /* 0x43DA68 */
void cycle_onscreen_targets(void);                                    /* 0x43DD1B */
void check_target(void);                                             /* 0x43DDFC */
void update_missile_warning(void);                                   /* 0x43E1B2 */
void determine_pilot_hand(void);                                      /* 0x43E297 */
void DrawPilotHandFrame(void);                                       /* 0x43E371 */
void CopyTrainSimPilotViewToRightVdu(void);                           /* 0x43F110 */
void animate_pilot(void);                                             /* 0x43E43A */
void ResetPilotHandAnimation(void);                                   /* 0x4601F1 */
void send_message(short obj, signed char message);                      /* 0x43E4A8 */
void npc_communication(void);                                        /* 0x43E5DA */
void clear_cockpit_damage(void);                                      /* 0x43E870 */
void explosion_draw(void);                                           /* 0x43E8B2 */
unsigned int DrawPendingCockpitDamage(void);                          /* 0x43E9E2 */
void RestoreCockpitExplosionBackground(void);                         /* 0x43EAE3 */
void cockpit_explosion(void);                                        /* 0x43EB46 */
void place_damage_on_cockpit(short damage);                           /* 0x43EC50 */
void vid_transmit(void);                                              /* 0x43ECD9 */
void vid_equiv(short obj, short message);                             /* 0x43F042 */
void update_dead_disp(short a);                                        /* 0x43F09D */
void check_stranded(void);                                           /* 0x43F0C3 */
void update_VDUs(void);                                              /* 0x43F2F5 */
void update_cockpit(void);                                           /* 0x43F6C9 */
void PlayCockpitSelectionSfx(short selectionSound);                  /* 0x43F849 */
void vdu_pop_all(short vdu);                                         /* 0x43F862 */
void SelectCockpitVduMode(short vdu, int mode);                       /* 0x43F8CD */
short __stdcall MeasureTextPixelWidthClamped(const char *text);         /* 0x453B30 */
int __stdcall SeekPacketSection(PacketSectionHandle *handle, int offset,
                                short origin);                         /* 0x4389FE */
unsigned short GetMusicDriverPresent(short mode);                              /* WC2 unmapped */
short __stdcall CollectActivePaletteIndices(Viewport *viewport,
                                             unsigned char *indices,
                                             short capacity);          /* 0x40ED9F */
short get_ship_max_velocity(short obj);                                /* 0x428E5C */
short recalc_max_velocity(short ship);                                /* 0x40A481 */
void drain_fuel(short ship, short amount);                            /* WC2 unmapped */
void damage_ion_drive(short ship, short amount,
                      short maximum);                                 /* WC2 unmapped */
int GetShipAccelerationRate(short ship);                          /* 0x40A5A0 */
void point_at(short obj, FixedVector point);                      /* 0x40A691 */
void look_at(short obj);                                         /* 0x40A6F7 */
void position_relative(FixedVector *position, FixedVector direction,
                       short distance);                          /* 0x40A725 */
void position_relative_ijk(FixedVector *position, short obj,
                           short right, short up, short forward); /* WC2 unmapped */
short FixedToShortSaturating(int value);                           /* 0x40A81B */
int MinInt(int a, int b);                                        /* 0x40A867 */
int MaxInt(int a, int b);                                        /* 0x40A893 */
int AbsInt(int v);                                               /* 0x40A8BF */
int intfract_sign(int sign, int magnitude);                       /* WC2 unmapped */
unsigned short SignShort(short v);                                  /* WC2 unmapped */
unsigned int SignFixed(int v);                                      /* 0x40A8EB */
short WrapDegrees(short degrees);                                     /* 0x40A925 */
int equ_vector(const FixedVector *left, const FixedVector *right);     /* 0x464072 */
int IsPairEqualityDifferentFromFlag(const unsigned int *values);      /* WC2 unmapped */
void zero_vector(FixedVector *vector);                                /* 0x40AA0B */
void negate_vector(FixedVector *vector);                              /* 0x43951A */
void AddFixedVectors(const FixedVector *left, const FixedVector *right,
                     FixedVector *sum);                               /* 0x40AA6A */
void SubtractFixedVectors(FixedVector *left, FixedVector *right,
                          FixedVector *difference);                    /* 0x40AAA8 */
void ComputeVectorDelta(FixedVector *from, FixedVector *to,
                        FixedVector *delta);                            /* 0x40AAE6 */
void ScaleFixedVector(FixedVector *vector, int scale,
                      FixedVector *result);                            /* 0x4118A9 */
void divide_vector(FixedVector *vector, int divisor,
                   FixedVector *result);                              /* 0x40AB78 */
short ChooseRandomSignedMagnitude(short minimum, short maximum,
                                  int allowNegative);                  /* WC2 unmapped */
void MakeRandomVectorFixed(short minimum, short maximum,
                           FixedVector *vector);                       /* 0x40AC24 */
void FillFixedVectorWithRandomComponents(short limit,
                                         FixedVector *vector);          /* 0x40AC88 */
void random_radial(const FixedVector *center, short radius,
                   FixedVector *position);                           /* 0x413A3B */
void MakeRandomNormalizedVector(FixedVector *vector);                 /* WC2 unmapped */
void rectangular_to_spherical(const FixedVector *rectangular,
                              SphericalVector *spherical);       /* 0x40ACE0 */
void ConvertShortVectorToFixedVector(const ShortVector *source,
                                     FixedVector *destination);       /* WC2 unmapped */
void ConvertFixedVectorToShortVector(const FixedVector *source,
                                     ShortVector *destination);       /* WC2 unmapped */
int dot_product(const FixedVector *left, const FixedVector *right);    /* 0x40AB24 */
short vector_angle(FixedVector left, FixedVector right);              /* 0x40AFFA */
void vector_cross_product(const FixedVector *left,
                          const FixedVector *right,
                          FixedVector *product);                 /* 0x40AE70 */
short NormalizeFixedVector(FixedVector *vector);                      /* 0x40AF18 */
int vector_length_in_dir(const FixedVector *vector,
                         const FixedVector *direction);          /* 0x40ACA5 */
void vector_component_in_dir(const FixedVector *vector,
                             const FixedVector *direction,
                             FixedVector *component);            /* 0x401FDD */
void rotate_about_i(short angle, FixedVector *j,
                    FixedVector *k);                            /* 0x40B17F */
void rotate_about_j(short angle, FixedVector *i,
                    FixedVector *k);                            /* 0x40B02F */
void rotate_about_k(short angle, FixedVector *i,
                    FixedVector *j);                            /* 0x40B2CF */
void init_ijk(short obj);                                       /* 0x40B41F */
void copy_frame(short source, short destination);                    /* 0x449FAE */
void fix_objects_ijk(short obj);                                 /* 0x40B5DF */
void transform_to_objects_frame(const FixedVector *source,
                                FixedVector *destination,
                                short obj);                           /* 0x418426 */
void alter_pitch(short angle, short obj);                        /* 0x40B747 */
void alter_yaw(short angle, short obj);                          /* 0x40B706 */
void alter_roll(short angle, short obj);                         /* 0x40B788 */
short distance_between_points(const FixedVector *from,
                              const FixedVector *to);                  /* WC2 unmapped */
short distance_from_point(short obj, const FixedVector *point);       /* 0x40B7C9 */
short distance_from_object(short obj, short other);                   /* 0x40B85C */
void get_facing_range_from_point(short obj, const FixedVector *point);/* 0x40B92F */
void get_facing_range_from_object(short obj, short other);            /* 0x40A3A0 */
void ship_vs_point(short obj, const FixedVector *point);              /* 0x40B898 */
void ship_vs_ship(short obj, short other);                             /* 0x42A222 */
short facing_to_object(short obj, FixedVector *point);                /* 0x40B9B5 */
short match_roll_orientation(short obj, short reference);             /* 0x40BA2E */
int set_ship_rotation_goals(short obj, short turnRate,
                            const FixedVector *direction,
                            short pointingMode,
                            short *yawGoal, short *pitchGoal);         /* 0x40BADB */
void point_ship(short obj, short turnRate,
                const FixedVector *direction);                        /* 0x40BC6B */
void point_ship_at_point(short obj, const FixedVector *point);        /* 0x40B805 */
void point_ship_at_object(short obj, short other);                    /* 0x40BCE8 */
void point_capital_ship_at_object(short obj, short other);            /* 0x40BCA9 */
void point_ship_behind_object(short obj, short other);                /* 0x41EBC1 */
void point_ship_below_object(short obj, short other);                 /* 0x40BE3A */
void point_perpendicular_to_point(short obj,
                                 const FixedVector *point);           /* 0x40BD58 */
void point_perpendicular(short obj, short other);                     /* 0x40BEB0 */
void point_parallel(short obj, short other);                           /* 0x462527 */
void MoveObjectAlongDirection(short obj, const FixedVector *direction,
                              short distance);                        /* 0x42A823 */
void NormalizeAndScaleVector(FixedVector *vector, int scale);         /* 0x40BF0B */
void SetVectorFixedPoint(unsigned int *p, short v);                           /* 0x40BF36 */
unsigned int IsPointWithinRange(FixedVector *from, FixedVector *to,
                                short range);                          /* 0x40BF55 */
short check_for_collision(short obj);                                 /* 0x4569C8 */
void position_child(short parent, short hardpoint,
                    FixedVector *position);                       /* 0x40B550 */
void child_object(short hardpoint, short child, short parent);    /* 0x425B9E */
short get_ship_slot(void);                                        /* 0x40C211 */
short find_vacant_3d_object(void);                                /* 0x40C266 */
void remove_object(short obj);                                        /* 0x40C2C9 */
void apply_force_to_objects_center(FixedVector *force, short obj);    /* 0x40C46C */
void apply_force_to_object(FixedVector *point, FixedVector *force,
                           short obj);                                /* 0x40C4D1 */
void rotational_acceleration(FixedVector *point, FixedVector *force,
                             short obj);                              /* 0x40C796 */
void ClampVectorTo30(short *p);                                           /* 0x464055 */
void ClampTo30(short *p);                                           /* 0x40C99F */
unsigned short IsPointWithinEyeViewCone(const FixedVector *point);   /* 0x43CF5A */
void transform_objects_to_your_view(void);                             /* 0x40C9DD */
void set_background_objects_rotation(short obj,
                                     FixedVector *direction);    /* 0x40CEF6 */
void get_right_shape(short obj, FixedVector *direction);          /* 0x40CFF8 */
short InitializeModalTextPanel(ModalTextPanel *panel, short fontIndex,
                               unsigned int topLeft,
                               unsigned int bottomRight,
                               short clearColour,
                               unsigned char backgroundColour,
                               short borderColour);                    /* 0x4593D6 */
void DrawModalTextPanel(ModalTextPanel *panel, short x, short y,
                        unsigned char alignment,
                        const char *format, ...);                      /* 0x42C725 */
void RestoreModalTextPanel(ModalTextPanel *panel);                     /* 0x4266B1 */
short ShowModalTextPanel(short fontIndex, const char *format, ...);    /* 0x4595EF */
void ReleaseModalTextPanel(void);                                      /* 0x45979B */
short AnySavedGames(void);                                             /* WC2 unmapped */
void CreateEmptySaveGameFile(void);                                    /* 0x40230E */
void EnsureSaveGameFile(void);                                         /* 0x446E9D */
void InitializeBarracksAnimation(BarracksAnimationState *state);       /* WC2 unmapped */
void FreeBarracksMenuLabel(char **label);                              /* WC2 unmapped */
void SetAwakenBarracksMenuLabel(char **label, int series, int mission,
                                char *description);                    /* 0x44F332 */
void FreeBarracksMenuLabels(void);                                     /* WC2 unmapped */
short SaveGame(short slot, SaveGameRecord *gameRecord);                /* 0x402385 */
short PromptForTextInput(short x, short y, const char *prompt,
                         char *destination, short maximumLength,
                         short inputMode);                             /* 0x42E3CF */
int WarnLoadGameFirst(void);                                           /* 0x4346E9 */
void SaveGameWithNamePrompt(short slot, CampaignState *campaign,
                            PilotRecord *pilots,
                            MissionObjective *objectives);             /* WC2 unmapped */
int LoadGame(short slot, SaveGameRecord *gameRecord);                  /* 0x446F14 */
void LoadGameFromSlot(short slot, CampaignState *campaign,
                      PilotRecord *pilots,
                      MissionObjective *objectives);                   /* 0x434ACE */
void SetBunkMenuLabel(short occupied, short side, char **label,
                      int series, int mission, char *description);     /* WC2 unmapped */
void GetBunkInfo(BarracksAnimationState *state);                       /* 0x433ED7 */
void DrawBarracksBunks(Viewport *viewport, unsigned char *shape,
                       BarracksAnimationState *state);                 /* 0x46138D */
void DrawBarracksStaticDetails(Viewport *viewport,
                               unsigned char *shape);                  /* 0x467300 */
void AnimateBarracks(Viewport *viewport, unsigned char *shape,
                     BarracksAnimationState *state);                   /* 0x40815F */
int ConfirmQuitWingCommander(void);                                   /* 0x434F7D */
int ConfirmAwakenAfterBadData(short slot);                             /* 0x436722 */
int ConfirmReplaceFaultyData(short slot);                              /* 0x43641C */
void HandleBarracksBunkSelection(Viewport *viewport,
                                 unsigned char *shape,
                                 BarracksAnimationState *state,
                                 short region);                        /* 0x418DAA */
void UpdateBarracksScreen(Viewport *viewport, unsigned char *shape,
                          BarracksAnimationState *state);              /* WC2 unmapped */
short BarracksScreen(void);                                            /* 0x419831 */
unsigned short __stdcall StepPaletteTransition(short *current,
                                                const short *target,
                                                short componentCount); /* 0x4225A0 */
char *__stdcall DosStrcat(char *destination, const char *source);      /* 0x446910 */
DWORD WINAPI DebugOverlayWorkerProc(void *parameter);                  /* 0x45AEE4 */
LRESULT CALLBACK DebugKeyboardHookProc(int code, WPARAM key,
                                       LPARAM flags);                  /* 0x42E692 */
void DebugOverlayPrintf(DebugOverlayConsole *console,
                        const char *format, ...);                      /* 0x45B0BB */
short MinShort(short a, short b);                                       /* 0x46F3D0 */
short MaxShort(short a, short b);                                       /* 0x40F072 */
void FreePacketAndClear(void *slot, unsigned short releaseFlags);      /* 0x40F0A4 */
void ReportPacketLoadError(void *packet, short logicalFile,
                           short retry, short section,
                           const char *sourceTag);                      /* 0x40F0D2 */
void *LoadPacketIntoBuffer(short logicalFile, short section,
                           void *destination);                         /* WC2 unmapped */
void *LoadPacketAllocated(short logicalFile, short section);           /* 0x40F49D */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              unsigned short flags);                    /* 0x40F5B6 */
unsigned int InitializeTextContextFromFont(TextContext *context,
                                           short fontIndex,
                                           unsigned char colour,
                                           signed char background);    /* 0x42067F */
unsigned int ReleaseTextFont(short fontIndex);                         /* 0x459082 */
unsigned int DrawTextAt(TextContext *context, short x, short y,
                        const char *text,
                        unsigned char alignment);                     /* 0x40F96E */
unsigned int SortSignedByteValuesAscending(signed char *values,
                                           short count);                /* 0x410680 */
short OpenDiskDataFile(short logicalFile);                              /* 0x4342E8 */
void __stdcall PromptInsertNumberedDisk(short logicalFile);             /* 0x437A44 */
unsigned int GetZeroUnused(void);                                        /* WC2 unmapped */
short CheckEscaped(void);                                               /* 0x43D81F */
short WaitForInputKey(void);                                         /* 0x40FA2C */
void WaitForSceneAdvance(short duration, short unused);                /* 0x409120 */
void MoveMenuPointerFromKeyboard(InputEventState *event);               /* 0x421E6D */
void EraseLastTextInputCharacter(void);                              /* 0x4345A1 */
short WaitForStreamInputKey(void);                                  /* 0x43456E */
short initialize_object(short obj, enum ObjectType type,
                        short owner);                                  /* 0x4103A6 */
short borrow_dust(void);                                             /* 0x41040D */
short new_object(enum ObjectType type, short owner);                 /* 0x4105BF */
short initialize_ship(enum ObjectType type, short owner);            /* 0x41062D */
short any_selected(unsigned char *loadout,
                   enum ObjectClass objectClass);                    /* 0x452D66 */
unsigned int remove_weapon(short obj, short weapon);                 /* WC2 unmapped */
void set_objects_data(short obj, enum ObjectType type,
                      short owner);                                  /* 0x410999 */
unsigned int match_rotation_goal(short *rotation, short *goal,
                                 short totalError, short rate);         /* 0x411172 */
void rotate_object_to_goal(short obj);                                  /* 0x41133D */
unsigned int check_for_lost_control(short obj);                        /* 0x42B15A */
unsigned int celerate(short ship, int delta);                     /* 0x4117AC */
unsigned int approach_speed(short ship, int targetSpeed);         /* 0x41181D */
unsigned int steady_object(short ship);                                 /* 0x4728E0 */
short real_velocity(short obj);                                       /* 0x4118F2 */
unsigned int fix_velocity(short obj);                                 /* 0x411922 */
unsigned int sort_viable_target_list(void);                           /* 0x43CE8F */
void send_appropriate_message(short attacker, short victim);         /* 0x411A50 */
int inflict_damage(short attacker, short victim, short damage,
                   const FixedVector *impactDirection);                /* 0x411C72 */
short pilot_hit(short obj);                                           /* 0x41262D */
int onboard_explosion(short obj);                                     /* 0x41270A */
void call_enemy(short obj);                                           /* 0x41280B */
int internal_damage(short attacker, short victim, short damage,
                    short quadrant);                                  /* 0x4128A7 */
void revise_shields(short obj);                                       /* 0x412F03 */
int your_internal_damage(short attacker, short damage,
                         short quadrant);                             /* 0x413099 */
short ReportComponentRepaired(short component, short minimumDamage);  /* 0x413578 */
void repair_internal_damage(void);                                   /* 0x4135F1 */
void Create_ship_hit_debris(short obj, short count);                  /* 0x4136E2 */
void check_next_wave(void);                                          /* WC2 unmapped */
unsigned int Create_explosion_debris(short obj);                      /* 0x4115F8 */
unsigned int affect_mission_score(short pilot, int event, short amount); /* 0x413D61 */
unsigned int score_for_kill(short pilot, short victim);               /* 0x413E4C */
unsigned int analyze_kill(short attacker, short victim);              /* 0x413F58 */
short ShipExplosion(short obj);                                      /* 0x42A313 */
short Explosion(short obj);                                          /* 0x41444C */
short the_creator(short obj);                                        /* WC2 unmapped */
int explosion_shock_wave(short obj, short blastDamage);              /* 0x414025 */
int explode(short attacker, short victim);                            /* 0x414835 */
int send_at_point(short obj, FixedVector *point, short speed);        /* WC2 unmapped */
short find_child_object(short parent, enum ObjectClass objectClass);  /* 0x410020 */
short find_child_ship(short parent, enum ObjectClass objectClass,
                      short target);                                  /* 0x414CA9 */
int launch_object(short parent, short child, FixedVector direction,
                  short speed);                                      /* 0x414D3D */
void fire(short obj, short target);                                   /* 0x414DC0 */
short hemisphere(FixedVector *target, FixedVector *parent,
                 FixedVector *hardpoint);                             /* 0x415508 */
int fire_flack(short owner, short explosion, short range,
               FixedVector *aim);                                    /* 0x415563 */
int rnd_aim(short radius, short speed, short maximum);                /* 0x40A556 */
short pop_flack(short obj, short range, FixedVector *hardpoint);      /* 0x444F53 */
int fire_turrets(short obj);                                          /* 0x4159DB */
int fire_weapon(short obj, short weapon);                              /* 0x415E2C */
int fire_missile(short ship);                                         /* 0x41670E */
int fire_fixed_projectile_weapon(short obj);                           /* 0x42C9B0 */
int drop_mine(short obj, signed char weapon, enum ObjectType type,
              short lifetime);                                       /* 0x448D58 */
void fire_afterburner(short obj, short time);                          /* 0x40ABCC */
short find_weapon(short obj, enum ObjectType weaponType);              /* WC2 unmapped */
unsigned int check_computer_damage(void);                              /* 0x412F90 */
short rnd_sign(short v);                                               /* WC2 unmapped */
unsigned int fire_super_brake(short ship);                              /* 0x4169CC */
short flip_angle(short ship, short angle);                              /* 0x4169F3 */
unsigned int place_exhaust_on_ships(void);                              /* 0x416A87 */
unsigned int reposition_fixed_child_objects(void);                     /* 0x42CC42 */
unsigned int housekeep_power_plant_and_fuel(short ship);                /* 0x4170F9 */
unsigned int replenish_shields(short ship);                            /* 0x450136 */
unsigned int replenish_weapon_energy_bank(short ship);                 /* 0x41724A */
void accelerate(short amount);                                         /* 0x41740F */
void your_afterburner(void);                                            /* 0x417471 */
unsigned int LoadGamePaletteFile(void);                                /* 0x45B810 */
unsigned int EMShutDown(void);                                         /* 0x45B905 */
unsigned short InitializeEventManagerResources(void);                  /* WC2 unmapped */
unsigned int EMStartUp(void);                                          /* 0x45B924 */
unsigned int LoadOriginFxDrivers(void);                                 /* 0x45B9D3 */
unsigned int InitializeGameTextContexts(void);                          /* 0x45C088 */
unsigned int initialize_direction_view_frame(short yaw, short pitch,
                                             signed char frame);       /* 0x45C1A1 */
unsigned int initialize_direction_view_frames(void);                    /* 0x45C279 */
unsigned int LoadSpaceflightResources(void);                           /* 0x420B12 */
unsigned int GetFxDriverInitResult(void);                                      /* WC2 unmapped */
unsigned int GetMessagePumpResult(void);                                      /* WC2 unmapped */
unsigned int GetFxDriverStatus(void);                                      /* WC2 unmapped */
short ace_status(short ace, unsigned char bits);                     /* 0x4587E0 */
void unflag_ace(short ace, unsigned char bits);                       /* WC2 unmapped */
void flag_ace(short ace, unsigned char bits);                         /* WC2 unmapped */
void kill_ace(short ace);                                             /* WC2 unmapped */
void ace_greeting(short obj);                                         /* 0x417838 */
void prepare_ace(short ace);                                          /* WC2 unmapped */
short signed_random(short range);                                     /* 0x429581 */
int alert_flag(short ship, unsigned int bits);                        /* 0x4295A9 */
unsigned int HasSpeechBuffer(void);                                      /* WC2 unmapped */
unsigned short set_alert(short obj, unsigned int bits);                /* 0x4295D9 */
void clear_alert(short obj);                                          /* 0x4295F2 */
void start_collision_alert(short obj, short other);                   /* 0x429625 */
void try2end_collision_alert(short obj);                              /* 0x429698 */
short normal_speed(short obj);                                         /* 0x415625 */
short real_crash_time(short obj, short other);                         /* 0x429736 */
void clear_crash_cache(void);                                         /* 0x446D55 */
short crash_time(short obj, short other);                             /* 0x429958 */
short detect_collisions(short obj);                                   /* 0x4299C9 */
unsigned int unactive(short ship);                                      /* 0x429B55 */
int are_alive(short obj);                                             /* WC2 unmapped */
int trim_goals(short obj, short amount);                              /* 0x429BF0 */
int report_kilrathi_rout(int mode);                                   /* 0x429CAD */
short find_ship_index(short missionShip);                              /* 0x429E24 */
int try2rout(short obj);                                              /* 0x429EE0 */
signed char no_goal(short ship);                                       /* 0x42A003 */
int being_tailed(short obj, short other);                             /* 0x465730 */
int any_enemy_tail(short obj);                                        /* WC2 unmapped */
short detect_enemy_tail(short obj);                                    /* 0x42A108 */
int is_ship_tailing_player_target(short obj);                         /* 0x42A1C5 */
int missile_on_tail(short obj);                                       /* 0x41596F */
short select_weighted_value(short *choices);                          /* 0x41F446 */
unsigned int build_squad_list(short leader);                          /* 0x42A062 */
unsigned int find_squad_center(FixedVector *center);                  /* 0x4210B8 */
unsigned int init_formation_burst(short obj);                          /* 0x42A39A */
unsigned int reset_mission_type(short obj,
                                enum ShipMissionType missionType);     /* 0x42A490 */
unsigned int change_mission_type(short obj,
                                 enum ShipMissionType missionType);    /* 0x42A4E5 */
unsigned int reset_objective(short ship, enum ShipObjective objective); /* 0x42A52A */
unsigned int alter_objective(short ship, enum ShipObjective objective); /* 0x42A55F */
unsigned int reset_tactic(short ship, enum ShipTactic tactic);          /* 0x42A594 */
unsigned int alter_tactic(short ship, enum ShipTactic tactic);          /* 0x4429A7 */
void reset_maneuver(short ship, short maneuver);                      /* 0x42A5F1 */
void try2reset_maneuver(short obj, short maneuver);                    /* 0x42A625 */
unsigned int set_special(short ship, enum SpecialManeuver special);     /* 0x42A664 */
unsigned int approach_zero_speed(short ship);                          /* 0x42A6E8 */
unsigned int approach_min_speed(short obj);                            /* 0x42A701 */
unsigned int approach_half_speed(short obj);                           /* 0x42A71D */
unsigned int approach_cruise_speed(short ship);                         /* 0x42A754 */
unsigned int approach_full_speed(short ship);                           /* 0x42A788 */
unsigned int approach_ship_speed(short obj, short other);             /* 0x42A7AF */
unsigned int get_front_spot(short obj, unsigned short distance,
                            FixedVector *point);                       /* WC2 unmapped */
unsigned int get_rear_spot(short obj, unsigned short distance,
                           FixedVector *point);                        /* 0x42A7D2 */
#ifdef WC1_SDL
unsigned int close_behind(short range);
#else
unsigned int close_behind();                                          /* 0x4407B0 */
#endif
short scan_for_enemy(short obj, unsigned short range);                  /* 0x42A8B5 */
int any_enemy(short obj, short range);                                  /* 0x42AA0D */
short nearest_enemy_range(short obj);                                 /* 0x4608E8 */
void fire_when_ready(short obj, short aimed);                         /* WC2 unmapped */
unsigned int ships_within_range(short obj, short other, short range); /* 0x42AB81 */
int attacker_in_range(short obj, short range);                          /* WC2 unmapped */
int in_danger(short obj);                                               /* 0x42ACAF */
unsigned int target_within_range(short obj);                           /* 0x41AD8B */
short build_target_list(short obj, short range);                       /* 0x42AAD3 */
int select_safe_target(short obj);                                    /* WC2 unmapped */
void inherit_leader_mission(short obj);                               /* 0x42AF60 */
void inherit_leader(short obj);                                         /* 0x42B00A */
unsigned int dead_ship(short missionShip);                            /* 0x45865D */
int gone_ship(short missionShip);                                       /* 0x42B0FB */
short skill_rating(short obj);                                        /* 0x429BA8 */
int skill_check(short obj, short difficulty);                         /* 0x45655D */
short find_ships_sphere(short missionShip);                           /* 0x42B1AE */
int locate_ship(short missionShip, FixedVector *point);               /* 0x42B2A7 */
unsigned int get_follow_point(short obj, FixedVector *point);         /* 0x42B38F */
unsigned int get_first_follow_point(short obj, FixedVector *point);   /* 0x42B524 */
int hostile_sphere(short obj, short navPoint);                        /* 0x42B565 */
int abandoned(short obj, short other);                                  /* 0x42B5F7 */
void engage(short obj, short target,
            enum ShipObjective objective);                              /* 0x42B6C5 */
int target_valid(short obj);                                            /* 0x42B73B */
unsigned int triumph(short obj);                                      /* 0x458467 */
short find_ratio(short inputMinimum, short inputMaximum, short input,
                 short outputMinimum, short outputMaximum);          /* 0x40C959 */
short evaluate_damage(short obj);                                       /* 0x42B985 */
short mine_available(short obj);                                      /* WC2 unmapped */
int LoadShapeSet(PacketResourceDescriptor *resources,
                 unsigned short flags,
                 short defaultLogicalFile);                            /* 0x4568E0 */
int FreeShapeSet(PacketResourceDescriptor *resources,
                 unsigned short releaseFlags);                         /* 0x432DCC */
int LoadPacketResourceList(PacketResourceDescriptor *resources,
                           short flags, int availableBytes);           /* 0x4589D0 */
unsigned int ResetCockpitPaletteEntries(void);                         /* 0x456A68 */
unsigned int initialize_cockpit(signed char mode);                     /* 0x456B1A */
unsigned int InitializeConstellationObject(
    const ConstellationObjectDefinition *definition,
    short object);                                                /* 0x459527 */
unsigned int FreeConstellationObject(short object);               /* 0x44C796 */
unsigned int init_constellation(short scene);                     /* 0x42917D */
unsigned int free_constellation(void);                            /* 0x45890E */
void init_vdus(void);                                                  /* 0x457720 */
unsigned int InitializeCockpitResources(signed char mode);             /* 0x4577D7 */
unsigned int free_cockpit(void);                                       /* 0x458196 */
#ifdef WC1_SDL
unsigned int init_3Space_objects(short scene);
#else
unsigned int init_3Space_objects();                                    /* 0x4560C0 */
#endif
unsigned int load_common_3Space_objects(void);                         /* 0x414BE9 */
unsigned int remove_all_3d_objects(void);                              /* 0x44EA50 */
unsigned int free_3Space(void);                                        /* 0x4293F9 */
unsigned int free_3Space_objects(void);                                /* 0x42E6EB */
unsigned int init_inflight_music(void);                                /* 0x43C601 */
unsigned int free_inflight_music(void);                                /* 0x458806 */
unsigned int PreloadMusicTrackHook(short track);                       /* 0x469AD0 */
unsigned int ReleaseMusicTrackHook(short track);                       /* 0x40E31F */
unsigned short LoadSceneAnimationResources(short scene, short variant); /* WC2 unmapped */
void ReleaseSceneAnimationResources(void);                             /* WC2 unmapped */
signed char *__stdcall FindSceneAnimationCommand(
    signed char *script, signed char command);                         /* WC2 unmapped */
short __stdcall SceneAnimationGoalReached(short delta, short current,
                                          short goal);                 /* WC2 unmapped */
unsigned int __stdcall UpdateSceneAnimationObject(
    SceneAnimationObject *object, Viewport *viewport);                 /* 0x446A1D */
void PlaySceneAnimation(char *text, short animation, short duration);  /* WC2 unmapped */
unsigned int WaitForKeyExceptXOrF12(void);                            /* WC2 unmapped */
void ShowMeanwhileTransition(short scene, short variant);             /* 0x407E40 */
void ApplyAnswerTextCipher(char *text, signed char direction);        /* WC2 unmapped */
void LoadAnswerPromptAndResponse(short entry, char *prompt,
                                 char *response);                     /* 0x447300 */
short PromptForAnswerText(short entry);                              /* 0x40DA0C */
void SceneEnterHook(void);                                            /* 0x4333F8 */
void CreateDebugOverlayConsole(HINSTANCE module, HWND window,
                               short columns, short rows);             /* 0x40FCD0 */
void DestroyGlobalDebugOverlayConsole(void);                           /* 0x40FF00 */
void SystemDebugPrintf(const char *format, ...);                           /* 0x40FDAD */
char PumpMessagesDuringWait(void);                                        /* 0x40FE23 */
unsigned char TakeDebugStepFlag(void);                                     /* 0x40FE3E */
void ResetDiskPromptTimer(void);                                             /* 0x40FE59 */
void SetConsoleTextColourHook(void);                                  /* 0x40FE7E */
void SetConsoleBackgroundColourHook(void);                            /* 0x40FEA3 */
void DiskPromptDrawHook(void);                                             /* 0x40FEBE */
void ClearDebugPauseFlags(void);                                              /* 0x40FED9 */
unsigned short EraseTextContextBackground(TextContext *context);      /* 0x43E472 */
int DisplayTrainSimHighScoreTable(short phase);                       /* 0x42C43D */
int AnimateTrainSimTitle(void);                                       /* 0x4595AA */
unsigned char *GetHighScoreEntry(short i);                           /* 0x40FFC0 */
unsigned int GetHighScoreValue(short i);                                      /* WC2 unmapped */
unsigned int SetHighScoreEntry(short i, unsigned char b,
                               unsigned int v);                       /* WC2 unmapped */
void SortTrainSimHighScores(void);                                   /* WC2 unmapped */
short FindTrainSimHighScore(short pilot);                             /* WC2 unmapped */
short InsertTrainSimHighScore(short pilot, unsigned int score);       /* WC2 unmapped */
void InitializeTrainSimHighScores(void);                              /* 0x41565E */
short IsHighScoreSlotUsed(short i);                                    /* WC2 unmapped */
void AddRandomTrainSimHighScores(void);                               /* WC2 unmapped */
void DrawTextInputCursor(char character);                             /* 0x439369 */
void ClearTextInputCharacter(char character);                         /* 0x459348 */
void ClearNextTextInputCharacter(char character);                     /* 0x442421 */
short ReadTextInput(char *destination, short maximumLength,
                    volatile short mode);                            /* 0x4597E3 */
void ReadRequiredPilotField(short x, short y, const char *label,
                            char *destination,
                            short maximumLength);                    /* 0x42075F */
void PromptForPilotField(short x, short y, const char *label,
                         char *destination, short maximumLength,
                         const char *defaultText);                    /* 0x4348C3 */
void InitializeTrainSimTextPanel(void);                               /* 0x4347D6 */
void ShowTrainSimTextMessage(const char *message);                    /* 0x42EBD0 */
void EnterPilotNameAndCallsign(void);                                 /* 0x458532 */
void UpdateTrainSimHighScores(int score);                             /* WC2 unmapped */
void ShowTrainSimHighScores(void);                                    /* 0x436A8F */
unsigned char *LoadTrainSimOpponentShape(int opponent);               /* WC2 unmapped */
short SelectTrainSimMission(short *mission);                           /* 0x4353D4 */
void RunTrainSim(void);                                                /* 0x410715 */
short LogMemoryUsage(void);                                               /* 0x46579D */
unsigned int ShowMemoryStatusDebug(void);                                 /* 0x437AEC */
void exit_squadron(const char *msg);                                    /* 0x437AB4 */
unsigned int GetJoystickButtonEdge(unsigned int a, short b);                    /* 0x43CBD3 */
#ifdef WC1_SDL
int Wc1GameMain(short argc, char **argv);
#else
int main(short argc, char **argv);                                      /* 0x45C558 */
#endif
unsigned int GetScreenUpdateFlag(void);                                         /* 0x43D7F2 */
unsigned int initialize_view_buffer(void);                                /* 0x465CF6 */
unsigned int dump_buffer_to_screen(void);                                 /* 0x465D55 */
unsigned int clear_view_buffer(void);                                     /* 0x465E88 */
unsigned int InitializeConversationViewport(void);                     /* 0x437CBF */
unsigned int ResetScreenClipToFullHeight(void);                                         /* 0x437D68 */
unsigned int InitializeConversationText(void);                         /* 0x437D92 */
unsigned int RefreshMemoryStatusOverlay(void);                                         /* 0x40963B */
unsigned int Update_3Space(void);                                       /* 0x465EE9 */
unsigned int UpdateSpacePaletteFade(void);                              /* 0x465FA3 */
unsigned int TriggerPlayerHitPaletteFlash(void);                      /* 0x465F3A */
unsigned int FadeFlightPaletteEntry(short *entry);                      /* 0x418288 */
unsigned int house_keep(void);                                          /* 0x4186A4 */
void init_player_input(void);                                           /* 0x46428B */
void get_player_input(void);                                           /* WC2 unmapped */
int process_player_input(void);                                        /* 0x4663A2 */
unsigned int fire_players_lasers(void);                                /* 0x466908 */
unsigned int players_flight_dynamics(void);                            /* 0x46696E */
unsigned int player_input(void);                                       /* 0x466B02 */
unsigned int SelectNextExternalViewObject(void);                       /* 0x465E25 */
unsigned int SelectPreviousExternalViewObject(void);                   /* WC2 unmapped */
unsigned int HandleFleetOverviewInput(void);                           /* 0x45F200 */
short MeasureMessageWidth(const char *text);                                /* 0x464E1E */
void WaitForKeyAcknowledge(int mode);                                     /* 0x418ECD */
void ShowModalMessage(const char *format, ...);                           /* 0x437C2E */
void ReportOutOfMemoryAndExit(const char *resource);                    /* 0x437C96 */
void ShowOnScreenMessage(int flags, short duration,
                         const char *format, ...);                        /* 0x437DFA */
void ShowGamePausedBanner(short showBanner);                           /* 0x437F2F */
void ShowVersionBanner(void);                                           /* 0x437F77 */
void SetMessageDisplaySpeed(void);                                     /* 0x437F9A */
void ReportFramesSkipped(short adjustment);                            /* 0x437FD3 */
int HandleSpaceFlightControls(void);                                   /* 0x46733D */
unsigned int Draw_3Space_Frame(void);                                  /* 0x4690FF */
void GetArcadeBonus(void);                                         /* 0x424E8C */
void FigureArcadeTime(void);                                       /* WC2 unmapped */
void DrawArcadeScorePanel(short x, short y);                            /* 0x434EBC */
void UpdateArcadeScoreDisplay(void);                                   /* 0x435064 */
unsigned int RenderSpaceViewFrame(void);                               /* 0x46903F */
unsigned int RefreshCockpitStatus(void);                                /* 0x401A10 */
short GetShipDistanceToNavPoint(short ship, MissionNavPoint *navPoint); /* 0x469143 */
short FindNearestNavPoint(short ship);                               /* 0x46918D */
unsigned int ReleaseStaleNavTarget(void);                                     /* 0x469223 */
int RunSpaceFlight(short entryNavPoint);                               /* 0x46925E */
int calculate_damage_level(void);                                     /* 0x4695FD */
void UpdateTrainSimMenuCursor(void);                                   /* 0x419A40 */
void ResetMouseCursorFrame(void);                                  /* 0x452A00 */
void UpdateRoomMenuCursor(void);                                   /* 0x440F01 */
void __stdcall FadeViewportPaletteToColour(Viewport *viewport,
                                           unsigned short colour,
                                           short enabled);             /* 0x453820 */
short find_objective(int type, short index);                         /* 0x44CE0A */
void arrive_from_warp(short obj);                                      /* 0x424AEE */
unsigned int unwarp(short obj);                                       /* 0x424C05 */
unsigned int warp(short obj);                                         /* 0x45924D */
int drop_player_mine(short obj);                                      /* WC2 unmapped */
unsigned int personality_killed(short personality);                   /* WC2 unmapped */
void clean_up_cockpit(void);                                          /* 0x440490 */
short find_next_gun(short obj, enum ObjectType currentGun);           /* WC2 unmapped */
int select_guns(short obj, short selectedGun);                         /* WC2 unmapped */
unsigned int select_new_gun(void);                                    /* 0x46166D */
unsigned int select_new_release_weapon(enum ObjectType preferredType); /* 0x4616B8 */
void WaitForDebugStep(void);                                          /* WC2 unmapped */
void CALLBACK FrameTimerCallback(UINT timerId, UINT message, DWORD user,
                                 DWORD first, DWORD second);       /* WC2 unmapped */
void SetMultimediaTimerCallback(int period);                       /* 0x40A2E7 */
void * __stdcall PacketLoad(const char *filename, short section,
                            void *destination, unsigned short flags,
                            void *decompressionWorkspace);            /* 0x4465A0 */
void InitializeAudioSystem(HWND window);                              /* 0x423CA0 */
void ServiceAudioStream(void);                                        /* 0x423D02 */
WaveTableEntry *AllocateWaveTableEntry(void);                         /* 0x423D4F */
WaveTableEntry *FindWaveTableEntryByName(const char *name);           /* 0x424A00 */
void RemoveWaveTableEntry(WaveTableEntry *entry);                     /* 0x42DD9F */
void FreeWaveTable(void);                                           /* 0x423ED1 */
ActiveSoundEntry *AllocateActiveSoundEntry(void);                     /* 0x423F3F */
void RemoveActiveSoundEntry(ActiveSoundEntry *entry);                 /* 0x423FAB */
ActiveSoundEntry *FindActiveSoundEntryBySample(IxSample *sample);    /* 0x461D29 */
void ReleaseFinishedSoundEntries(void);                              /* 0x465CBC */
void StopSoundsUsingWave(const char *name);                          /* 0x42D444 */
void playWAVE(const char *filename, int looping, int volume);         /* 0x42418C */
void stop_all_sounds(void);                                         /* 0x4245A2 */
void PlaySnowStaticSound(void);                                     /* 0x4245F8 */
void ServiceSoundSystem(void);                                         /* WC2 unmapped */
void SetSoundEffectsVolume(int volume);                               /* 0x42482E */
LONG RegistryQueryValue(HKEY key, LPCSTR name, DWORD type,
                        LPBYTE data, DWORD size);                     /* 0x42485C */
void RegistryStoreValue(HKEY key, LPCSTR name, DWORD type,
                        const BYTE *data, DWORD size);                  /* 0x424888 */
void LoadVolumeSettingsFromRegistry(void);                            /* 0x4248B4 */
void SaveVolumeSettingsToRegistry(void);                               /* 0x424980 */
void DrawLaunchDoorFrame(short distance);                              /* 0x459160 */
void LaunchPlayerShip(void);                                           /* 0x42ECCB */
unsigned int ShowCarrierLaunchSequence(signed char sceneObject);       /* 0x43B7C0 */
void FxDriverShutdownHook(void);                                            /* WC2 unmapped */
unsigned short InitializeDiskPromptTextContext(void);                 /* WC2 unmapped */
unsigned short RewriteDiskFileGraphicsExtensions(short videoMode);    /* 0x432E23 */
short LoadWingCmdrCfgFile(short argc, char **argv);                    /* 0x401000 */
unsigned short LoadInstallDat(void);                                   /* 0x42949F */
void show_damage_disp(void);                                           /* 0x401120 */
void UpdateDamageDisplay(void);                                        /* 0x4012E1 */
unsigned short GetJoystickPresentUnused(void);                                    /* WC2 unmapped */
unsigned int parse_view_script(void);                                  /* 0x457434 */
unsigned int update_scripted_view(void);                               /* 0x4394A0 */
void initialize_scripted_view(const short *script);                    /* WC2 unmapped */
unsigned int InitializeFireworks(void);                                /* 0x45A2C0 */
short TheEndFireWorks(Viewport *viewport, short count);                /* 0x45A300 */
unsigned int InitializeConstellationField(Viewport *viewport,
                                          short direction,
                                          short density);             /* 0x45A441 */
unsigned int DrawConstellationField(void);                             /* 0x45A634 */
short __stdcall OpenPacketSection(const char *filename, short section,
                                  PacketSectionHandle *handle);        /* 0x44F010 */
void __stdcall CloseDataFileByHandle(unsigned short *p);                       /* 0x44F1C7 */
void * __stdcall DecompressPacketSection(
    PacketSectionHandle *handle, void *destination, unsigned short flags,
    void *decompressionWorkspace);                                    /* 0x445AAC */
short GetTargetColourIndex(void);                                                /* WC2 unmapped */
void show_target_disp(void);                                         /* 0x43FAC0 */
void DrawTargetRangeReadout(void);                                   /* 0x43FF40 */
void LogDisplayMode(const char *mode);                                 /* 0x45CA50 */
short __stdcall CalcRectangleArea(const Viewport *viewport);           /* 0x45CA80 */
unsigned short __stdcall AllocateViewport(Viewport *viewport,
                                          short clearColour,
                                          short flags);                /* 0x45CAD2 */
void __stdcall AlignSpriteFrameToRectCorner(
    const ShortRect *rectangle, ShortPoint *position, short corner,
    unsigned char *shape, short frame);                                /* 0x46505E */
void FadeMusic(void);                                                  /* WC2 unmapped */
void SetMusicOn(short enabled);                                         /* 0x42F100 */
void StopMusic(short unused);                                          /* 0x452A26 */
void SetMusBreakpt(int first, int second);                             /* 0x4641C5 */
void PaletteFadeHook(void);                                            /* WC2 unmapped */
void FlushSoundEffect(void);                                               /* 0x452AAB */
void FlushSoundEffects(void);                                               /* WC2 unmapped */
void SceneLeaveHook();                                                /* 0x452AF0 */
void SelectFlightMusicTrack(int track);                              /* 0x452B03 */
int MapMusicTrackToStreamerCommand(int track);                       /* WC2 unmapped */
void ProcessMusicScriptCommand(int track, int command,
                               short enabled);                       /* WC2 unmapped */
unsigned int spacetrack(int track, int mode,
                        short enabled);                               /* 0x452CF4 */
void StopMusicUnlessSuppressed(void);                                        /* 0x452D32 */
unsigned short GetMusicMode(void);                                /* WC2 unmapped */
void wait_for_end_of_music(void);                                     /* 0x452DB0 */
void new_space_music_changes(short attacker, short victim);          /* 0x452E46 */
int changetrack(void);                                                /* 0x452F90 */
void gametrack(void);                                                 /* 0x453085 */
void servicetrack(void);                                              /* 0x453240 */
void ResetSoundState(void);                                             /* 0x42DE62 */
void ResetSoundStateForScene(void);                                               /* 0x4732E0 */
void ResetSoundStateForFlight(void);                                               /* WC2 unmapped */
void EnableMusicForScene(void);                                               /* WC2 unmapped */
unsigned int SoundFxTick(void);                                       /* WC2 unmapped */
void FlushSoundEffectsAndLog(void);                                               /* 0x45357E */
void sound_effect(void);                                                /* 0x4535BB */
#ifdef __cplusplus
void PlaySfxWaveFileByNumber(int soundNumber, int sourceObject,
                             int looping);                            /* 0x45373B */
#else
void PlaySfxWaveFileByNumber();                                      /* 0x45373B */
#endif
void cleanup_objectives(void);                                        /* 0x421910 */
int too_busy(short ship);                                             /* WC2 unmapped */
void reply(short ship, short accepted);                               /* WC2 unmapped */
int disobey_formation(short ship);                                    /* 0x421C35 */
int bad_target(short ship, short target);                             /* 0x40F91C */
short can_land(void);                                                 /* 0x41F337 */
short i_wanna_rout(short ship, int pilot);                            /* 0x41693A */
void request(short requester, short ship, short command);             /* 0x421F86 */
unsigned short __stdcall ShouldSuspendCursorForRect(
    const ShortRect *bounds);                                           /* WC2 unmapped */
unsigned short __stdcall InitializeDIBScreenViewport(
    Viewport *viewport, unsigned short colour);                       /* 0x40FB80 */
void InitFullScreenViewport(int *vp, short arg);                                    /* 0x40FC7E */
unsigned int __stdcall GetPacketSize(const char *filename,
                                     short section);                  /* 0x453A70 */
int GetFreeNearHeapBytes(void);                                       /* 0x421144 */
void FrameStartHook(int mode);                                         /* 0x40A2A3 */
unsigned short IsSoundHardwarePresent(short device);                           /* 0x458E20 */
void MessagePumpHook(int mode);                                        /* WC2 unmapped */
void *PushMemoryStackFrame(void *memory, int offset);                   /* 0x458E49 */
int IsPushedPacketHandle(void *handle);                                /* 0x458EED */
void *MapPacketHandleToBlock(void *handle);                            /* WC2 unmapped */
void *AllocateTaggedMemory(unsigned int size, unsigned short flags);   /* 0x45901D */
void ReleasePacketHandle(void *handle);                               /* 0x428B3C */
unsigned int GetFixedOneMillionThunk(short memoryType);                /* WC2 unmapped */
unsigned int GetFixedOneMillionThunkAlt(short memoryType);             /* 0x459149 */
void CreateCannedSceneObject(short *object, short yaw, short unusedPitch,
                             short distance, unsigned char *shape,
                             short frame, short type,
                             short scale);                         /* WC2 unmapped */
unsigned int ShowCampaignVictorySequence(void);                    /* 0x433C84 */
unsigned int ShowTigerClawEscapeScene(void);                          /* 0x446823 */
unsigned int ShowTheEndScreen(short enableFireworks);                 /* 0x429261 */
short __stdcall UpdateInputDeviceTransitions(short raw);               /* 0x421D59 */
void PollJoystickButtonEvents(void);                                   /* 0x421409 */
void PollMenuInputDevices(void);                                       /* 0x421530 */
short get_face(short rating, unsigned int side);                      /* 0x44F52E */
void LoadCommPortraitShape(short face, signed char alternate);        /* 0x44BCF7 */
void ResetCommMenuChoices(short reuse);                               /* 0x438CEB */
int IsCommMenuIdle(void);                                              /* 0x4473D1 */
void AppendCommMenuChoice(const char *text, short command);            /* 0x40A981 */
void SendCommMenuChoice(short i);                                          /* 0x447369 */
void OpenCommMenuForTarget(const char *heading,
                           const char *message);                       /* 0x4474CA */
int IsCommChoiceMenuOpen(void);                                       /* 0x4534FC */
short GetPendingMenuAction(void);                                            /* WC2 unmapped */
void SetPendingMenuAction(unsigned char v);                                 /* WC2 unmapped */
void OpenCommRecipientMenu(void);                                     /* 0x447526 */
void CloseCommChoiceMenu(void);                                       /* 0x4474F4 */
int wingman_dead(void);                                               /* 0x44756F */
short have_target(void);                                              /* 0x447479 */
unsigned short CanOpenCommMenu(void);                                 /* 0x4475D3 */
void SelectCommRecipient(short recipient);                            /* WC2 unmapped */
void BuildCommunicationRecipientMenu(void);                           /* 0x447629 */
void BuildCommunicationCommandMenu(void);                             /* 0x447890 */
void RefreshCommunicationMenu(void);                                 /* 0x447D85 */
void HandleCommunicationMenuRequest(void);                            /* 0x447DE3 */
void show_communications_disp(void);                                  /* 0x447E47 */
unsigned int Chosen_communicate_option(short choice);                 /* 0x447EF5 */
void talk_equiv(void);                                                /* WC2 unmapped */
void FreeCommDisplayResources(void);                                  /* WC2 unmapped */
void EndCommSessionWithWingman(void);                                  /* 0x448070 */
void EndCommMenu(void);                                              /* 0x4480C6 */
void ShowCentredPrompt(char *text, unsigned short arg);                       /* 0x448100 */
short LoadCommDisplayResources(short rating, enum Side side);         /* 0x42B80C */
char *ExpandCommMessageTokens(const char *text);                      /* 0x448136 */
void real_vid_transmit(short obj, short message);                     /* 0x4482CA */
void __stdcall ShutdownVideoHook(short mode);                           /* 0x458E39 */
short __stdcall ReserveContiguousPaletteEntries(short entryCount);    /* 0x424A60 */
void __stdcall ReleaseContiguousPaletteEntries(short firstEntry);     /* 0x420340 */
void PrintPaletteAllocationMap(void);                                 /* 0x401978 */
void LoadJoystickCalibrationFile(short horizontalRange,
                                 short verticalRange,
                                 short horizontalDeadZone,
                                 short verticalDeadZone);              /* 0x423480 */
short ReadCalibratedJoystick(void);                                    /* 0x4238E9 */
void __stdcall UnionRectBounds(ShortRect *destination,
                               const ShortRect *first,
                               const ShortRect *second);               /* WC2 unmapped */
void ThrottleFrameAndDrawFps(HDC dc);                                       /* 0x45CD2C */
void ReportSpaceFlightMaxFps(float adjustment);                      /* 0x45CE9A */
void SetSpaceFlightFrameTiming(void);                                  /* 0x45CF3B */
void SetCinematicFrameTiming(void);                                    /* 0x45CF7F */
void DIBerror(const char *tag, int hr);                                 /* 0x45D004 */
void DIBpositionWindow(void);                                           /* 0x45D11F */
void DIBreInstall(void);                                                /* 0x45D1BB */
void DIBinstall(HWND window);                                           /* 0x45D23B */
int DIBcascade(int mode, int *reportedResult);                          /* 0x45D3A1 */
void DIBunInstall(void);                                              /* 0x428A6D */
void DIBmakeDIB(void);                                                  /* 0x45D80F */
void DIBdestroyDIB(void);                                             /* 0x45DA8C */
void DIBslam(void);                                               /* 0x45DB8C */
void DIBslamReal(void);                                           /* 0x45DC33 */
void DIBupdate(int left, int top, int right, int bottom);               /* 0x45E060 */
unsigned char *GetDIBPixelBuffer(void);                                  /* 0x45E276 */
HWND GetDIBWindowHandle(void);                                           /* WC2 unmapped */
void CachePaletteEntryFromWords(short index, unsigned short *rgb);        /* WC2 unmapped */
void DIBramPalette(void);                                                 /* 0x45E37C */
void DIBsetPalette(short index, short *rgb);                              /* 0x45E46C */
void GetPaletteEntryAsWords(short i, unsigned short *rgb);                /* 0x45E9C5 */
void DIBwholePaletteFromTriplets(unsigned char *palette);             /* 0x45E667 */
void DIBwholePaletteFromWords(unsigned short *palette);                /* 0x45E816 */
void DIBwaitForVerticalBlank(void);                                      /* WC2 unmapped */
char *DirectDrawResultToText(int result);                              /* WC2 unmapped */
void show_info_disp(void);                                             /* 0x43C364 */
unsigned int steer_away_from_object(short obj, short other,
                                    short amount);                    /* 0x41E4E0 */
void steer_away_from_predicted_object(short obj, short other,
                                      short predictionTicks,
                                      short amount);                  /* 0x41E5C8 */
void prevent_collision(short obj);                                    /* 0x41E6C8 */
int handle_collisions(short obj);                                     /* WC2 unmapped */
int regulate_turn(short obj);                                         /* 0x421CF6 */
unsigned int select_target(short obj);                                  /* 0x41EA86 */
unsigned int veer_random(short obj, short amount);                      /* 0x41EB1D */
unsigned int offset_location(short obj, const ShortVector *offset,
                             FixedVector *location);                  /* 0x40A774 */
unsigned int compute_formation_destination(short leader,
                                           const ShortVector *offset,
                                           FixedVector *destination); /* 0x41EC73 */
unsigned int control_speed(short obj, unsigned short range,
                           int desiredSpeed);                         /* 0x41ECEF */
unsigned int chase_location(short obj, const FixedVector *destination,
                            short reference);                         /* 0x41EDCC */
unsigned int goto_location(short obj,
                           const FixedVector *destination);           /* 0x41EFFE */
unsigned int goto_formation(short obj, const FixedVector *destination,
                            short leader);                            /* 0x41F0C5 */
unsigned int maintain_formation(short obj);                             /* 0x40BD0E */
unsigned int reset_stress(short obj);                                  /* WC2 unmapped */
short stress_morale(short obj);                                        /* 0x41F3EF */
enum ShipManeuver any_defense(short obj);                              /* WC2 unmapped */
enum ShipManeuver pick_regular_maneuver(short obj, int event);         /* 0x41F4C6 */
enum ShipManeuver pick_from_list(const ManeuverChoice *choice,
                                 short obj);                           /* 0x41F714 */
enum ShipManeuver pick_kilrathi_maneuver(short obj, int event);        /* 0x41F83F */
unsigned int process_maneuver_node(short obj, int event);              /* 0x41E860 */
void handle_stress(short obj, int event);                              /* 0x41FD2B */
void intelligence_events(short obj);                                    /* 0x41FF37 */
unsigned int chase_speed(short obj, short range);                      /* 0x440571 */
short RandomBelow(short n);                                           /* 0x4618E0 */
void __stdcall SeedRandomFromClock(void);                                               /* 0x4618FF */
short __stdcall RandomInRange(short lo, short hi);                      /* 0x461942 */
short RandomBelowOrEqual(short n);                                      /* 0x4619A1 */
long MultiplyFixed(int left, int right);                              /* 0x4619E2 */
long DivideFixed(int numerator, int denominator);                     /* 0x461A2D */
long SinFixed(short degrees);                                    /* 0x461A91 */
long CosFixed(short degrees);                                    /* 0x461AD6 */
long ArcSin(int value);                                               /* 0x461B1B */
long ArcCos(int value);                                               /* 0x461B6B */
long Magnitude(int value);                                           /* 0x461BBB */
long PlanarMagnitude(int x, int y);                                  /* 0x45CCC5 */
long Vector_magnitude(const FixedVector *vector);                     /* 0x461C71 */
void __stdcall SetTextCursor(unsigned short a, unsigned short b);   /* 0x488B20 */
void __stdcall SetTextContext(TextContext *context);                  /* 0x40F882 */
void WaitForVerticalBlankThunk(void);                                  /* WC2 unmapped */
void *__stdcall IdentityHandle(void *v);                               /* WC2 unmapped */
void __stdcall SetWholePaletteFromTriplets(unsigned char *palette);               /* 0x461D90 */
unsigned short __stdcall ReadWord(unsigned short *p);                        /* 0x461DA7 */
unsigned short __stdcall GetFontCharWidth(char i);                                     /* 0x461DBD */
void ReleaseVideoResourcesHook(void);                                           /* 0x4641A0 */
short __stdcall GetShapeFrameBounds(short *bounds, short x, short y,
                                    unsigned char *shape, short frame); /* 0x461DF0 */
short __stdcall IsPointInRect(short x, short y, const short *rect);       /* 0x461E97 */
void __stdcall SplitPackedPoint(ShortPoint point, short *p);             /* 0x461EFE */
void __stdcall DrawTextString(const char *text);                       /* 0x461F22 */
void __stdcall DrawTextCharacter(char character);                    /* 0x4621D5 */
void __stdcall AppendTextCharacter(char character);                  /* WC2 unmapped */
int __stdcall MeasureShapeFrameStorage(unsigned char *shape,
                                       short frame);                   /* 0x417124 */
void ResetTextCursor(void);                                           /* WC2 unmapped */
unsigned int __stdcall DosFarPtrToNear(void *v);                       /* WC2 unmapped */
void *__stdcall DosNearPtrToFar(unsigned int v);                       /* 0x462452 */
char *__stdcall DosStrrchr(char *s, short c);                       /* 0x462465 */
char *__stdcall DosStrchr(const char *s, short c);                  /* 0x462486 */
char *__stdcall DosStrcpy(char *dst, const char *src);               /* 0x42476B */
char *__stdcall CopyFarString(char *destination,
                              const char *source);                   /* WC2 unmapped */
short __stdcall DosStrlen(const char *s);                                   /* 0x46250B */
void __stdcall DosMemcpy(void *dst, const void *src, size_t n);               /* WC2 unmapped */
void __stdcall DosMemset(void *destination, unsigned int count,
                         short value);                               /* WC2 unmapped */
unsigned short GetEventManagerStatus(void);                            /* WC2 unmapped */
void __stdcall RegisterEventManagerShutdown(void (*fn)(void));         /* WC2 unmapped */
short __stdcall InitializeEventManager(short period,
                                       unsigned short (*initialize)(void),
                                       void *configuration);           /* WC2 unmapped */
void ShutdownEventManager(void);                                      /* WC2 unmapped */
unsigned short __stdcall ConfigureEventManagerPointer(
    unsigned char *shape, short frame);                               /* WC2 unmapped */
void __stdcall EventManagerHook(void (*callback)(void));               /* 0x464E35 */
void __stdcall SetEventManagerPump(void (*pump)(void));                /* 0x464E4F */
void TranslatePolledInputEvent(unsigned short type,
                               unsigned int value);                   /* 0x462625 */
void QueueInputEventAtCursor(unsigned int type, short primaryButton,
                             short secondaryButton);                   /* 0x462849 */
InputEvent *AllocateInputEvent(void);                                  /* WC2 unmapped */
void ReleaseInputEvent(InputEvent *event);                             /* 0x42C04B */
void QueueInputEvent(unsigned short type, unsigned short x,
                     unsigned short y, unsigned short value,
                     int primaryButton, int secondaryButton,
                     unsigned int timestamp);                          /* 0x4629A7 */
void ReleaseInputEventQueue(void);                                    /* WC2 unmapped */
void RetainInputEventsOfType(int type);                                /* 0x458F5A */
void RemoveInputEvent(InputEvent *event);                              /* WC2 unmapped */
short __stdcall GetNextInputEvent(InputEventState *event);             /* WC2 unmapped */
short __stdcall PollInputEvent(InputEventState *event, short filter);  /* 0x464D5F */
short __stdcall PeekInputEvent(InputEventState *event, short type);    /* 0x43AFD3 */
short __stdcall IsInputEventQueued(short type);                        /* WC2 unmapped */
void FlushInputEvents(void);                                          /* 0x46354F */
short __stdcall ResetAllocationDepth(int x, int y);                                          /* WC2 unmapped */
void CheckCursor(void);                                                /* WC2 unmapped */
void CaptureMouseCursorBackground(void);                           /* 0x4635E3 */
void DrawMouseCursor(void);                                        /* 0x4636F7 */
void RestoreMouseCursorBackground(void);                           /* 0x4637F4 */
void RefreshMouseCursorDisplay(void);                              /* 0x4638F1 */
void EnterAllocationScope(void);                                                 /* 0x463BA1 */
void LeaveAllocationScope(void);                                                 /* 0x463E19 */
void __stdcall SetMouseCursorShape(unsigned char *shape, short frame); /* 0x4379F0 */
void __stdcall SetMouseHomePosition(short x, short y);                /* 0x463F74 */
void __stdcall ApplyPackedMousePosition(ShortPoint point);             /* WC2 unmapped */
void SetFrameTimerPeriod(short period);                                   /* 0x464009 */
void __stdcall SetFrameTimerAndWait(short period);                                       /* WC2 unmapped */
void __stdcall SetFrameTimerPeriodDirect(short p);                    /* 0x46403D */
void WaitForFrameTick(void);                                          /* 0x484453 */
int IsFrameTickElapsed(void);                                            /* 0x44A03C */
unsigned short GetSoundHardwareFlag(void);                                    /* WC2 unmapped */
void TimerResetHook(void);                                           /* WC2 unmapped */
unsigned int GetVideoReleaseResult(void);                                      /* WC2 unmapped */
unsigned short IdentityWord(unsigned short v);                         /* 0x46418D */
void TimerStopHook(void);                                           /* WC2 unmapped */
unsigned int GetFixedOneMillion(void);                                 /* WC2 unmapped */
unsigned int GetFixedOneMillionAlt(void);                              /* WC2 unmapped */
void ClearInputKeyStatePreservingModifiers(void);                     /* 0x4642D6 */
void ClearInputKeyState(void);                                        /* WC2 unmapped */
void SetInputKeyState(int scanCode, unsigned char pressed);            /* 0x46431A */
void sort_object_depth(void);                                          /* WC2 unmapped */
void draw_sorted_objects_to_buffer(void);                              /* 0x433690 */
void intro_drawbackgroundships(void);                                  /* 0x46470E */
void set_up_screen_viewport(signed char mode);                         /* 0x4648C5 */
void MouseIdleHook(void);                                           /* WC2 unmapped */
unsigned short GetNavRangeSentinel(void);                                   /* WC2 unmapped */
unsigned short GetOriginalFreeMemory(void);                                   /* WC2 unmapped */
void StartupHook(unsigned int (*callback)(unsigned int, short));  /* WC2 unmapped */
unsigned int JoystickEdgeHook(int button);                         /* WC2 unmapped */
void FreeIfNotNull(void *p);                                            /* 0x464B3A */
unsigned int GetStartupErrorCode(int vector);                       /* WC2 unmapped */
void ShutdownHook(int vector, void *handler);                       /* WC2 unmapped */
unsigned short __stdcall SelectDiskDriveHook(short drive);             /* WC2 unmapped */
short GetCurrentDiskDriveHook(void);                                    /* WC2 unmapped */
unsigned short __stdcall GetShutdownErrorCode(
    unsigned char *driveState);                                    /* WC2 unmapped */
void VideoReleaseHook(void);                                           /* WC2 unmapped */
void ExitCleanupHook(void);                                           /* WC2 unmapped */
unsigned int IsVectorWithinRange(FixedVector *vector, short range);    /* WC2 unmapped */
unsigned int shrink_vector(FixedVector *vector);                       /* 0x419C10 */
unsigned int shrink(int *component);                                   /* 0x47AF60 */
void __stdcall FillGraphicSuffix(char *path, short number,
                                 short digits);                        /* 0x42BAC0 */
void __stdcall ConvertChar_Int(char *text, short number,
                              short digits);                          /* 0x42BB17 */
unsigned int LoadBriefingRoom(void);                                  /* 0x4401C0 */
unsigned int ViewMedals(void);                                        /* 0x42D568 */
unsigned int AwardCampaignMedal(short medal);                         /* 0x44BEE5 */
int no_objectives_achieved(void);                                      /* 0x4296E2 */
short wing_status(short personality);                                  /* 0x42B0B7 */
short int_value(char **text);                                          /* WC2 unmapped */
ConversationSceneRecord *ParseTests(ConversationSceneRecord *record,
                                    ConversationSceneRecord *sceneData,
                                    unsigned char *textData);          /* 0x41D591 */
unsigned int DrawMedalChest(char *text, short duration);                /* WC2 unmapped */
unsigned int DrawMedalLongShot(short *animation, char *text,
                               short duration);                        /* 0x434177 */
unsigned int MedalEstablish(char *text, short duration);                /* WC2 unmapped */
unsigned int PinMedal(char *text, short duration);                      /* WC2 unmapped */
unsigned int DrawMedals(void);                                          /* 0x452AC8 */
unsigned int EstablishingShot(char *text, short duration);              /* WC2 unmapped */
unsigned int DrawBriefingLongShot(void);                                /* WC2 unmapped */
unsigned int ReturnToBriefingLongShot(char *text, short duration);      /* WC2 unmapped */
unsigned int Dismissed(char *text, short duration);                     /* 0x433F94 */
unsigned int DrawDebriefingLongShot(void);                              /* WC2 unmapped */
unsigned int DebriefingEstablishingShot(char *text, short duration);    /* WC2 unmapped */
unsigned int TalkerInit(void);                                         /* WC2 unmapped */
unsigned int FreeTalker(void);                                         /* WC2 unmapped */
unsigned int SceneDirector(short sceneType, unsigned char *sceneData,
                           unsigned char *textData);                    /* 0x434BB5 */
unsigned int DrawPodiumShot(void);                                      /* 0x446710 */
unsigned int DrawBriefingCharacter(short character, short pose,
                                   short animationFrame,
                                   const signed char *unusedXOffsets,
                                   const signed char *unusedYOffsets);  /* 0x4021A7 */
unsigned int DrawFuneralLongShot(short shot, char *text,
                                 short duration);                       /* 0x42E868 */
void __stdcall SetViewportRect(Viewport *viewport, unsigned short left,
                               unsigned short top, unsigned short right,
                               unsigned short bottom);                /* 0x40A2D0 */
void __stdcall PanToScreen(Viewport *source, Viewport *destination);    /* 0x433410 */
unsigned int death_sequence(void);                                    /* 0x4697A0 */
unsigned int ShowGetReadyScreen(void);                                 /* WC2 unmapped */
unsigned int ShowVictoryScreen(void);                                  /* 0x408CC8 */
unsigned int ShowGameOverScreen(void);                                 /* 0x45F7EC */
void __stdcall FillRasterBytes(void *destination, unsigned int length,
                               short value);                          /* WC2 unmapped */
char *CopyRasterDriverName(void *const *callbacks);                   /* 0x40243C */
void InstallRasterDriverCallbacks(void *const *callbacks);            /* 0x402465 */
unsigned int SetRasterClipPixel(RasterClip *clip, int x, int y,
                                int colour);                           /* 0x402484 */
int ReadRasterClipPixel(RasterClip *clip, int x, int y);                /* 0x40255F */
unsigned int DrawClippedLine(RasterClip *clip, int x1, int y1, int x2, int y2,
                             int mode, int colour);                    /* 0x402635 */
int FillRasterClipCheckerboard(RasterClip *clip, int left, int top,
                               int right, int bottom,
                               unsigned char colour);                 /* 0x403037 */
int DrawRLEImage(RasterClip *clip, unsigned char *shape, int frame,
                 int x, int y);                                      /* 0x403170 */
int DrawRLEImageUnclipped(RasterClip *clip, RLEFrameHeader *frameHeader,
                          int x, int y,
                          int strideScratch);                         /* 0x403574 */
void SetPaletteTranslationTable(const unsigned char *translation);    /* 0x40363B */
int DrawRLEImageColor(RasterClip *clip, unsigned char *shape, int frame,
                      int x, int y);                                 /* 0x40365A */
int DrawRLEImageColorUnclipped(RasterClip *clip,
                               RLEFrameHeader *frameHeader,
                               int x, int y,
                               int strideScratch);                    /* 0x403B32 */
int RotateRLEImage(RasterClip *clip, unsigned char *shape, int frame,
                   int x, int y, unsigned char *scratch,
                   unsigned int angleTenths, int scaleX, int scaleY,
                   unsigned int flags);                              /* 0x403C65 */
unsigned int GetRLEFrameBounds(unsigned char *shape, int frame,
                               int x, int y, unsigned int flags,
                               int *bounds);                          /* 0x404811 */
int EncodeRasterClipToRLEFrame(RasterClip *clip,
                               unsigned char transparentColour,
                               int originX, int originY,
                               unsigned char *output);                /* 0x404989 */
unsigned int TranslateRLEFramePalette(unsigned char *shape,
                                      int frame);                     /* 0x404C0C */
void EncodeRLEScanline(int pixelCount, unsigned char transparentColour,
                       int sourceX);                                  /* 0x404C9E */
void EmitRLEScanlineRun(int runType, int trailingCount,
                        int sourceX);                                 /* 0x404E27 */
int FillRasterClip(RasterClip *clip, int colour);                      /* 0x405004 */
int BlitRasterClip(RasterClip *source, int sourceX, int sourceY,
                   RasterClip *destination, int destinationX,
                   int destinationY, unsigned int colour);            /* 0x4050E3 */
int ScrollRasterClipWrapped(RasterClip *clip, int deltaX, int deltaY,
                            int mode,
                            unsigned int fillOrScratch);              /* 0x40547F */
unsigned int DrawRasterEllipse(RasterClip *clip, int x, int y,
                               int horizontalRadius, int verticalRadius,
                               int colour);                            /* 0x40567C */
unsigned int FillRasterEllipse(RasterClip *clip, int x, int y,
                               int horizontalRadius, int verticalRadius,
                               int colour);                            /* 0x4059BD */
void GetRLETransformTrig(int angleTenths, int *cosine, int *sine);     /* 0x406ACF */
void CalculateRoundedRLEFixedProduct(int left, int right, int *result); /* 0x406B87 */
void TransformRLEPoint(int *point, int *result, int *origin,
                       unsigned int angleTenths, int scaleX,
                       int scaleY);                                   /* 0x406BAD */
unsigned int GetRawImageHeight(unsigned char *shape);                 /* 0x406C74 */
unsigned int GetRawFrameWidth(unsigned char *shape, int frame);       /* 0x406C87 */
int BlitRawFrame(RasterClip *clip, int x, int y, unsigned char *shape,
                 int frame, unsigned char *translation);              /* 0x406CA7 */
void BlitSelectedRawFrames(RasterClip *clip, int x, int y,
                           unsigned char *shape,
                           const unsigned char *frames,
                           unsigned char *translation);               /* 0x406E3A */
int BlitRawScanline(RasterClip *clip, int y,
                    const unsigned char *pixels,
                    int width);                                      /* 0x406E71 */
unsigned char *FindIFFChunkData(const char *chunkId,
                                const unsigned char *iffData);        /* 0x406F80 */
unsigned int DecodeIFFImage(RasterClip *clip,
                            const unsigned char *iffData);            /* 0x406FC2 */
void CopyILBMPalette(const unsigned char *iffData,
                     unsigned char *palette);                         /* 0x407189 */
unsigned int GetILBMImageSize(const unsigned char *iffData);          /* 0x4071BA */
unsigned int DecodePCXImage(RasterClip *clip,
                            const unsigned char *pcxData);            /* 0x4071E7 */
void CopyPCXPaletteFromFileTail(const unsigned char *fileData,
                                unsigned int fileSize,
                                unsigned char *palette);              /* 0x407269 */
unsigned int GetPCXImageSize(const unsigned char *header);            /* 0x407294 */
void ResetGIFLZWDictionary(void);                                     /* 0x4072BB */
unsigned int ReadGIFDataSubBlockByte(void);                           /* 0x407303 */
unsigned int ReadGIFLZWCode(void);                                    /* 0x40731C */
void AppendGIFLZWDictionaryEntry(void);                               /* 0x407362 */
void EmitGIFDecodedPixel(void);                                       /* 0x4073A8 */
unsigned int ExpandGIFLZWImage(RasterClip *clip,
                               const unsigned char *gifData,
                               void *workspace);                      /* 0x407425 */
void CopyGIFPalette(const unsigned char *gifData,
                    unsigned char *palette);                          /* 0x40763E */
unsigned int GetGIFImageSize(const unsigned char *gifData);           /* 0x40769F */
unsigned int GetRLEFrameDimensions(unsigned char *shape,
                                    int frame);                        /* 0x4076D7 */
unsigned int GetRLEFrameExtents(unsigned char *shape,
                                int frame);                           /* 0x4076F9 */
unsigned int GetRLEImageSize(unsigned char *shape, int frame);         /* 0x40771C */
unsigned int GetRLEImageOrigin(unsigned char *shape, int frame);       /* 0x407750 */
void ApplyRLEFramePalette(unsigned char *shape, int frame,
                          unsigned char *palette);                    /* 0x40777A */
unsigned int CopyRLEFramePalette(unsigned char *shape, int frame,
                                 unsigned char *entries);             /* 0x4077C5 */
unsigned int SetRLEFramePalette(unsigned char *shape, int frame,
                                const unsigned char *entries);        /* 0x40780D */
unsigned int GetRLEFrameCount(const unsigned char *shape);            /* 0x407857 */
int CollectUniqueRLEImageFrames(const unsigned char *shape,
                                unsigned int *frames);                /* 0x40786A */
int CollectUniqueRLEPaletteFrames(const unsigned char *shape,
                                  unsigned int *frames);              /* 0x4078CC */
void FadeRasterPaletteToPalette(RasterSurface *surface,
                                const unsigned char *targetPalette,
                                unsigned int duration);               /* 0x407C21 */
int CollectRasterClipColours(RasterClip *clip,
                             unsigned int *colours);                  /* 0x407DA5 */
void CorrectPointers(void);                                            /* 0x47EA00 */
void ClearRoomMenuLabel(void);                                        /* WC2 unmapped */
int IsRoomMenuLabelEmpty(void);                                       /* WC2 unmapped */
void DrawRoomMenuLabel(TextContext *context,
                       const char *label);                            /* WC2 unmapped */
void RefreshRoomMenuLabel(void);                                      /* 0x45641B */
void ClearRoomMenuCursorFrame(void);                                  /* WC2 unmapped */
void SelectRoomMenuLabel(short i);                                    /* WC2 unmapped */
void InitializeRoomMenu(TitleMenuRegion *regions, char **labels,
                        Viewport *viewport, char *text,
                        unsigned char alignment);                     /* 0x4575B4 */
int FindMenuRegionAtPoint(short x, short y,
                          const TitleMenuRegion *regions);             /* 0x464E06 */
void InitializeRoomViewports(void);                                   /* 0x45C128 */
short RecRoom(void);                                                  /* 0x418FFC */
void ShowChalkBoard(void);                                            /* 0x459C4D */
void ResetCampaignData(void);                                         /* WC2 unmapped */
short __stdcall ReadPacketSectionData(PacketSectionHandle *handle,
                                      void *destination,
                                      unsigned int length);            /* 0x469BE0 */
void CheckHeapBlockSignature(unsigned char *shape);                  /* 0x4254C0 */
unsigned char *GetPreparedShapeData(unsigned char *shape);           /* 0x42553A */
short __stdcall GetShapeFrameCount(unsigned char *shape);            /* 0x425550 */
void GetShapeFrameExtents(unsigned char *shape, short frame,
                          short *width, short *height,
                          short *leftExtent, short *topExtent);       /* 0x42557C */
void DecodeShapeFrame(unsigned char *shape, short frame,
                      unsigned char *bitmap, int width, short height,
                      int leftExtent, int topExtent);                 /* 0x425618 */
int SignExtendClipCoord(volatile short v);                             /* 0x4259E2 */
void ValidateViewportBounds(Viewport *viewport, RasterSurface *surface,
                            RasterClip *clip);                         /* 0x425A16 */
void ClipViewportToScreen(Viewport *viewport);                         /* WC2 unmapped */
void SetSolidColourTranslation(unsigned char colour);                 /* 0x425BBF */
void PrepareShapeRLEData(unsigned char *shape);                        /* 0x425BF6 */
void DrawSpriteTransformed(Viewport *viewport, int x, int y,
                           unsigned char *shape, int frame,
                           int angle, int scaleX, int scaleY,
                           int flip, int blendMode);                /* 0x425FB4 */
void RasterLineHook(const void *marker);                              /* 0x4261D5 */
void DrawFontGlyph(char character, TextContext *context, int height,
                   int width, int y);                                 /* 0x4261E5 */
void __stdcall MarkActivePaletteEntries(Viewport *viewport,
                                         unsigned char *active);       /* WC2 unmapped */
void __stdcall GetPaletteEntry(short index, unsigned short *rgb);         /* 0x461D68 */
void __stdcall SetPaletteEntry(short index, short *rgb);                  /* 0x4266CC */
void DrawSpriteDefault(Viewport *viewport, short x, short y,
                       unsigned char *shape, short frame);          /* 0x4266E7 */
void CaptureSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, unsigned char *shape,
                             short frame);                          /* 0x426769 */
void RestoreSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, unsigned char *shape,
                             short frame);                          /* 0x426B96 */
void DrawSolidColourSprite(Viewport *viewport, short x, short y,
                           unsigned char *shape, short frame,
                           unsigned char colour);                    /* 0x426FD9 */
void CopyViewportContents(Viewport *source, Viewport *destination);    /* 0x427047 */
void ClearViewport(Viewport *viewport, short colour);                  /* 0x427123 */
void DrawViewportPixel(Viewport *viewport, short x, short y,
                       short colour);                                 /* 0x427179 */
int GetViewportPixel(Viewport *viewport, short x, short y);            /* 0x4271D2 */
void DrawViewportLine(Viewport *viewport, short x1, short y1,
                      short x2, short y2, short colour);             /* WC2 unmapped */
void TriangleRasterizerHook(void);                                    /* WC2 unmapped */
void DrawFilledViewportRect(Viewport *viewport, short left, short top,
                            short right, short bottom,
                            short colour);                           /* 0x42722E */
void DrawViewportBorder(Viewport *viewport, short left, short top,
                        short right, short bottom,
                        short colour);                                /* 0x427EC8 */
void DrawViewportEllipse(Viewport *viewport, short x, short y,
                         short verticalRadius, short horizontalRadius,
                         short colour);                               /* 0x428023 */
void FillViewportEllipse(Viewport *viewport, short x, short y,
                         short verticalRadius, short horizontalRadius,
                         short colour);                               /* 0x42808F */
void DrawViewportEllipseShadow(Viewport *viewport, short x, short y,
                               short verticalRadius,
                               short horizontalRadius,
                               short colour);                         /* 0x4280FB */
void DrawSpriteScaled(Viewport *viewport, short x, short y,
                      unsigned char *shape, short frame, short angle,
                      short scale, short flip);                       /* 0x440549 */
void DrawSolidColourSpriteScaled(Viewport *viewport, short x, short y,
                                 unsigned char *shape, short frame,
                                 short angle, short scale, short flip,
                                 unsigned char colour);                /* 0x428344 */
int GetTransformedShapeBounds(Viewport *viewport, short x, short y,
                              unsigned char *shape, short frame,
                              short angle, short scale, int flip,
                              short *bounds);                         /* 0x4283A4 */
void fizzle_fade(Viewport *source, Viewport *destination,
                 const ScreenViewportGeometry *geometry);            /* 0x428690 */
void snow_viewport(Viewport *viewport, int effect,
                   unsigned short colour);                            /* 0x427DE8 */
void UpdateStreamerStoppedFlag(void);                                    /* WC2 unmapped */
void SignalAudioMixerWakeEvent(void);                                 /* 0x42894B */
void InitializeAudioStreamer(HWND window);                            /* 0x428900 */
void Streamer_open(const char *streamName);                           /* 0x4289BF */
void Streamer_play(void);                                             /* 0x428A2F */
void Streamer_stop(void);                                                /* WC2 unmapped */
void ClearStreamerTrigger(void);                                      /* 0x428AEB */
void Streamer_trigger(int trigger);                                   /* 0x424113 */
void SetStreamerIntensity(unsigned char intensity);                   /* 0x443631 */
void ForceStreamerTrigger(int trigger);                               /* WC2 unmapped */
void SetMusicStreamVolume(unsigned short volume);                     /* 0x428DA3 */
void Streamer_close(void);                                            /* 0x428B86 */
int ReadCheaterFlagFromRegistry(void);                                /* 0x428D09 */
int ix_system_init(void);                                              /* 0x469C80 */
void ix_system_service_sounds(void);                                     /* 0x469D27 */
void ix_system_shutdown(void);                                            /* 0x469E73 */
void ix_system_configure(int option, void *value);                     /* 0x4624A7 */
void ix_system_set_master_volume(unsigned short volume);              /* 0x469EDB */
void ix_system_set_voice_count(int voiceCount);                        /* 0x469F0C */
IxSample *ix_system_new_sample(void);                                  /* 0x469F63 */
void ix_system_delete_all_samples(void);                                  /* 0x469FC8 */
IxSound *ix_system_new_sound(IxSample *sample);                        /* 0x46A00D */
void ix_system_delete_all_sounds(void);                                   /* 0x46A090 */
void ix_system_delete_sound(IxSound *sound);                              /* 0x46A047 */
void __fastcall ix_sound_start(IxSound *sound);                       /* 0x46A758 */
void __fastcall ix_sound_release(IxSound *sound);                    /* 0x46AA9E */
void __fastcall ix_sound_stop(IxSound *sound);                       /* 0x46AB4F */
int __fastcall ix_sound_is_playing(IxSound *sound);                  /* WC2 unmapped */
int ix_streamer_init(void);                                           /* 0x46BD50 */
void ix_streamer_destroy(void);                                      /* 0x46BE6F */
void ix_streamer_configure(int option, void *value);                  /* 0x469EA6 */
int ix_streamer_open_stream_file(char *path);                         /* 0x46BFB6 */
void ix_streamer_close_stream_file(void);                             /* 0x46C67A */
void ix_streamer_audio_play(void);                                    /* 0x46C7F3 */
void ix_streamer_audio_stop(void);                                    /* 0x46C853 */
void ix_streamer_set_intensity(unsigned char intensity);              /* 0x46C96B */
void ix_streamer_set_trigger(char trigger);                           /* 0x46C9C1 */
void ix_streamer_force_trigger(char trigger);                         /* 0x46CA2E */
void ix_streamer_set_volume(unsigned short volume);                   /* 0x46CBBE */

#endif /* WC1_FUNCS_H */
