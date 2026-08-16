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

void ShowEnemyTargetSelectMenu(void);                             /* 0x447629 */
void _free_dbg(void *p, int blockType);                           /* 0x471180 */
long _ftol(void);                                                 /* Pointer table of prompt strings indexed by SendCommMenuChoice. */
#endif /* WC1_EXTERN_H */
