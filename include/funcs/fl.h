/* fl.c */
void SetManeuverProximityRange(unsigned short v);                                 /* 0x004060A0 */
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
void Mgloat(short ship, short target);                                  /* 0x00406C70 */
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
void general_zig(short ship, short target, short pitch);                /* 0x00407270 */
void Mzig_zag(short ship, short target);                               /* 0x00407350 */
void Mzig_zag_pitch(short ship, short target);                         /* 0x00407370 */
void Mcorkscrew(short ship);                                            /* 0x00407390 */
void Mveer_away(short ship, short target);                              /* 0x00407450 */
void ShipAiState44(short ship);                                         /* 0x00407560 */
void Mtarget_laser(short ship, short target);                           /* 0x00407580 */
void Mrout_me(short ship);                                              /* 0x004075A0 */
void Mnone(void);                                                       /* 0x004075B0 */
void Mreset(short ship);                                                /* 0x004075C0 */
void perform_maneuver(short obj);                                       /* 0x004075D0 */
