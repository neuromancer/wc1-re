/* cockpt.c */
void EmitTextString(void (__stdcall *writer)(int), const char *text);   /* 0x00413A10 */
#ifdef SDL_PORT
void FormatTextTokens(void (__stdcall *writer)(int),
                      const char *format, va_list *arguments);         /* 0x00413A40 */
#else
void FormatTextTokens(void (__stdcall *writer)(int),
                      const char *format, va_list arguments);    /* 0x00413A40 */
#endif
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
void DrawCockpitReadout(signed char slot, const char *text);          /* 0x00413FB0 */
void EraseCockpitReadoutAtPosition(signed char slot, short left,
                                   short top);                        /* 0x00414050 */
short DrawHudMessageSlot(HudMessageSlot *slot);                        /* 0x004140A0 */
void ClearHudMessageSlot(HudMessageSlot *slot);                        /* 0x00414180 */
void ClearHudMessageIfMatching(HudMessageSlot *slot,
                               const char *text);                     /* 0x004141B0 */
void ClearHudGunReadouts(void);                                             /* 0x004141D0 */
void SetHudMessageSlot(HudMessageSlot *slot, TextContext *context,
                       short x, short y, const char *text,
                       unsigned short colour,
                       signed char flashCount);                        /* 0x004141F0 */
void UpdateMessage(HudMessageSlot *slot);                              /* 0x00414240 */
void set_global_message(const char *text, unsigned short colour,
                        int flashCount);                               /* 0x00414270 */
void CockpitMessage(const char *text, unsigned short colour,
                    int flashCount);                                  /* 0x004142B0 */
void remove_message(const char *text);                                /* 0x004142E0 */
short kilrathi_near(short obj, short range);                           /* 0x00414300 */
short auto_pilot_valid(short showReason);                              /* 0x00414380 */
void *reset_cockpit(void);                                            /* 0x00414410 */
unsigned int SetCockpitLightBlink(signed char light, short interval); /* 0x00414440 */
void draw_cockpit_lights(void);                                      /* 0x00414490 */
void update_lights(void);                                            /* 0x004145B0 */
void update_bars(void);                                              /* 0x00414690 */
short get_mode(short i);                                              /* 0x004147E0 */
void set_mode(short i, int state);                                    /* 0x00414800 */
unsigned short SetVduModeIfChanged(short i, int state);               /* 0x00414850 */
short GetVduModeStackDepth(short i);                                  /* 0x00414890 */
void push_mode(short i, int state);                                   /* 0x004148A0 */
void pop_mode(short i);                                               /* 0x004148E0 */
void set_new_vdu(short vdu);                                          /* 0x00414910 */
short update_vid_disp(short vdu);                                     /* 0x00414980 */
void InvalidateVduMode(short i);                                          /* 0x004149C0 */
void clear_message_time(void);                                              /* 0x004149E0 */
short message_showing(void);                                          /* 0x004149F0 */
unsigned short set_message_time(unsigned short v);                        /* 0x00414A10 */
void check_message(void);                                             /* 0x00414A20 */
unsigned int update_digital_readouts(void);                          /* 0x00414A50 */
void PlayTargetLockSfx(void);                                           /* 0x00414AD0 */
void malf_sound(void);                                                  /* 0x00414AE0 */
short malf(char component);                                           /* 0x00414AF0 */
unsigned short vdu_malf(short vdu, short sound);                       /* 0x00414B20 */
void ShowComponentHitHudMessage(const char *text, unsigned short colour,
                                short flashCount);                     /* 0x00414B70 */
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
const char *objective_name(short objective);                          /* 0x00415130 */
void show_navigation_disp(void);                                      /* 0x00415180 */
short hidden_objective(short objective);                              /* 0x004151F0 */
int set_new_objective(short pathIndex);                               /* 0x004152C0 */
short cycle_next_objective(void);                                     /* 0x00415370 */
void set_next_destination(void);                                       /* 0x004153D0 */
short LocateMobileObjective(short objective);                         /* 0x00415470 */
unsigned int someone_coming(void);                                     /* 0x004154C0 */
short escorting_a_ship(void);                                          /* 0x00415510 */
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
void send_message(short obj, short message);                      /* 0x00417420 */
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
