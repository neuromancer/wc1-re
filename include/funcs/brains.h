/* brains.c */
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
