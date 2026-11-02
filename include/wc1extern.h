/**
 * @file
 * @ingroup headers
 * @brief Three prototypes that nothing calls, kept for the exact build.
 *
 * Every function is now written, and these declarations are leftovers of the
 * early reconstruction: ShowEnemyTargetSelectMenu is an old name of
 * BuildCommunicationRecipientMenu, and _free_dbg and _ftol are CRT routines.
 * Each declaration still takes CL 4.20 symbol indices in every unit that
 * includes this header, and those indices break register-allocation ties:
 * removing the three changes the generated code.
 */
#ifndef GAME_EXTERN_H
#define GAME_EXTERN_H

void ShowEnemyTargetSelectMenu(void);                             /* 0x00430E90 */
void _free_dbg(void *p, int blockType);
long _ftol(void);
#endif /* GAME_EXTERN_H */
