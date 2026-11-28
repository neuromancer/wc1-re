/* cutscene.c */
short __stdcall GetShapeFrameExtent(short x, short y,
                                    struct Shape *shape, short frame,
                                    short extent);                       /* 0x00407710 */
unsigned int AnimateScrambleWalk(short ticks);                          /* 0x00407750 */
unsigned int PlayScrambleHangarScene(void);                            /* 0x004079C0 */
unsigned int DrawScrambleActor(short actorIndex);                      /* 0x00407C90 */
unsigned int ConfigureScrambleActor(short x, short y, short deltaX,
                                    short deltaY, struct Shape *shape,
                                    short scale, short angle,
                                    signed char flip,
                                    short actorIndex);                 /* 0x00407D90 */
unsigned int DrawScrambleFrame(void);                                  /* 0x00407E10 */
unsigned int scramble(void);                                           /* 0x00408200 */
unsigned int landing(signed char damageLevel);                         /* 0x00408650 */
unsigned int funeral_player(void);                                     /* 0x00408B30 */
unsigned int funeral_wingman(char *text, short duration);              /* 0x00408D50 */
unsigned int funeral_sequence(int playerFuneral);                      /* 0x00408DE0 */
unsigned int RunAnimationDemoLoop(signed char animation);             /* 0x004094E0 */
