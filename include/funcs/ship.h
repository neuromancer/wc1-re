/* ship.c */
short initialize_object(short obj, enum ObjectType type,
                        short owner);                                  /* 0x0041DEE0 */
short borrow_dust(void);                                             /* 0x0041DF40 */
short new_object(enum ObjectType type, short owner);                 /* 0x0041DF70 */
short initialize_ship(enum ObjectType type, short owner);            /* 0x0041DFA0 */
short any_selected(signed char *loadout,
                   enum ObjectClass objectClass);                    /* 0x0041DFE0 */
unsigned int remove_weapon(short obj, short weapon);                 /* 0x0041E040 */
void set_objects_data(short obj, enum ObjectType type,
                      short owner);                                  /* 0x0041E120 */
unsigned int match_rotation_goal(short *rotation, short *goal,
                                 short totalError, short rate);         /* 0x0041E400 */
int rotate_object_to_goal(short obj);                                   /* 0x0041E520 */
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
int fire_fixed_projectile_weapon(short obj);                           /* 0x00421220 */
int drop_mine(short obj, signed char weapon, enum ObjectType type,
              short lifetime);                                       /* 0x004212A0 */
void fire_afterburner(short obj, short time);                          /* 0x00421350 */
short find_weapon(short obj, enum ObjectType weaponType);              /* 0x00421100 */
unsigned int check_computer_damage(void);                              /* 0x0041F5D0 */
short rnd_sign(short v);                                               /* 0x004208C0 */
unsigned int fire_super_brake(short ship);                              /* 0x004213B0 */
short flip_angle(short ship, short angle);                              /* 0x004213D0 */
unsigned int place_exhaust_on_ships(void);                              /* 0x00421430 */
unsigned int reposition_fixed_child_objects(void);                     /* 0x004215E0 */
unsigned int housekeep_power_plant_and_fuel(short ship);                /* 0x00421760 */
unsigned int replenish_shields(short ship);                            /* 0x00421780 */
unsigned int replenish_weapon_energy_bank(short ship);                 /* 0x00421830 */
void accelerate(short amount);                                         /* 0x004218D0 */
void your_afterburner(void);                                            /* 0x00421920 */
