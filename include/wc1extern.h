/*
 *  Functions referenced by the reconstruction but not yet reimplemented.
 *
 *  Each one still carries an operational Ghidra label because nothing has
 *  been written for it yet; the label describes mechanism, not intent
 *  (docs/LABELS.md).  Entries move to include/wc1funcs.h as they are
 *  implemented, and get a real name at that point.
 */
#ifndef WC1_EXTERN_H
#define WC1_EXTERN_H

short LoadWingCmdrCfgFile(short argc, char **argv);               /* 0x0042C580 */
void ShowEnemyTargetSelectMenu(void);                             /* 0x00430E90 */
void _free_dbg(void *p, int blockType);                           /* Function start: 0x401250 */
long _ftol(void);                                                 /* Pointer table of prompt strings indexed by SendCommMenuChoice. */
#endif /* WC1_EXTERN_H */
