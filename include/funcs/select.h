/* select.c */
short find_next_gun(short obj, enum ObjectType currentGun);           /* 0x0042AD00 */
int select_guns(short obj, short selectedGun);                         /* 0x0042ADA0 */
unsigned int select_new_gun(void);                                    /* 0x0042AE10 */
unsigned int select_new_release_weapon(enum ObjectType preferredType); /* 0x0042AE50 */
