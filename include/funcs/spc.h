/* spc.c */
void SetFleetOverviewView(int initializeCockpit);                     /* 0x00410740 */
unsigned int rotate_eye_to_goal(void);                                 /* 0x00410A30 */
short GetVectorMagnitude(const FixedVector *vector);                   /* 0x00410AD0 */
unsigned int set_eye_direction_and_position(void);                     /* 0x00410AF0 */
unsigned int force_view(int view, short obj);                          /* 0x004117B0 */
unsigned int new_view(int view, short obj);                            /* 0x004117D0 */
unsigned int start_dust(short obj, FixedVector origin,
                        short forwardDistance, int rightOffset,
                        int upOffset);                                 /* 0x00411EC0 */
unsigned int generate_stars(void);                                    /* 0x00411FE0 */
unsigned int update_star_field(void);                                 /* 0x00412100 */
short count_down(short obj);                                          /* 0x00412410 */
unsigned int house_keep_objects(void);                                 /* 0x00412430 */
unsigned int update_objects_in_space(void);                            /* 0x00412820 */
unsigned int rotate_object(short obj);                                 /* 0x00412920 */
unsigned int accelerate_and_move_object(short obj);                    /* 0x004129A0 */
unsigned int animate_shape(short obj);                                 /* 0x00412CD0 */
unsigned int animate_object(short obj);                                /* 0x00412E30 */
unsigned int hit_asteroid(short asteroid, short destructionChance);    /* 0x00413030 */
int object_collision(short obj);                                      /* 0x004130D0 */
unsigned int object_intelligence(short obj);                           /* 0x00413880 */
