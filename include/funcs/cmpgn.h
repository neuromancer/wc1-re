/* cmpgn.c */
unsigned int ParseFaceAnimation(char *text, short *commands);           /* 0x00404CD0 */
unsigned int ParseMouthAnimation(char *text, short *commands);          /* 0x00404D70 */
char *AddPCName(const char *text);                                      /* 0x00404E10 */
unsigned int LoadFace(short face);                                      /* 0x004050B0 */
unsigned int LongTalk(struct Shape *talker, char *text,
                      short *mouthCommands, short *faceCommands,
                      short duration);                                 /* 0x00405290 */
unsigned int CloseTalk(struct Shape *talker, short mouthFrame,
                       short faceFrame);                               /* 0x004054B0 */
unsigned int Briefing(short series, short mission);                    /* 0x00405660 */
unsigned int DeBriefing(short series, short mission);                  /* 0x004056F0 */
unsigned int Office(void);                                             /* 0x00405840 */
unsigned int LoadBriefingData(short series, short mission);             /* 0x00405910 */
unsigned int LoadMissionData(short series, short mission);              /* 0x004059B0 */
unsigned int UpdateMap(char *text, short duration);                     /* 0x00405CC0 */
unsigned int CloseLook(struct Shape *shape, short shot,
                       short *animation, char *text, short duration,
                       int unused);                                    /* 0x00405DE0 */
