/* screens.c */
unsigned int LoadBriefingRoom(void);                                  /* 0x00436D00 */
unsigned int ViewMedals(void);                                        /* 0x00436E30 */
unsigned int AwardCampaignMedal(short medal);                         /* 0x00436F50 */
short no_objectives_achieved(void);                                    /* 0x00438090 */
short wing_status(short personality);                                  /* 0x004380D0 */
short int_value(char **text);                                          /* 0x00438110 */
ConversationSceneRecord *ParseTests(ConversationSceneRecord *record,
                                    ConversationSceneRecord *sceneData,
                                    unsigned char *textData);          /* 0x00438160 */
unsigned int DrawMedalChest(char *text, short duration);                /* 0x004370D0 */
unsigned int DrawMedalLongShot(short *animation, char *text,
                               short duration);                        /* 0x00437250 */
unsigned int MedalEstablish(char *text, short duration);                /* 0x004373E0 */
unsigned int PinMedal(char *text, short duration);                      /* 0x004374B0 */
unsigned int DrawMedals(void);                                          /* 0x004375C0 */
unsigned int EstablishingShot(char *text, short duration);              /* 0x00437770 */
unsigned int DrawBriefingLongShot(void);                                /* 0x004378D0 */
unsigned int ReturnToBriefingLongShot(char *text, short duration);      /* 0x00437980 */
unsigned int Dismissed(char *text, short duration);                     /* 0x00437B80 */
unsigned int DrawDebriefingLongShot(void);                              /* 0x00437DC0 */
unsigned int DebriefingEstablishingShot(char *text, short duration);    /* 0x00437F20 */
unsigned int TalkerInit(void);                                         /* 0x00438B90 */
unsigned int FreeTalker(void);                                         /* 0x00438BC0 */
unsigned int SceneDirector(short sceneType, unsigned char *sceneData,
                           unsigned char *textData);                    /* 0x00438C00 */
unsigned int DrawPodiumShot(void);                                      /* 0x00439070 */
unsigned int DrawBriefingCharacter(short character, short pose,
                                   short animationFrame,
                                   const signed char *unusedXOffsets,
                                   const signed char *unusedYOffsets);  /* 0x00439150 */
unsigned int DrawFuneralLongShot(short shot, char *text,
                                 short duration);                       /* 0x00439220 */
void __stdcall SetViewportRect(Viewport *viewport, unsigned short left,
                               unsigned short top, unsigned short right,
                               unsigned short bottom);                /* 0x00439400 */
void __stdcall PanToScreen(Viewport *source, Viewport *destination);    /* 0x00439430 */
unsigned int death_sequence(void);                                    /* 0x00439660 */
unsigned int ShowGetReadyScreen(void);                                 /* 0x00439840 */
unsigned int ShowVictoryScreen(void);                                  /* 0x00439910 */
unsigned int ShowGameOverScreen(void);                                 /* 0x00439A80 */
void __stdcall FillRasterBytes(void *destination, unsigned int length,
                               short value);                          /* 0x00439C10 */
