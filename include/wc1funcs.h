/*
 *  Prototypes for every reimplemented function, in address order.
 *
 *  Address order is link order, so this list doubles as the reconstruction
 *  inventory: see src/map for the same list with the original addresses.
 */
#ifndef WC1_FUNCS_H
#define WC1_FUNCS_H

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
int __stdcall GetJoystickPosition(unsigned int *x, unsigned int *y,
                                  unsigned int *buttons, short joystick,
                                  unsigned int fallback);              /* 0x00402A20 */
short GetJoystickButtons(void);                                                   /* 0x00402AC0 */
void GetJoystickDevCaps(short joystick, short *xMin, short *xMax,
                        short *yMin, short *yMax);                    /* 0x00402AE0 */
HINSTANCE GetApplicationInstance(void);                               /* 0x00402B80 */
HWND GetMainWindowHandle(void);                                       /* 0x00402B90 */
HDC GetMainWindowDeviceContext(void);                                 /* 0x00402BA0 */
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
void FreeFontWorkspace(FontWorkspace **workspace);                    /* 0x00403180 */
char *LocateStreamsDirOnDisc(void);                                  /* 0x004031C0 */
char FindCdRomDriveByVolumeLabel(const char *label,
                                 const char *directory);              /* 0x00403290 */
int SetCurrentDirOnDrive(char drive, const char *directory);          /* 0x004033E0 */
int PromptInsertCorrectCd(void);                                      /* 0x00403450 */
short __stdcall OpenDataFileOrDie(const char *path);                           /* 0x004034D0 */
void __stdcall CloseDataFile(unsigned short fd);                           /* 0x00403500 */
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
int print_subtitle(Viewport *viewport, short colour,
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
unsigned short __stdcall LoadPaletteTripletsFile(const char *path);     /* 0x00404610 */
unsigned int ejection_sequence(void);                                    /* 0x004046A0 */
void stranded_sequence(void);                                            /* 0x00404BE0 */
unsigned int ParseFaceAnimation(char *text, short *commands);           /* 0x00404CD0 */
unsigned int ParseMouthAnimation(char *text, short *commands);          /* 0x00404D70 */
char *AddPCName(char *text);                                            /* 0x00404E10 */
unsigned int LoadFace(short face);                                      /* 0x004050B0 */
unsigned int LongTalk(unsigned char *talker, char *text,
                      short *mouthCommands, short *faceCommands,
                      short duration);                                 /* 0x00405290 */
unsigned int CloseTalk(unsigned char *talker, short mouthFrame,
                       short faceFrame);                               /* 0x004054B0 */
unsigned int Briefing(short series, short mission);                    /* 0x00405660 */
unsigned int DeBriefing(short series, short mission);                  /* 0x004056F0 */
unsigned int Office(void);                                             /* 0x00405840 */
unsigned int LoadBriefingData(short series, short mission);             /* 0x00405910 */
unsigned int LoadMissionData(short series, short mission);              /* 0x004059B0 */
unsigned int UpdateMap(char *text, short duration);                     /* 0x00405CC0 */
unsigned int CloseLook(unsigned char *shape, short shot,
                       short *animation, char *text, short duration,
                       short unused);                                  /* 0x00405DE0 */
void SetShipAiScratchWord(unsigned short v);                                 /* 0x004060A0 */
void maneuver_complete(short ship);                                     /* 0x004060B0 */
void Mline_up_drop(short ship, short target);                            /* 0x004060D0 */
void Mwabble(short ship);                                                /* 0x00406130 */
void advance(short ship);                                               /* 0x004061E0 */
void ShipAiState35(short ship, short target);                            /* 0x00406200 */
void Mfull_ahead(short ship);                                           /* 0x00406310 */
void Mchill(short ship, short target);                                  /* 0x00406350 */
void Mdrop_a_mine(short ship);                                          /* 0x004063B0 */
void Mthink(short ship);                                                /* 0x00406400 */
void Mtight_loop(short ship);                                           /* 0x00406440 */
void Mhard_break(short ship);                                           /* 0x004064F0 */
void Msit_n_spin(short ship, short target);                             /* 0x004065A0 */
void Mturn_n_spin(short ship, short target);                            /* 0x004067A0 */
void Mburnout(short ship, short target);                                /* 0x00406860 */
void Mkickit(short ship);                                               /* 0x004068D0 */
void Mturn_n_kick(short ship);                                          /* 0x00406910 */
void Mroll_over(short ship);                                            /* 0x00406990 */
void Mhard_turn(short ship);                                            /* 0x004069F0 */
void Mfish_hook(short ship, short target);                              /* 0x00406A50 */
void Mtry2tail(short ship, short target);                               /* 0x00406B60 */
void Msplit_left(short ship);                                           /* 0x00406BD0 */
void Msplit_right(short ship);                                          /* 0x00406C20 */
void Mgloat(short ship);                                                /* 0x00406C70 */
void Mtail_fire(short ship, short target);                              /* 0x00406D20 */
void Mzip_past(short ship, short target);                               /* 0x00406D80 */
void Mtarget_missile(short ship, short target);                         /* 0x00406E10 */
void Mram_missile(short ship, short target);                            /* 0x00406EC0 */
void Mbuzz_debris(short ship);                                          /* 0x00406F20 */
void Mstrafe_enemy(short ship, short target);                           /* 0x00406FB0 */
void Mbest_strafe(short ship, short target);                            /* 0x00407030 */
void Msit_n_fire(short ship, short target);                             /* 0x00407060 */
void Mstrafe_n_roll(short ship, short target);                          /* 0x004070D0 */
void Mkill_missile(short ship, short target);                           /* 0x00407100 */
void Msuicide_run(short ship, short target);                            /* 0x004071B0 */
void Mget_distance(short ship, short target);                           /* 0x004071E0 */
void general_zig(short ship, unsigned int target, short pitch);         /* 0x00407270 */
void Mzig_zag(short ship, unsigned int target);                         /* 0x00407350 */
void Mzig_zag_pitch(short ship, unsigned int target);                   /* 0x00407370 */
void Mcorkscrew(short ship);                                            /* 0x00407390 */
void Mveer_away(short ship, short target);                              /* 0x00407450 */
void ShipAiState44(short ship);                                         /* 0x00407560 */
void Mtarget_laser(short ship, short target);                           /* 0x00407580 */
void Mrout_me(short ship);                                              /* 0x004075A0 */
void Mnone(void);                                                       /* 0x004075B0 */
void Mreset(short ship);                                                /* 0x004075C0 */
void perform_maneuver(short obj);                                       /* 0x004075D0 */
short __stdcall GetShapeFrameExtent(short x, short y,
                                    unsigned char *shape, short frame,
                                    short extent);                       /* 0x00407710 */
unsigned int AnimateScrambleWalk(short ticks);                          /* 0x00407750 */
unsigned int PlayScrambleHangarScene(void);                            /* 0x004079C0 */
unsigned int DrawScrambleActor(short actorIndex);                      /* 0x00407C90 */
unsigned int ConfigureScrambleActor(short x, short y, short deltaX,
                                    short deltaY, unsigned char *shape,
                                    short scale, short angle,
                                    signed char flip,
                                    short actorIndex);                 /* 0x00407D90 */
unsigned int DrawScrambleFrame(void);                                  /* 0x00407E10 */
unsigned int scramble(void);                                           /* 0x00408200 */
unsigned int landing(signed char damageLevel);                         /* 0x00408650 */
unsigned int funeral_player(void);                                     /* 0x00408B30 */
unsigned int funeral_wingman(char *text, short duration);              /* 0x00408D50 */
unsigned int funeral_sequence(int playerFuneral);                      /* 0x00408DE0 */
unsigned int RunAnimationDemoLoop(signed char animation);             /* 0x004094E0 */
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
void set_sphere_point(const MissionShipRecord *record,
                      FixedVector *position);                           /* 0x0040B670 */
unsigned int is_alive(int pilot);                                       /* 0x0040B6A0 */
unsigned int check_futurion(short i);                                   /* 0x0040B700 */
unsigned int init_mission(short series, short mission);                 /* 0x0040B730 */
void prepare_mission(void);                                            /* 0x0040B7A0 */
int release_all_capital_ship_shapes(void);                             /* 0x0040B940 */
int release_capital_ship_shapes(enum ObjectType type);                 /* 0x0040B990 */
int load_ship(enum ObjectType type, short slot);                       /* 0x0040B9F0 */
int free_ship(short slot);                                             /* 0x0040BC70 */
int free_all_slots(void);                                              /* 0x0040BE20 */
int load_all_slots(void);                                              /* 0x0040BE60 */
void remove_nav_point_objects(void);                                   /* 0x0040BEA0 */
short get_shape_slot(void);                                            /* 0x0040BEC0 */
int shape_loaded(enum ObjectType type);                                /* 0x0040BEF0 */
int shape_needed(const MissionNavPoint *navPoint,
                 enum ObjectType type);                                /* 0x0040BF20 */
int new_sphere_shapes(MissionNavPoint *navPoint);                      /* 0x0040BF50 */
int set_up_action_sphere(short navPoint);                              /* 0x0040BFF0 */
void free_pilot_talk(short personality);                               /* 0x0040C150 */
void get_pilot_talk(short personality);                                /* 0x0040C1C0 */
unsigned int init_personalities(void);                                 /* 0x0040C2B0 */
unsigned int room_for_me(short obj, short minimum);                    /* 0x0040C350 */
void approve_xyz(short obj, short minimum, short maximum);             /* 0x0040C360 */
void set_up_next_wave(void);                                           /* 0x0040C3C0 */
unsigned int sub_int_vector(const ShortVector *left,
                            const ShortVector *right,
                            ShortVector *difference);                  /* 0x0040C4A0 */
unsigned int set_formation_position(short obj,
                                    const MissionShipRecord *record);  /* 0x0040C4E0 */
void Set_up_ship_info(short obj, short missionShip,
                      signed char navPoint);                           /* 0x0040C5E0 */
unsigned int is_team_member(short missionShip);                        /* 0x0040C740 */
unsigned int find_next_ship_turn_slot(short obj);                     /* 0x0040C780 */
short init_ship(short missionShip, short navPoint);                    /* 0x0040C800 */
unsigned int init_intelligence_data(short obj);                       /* 0x0040C950 */
short __stdcall SampleBothJoysticks(InputDeviceSample *samples,
                                    unsigned int fallback);             /* 0x0040CAA0 */
int __stdcall SampleJoystickDevice(InputDeviceSample *samples,
                                   short joystick,
                                   unsigned int fallback);              /* 0x0040CAE0 */
void SampleActiveJoystickDevice(void);                                 /* 0x0040CB30 */
void DrawNavTextLine(unsigned char alignment, unsigned short colour,
                     const char *format, ...);                         /* 0x0040CB60 */
void SetNavMapCoordinateScaling(short enabled);                       /* 0x0040CBB0 */
void ScaleNavMapMarkerSize(short *size);                              /* 0x0040CBC0 */
void ScaleNavMapCoordinates(short *x, short *y,
                            short mapX, short mapY);                  /* 0x0040CBE0 */
void nav_getxy(short *x, short *y, int worldX, int worldZ);           /* 0x0040CC30 */
void CheckPoint(short x, short y);                                    /* 0x0040CC80 */
void IncludeNavMapWorldPoint(int worldX, int worldZ);                  /* 0x0040CCF0 */
void SetScale(void);                                                   /* 0x0040CD30 */
void Build_objective_list(void);                                       /* 0x0040CED0 */
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
                          short size, signed char colour);            /* 0x0040D640 */
