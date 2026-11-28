/* smart.c */
unsigned int steer_away_from_object(short obj, short other,
                                    short amount);                    /* 0x00433AC0 */
void steer_away_from_predicted_object(short obj, short other,
                                      short predictionTicks,
                                      short amount);                  /* 0x00433B90 */
void prevent_collision(short obj);                                    /* 0x00433C80 */
short handle_collisions(short obj);                                   /* 0x00433D90 */
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
