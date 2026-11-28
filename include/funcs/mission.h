/* mission.c */
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
unsigned int Set_up_ship_info(short obj, short missionShip,
                              signed char navPoint);                   /* 0x0040C5E0 */
unsigned int is_team_member(short missionShip);                        /* 0x0040C740 */
unsigned int find_next_ship_turn_slot(short obj);                     /* 0x0040C780 */
short init_ship(short missionShip, short navPoint);                    /* 0x0040C800 */
unsigned int init_intelligence_data(short obj);                       /* 0x0040C950 */
short __stdcall SampleBothJoysticks(InputDeviceSample *samples,
                                    short fallback);                    /* 0x0040CAA0 */
int __stdcall SampleJoystickDevice(InputDeviceSample *samples,
                                   short joystick,
                                   unsigned short fallback);            /* 0x0040CAE0 */
short ReadJoystickButtons(void);                                        /* 0x0040CB20 */
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