void DrawNavSquareMarker(short x, short y, short size,
                         short shadow, unsigned short colour,
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
void DrawNavPlayerMarker(unsigned char colour, short reserve);        /* 0x0040D980 */
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
unsigned short MergeAdjacentNearHeapBlocks(int descriptorAddress);   /* 0x0040E890 */
int ReleaseNearHeapBlock(int descriptorAddress);                     /* 0x0040E900 */
void PurgeNearHeapBlocks(unsigned short flags);                       /* 0x0040E950 */
unsigned short InitializeNearHeap(void);                              /* 0x0040E9E0 */
void *AllocateNearHeapBlockFromEnd(int size, unsigned short flags);   /* 0x0040EB70 */
void *AllocateNearHeapBlockByFlags(int size, unsigned short flags);   /* 0x0040ED30 */
void add_statistics(short pilot, short missions, short kills);         /* 0x0040EFE0 */
void PostMission(void);                                                 /* 0x0040F010 */
int FullMissionScore(void);                                             /* 0x0040F190 */
int PlayersMissionScore(void);                                          /* 0x0040F1E0 */
unsigned int UpdateSeries(void);                                        /* 0x0040F240 */
unsigned int MoveNewCampaign(void);                                     /* 0x0040F3F0 */
unsigned int StartNewCampaign(short campaign);                         /* 0x0040F440 */
short GameFlow(void);                                                   /* 0x0040F4B0 */
void __stdcall free_viewport(Viewport *viewport);                      /* 0x0040F940 */
unsigned short GetPaletteReadyUnused(void);                                    /* 0x0040FA30 */
void DrawTitleLogo(short distance, short y);                            /* 0x0040FA40 */
void UpdateTitleMenuCursor(void);                                      /* 0x0040FB10 */
int Title_Sequence(void);                                               /* 0x0040FB70 */
void CalibrateJoystickInteractive();                                  /* 0x004102B0 */
void WaitForJoystickButtonRelease(void);                               /* 0x004106C0 */
void WaitForJoystickButtonPress(void);                                 /* 0x00410700 */
void SetFleetOverviewView(int initializeCockpit);                     /* 0x00410740 */
void rotate_eye_to_goal(void);                                         /* 0x00410A30 */
short GetVectorMagnitude(const FixedVector *vector);                   /* 0x00410AD0 */
unsigned int set_eye_direction_and_position(void);                     /* 0x00410AF0 */
unsigned int force_view(int view, short obj);                          /* 0x004117B0 */
unsigned int new_view(int view, short obj);                            /* 0x004117D0 */
unsigned int start_dust(short obj, FixedVector origin,
                        short forwardDistance, int rightOffset,
                        int upOffset);                                 /* 0x00411EC0 */
unsigned int generate_stars(void);                                    /* 0x00411FE0 */
unsigned int update_star_field(void);                                 /* 0x00412100 */
unsigned short count_down(short obj);                                   /* 0x00412410 */
unsigned int house_keep_objects(void);                                 /* 0x00412430 */
void update_objects_in_space(void);                                    /* 0x00412820 */
unsigned int rotate_object(short obj);                                 /* 0x00412920 */
unsigned int accelerate_and_move_object(short obj);                    /* 0x004129A0 */
unsigned int animate_shape(short obj);                                 /* 0x00412CD0 */
unsigned int animate_object(short obj);                                /* 0x00412E30 */
unsigned int hit_asteroid(short asteroid, short destructionChance);    /* 0x00413030 */
int object_collision(short obj);                                      /* 0x004130D0 */
unsigned int object_intelligence(short obj);                           /* 0x00413880 */
void EmitTextString(void (__stdcall *writer)(int), const char *text);   /* 0x00413A10 */
void FormatTextTokens(void (__stdcall *writer)(int),
                      const char *format, unsigned long *arguments);    /* 0x00413A40 */
void DrawFormattedText(const char *format, ...);                        /* 0x00413C40 */
void FormatTextBufferFromStart(const char *format, ...);                /* 0x00413C70 */
void AppendFormattedText(const char *format, ...);                      /* 0x00413CB0 */
void FatalErrorAndExit(const char *format, ...);                       /* 0x00413CE0 */
unsigned short IsCockpitExplosionActive(void);                         /* 0x00413D20 */
void EraseCockpitReadoutRegion(Viewport *viewport, short left,
                               short top, short right, short bottom,
                               short colour);                         /* 0x00413D40 */
void vdu_polygon(signed char bar, short percent);                     /* 0x00413DA0 */
unsigned int InitializeCockpitReadout(signed char slot,
                                      TextContext *context);          /* 0x00413F70 */
void DrawCockpitReadout(signed char slot, char *text);                /* 0x00413FB0 */
void EraseCockpitReadoutAtPosition(signed char slot, short left,
                                   short top);                        /* 0x00414050 */
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
short kilrathi_near(short obj, short range);                           /* 0x00414300 */
short auto_pilot_valid(short showReason);                              /* 0x00414380 */
void *reset_cockpit(void);                                            /* 0x00414410 */
unsigned int SetCockpitLightBlink(signed char light, short interval); /* 0x00414440 */
void draw_cockpit_lights(void);                                      /* 0x00414490 */
void update_lights(void);                                            /* 0x004145B0 */
void update_bars(void);                                              /* 0x00414690 */
unsigned short get_mode(short i);                                     /* 0x004147E0 */
void set_mode(short i, int state);                                    /* 0x00414800 */
unsigned short SetVduModeIfChanged(short i, int state);               /* 0x00414850 */
int GetVduModeStackDepth(short i);                                    /* 0x00414890 */
void push_mode(short i, int state);                                   /* 0x004148A0 */
void pop_mode(short i);                                               /* 0x004148E0 */
void set_new_vdu(short vdu);                                          /* 0x00414910 */
short update_vid_disp(short vdu);                                     /* 0x00414980 */
void InvalidateVduMode(short i);                                          /* 0x004149C0 */
void clear_message_time(void);                                              /* 0x004149E0 */
unsigned short message_showing(void);                               /* 0x004149F0 */
unsigned short set_message_time(unsigned short v);                        /* 0x00414A10 */
void check_message(void);                                             /* 0x00414A20 */
unsigned int update_digital_readouts(void);                          /* 0x00414A50 */
void PlayTargetLockSfx(void);                                           /* 0x00414AD0 */
void malf_sound(void);                                                  /* 0x00414AE0 */
unsigned short malf(char component);                                  /* 0x00414AF0 */
unsigned short vdu_malf(short vdu, short sound);                       /* 0x00414B20 */
void ShowComponentHitHudMessage(char *text, unsigned short colour,
                                signed char flashCount);               /* 0x00414B70 */
int damage_your_component(char component, char amount, char maximum); /* 0x00414BF0 */
void RemovePlayerReleaseWeapon(signed char weapon);                  /* 0x00414CB0 */
void fire_computer_graphic_missile(void);                            /* 0x00414D50 */
void show_weapon_disp(void);                                         /* 0x00414EA0 */
void update_status_text(void);                                        /* 0x00415040 */
short sighted(short objective);                                       /* 0x00415050 */
short visited(short objective);                                       /* 0x00415070 */
short achieved(short objective);                                      /* 0x00415090 */
void flag_objective(short objective, unsigned char flags);            /* 0x004150B0 */
void DrawCalculatingLabel(void);                                      /* 0x004150D0 */
char *objective_name(short objective);                                /* 0x00415130 */
void show_navigation_disp(void);                                      /* 0x00415180 */
short hidden_objective(short objective);                              /* 0x004151F0 */
int set_new_objective(short pathIndex);                               /* 0x004152C0 */
short cycle_next_objective(void);                                     /* 0x00415370 */
void set_next_destination(void);                                       /* 0x004153D0 */
short LocateMobileObjective(short objective);                         /* 0x00415470 */
unsigned int someone_coming(void);                                     /* 0x004154C0 */
unsigned int escorting_a_ship(void);                                   /* 0x00415510 */
void flag_reached(short objective, short reached);                     /* 0x00415530 */
void check_sighting(short objective, short range, short object);      /* 0x004156D0 */
void check_visit(short objective, short range);                       /* 0x00415720 */
void update_objective_location(short objective);                     /* 0x00415770 */
unsigned int objective_lost(short objective);                         /* 0x00415850 */
void check_objectives(void);                                          /* 0x004158A0 */
void rotational_pos_to_scanner_pos(signed char object,
                                   const SphericalVector *position);  /* 0x004158F0 */
short mobile_objective(short objective);                              /* 0x00415A30 */
void ResetScannerContacts(void);                                     /* 0x00415A70 */
void clear_head_up_display(void);                                    /* 0x00415A90 */
unsigned int set_objective_range(short showOnScanner);                /* 0x00415B70 */
short get_color(short object, unsigned short *colour);                /* 0x00415C00 */
unsigned int draw_3d_scanner(void);                                   /* 0x00415CE0 */
void start_lock(unsigned short v);                                    /* 0x00415FC0 */
unsigned short starting_lock(unsigned short v);                       /* 0x00415FF0 */
void lock_off(void);                                                   /* 0x00416010 */
short CheckTargetLockMalfunction(void);                               /* 0x00416040 */
short decrement_lock_time(short screenX);                             /* 0x00416090 */
void target_locking(signed char target);                              /* 0x00416120 */
void SetRectBounds(Viewport *viewport, unsigned short left,
                   unsigned short top, unsigned short right,
                   unsigned short bottom);                           /* 0x00416220 */
short GetRectHeight(const Viewport *viewport);                        /* 0x00416250 */
void print_message_text(char *text, unsigned short colour);            /* 0x00416260 */
void ShowHudTextLine(char *s, unsigned short b);                       /* 0x00416460 */
void SetHudTextColour(short v);                                              /* 0x00416480 */
void draw_target_box(unsigned short colour, signed char object,
                     short solid, short drawLockMarker, short padding,
                     ShortRect *savedBounds);                         /* 0x004164B0 */
void remove_nav_pointer(void);                                        /* 0x004168A0 */
void draw_nav_pointer(void);                                          /* 0x004168C0 */
unsigned int overlay_head_up_display(void);                           /* 0x00416AC0 */
void RestoreCockpitExplosionIfVisible(void);                           /* 0x00416C90 */
unsigned int RestoreTransientCockpitGraphics(void);                    /* 0x00416CB0 */
void SetHudMessageText(char *text, unsigned short colour,
                       unsigned short duration);                       /* 0x00416DE0 */
void malf_noise(short vdu, int effect, unsigned short colour,
                short sound, short refresh);                           /* 0x00416E20 */
void build_your_target_list(short *hasEnemy);                         /* 0x00416E90 */
void cycle_onscreen_targets(void);                                    /* 0x00416F30 */
void check_target(void);                                             /* 0x00416FD0 */
void update_missile_warning(void);                                   /* 0x00417190 */
void determine_pilot_hand(void);                                      /* 0x004171D0 */
void DrawPilotHandFrame(void);                                       /* 0x00417260 */
void CopyTrainSimPilotViewToRightVdu(void);                           /* 0x00417320 */
void animate_pilot(void);                                             /* 0x004173C0 */
void ResetPilotHandAnimation(void);                                   /* 0x004173F0 */
void send_message(short obj, signed char message);                      /* 0x00417420 */
void npc_communication(void);                                        /* 0x004174F0 */
void clear_cockpit_damage(void);                                      /* 0x00417610 */
void explosion_draw(void);                                           /* 0x00417630 */
unsigned int DrawPendingCockpitDamage(void);                          /* 0x004176C0 */
void RestoreCockpitExplosionBackground(void);                         /* 0x00417760 */
void cockpit_explosion(void);                                        /* 0x004177B0 */
void place_damage_on_cockpit(short damage);                           /* 0x004178A0 */
void vid_transmit(void);                                              /* 0x00417910 */
void vid_equiv(short obj, short message);                             /* 0x00417AC0 */
void update_dead_disp(short a);                                        /* 0x00417B10 */
void check_stranded(void);                                           /* 0x00417B30 */
void update_VDUs(void);                                              /* 0x00417B70 */
void update_cockpit(void);                                           /* 0x00417E70 */
void PlayCockpitSelectionSfx(short selectionSound);                  /* 0x00417F00 */
void vdu_pop_all(short vdu);                                         /* 0x00417F10 */
void SelectCockpitVduMode(short vdu, int mode);                       /* 0x00417F60 */
short __stdcall MeasureTextPixelWidthClamped(const char *text);         /* 0x00418080 */
int __stdcall SeekPacketSection(PacketSectionHandle *handle, int offset,
                                short origin);                         /* 0x004180C0 */
unsigned short GetMusicDriverPresent(void);                                    /* 0x00418130 */
short __stdcall CollectActivePaletteIndices(Viewport *viewport,
                                             unsigned char *indices,
                                             short capacity);          /* 0x00418140 */
short get_ship_max_velocity(short obj);                                /* 0x004181C0 */
short recalc_max_velocity(short ship);                                /* 0x00418210 */
void drain_fuel(short ship, short amount);                            /* 0x00418280 */
void damage_ion_drive(short ship, short amount,
                      short maximum);                                 /* 0x004182B0 */
int GetShipAccelerationRate(short ship);                          /* 0x004182F0 */
void point_at(short obj, FixedVector point);                      /* 0x00418330 */
void look_at(short obj);                                         /* 0x004183A0 */
void position_relative(FixedVector *position, FixedVector direction,
                       short distance);                          /* 0x004183D0 */
void position_relative_ijk(FixedVector *position, short obj,
                           short right, short up, short forward); /* 0x00418420 */
short FixedToShortSaturating(int value);                           /* 0x004184C0 */
int MinInt(int a, int b);                                        /* 0x004184E0 */
int MaxInt(int a, int b);                                        /* 0x004184F0 */
int AbsInt(int v);                                               /* 0x00418500 */
int intfract_sign(int sign, int magnitude);                       /* 0x00418510 */
unsigned short SignShort(short v);                                  /* 0x00418520 */
unsigned int SignFixed(int v);                                      /* 0x00418540 */
short WrapDegrees(short degrees);                                     /* 0x00418560 */
int equ_vector(const FixedVector *left, const FixedVector *right);     /* 0x00418590 */
int IsPairEqualityDifferentFromFlag(const unsigned int *values);      /* 0x004185C0 */
void zero_vector(FixedVector *vector);                                /* 0x004185F0 */
void negate_vector(FixedVector *vector);                              /* 0x00418600 */
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
void random_radial(const FixedVector *center, short radius,
                   FixedVector *position);                           /* 0x00418800 */
void MakeRandomNormalizedVector(FixedVector *vector);                 /* 0x00418840 */
void rectangular_to_spherical(const FixedVector *rectangular,
                              SphericalVector *spherical);       /* 0x00418890 */
void ConvertShortVectorToFixedVector(const ShortVector *source,
                                     FixedVector *destination);       /* 0x00418980 */
void ConvertFixedVectorToShortVector(const FixedVector *source,
                                     ShortVector *destination);       /* 0x004189B0 */
int dot_product(const FixedVector *left, const FixedVector *right);    /* 0x004189E0 */
short vector_angle(FixedVector left, FixedVector right);              /* 0x00418A30 */
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
short facing_to_object(short obj, FixedVector *point);                /* 0x004193D0 */
short match_roll_orientation(short obj, short reference);             /* 0x00419440 */
int set_ship_rotation_goals(short obj, short turnRate,
                            const FixedVector *direction,
                            short pointingMode,
                            short *yawGoal, short *pitchGoal);         /* 0x004194D0 */
void point_ship(short obj, short turnRate,
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
void MoveObjectAlongDirection(short obj, const FixedVector *direction,
                              short distance);                        /* 0x004198A0 */
void NormalizeAndScaleVector(FixedVector *vector, int scale);         /* 0x00419950 */
void SetVectorFixedPoint(unsigned int *p, short v);                           /* 0x00419970 */
unsigned int IsPointWithinRange(FixedVector *from, FixedVector *to,
                                short range);                          /* 0x00419990 */
short check_for_collision(short obj);                                 /* 0x004199C0 */
void position_child(short parent, short hardpoint,
                    FixedVector *position);                       /* 0x00419A70 */
void child_object(short hardpoint, short child, short parent);    /* 0x00419B40 */
short get_ship_slot(void);                                        /* 0x00419B70 */
short find_vacant_3d_object(void);                                /* 0x00419BA0 */
void remove_object(short obj);                                        /* 0x00419BD0 */
void apply_force_to_objects_center(FixedVector *force, short obj);    /* 0x00419CC0 */
void apply_force_to_object(FixedVector *point, FixedVector *force,
                           short obj);                                /* 0x00419D10 */
void rotational_acceleration(FixedVector *point, FixedVector *force,
                             short obj);                              /* 0x00419F70 */
void ClampVectorTo30(short *p);                                           /* 0x0041A0F0 */
void ClampTo30(short *p);                                           /* 0x0041A110 */
unsigned short IsPointWithinEyeViewCone(const FixedVector *point);   /* 0x0041A130 */
void transform_objects_to_your_view(void);                             /* 0x0041A1D0 */
void set_background_objects_rotation(short obj,
                                     FixedVector *direction);    /* 0x0041A530 */
void get_right_shape(short obj, FixedVector *direction);          /* 0x0041A610 */
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
void CreateEmptySaveGameFile(void);                                    /* 0x0041ADA0 */
void EnsureSaveGameFile(void);                                         /* 0x0041B020 */
void InitializeBarracksAnimation(BarracksAnimationState *state);       /* 0x0041B070 */
void FreeBarracksMenuLabel(char **label);                              /* 0x0041B0E0 */
void SetAwakenBarracksMenuLabel(char **label, int series, int mission,
                                char *description);                    /* 0x0041B110 */
void FreeBarracksMenuLabels(void);                                     /* 0x0041B180 */
short SaveGame(short slot, SaveGameRecord *gameRecord);                /* 0x0041B1E0 */
short PromptForTextInput(short x, short y, char *prompt,
                         char *destination, short maximumLength,
                         short inputMode);                             /* 0x0041B420 */
int WarnLoadGameFirst(void);                                           /* 0x0041B550 */
void SaveGameWithNamePrompt(short slot, CampaignState *campaign,
                            PilotRecord *pilots,
                            MissionObjective *objectives);             /* 0x0041B5C0 */
int LoadGame(short slot, SaveGameRecord *gameRecord);                  /* 0x0041B710 */
void LoadGameFromSlot(short slot, CampaignState *campaign,
                      PilotRecord *pilots,
                      MissionObjective *objectives);                   /* 0x0041B980 */
void SetBunkMenuLabel(short occupied, short side, char **label,
                      int series, int mission, char *description);     /* 0x0041BAD0 */
void GetBunkInfo(BarracksAnimationState *state);                       /* 0x0041BB20 */
void DrawBarracksBunks(Viewport *viewport, unsigned char *shape,
                       BarracksAnimationState *state);                 /* 0x0041BBD0 */
void DrawBarracksStaticDetails(Viewport *viewport,
                               unsigned char *shape);                  /* 0x0041BC90 */
void AnimateBarracks(Viewport *viewport, unsigned char *shape,
                     BarracksAnimationState *state);                   /* 0x0041BCE0 */
int ConfirmQuitWingCommander(void);                                   /* 0x0041BF10 */
int ConfirmAwakenAfterBadData(short slot);                             /* 0x0041BF60 */
int ConfirmReplaceFaultyData(short slot);                              /* 0x0041BFE0 */
void HandleBarracksBunkSelection(Viewport *viewport,
                                 unsigned char *shape,
                                 BarracksAnimationState *state,
                                 short region);                        /* 0x0041C090 */
void UpdateBarracksScreen(Viewport *viewport, unsigned char *shape,
                          BarracksAnimationState *state);              /* 0x0041C140 */
short BarracksScreen(void);                                            /* 0x0041C170 */
unsigned short __stdcall StepPaletteTransition(short *current,
                                                const short *target,
                                                short componentCount); /* 0x0041C510 */
char *__stdcall DosStrcat(char *destination, const char *source);      /* 0x0041C740 */
DWORD WINAPI DebugOverlayWorkerProc(void *parameter);                  /* 0x0041C960 */
LRESULT CALLBACK DebugKeyboardHookProc(int code, WPARAM key,
                                       LPARAM flags);                  /* 0x0041CA60 */
void DebugOverlayPrintf(DebugOverlayConsole *console,
                        const char *format, ...);                      /* 0x0041CAB0 */
short MinShort(short a, short b);                                       /* 0x0041D0C0 */
short MaxShort(short a, short b);                                       /* 0x0041D0E0 */
void FreePacketAndClear(int *p, unsigned short releaseFlags);          /* 0x0041D100 */
void ReportPacketLoadError(void *packet, short logicalFile,
                           short retry, short section,
                           const char *sourceTag);                      /* 0x0041D120 */
void *LoadPacketIntoBuffer(short logicalFile, short section,
                           void *destination);                         /* 0x0041D200 */
void *LoadPacketAllocated(short logicalFile, short section);           /* 0x0041D250 */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              unsigned short flags);                    /* 0x0041D2E0 */
unsigned int InitializeTextContextFromFont(TextContext *context,
                                           short fontIndex,
                                           unsigned char colour,
                                           signed char background);    /* 0x0041D510 */
unsigned int ReleaseTextFont(short fontIndex);                         /* 0x0041D590 */
unsigned int DrawTextAt(TextContext *context, short x, short y,
                        char *text, unsigned char alignment);           /* 0x0041D5F0 */
unsigned int SortSignedByteValuesAscending(signed char *values,
                                           short count);                /* 0x0041D650 */
short OpenDiskDataFile(short logicalFile);                              /* 0x0041D6C0 */
void __stdcall PromptInsertNumberedDisk(short logicalFile);             /* 0x0041D760 */
unsigned int GetZeroUnused(void);                                        /* 0x0041DA00 */
short CheckEscaped(void);                                               /* 0x0041DA10 */
short WaitForInputKey(void);                                         /* 0x0041DAA0 */
void WaitForSceneAdvance(short duration, short unused);                /* 0x0041DBA0 */
void MoveMenuPointerFromKeyboard(InputEventState *event);               /* 0x0041DC70 */
void EraseLastTextInputCharacter(void);                              /* 0x0041DDF0 */
short WaitForStreamInputKey(void);                                  /* 0x0041DEB0 */
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
unsigned int match_rotation_goal(short *rotation, short *goal,
                                 short totalError, short rate);         /* 0x0041E400 */
void rotate_object_to_goal(short obj);                                  /* 0x0041E520 */
unsigned int check_for_lost_control(short obj);                        /* 0x0041E650 */
unsigned int celerate(short ship, int delta);                     /* 0x0041E710 */
unsigned int approach_speed(short ship, int targetSpeed);         /* 0x0041E750 */
unsigned int steady_object(short ship);                                 /* 0x0041E7C0 */
short real_velocity(short obj);                                       /* 0x0041E7F0 */
unsigned int fix_velocity(short obj);                                 /* 0x0041E820 */
unsigned int sort_viable_target_list(void);                           /* 0x0041E860 */
void send_appropriate_message(short attacker, short victim);         /* 0x0041E900 */
int inflict_damage(short attacker, short victim, short damage,
                   const FixedVector *impactDirection);                /* 0x0041E9B0 */
short pilot_hit(short obj);                                           /* 0x0041EC60 */
int onboard_explosion(short obj);                                     /* 0x0041ECE0 */
void call_enemy(short obj);                                           /* 0x0041EDB0 */
int internal_damage(short attacker, short victim, short damage,
                    short quadrant);                                  /* 0x0041EE20 */
void revise_shields(short obj);                                       /* 0x0041F1A0 */
int your_internal_damage(short attacker, short damage,
                         short quadrant);                             /* 0x0041F220 */
short ReportComponentRepaired(short component, short minimumDamage);  /* 0x0041F5F0 */
void repair_internal_damage(void);                                   /* 0x0041F660 */
void Create_ship_hit_debris(short obj, short count);                  /* 0x0041F700 */
void check_next_wave(void);                                          /* 0x0041F7C0 */
unsigned int Create_explosion_debris(short obj);                      /* 0x0041F800 */
unsigned int affect_mission_score(short pilot, int event, short amount); /* 0x0041F9E0 */
unsigned int score_for_kill(short pilot, short victim);               /* 0x0041FA90 */
unsigned int analyze_kill(short attacker, short victim);              /* 0x0041FB40 */
short ShipExplosion(short obj);                                      /* 0x0041FBC0 */
short Explosion(short obj);                                          /* 0x0041FCD0 */
short the_creator(short obj);                                        /* 0x0041FEB0 */
int explosion_shock_wave(short obj, short blastDamage);              /* 0x0041FEE0 */
int explode(short attacker, short victim);                            /* 0x00420040 */
int send_at_point(short obj, FixedVector *point, short speed);        /* 0x00420190 */
short find_child_object(short parent, enum ObjectClass objectClass);  /* 0x004201D0 */
short find_child_ship(short parent, enum ObjectClass objectClass,
                      short target);                                  /* 0x00420210 */
int launch_object(short parent, short child, FixedVector direction,
                  short speed);                                      /* 0x00420260 */
void fire(short obj, short target);                                   /* 0x004202D0 */
short hemisphere(FixedVector *target, FixedVector *parent,
                 FixedVector *hardpoint);                             /* 0x004207E0 */
int fire_flack(short owner, short explosion, short range,
               FixedVector *aim);                                    /* 0x00420840 */
int rnd_aim(short radius, short speed, short maximum);                /* 0x004208E0 */
short pop_flack(short obj, short range, FixedVector *hardpoint);      /* 0x00420920 */
int fire_turrets(short obj);                                          /* 0x00420AA0 */
int fire_weapon(short obj, short weapon);                              /* 0x00420C20 */
int fire_missile(short ship);                                         /* 0x00421150 */
short fire_fixed_projectile_weapon(short obj);                         /* 0x00421220 */
int drop_mine(short obj, signed char weapon, enum ObjectType type,
              short lifetime);                                       /* 0x004212A0 */
void fire_afterburner(short obj, short time);                          /* 0x00421350 */
short find_weapon(short obj, enum ObjectType weaponType);              /* 0x00421100 */
unsigned int check_computer_damage(void);                              /* 0x0041F5D0 */
short rnd_sign(short v);                                               /* 0x004208C0 */
unsigned int fire_super_brake(short ship);                              /* 0x004213B0 */
short flip_angle(short ship, short angle);                              /* 0x004213D0 */
void place_exhaust_on_ships(void);                                      /* 0x00421430 */
void reposition_fixed_child_objects(void);                             /* 0x004215E0 */
unsigned int housekeep_power_plant_and_fuel(short ship);                /* 0x00421760 */
unsigned int replenish_shields(short ship);                            /* 0x00421780 */
unsigned int replenish_weapon_energy_bank(short ship);                 /* 0x00421830 */
void accelerate(short amount);                                         /* 0x004218D0 */
void your_afterburner(void);                                            /* 0x00421920 */
unsigned int LoadGamePaletteFile(void);                                /* 0x004219C0 */
unsigned int EMShutDown(void);                                         /* 0x00421A40 */
unsigned short InitializeEventManagerResources(void);                  /* 0x00421A60 */
unsigned int EMStartUp(void);                                          /* 0x00421AB0 */
unsigned int LoadOriginFxDrivers(void);                                 /* 0x00421B10 */
unsigned int InitializeGameTextContexts(void);                          /* 0x00421D80 */
unsigned int initialize_direction_view_frame(short yaw, short pitch,
                                             signed char frame);       /* 0x00421E20 */
unsigned int initialize_direction_view_frames(void);                    /* 0x00421EF0 */
unsigned int LoadSpaceflightResources(void);                           /* 0x00421F50 */
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
unsigned short set_alert(short obj, unsigned int bits);                /* 0x00422140 */
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
int trim_goals(short obj, short amount);                              /* 0x004225C0 */
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
int LoadShapeSet(PacketResourceDescriptor *resources,
                 unsigned short flags,
                 short defaultLogicalFile);                            /* 0x00423CE0 */
int FreeShapeSet(PacketResourceDescriptor *resources,
                 unsigned short releaseFlags);                         /* 0x00423D50 */
int LoadPacketResourceList(PacketResourceDescriptor *resources,
                           short flags, int availableBytes);           /* 0x00423D80 */
unsigned int ResetCockpitPaletteEntries(void);                         /* 0x00423E10 */
unsigned int initialize_cockpit(signed char mode);                     /* 0x00423E90 */
unsigned int InitializeConstellationObject(
    const ConstellationObjectDefinition *definition,
    short object);                                                /* 0x004242D0 */
unsigned int FreeConstellationObject(short object);               /* 0x004243B0 */
void init_constellation(short scene);                             /* 0x004243E0 */
unsigned int free_constellation(void);                            /* 0x00424490 */
void init_vdus(void);                                                  /* 0x004244E0 */
unsigned int InitializeCockpitResources(signed char mode);             /* 0x004245B0 */
unsigned int free_cockpit(void);                                       /* 0x004249A0 */
unsigned int init_3Space_objects(short scene);                         /* 0x00424A80 */
unsigned int load_common_3Space_objects(void);                         /* 0x00424B00 */
unsigned int remove_all_3d_objects(void);                              /* 0x00424B80 */
unsigned int free_3Space(void);                                        /* 0x00424BA0 */
unsigned int free_3Space_objects(void);                                /* 0x00424BE0 */
unsigned int init_inflight_music(void);                                /* 0x00424C60 */
unsigned int free_inflight_music(void);                                /* 0x00424C80 */
unsigned int PreloadMusicTrackHook(short track);                       /* 0x00424CE0 */
unsigned int ReleaseMusicTrackHook(short track);                       /* 0x00424CF0 */
unsigned int LoadSceneAnimationResources(short scene, short variant);  /* 0x00424D00 */
void ReleaseSceneAnimationResources(void);                             /* 0x00424DA0 */
signed char *__stdcall FindSceneAnimationCommand(
    signed char *script, signed char command);                         /* 0x00424DE0 */
short __stdcall SceneAnimationGoalReached(short delta, short current,
                                          short goal);                 /* 0x00424EA0 */
unsigned int __stdcall UpdateSceneAnimationObject(
    SceneAnimationObject *object, Viewport *viewport);                 /* 0x00424EF0 */
void PlaySceneAnimation(char *text, short animation, short duration);  /* 0x00425500 */
unsigned int WaitForKeyExceptXOrF12(void);                            /* 0x00425730 */
void ShowMeanwhileTransition(short scene, short variant);             /* 0x00425770 */
void ApplyAnswerTextCipher(char *text, signed char direction);        /* 0x004258D0 */
void LoadAnswerPromptAndResponse(short entry, char *prompt,
                                 char *response);                     /* 0x00425910 */
short PromptForAnswerText(short entry);                              /* 0x004259B0 */
void SceneEnterHook(void);                                            /* 0x00425AF0 */
void CreateDebugOverlayConsole(HINSTANCE module, HWND window,
                               short columns, short rows);             /* 0x00425B00 */
void DestroyGlobalDebugOverlayConsole(void);                           /* 0x00425B90 */
void SystemDebugPrintf(const char *format, ...);                           /* 0x00425BB0 */
char PumpMessagesDuringWait(void);                                        /* 0x00425BC0 */
unsigned char TakeDebugStepFlag(void);                                     /* 0x00425BD0 */
void ResetDiskPromptTimer(void);                                             /* 0x00425BE0 */
void SetConsoleTextColourHook(void);                                  /* 0x00425BF0 */
void SetConsoleBackgroundColourHook(void);                            /* 0x00425C00 */
void DiskPromptDrawHook(void);                                             /* 0x00425C10 */
void ClearDebugPauseFlags(void);                                              /* 0x00425C20 */
unsigned short EraseTextContextBackground(TextContext *context);      /* 0x00425C30 */
int DisplayTrainSimHighScoreTable(short phase);                       /* 0x00425C60 */
int AnimateTrainSimTitle(void);                                       /* 0x00425D00 */
unsigned char *GetHighScoreEntry(short i);                           /* 0x00425DF0 */
unsigned int GetHighScoreValue(short i);                                      /* 0x00425E20 */
void SetHighScoreEntry(short i, unsigned char b, unsigned int v);    /* 0x00425E30 */
void SortTrainSimHighScores(void);                                   /* 0x00425E50 */
short FindTrainSimHighScore(short pilot);                             /* 0x00425ED0 */
short InsertTrainSimHighScore(short pilot, unsigned int score);       /* 0x00425EF0 */
void InitializeTrainSimHighScores(void);                              /* 0x00425F40 */
int IsHighScoreSlotUsed(short i);                                      /* 0x00425FE0 */
void AddRandomTrainSimHighScores(void);                               /* 0x00426000 */
void DrawTextInputCursor(char character);                             /* 0x004260E0 */
void ClearTextInputCharacter(char character);                         /* 0x00426140 */
void ClearNextTextInputCharacter(char character);                     /* 0x004261D0 */
short ReadTextInput(char *destination, short maximumLength,
                    volatile short mode);                            /* 0x00426200 */
void ReadRequiredPilotField(short x, short y, const char *label,
                            char *destination,
                            short maximumLength);                    /* 0x004265A0 */
void PromptForPilotField(short x, short y, const char *label,
                         char *destination, short maximumLength,
                         const char *defaultText);                    /* 0x00426600 */
void InitializeTrainSimTextPanel(void);                               /* 0x00426660 */
void ShowTrainSimTextMessage(const char *message);                    /* 0x00426700 */
void EnterPilotNameAndCallsign(void);                                 /* 0x00426750 */
void UpdateTrainSimHighScores(int score);                             /* 0x00426820 */
void ShowTrainSimHighScores(void);                                    /* 0x004268E0 */
unsigned char *LoadTrainSimOpponentShape(int opponent);               /* 0x00426C50 */
short SelectTrainSimMission(short *mission);                           /* 0x00426C70 */
void RunTrainSim(void);                                                /* 0x00427080 */
short LogMemoryUsage(void);                                               /* 0x004272F0 */
unsigned int ShowMemoryStatusDebug(void);                                 /* 0x004273C0 */
void exit_squadron(const char *msg);                                    /* 0x00427370 */
unsigned int GetJoystickButtonEdge(unsigned int a, short b);                    /* 0x004274C0 */
int main(short argc, char **argv);                                      /* 0x004274E0 */
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
void * __stdcall PacketLoad(const char *filename, short section,
                            void *destination, unsigned short flags,
                            void *decompressionWorkspace);            /* 0x0042B050 */
void InitializeAudioSystem(HWND window);                              /* 0x0042B160 */
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
void PlaySnowStaticSound(void);                                     /* 0x0042B680 */
void ServiceSoundSystem(void);                                         /* 0x0042B7D0 */
void SetSoundEffectsVolume(int volume);                               /* 0x0042B7E0 */
LONG RegistryQueryValue(HKEY key, LPCSTR name, DWORD type,
                        LPBYTE data, DWORD size);                     /* 0x0042B810 */
void RegistryStoreValue(HKEY key, LPCSTR name, DWORD type,
                        const BYTE *data, DWORD size);                  /* 0x0042B840 */
void LoadVolumeSettingsFromRegistry(void);                            /* 0x0042B870 */
void SaveVolumeSettingsToRegistry(void);                               /* 0x0042B930 */
void DrawLaunchDoorFrame(short distance);                              /* 0x0042B9A0 */
void LaunchPlayerShip(void);                                           /* 0x0042BA90 */
unsigned int ShowCarrierLaunchSequence(signed char sceneObject);       /* 0x0042BC00 */
void FxDriverShutdownHook(void);                                            /* 0x0042C410 */
unsigned short InitializeDiskPromptTextContext(void);                 /* 0x0042C420 */
unsigned short RewriteDiskFileGraphicsExtensions(short videoMode);    /* 0x0042C510 */
short LoadWingCmdrCfgFile(short argc, char **argv);                    /* 0x0042C580 */
unsigned int LoadInstallDat(void);                                     /* 0x0042C660 */
void show_damage_disp(void);                                           /* 0x0042C800 */
void UpdateDamageDisplay(void);                                        /* 0x0042C970 */
unsigned short GetJoystickPresentUnused(void);                                    /* 0x0042CDA0 */
unsigned int parse_view_script(void);                                  /* 0x0042CDB0 */
unsigned int update_scripted_view(void);                               /* 0x0042D1C0 */
void initialize_scripted_view(const short *script);                    /* 0x0042D230 */
unsigned int InitializeFireworks(void);                                /* 0x0042D270 */
short TheEndFireWorks(Viewport *viewport, short count);                /* 0x0042D2A0 */
unsigned int InitializeConstellationField(Viewport *viewport,
                                          short direction,
                                          short density);             /* 0x0042D390 */
unsigned int DrawConstellationField(void);                             /* 0x0042D500 */
short __stdcall OpenPacketSection(const char *filename, short section,
                                  PacketSectionHandle *handle);        /* 0x0042D730 */
void __stdcall CloseDataFileByHandle(unsigned short *p);                       /* 0x0042D870 */
void * __stdcall DecompressPacketSection(
    PacketSectionHandle *handle, void *destination, unsigned short flags,
    void *decompressionWorkspace);                                    /* 0x0042D880 */
short GetTargetColourIndex(void);                                                /* 0x0042DB70 */
void show_target_disp(void);                                         /* 0x0042DB90 */
void DrawTargetRangeReadout(void);                                   /* 0x0042DEA0 */
void LogDisplayMode(const char *mode);                                 /* 0x0042E020 */
short __stdcall CalcRectangleArea(const Viewport *viewport);           /* 0x0042E050 */
unsigned short __stdcall AllocateViewport(Viewport *viewport,
                                          short clearColour,
                                          short flags);                /* 0x0042E090 */
void __stdcall AlignSpriteFrameToRectCorner(
    const ShortRect *rectangle, ShortPoint *position, short corner,
    unsigned char *shape, short frame);                                /* 0x0042E1D0 */
void FadeMusic(void);                                                  /* 0x0042E320 */
void SetMusicOn(short enabled);                                         /* 0x0042E330 */
void StopMusic(short unused);                                          /* 0x0042E350 */
void SetMusBreakpt(int first, int second);                             /* 0x0042E380 */
void PaletteFadeHook(void);                                            /* 0x0042E390 */
void FlushSoundEffect(void);                                               /* 0x0042E3A0 */
void FlushSoundEffects(void);                                               /* 0x0042E3C0 */
void SceneLeaveHook();                                                /* 0x0042E3E0 */
void SelectFlightMusicTrack(int track);                              /* 0x0042E3F0 */
int MapMusicTrackToStreamerCommand(int track);                       /* 0x0042E520 */
void ProcessMusicScriptCommand(int track, int command,
                               short enabled);                       /* 0x0042E6F0 */
unsigned int spacetrack(int track, int mode,
                        short enabled);                               /* 0x0042E880 */
void StopMusicUnlessSuppressed(void);                                        /* 0x0042E8B0 */
unsigned short GetMusicMode(void);                                /* 0x0042E8D0 */
void wait_for_end_of_music(void);                                     /* 0x0042E900 */
void new_space_music_changes(short attacker, short victim);          /* 0x0042E9E0 */
int changetrack(void);                                                /* 0x0042EAD0 */
void gametrack(void);                                                 /* 0x0042EB60 */
void servicetrack(void);                                              /* 0x0042ECB0 */
void ResetSoundState(void);                                             /* 0x0042EE80 */
void ResetSoundStateForScene(void);                                               /* 0x0042EEA0 */
void ResetSoundStateForFlight(void);                                               /* 0x0042EEB0 */
void EnableMusicForScene(void);                                               /* 0x0042EEE0 */
unsigned int SoundFxTick(void);                                       /* 0x0042EF00 */
void FlushSoundEffectsAndLog(void);                                               /* 0x0042EF10 */
void sound_effect(void);                                                /* 0x0042EF20 */
void PlaySfxWaveFileByNumber(int soundNumber, int sourceObject,
                             int looping);                            /* 0x0042EF30 */
void cleanup_objectives(void);                                        /* 0x0042EFC0 */
int too_busy(short ship);                                             /* 0x0042F1F0 */
void reply(short ship, short accepted);                               /* 0x0042F210 */
int disobey_formation(short ship);                                    /* 0x0042F240 */
int bad_target(short ship, short target);                             /* 0x0042F270 */
short can_land(void);                                                 /* 0x0042F2B0 */
short i_wanna_rout(short ship, int pilot);                            /* 0x0042F350 */
void request(short requester, short ship, short command);             /* 0x0042F3F0 */
unsigned short __stdcall ShouldSuspendCursorForRect(
    const ShortRect *bounds);                                           /* 0x0042F730 */
unsigned short __stdcall InitializeDIBScreenViewport(
    Viewport *viewport, unsigned short colour);                       /* 0x0042F740 */
void InitFullScreenViewport(int *vp, short arg);                                    /* 0x0042F7E0 */
unsigned int __stdcall GetPacketSize(const char *filename,
                                     short section);                  /* 0x0042F810 */
int GetFreeNearHeapBytes(void);                                       /* 0x0042F890 */
void FrameStartHook(int mode);                                         /* 0x0042F930 */
unsigned short IsSoundHardwarePresent(void);                                     /* 0x0042F940 */
void MessagePumpHook(int mode);                                        /* 0x0042F950 */
int PushMemoryStackFrame(int memory, int offset);                       /* 0x0042F960 */
int IsPushedPacketHandle(int handle);                                  /* 0x0042F9E0 */
int MapPacketHandleToBlock(int handle);                               /* 0x0042FA20 */
void *AllocateTaggedMemory(unsigned int size, unsigned short flags);   /* 0x0042FA90 */
void ReleasePacketHandle(int handle);                                 /* 0x0042FAE0 */
unsigned int GetFixedOneMillionThunk(void);                  /* 0x0042FB20 */
unsigned int GetFixedOneMillionThunkAlt(void);               /* 0x0042FB30 */
void CreateCannedSceneObject(short *object, short yaw, short unusedPitch,
                             short distance, unsigned char *shape,
                             short frame, short type,
                             short scale);                         /* 0x0042FB40 */
unsigned int ShowCampaignVictorySequence(void);                    /* 0x0042FC00 */
unsigned int ShowTigerClawEscapeScene(void);                          /* 0x00430150 */
unsigned int ShowTheEndScreen(short enableFireworks);                 /* 0x004304F0 */
short __stdcall UpdateInputDeviceTransitions(short raw);               /* 0x00430710 */
void PollJoystickButtonEvents(void);                                   /* 0x00430840 */
void PollMenuInputDevices(void);                                       /* 0x00430920 */
short get_face(short rating, unsigned int side);                      /* 0x00430BC0 */
void LoadCommPortraitShape(short face, signed char alternate);        /* 0x00430BF0 */
void ResetCommMenuChoices(short reuse);                               /* 0x00430C50 */
int IsCommMenuIdle(void);                                              /* 0x00430CA0 */
void AppendCommMenuChoice(char *text, short command);                    /* 0x00430CB0 */
void SendCommMenuChoice(short i);                                          /* 0x00430D30 */
void OpenCommMenuForTarget(char *heading, char *message);              /* 0x00430D50 */
int IsCommChoiceMenuOpen(void);                                       /* 0x00430D80 */
short GetPendingMenuAction(void);                                            /* 0x00430DA0 */
void SetPendingMenuAction(unsigned char v);                                 /* 0x00430DB0 */
void OpenCommRecipientMenu(void);                                     /* 0x00430DC0 */
void CloseCommChoiceMenu(void);                                       /* 0x00430DE0 */
int wingman_dead(void);                                               /* 0x00430E10 */
short have_target(void);                                              /* 0x00430E30 */
unsigned short CanOpenCommMenu(void);                                 /* 0x00430E50 */
void SelectCommRecipient(signed char recipient);                      /* 0x00430E70 */
void BuildCommunicationRecipientMenu(void);                           /* 0x00430E90 */
void BuildCommunicationCommandMenu(void);                             /* 0x00430FC0 */
void RefreshCommunicationMenu(void);                                 /* 0x00431200 */
void HandleCommunicationMenuRequest(void);                            /* 0x00431240 */
void show_communications_disp(void);                                  /* 0x00431290 */
unsigned int Chosen_communicate_option(short choice);                 /* 0x00431350 */
void talk_equiv(void);                                                /* 0x00431400 */
void FreeCommDisplayResources(void);                                  /* 0x00431410 */
void EndCommSessionWithWingman(void);                                  /* 0x00431470 */
void EndCommMenu(void);                                              /* 0x004314C0 */
void ShowCentredPrompt(char *text, unsigned short arg);                       /* 0x004314F0 */
short LoadCommDisplayResources(short rating, enum Side side);         /* 0x00431520 */
char *ExpandCommMessageTokens(char *text);                            /* 0x004315C0 */
void real_vid_transmit(short obj, short message);                     /* 0x004316E0 */
void __stdcall ShutdownVideoHook(short mode);                           /* 0x004318F0 */
short __stdcall ReserveContiguousPaletteEntries(short entryCount);    /* 0x00431900 */
void __stdcall ReleaseContiguousPaletteEntries(short firstEntry);     /* 0x00431970 */
void PrintPaletteAllocationMap(void);                                 /* 0x004319B0 */
void LoadJoystickCalibrationFile(short horizontalRange,
                                 short verticalRange,
                                 short horizontalDeadZone,
                                 short verticalDeadZone);              /* 0x00431A10 */
short ReadCalibratedJoystick(void);                                    /* 0x00431D20 */
void __stdcall UnionRectBounds(ShortRect *destination,
                               const ShortRect *first,
                               const ShortRect *second);               /* 0x00431EA0 */
void ThrottleFrameAndDrawFps(HDC dc);                                       /* 0x00431F00 */
void ReportSpaceFlightMaxFps(float adjustment);                      /* 0x00432050 */
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
void __stdcall SeedRandomFromClock(void);                                               /* 0x00434CF0 */
short __stdcall RandomInRange(short lo, short hi);                      /* 0x00434D20 */
short RandomBelowOrEqual(short n);                                      /* 0x00434D50 */
long MultiplyFixed(int left, int right);                              /* 0x00434D80 */
long DivideFixed(int numerator, int denominator);                     /* 0x00434DB0 */
long SinFixed(short degrees);                                    /* 0x00434E00 */
long CosFixed(short degrees);                                    /* 0x00434E30 */
long ArcSin(int value);                                               /* 0x00434E60 */
long ArcCos(int value);                                               /* 0x00434E90 */
long Magnitude(int value);                                           /* 0x00434EC0 */
long PlanarMagnitude(int x, int y);                                  /* 0x00434EE0 */
long Vector_magnitude(const FixedVector *vector);                     /* 0x00434F20 */
void __stdcall SetTextCursor(unsigned short a, unsigned short b);   /* 0x00434F70 */
void __stdcall SetTextContext(TextContext *context);                  /* 0x00434FA0 */
void WaitForVerticalBlankThunk(void);                                  /* 0x00434FB0 */
unsigned int __stdcall IdentityHandle(unsigned int v);                             /* 0x00434FC0 */
void __stdcall SetWholePaletteFromTriplets(unsigned char *palette);               /* 0x00434FD0 */
unsigned short __stdcall ReadWord(unsigned short *p);                        /* 0x00434FE0 */
unsigned short __stdcall GetFontCharWidth(char i);                                     /* 0x00434FF0 */
void ReleaseVideoResourcesHook(void);                                           /* 0x00435010 */
short __stdcall GetShapeFrameBounds(short *bounds, short x, short y,
                                    unsigned char *shape, short frame); /* 0x00435020 */
short __stdcall IsPointInRect(short x, short y, const short *rect);       /* 0x00435090 */
void __stdcall SplitPackedPoint(ShortPoint point, short *p);             /* 0x004350D0 */
void __stdcall DrawTextString(char *text);                                      /* 0x004350F0 */
void __stdcall DrawTextCharacter(char character);                    /* 0x00435290 */
void __stdcall AppendTextCharacter(char character);                  /* 0x00435310 */
int __stdcall MeasureShapeFrameStorage(unsigned char *shape,
                                       short frame);                   /* 0x00435340 */
void ResetTextCursor(void);                                           /* 0x004353F0 */
unsigned int __stdcall DosFarPtrToNear(unsigned int v);                             /* 0x00435410 */
unsigned int __stdcall DosNearPtrToFar(unsigned int v);                             /* 0x00435420 */
char *__stdcall DosStrrchr(char *s, short c);                       /* 0x00435430 */
char *__stdcall DosStrchr(char *s, short c);                        /* 0x00435450 */
char *__stdcall DosStrcpy(char *dst, const char *src);               /* 0x00435470 */
char *__stdcall CopyFarString(char *destination,
                              const char *source);                   /* 0x004354A0 */
short __stdcall DosStrlen(const char *s);                                   /* 0x004354D0 */
void __stdcall DosMemcpy(void *dst, const void *src, size_t n);               /* 0x004354F0 */
void __stdcall DosMemset(void *destination, unsigned int count,
                         short value);                               /* 0x00435510 */
unsigned short GetEventManagerStatus(void);                            /* 0x00435550 */
void __stdcall RegisterEventManagerShutdown(void (*fn)(void));         /* 0x00435560 */
short __stdcall InitializeEventManager(short period,
                                       void (*initialize)(void),
                                       void *configuration);           /* 0x00435570 */
void ShutdownEventManager(void);                                      /* 0x00435590 */
unsigned short __stdcall ConfigureEventManagerPointer(
    unsigned char *shape, short frame);                               /* 0x004355A0 */
void __stdcall EventManagerHook(void (*callback)(void));               /* 0x004355C0 */
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
short __stdcall ResetAllocationDepth(int x, int y);                                          /* 0x00435DC0 */
void CheckCursor(void);                                                /* 0x00435DE0 */
void CaptureMouseCursorBackground(void);                           /* 0x00435E20 */
void DrawMouseCursor(void);                                        /* 0x00435EF0 */
void RestoreMouseCursorBackground(void);                           /* 0x00435FA0 */
void RefreshMouseCursorDisplay(void);                              /* 0x00436060 */
void EnterAllocationScope(void);                                                 /* 0x004360D0 */
void LeaveAllocationScope(void);                                                 /* 0x004360E0 */
void __stdcall SetMouseCursorShape(unsigned char *shape, short frame); /* 0x004360F0 */
void __stdcall SetMouseHomePosition(short x, short y);                /* 0x00436160 */
void __stdcall ApplyPackedMousePosition(ShortPoint point);             /* 0x00436190 */
void SetFrameTimerPeriod(short period);                                   /* 0x004361E0 */
void __stdcall SetFrameTimerAndWait(short period);                                       /* 0x004361F0 */
void __stdcall SetFrameTimerPeriodDirect(short p);                    /* 0x00436210 */
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
void StartupHook(unsigned int (*callback)(unsigned int, short));  /* 0x00436900 */
unsigned int JoystickEdgeHook(int button);                         /* 0x00436910 */
void FreeIfNotNull(void *p);                                            /* 0x00436950 */
unsigned int GetStartupErrorCode(int vector);                       /* 0x00436970 */
void ShutdownHook(int vector, void *handler);                       /* 0x00436980 */
unsigned short __stdcall SelectDiskDriveHook(short drive);             /* 0x004369A0 */
short GetCurrentDiskDriveHook(void);                                    /* 0x004369B0 */
unsigned short __stdcall GetShutdownErrorCode(
    unsigned char *driveState);                                    /* 0x004369C0 */
void VideoReleaseHook(void);                                           /* 0x004369D0 */
void ExitCleanupHook(void);                                           /* 0x004369E0 */
unsigned int IsVectorWithinRange(FixedVector *vector, short range);    /* 0x00436A00 */
unsigned int shrink_vector(FixedVector *vector);                       /* 0x00436A30 */
unsigned int shrink(int *component);                                   /* 0x00436A70 */
void __stdcall FillGraphicSuffix(char *path, short number,
                                 short digits);                        /* 0x00436C70 */
void __stdcall ConvertChar_Int(char *text, short number,
                              short digits);                          /* 0x00436CB0 */
unsigned int LoadBriefingRoom(void);                                  /* 0x00436D00 */
unsigned int ViewMedals(void);                                        /* 0x00436E30 */
unsigned int AwardCampaignMedal(short medal);                         /* 0x00436F50 */
int no_objectives_achieved(void);                                      /* 0x00438090 */
short wing_status(short personality);                                  /* 0x004380D0 */
short int_value(char **text);                                          /* 0x00438110 */
ConversationSceneRecord *ParseTests(ConversationSceneRecord *record,
                                    ConversationSceneRecord *sceneData,
                                    unsigned char *textData);          /* 0x00438160 */
unsigned int DrawMedalChest(char *text, short duration);                /* 0x004370D0 */
unsigned int DrawMedalLongShot(short *animation, char *text,
                               short duration);                        /* 0x00437250 */
unsigned int MedalEstablish(char *text, short duration);                /* 0x004373E0 */
unsigned int PinMedal(char *text, short duration);                      /* 0x004374B0 */
unsigned int DrawMedals(void);                                          /* 0x004375C0 */
unsigned int EstablishingShot(char *text, short duration);              /* 0x00437770 */
unsigned int DrawBriefingLongShot(void);                                /* 0x004378D0 */
unsigned int ReturnToBriefingLongShot(char *text, short duration);      /* 0x00437980 */
unsigned int Dismissed(char *text, short duration);                     /* 0x00437B80 */
unsigned int DrawDebriefingLongShot(void);                              /* 0x00437DC0 */
unsigned int DebriefingEstablishingShot(char *text, short duration);    /* 0x00437F20 */
unsigned int TalkerInit(void);                                         /* 0x00438B90 */
unsigned int FreeTalker(void);                                         /* 0x00438BC0 */
unsigned int SceneDirector(short sceneType, unsigned char *sceneData,
                           unsigned char *textData);                    /* 0x00438C00 */
unsigned int DrawPodiumShot(void);                                      /* 0x00439070 */
unsigned int DrawBriefingCharacter(short character, short pose,
                                   short animationFrame,
                                   const signed char *unusedXOffsets,
                                   const signed char *unusedYOffsets);  /* 0x00439150 */
unsigned int DrawFuneralLongShot(short shot, char *text,
                                 short duration);                       /* 0x00439220 */
void __stdcall SetViewportRect(Viewport *viewport, unsigned short left,
                               unsigned short top, unsigned short right,
                               unsigned short bottom);                /* 0x00439400 */
void __stdcall PanToScreen(Viewport *source, Viewport *destination);    /* 0x00439430 */
unsigned int death_sequence(void);                                    /* 0x00439660 */
unsigned int ShowGetReadyScreen(void);                                 /* 0x00439840 */
unsigned int ShowVictoryScreen(void);                                  /* 0x00439910 */
unsigned int ShowGameOverScreen(void);                                 /* 0x00439A80 */
void __stdcall FillRasterBytes(void *destination, unsigned int length,
                               short value);                          /* 0x00439C0E */
char *CopyRasterDriverName(void *const *callbacks);                   /* 0x00439C3E */
void InstallRasterDriverCallbacks(void *const *callbacks);            /* 0x00439C69 */
unsigned int SetRasterClipPixel(RasterClip *clip, int x, int y,
                                int colour);                           /* 0x00439C88 */
int ReadRasterClipPixel(RasterClip *clip, int x, int y);                /* 0x00439D63 */
unsigned int DrawClippedLine(RasterClip *clip, int x1, int y1, int x2, int y2,
                             int mode, int colour);                    /* 0x00439E39 */
int FillRasterClipCheckerboard(RasterClip *clip, int left, int top,
                               int right, int bottom,
                               unsigned char colour);                 /* 0x0043A83B */
int DrawRLEImage(RasterClip *clip, unsigned char *shape, int frame,
                 int x, int y);                                      /* 0x0043A974 */
int DrawRLEImageUnclipped(RasterClip *clip, RLEFrameHeader *frameHeader,
                          int x, int y,
                          int strideScratch);                         /* 0x0043AD78 */
void SetPaletteTranslationTable(const unsigned char *translation);    /* 0x0043AE3F */
int DrawRLEImageColor(RasterClip *clip, unsigned char *shape, int frame,
                      int x, int y);                                 /* 0x0043AE5E */
int DrawRLEImageColorUnclipped(RasterClip *clip,
                               RLEFrameHeader *frameHeader,
                               int x, int y,
                               int strideScratch);                    /* 0x0043B336 */
int RotateRLEImage(RasterClip *clip, unsigned char *shape, int frame,
                   int x, int y, unsigned char *scratch,
                   unsigned int angleTenths, int scaleX, int scaleY,
                   unsigned int flags);                              /* 0x0043B469 */
unsigned int GetRLEFrameBounds(unsigned char *shape, int frame,
                               int x, int y, unsigned int flags,
                               int *bounds);                          /* 0x0043C015 */
int EncodeRasterClipToRLEFrame(RasterClip *clip,
                               unsigned char transparentColour,
                               int originX, int originY,
                               unsigned char *output);                /* 0x0043C18D */
unsigned int TranslateRLEFramePalette(unsigned char *shape,
                                      int frame);                     /* 0x0043C410 */
void EncodeRLEScanline(int pixelCount, unsigned char transparentColour,
                       int sourceX);                                  /* 0x0043C4A2 */
void EmitRLEScanlineRun(int runType, int trailingCount,
                        int sourceX);                                 /* 0x0043C62B */
int FillRasterClip(RasterClip *clip, int colour);                      /* 0x0043C808 */
int BlitRasterClip(RasterClip *source, int sourceX, int sourceY,
                   RasterClip *destination, int destinationX,
                   int destinationY, unsigned int colour);            /* 0x0043C8E7 */
int ScrollRasterClipWrapped(RasterClip *clip, int deltaX, int deltaY,
                            int mode,
                            unsigned int fillOrScratch);              /* 0x0043CC83 */
unsigned int DrawRasterEllipse(RasterClip *clip, int x, int y,
                               int horizontalRadius, int verticalRadius,
                               int colour);                            /* 0x0043CE80 */
unsigned int FillRasterEllipse(RasterClip *clip, int x, int y,
                               int horizontalRadius, int verticalRadius,
                               int colour);                            /* 0x0043D1C1 */
void GetRLETransformTrig(int angleTenths, int *cosine, int *sine);     /* 0x0043E2D3 */
void CalculateRoundedRLEFixedProduct(int left, int right, int *result); /* 0x0043E38B */
void TransformRLEPoint(int *point, int *result, int *origin,
                       unsigned int angleTenths, int scaleX,
                       int scaleY);                                   /* 0x0043E3B1 */
unsigned int GetRawImageHeight(unsigned char *shape);                 /* 0x0043E478 */
unsigned int GetRawFrameWidth(unsigned char *shape, int frame);       /* 0x0043E48B */
int BlitRawFrame(RasterClip *clip, int x, int y, unsigned char *shape,
                 int frame, unsigned char *translation);              /* 0x0043E4AB */
void BlitSelectedRawFrames(RasterClip *clip, int x, int y,
                           unsigned char *shape,
                           const unsigned char *frames,
                           unsigned char *translation);               /* 0x0043E63E */
int BlitRawScanline(RasterClip *clip, int y,
                    const unsigned char *pixels,
                    int width);                                      /* 0x0043E675 */
unsigned char *FindIFFChunkData(const char *chunkId,
                                const unsigned char *iffData);        /* 0x0043E784 */
unsigned int DecodeIFFImage(RasterClip *clip,
                            const unsigned char *iffData);            /* 0x0043E7C6 */
void CopyILBMPalette(const unsigned char *iffData,
                     unsigned char *palette);                         /* 0x0043E98D */
unsigned int GetILBMImageSize(const unsigned char *iffData);          /* 0x0043E9BE */
unsigned int DecodePCXImage(RasterClip *clip,
                            const unsigned char *pcxData);            /* 0x0043E9EB */
void CopyPCXPaletteFromFileTail(const unsigned char *fileData,
                                unsigned int fileSize,
                                unsigned char *palette);              /* 0x0043EA6D */
unsigned int GetPCXImageSize(const unsigned char *header);            /* 0x0043EA98 */
void ResetGIFLZWDictionary(void);                                     /* 0x0043EABF */
unsigned int ReadGIFDataSubBlockByte(void);                           /* 0x0043EB07 */
unsigned int ReadGIFLZWCode(void);                                    /* 0x0043EB20 */
void AppendGIFLZWDictionaryEntry(void);                               /* 0x0043EB66 */
void EmitGIFDecodedPixel(void);                                       /* 0x0043EBAC */
unsigned int ExpandGIFLZWImage(RasterClip *clip,
                               const unsigned char *gifData,
                               void *workspace);                      /* 0x0043EC29 */
void CopyGIFPalette(const unsigned char *gifData,
                    unsigned char *palette);                          /* 0x0043EE42 */
unsigned int GetGIFImageSize(const unsigned char *gifData);           /* 0x0043EEA3 */
unsigned int GetRLEFrameDimensions(unsigned char *shape,
                                    int frame);                        /* 0x0043EEDB */
unsigned int GetRLEFrameExtents(unsigned char *shape,
                                int frame);                           /* 0x0043EEFD */
unsigned int GetRLEImageSize(unsigned char *shape, int frame);         /* 0x0043EF20 */
unsigned int GetRLEImageOrigin(unsigned char *shape, int frame);       /* 0x0043EF54 */
void ApplyRLEFramePalette(unsigned char *shape, int frame,
                          unsigned char *palette);                    /* 0x0043EF7E */
unsigned int CopyRLEFramePalette(unsigned char *shape, int frame,
                                 unsigned char *entries);             /* 0x0043EFC9 */
unsigned int SetRLEFramePalette(unsigned char *shape, int frame,
                                const unsigned char *entries);        /* 0x0043F011 */
unsigned int GetRLEFrameCount(const unsigned char *shape);            /* 0x0043F05B */
int CollectUniqueRLEImageFrames(const unsigned char *shape,
                                unsigned int *frames);                /* 0x0043F06E */
int CollectUniqueRLEPaletteFrames(const unsigned char *shape,
                                  unsigned int *frames);              /* 0x0043F0D0 */
void FadeRasterPaletteToPalette(RasterSurface *surface,
                                const unsigned char *targetPalette,
                                unsigned int duration);               /* 0x0043F425 */
int CollectRasterClipColours(RasterClip *clip,
                             unsigned int *colours);                  /* 0x0043F5A9 */
void CorrectPointers(void);                                            /* 0x0043F640 */
void ClearRoomMenuLabel(void);                                        /* 0x0043F690 */
int IsRoomMenuLabelEmpty(void);                                       /* 0x0043F6A0 */
void DrawRoomMenuLabel(TextContext *context, char *label);             /* 0x0043F6B0 */
void RefreshRoomMenuLabel(void);                                      /* 0x0043F6F0 */
void ClearRoomMenuCursorFrame(void);                                  /* 0x0043F720 */
void SelectRoomMenuLabel(short i);                                    /* 0x0043F730 */
void InitializeRoomMenu(TitleMenuRegion *regions, char **labels,
                        Viewport *viewport, char *text,
                        unsigned char alignment);                     /* 0x0043F750 */
short FindMenuRegionAtPoint(short x, short y,
                            const TitleMenuRegion *regions);           /* 0x0043F7C0 */
void InitializeRoomViewports(void);                                   /* 0x0043F810 */
short RecRoom(void);                                                  /* 0x0043F940 */
void ShowChalkBoard(void);                                            /* 0x00440510 */
void ResetCampaignData(void);                                         /* 0x00440800 */
short __stdcall ReadPacketSectionData(PacketSectionHandle *handle,
                                      void *destination,
                                      unsigned int length);            /* 0x00440840 */
void CheckHeapBlockSignature(unsigned char *shape);                  /* 0x004408A0 */
unsigned char *GetPreparedShapeData(unsigned char *shape);           /* 0x004408C0 */
short __stdcall GetShapeFrameCount(unsigned char *shape);            /* 0x004408D0 */
void GetShapeFrameExtents(unsigned char *shape, short frame,
                          short *width, short *height,
                          short *leftExtent, short *topExtent);       /* 0x004408F0 */
void DecodeShapeFrame(unsigned char *shape, short frame,
                      unsigned char *bitmap, int width, short height,
                      int leftExtent, int topExtent);                 /* 0x00440960 */
unsigned int SignExtendClipCoord(volatile int v);                      /* 0x00440BE0 */
void ValidateViewportBounds(Viewport *viewport, RasterSurface *surface,
                            RasterClip *clip);                         /* 0x00440C00 */
void ClipViewportToScreen(Viewport *viewport);                         /* 0x00440CF0 */
void SetSolidColourTranslation(unsigned char colour);                 /* 0x00440D10 */
void PrepareShapeRLEData(unsigned char *shape);                        /* 0x00440D50 */
void DrawSpriteTransformed(Viewport *viewport, int x, int y,
                           unsigned char *shape, int frame,
                           int angle, int scaleX, int scaleY,
                           int flip, int blendMode);                /* 0x00440FE0 */
void RasterLineHook(void *marker);                                    /* 0x00441140 */
void DrawFontGlyph(char character, TextContext *context, int height,
                   int width, int y);                                 /* 0x00441150 */
void __stdcall MarkActivePaletteEntries(Viewport *viewport,
                                         unsigned char *active);       /* 0x00441370 */
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
void DrawViewportPixel(Viewport *viewport, short x, short y,
                       short colour);                                 /* 0x00441B20 */
int GetViewportPixel(Viewport *viewport, short x, short y);            /* 0x00441B60 */
void DrawViewportLine(Viewport *viewport, short x1, short y1,
                      short x2, short y2, short colour);             /* 0x00441BA0 */
void TriangleRasterizerHook(void);                                    /* 0x00441BF0 */
void DrawFilledViewportRect(Viewport *viewport, short left, short top,
                            short right, short bottom,
                            short colour);                           /* 0x00441C70 */
void DrawViewportBorder(Viewport *viewport, short left, short top,
                        short right, short bottom,
                        short colour);                                /* 0x00441CF0 */
void DrawViewportEllipse(Viewport *viewport, short x, short y,
                         short verticalRadius, short horizontalRadius,
                         short colour);                               /* 0x00441DD0 */
void FillViewportEllipse(Viewport *viewport, short x, short y,
                         short verticalRadius, short horizontalRadius,
                         short colour);                               /* 0x00441E20 */
void DrawViewportEllipseShadow(Viewport *viewport, short x, short y,
                               short verticalRadius,
                               short horizontalRadius,
                               short colour);                         /* 0x00441E70 */
void DrawSpriteScaled(Viewport *viewport, short x, short y,
                      unsigned char *shape, short frame, short angle,
                      short scale, short flip);                       /* 0x00441FC0 */
void DrawSolidColourSpriteScaled(Viewport *viewport, short x, short y,
                                 unsigned char *shape, short frame,
                                 short angle, short scale, short flip,
                                 unsigned char colour);                /* 0x00442000 */
short GetTransformedShapeBounds(Viewport *viewport, short x, short y,
                                unsigned char *shape, short frame,
                                short angle, short scale, int flip,
                                short *bounds);                       /* 0x00442050 */
void fizzle_fade(Viewport *source, Viewport *destination,
                 const ScreenViewportGeometry *geometry);            /* 0x00442200 */
void snow_viewport(Viewport *viewport, int effect,
                   unsigned short colour);                            /* 0x00442300 */
void UpdateStreamerStoppedFlag(void);                                    /* 0x00442330 */
void SignalAudioMixerWakeEvent(void);                                 /* 0x00442350 */
void InitializeAudioStreamer(HWND window);                            /* 0x00442370 */
void Streamer_open(const char *streamName);                           /* 0x004423B0 */
void Streamer_play(void);                                             /* 0x00442430 */
void Streamer_stop(void);                                                /* 0x00442460 */
void ClearStreamerTrigger(void);                                      /* 0x004424B0 */
void Streamer_trigger(int trigger);                                   /* 0x004424D0 */
void SetStreamerIntensity(unsigned char intensity);                   /* 0x00442520 */
void ForceStreamerTrigger(int trigger);                               /* 0x00442550 */
void SetMusicStreamVolume(unsigned short volume);                     /* 0x00442590 */
void Streamer_close(void);                                            /* 0x004425D0 */
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
int ix_streamer_init(void);                                           /* 0x00442750 */
void ix_streamer_configure(int option, void *value);                  /* 0x0044291E */
int ix_streamer_open_stream_file(char *path);                         /* 0x004429B6 */
void ix_streamer_close_stream_file(void);                             /* 0x0044307A */
void ix_streamer_audio_play(void);                                    /* 0x004431F3 */
void ix_streamer_set_intensity(unsigned char intensity);              /* 0x0044336B */
void ix_streamer_set_trigger(char trigger);                           /* 0x004433C1 */
void ix_streamer_force_trigger(char trigger);                         /* 0x0044342E */
void ix_streamer_set_volume(unsigned short volume);                   /* 0x004435BE */

#endif /* WC1_FUNCS_H */
