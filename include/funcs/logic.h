/* logic.c */
/* Logic omits these prototype parameter names to preserve CL 4.20's
 * measured declaration context; the signatures remain identical. */
#ifdef WC1_LOGIC_SOURCE
short ace_status(short, unsigned char);                                /* 0x00422010 */
void unflag_ace(short, unsigned char);                                 /* 0x00422030 */
void flag_ace(short, unsigned char);                                   /* 0x00422050 */
void kill_ace(short);                                                  /* 0x00422060 */
void ace_greeting(short);                                              /* 0x00422090 */
void prepare_ace(short);                                               /* 0x004220D0 */
short signed_random(short);                                            /* 0x004220F0 */
int alert_flag(short, unsigned int);                                   /* 0x00422110 */
unsigned int HasSpeechBuffer(void);                                    /* 0x00422130 */
unsigned short set_alert(short, unsigned int);                         /* 0x00422140 */
void clear_alert(short);                                               /* 0x00422160 */
void start_collision_alert(short, short);                              /* 0x00422180 */
void try2end_collision_alert(short);                                   /* 0x004221E0 */
short normal_speed(short);                                             /* 0x00422220 */
short real_crash_time(short, short);                                   /* 0x00422260 */
void clear_crash_cache(void);                                          /* 0x00422440 */
short crash_time(short, short);                                        /* 0x00422460 */
short detect_collisions(short);                                        /* 0x004224F0 */
unsigned int unactive(short);                                          /* 0x00422560 */
int are_alive(short);                                                  /* 0x00422590 */
int trim_goals(short, short);                                          /* 0x004225C0 */
int report_kilrathi_rout(int);                                         /* 0x00422640 */
short find_ship_index(short);                                          /* 0x00422710 */
short try2rout(short);                                                 /* 0x00422780 */
signed char no_goal(short);                                            /* 0x00422830 */
int being_tailed(short, short);                                        /* 0x00422860 */
int any_enemy_tail(short);                                             /* 0x004228A0 */
short detect_enemy_tail(short);                                        /* 0x00422930 */
int is_ship_tailing_player_target(short);                              /* 0x004229B0 */
int missile_on_tail(short);                                            /* 0x004229F0 */
short select_weighted_value(short *);                                  /* 0x00422A30 */
unsigned int build_squad_list(short);                                  /* 0x00422A70 */
unsigned int find_squad_center(FixedVector *);                         /* 0x00422AC0 */
unsigned int init_formation_burst(short);                              /* 0x00422B30 */
unsigned int reset_mission_type(short, enum ShipMissionType);          /* 0x00422BE0 */
unsigned int change_mission_type(short, enum ShipMissionType);         /* 0x00422C30 */
unsigned int reset_objective(short, enum ShipObjective);               /* 0x00422C70 */
unsigned int alter_objective(short, enum ShipObjective);               /* 0x00422CA0 */
unsigned int reset_tactic(short, enum ShipTactic);                     /* 0x00422CD0 */
unsigned int alter_tactic(short, enum ShipTactic);                     /* 0x00422D00 */
void reset_maneuver(short, short);                                     /* 0x00422D30 */
void try2reset_maneuver(short, short);                                 /* 0x00422D60 */
unsigned int set_special(short, enum SpecialManeuver);                 /* 0x00422D90 */
#else
short ace_status(short ace, unsigned char bits);                     /* 0x00422010 */
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
short try2rout(short obj);                                              /* 0x00422780 */
signed char no_goal(short ship);                                       /* 0x00422830 */
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
#endif
unsigned int approach_zero_speed(short ship);                          /* 0x00422DD0 */
unsigned int approach_min_speed(short obj);                            /* 0x00422DF0 */
unsigned int approach_half_speed(short obj);                           /* 0x00422E10 */
unsigned int approach_cruise_speed(short ship);                         /* 0x00422E50 */
unsigned int approach_full_speed(short ship);                           /* 0x00422E80 */
unsigned int approach_ship_speed(short obj, short other);             /* 0x00422EA0 */
#ifdef WC1_LOGIC_SOURCE
unsigned int get_front_spot(short, unsigned short, FixedVector *);     /* 0x00422EC0 */
unsigned int get_rear_spot(short, unsigned short, FixedVector *);      /* 0x00422F10 */
#else
unsigned int get_front_spot(short obj, unsigned short distance,
                            FixedVector *point);                       /* 0x00422EC0 */
unsigned int get_rear_spot(short obj, unsigned short distance,
                           FixedVector *point);                        /* 0x00422F10 */
#endif
#ifdef SDL_PORT
unsigned int close_behind(short range);
#else
unsigned int close_behind();                                          /* 0x00422F60 */
#endif
short scan_for_enemy(short obj, unsigned short range);                  /* 0x00422F80 */
int any_enemy(short obj, short range);                                  /* 0x00423070 */
short nearest_enemy_range(short obj);                                 /* 0x004230F0 */
void fire_when_ready(short obj, short aimed);                         /* 0x00423210 */
unsigned int ships_within_range(short obj, short other, short range); /* 0x00423260 */
int attacker_in_range(short obj, unsigned short range);                 /* 0x004232B0 */
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
unsigned int initialize_cockpit(int requestedMode);                     /* 0x00423E90 */
unsigned int InitializeConstellationObject(
    const ConstellationObjectDefinition *definition,
    short object);                                                /* 0x004242D0 */
unsigned int FreeConstellationObject(short object);               /* 0x004243B0 */
unsigned int init_constellation(short scene);                     /* 0x004243E0 */
unsigned int free_constellation(void);                            /* 0x00424490 */
void init_vdus(void);                                                  /* 0x004244E0 */
unsigned int InitializeCockpitResources(signed char mode);             /* 0x004245B0 */
unsigned int free_cockpit(void);                                       /* 0x004249A0 */
#ifdef SDL_PORT
unsigned int init_3Space_objects(short scene);
#else
unsigned int init_3Space_objects();                                    /* 0x00424A80 */
#endif
unsigned int load_common_3Space_objects(void);                         /* 0x00424B00 */
unsigned int remove_all_3d_objects(void);                              /* 0x00424B80 */
unsigned int free_3Space(void);                                        /* 0x00424BA0 */
unsigned int free_3Space_objects(void);                                /* 0x00424BE0 */
unsigned int init_inflight_music(void);                                /* 0x00424C60 */
unsigned int free_inflight_music(void);                                /* 0x00424C80 */
unsigned int PreloadMusicTrackHook(short track);                       /* 0x00424CE0 */
unsigned int ReleaseMusicTrackHook(short track);                       /* 0x00424CF0 */
unsigned short LoadSceneAnimationResources(short scene, short variant); /* 0x00424D00 */
void ReleaseSceneAnimationResources(void);                             /* 0x00424DA0 */
signed char *__stdcall FindSceneAnimationCommand(
    signed char *script, signed char command);                         /* 0x00424DE0 */
short __stdcall SceneAnimationGoalReached(short delta, short current,
                                          short goal);                 /* 0x00424EA0 */
unsigned short __stdcall UpdateSceneAnimationObject(
    SceneAnimationObject *object, Viewport *viewport);                 /* 0x00424EF0 */
void PlaySceneAnimation(char *text, short animation, short duration);  /* 0x00425500 */
