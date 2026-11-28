/* gameflow.c */
void add_statistics(short pilot, short missions, short kills);         /* 0x0040EFE0 */
void PostMission(void);                                                 /* 0x0040F010 */
int FullMissionScore(void);                                             /* 0x0040F190 */
int PlayersMissionScore(void);                                          /* 0x0040F1E0 */
unsigned int UpdateSeries(void);                                        /* 0x0040F240 */
unsigned int MoveNewCampaign(void);                                     /* 0x0040F3F0 */
unsigned int StartNewCampaign(short campaign);                         /* 0x0040F440 */
short GameFlow(void);                                                   /* 0x0040F4B0 */
void __stdcall free_viewport(Viewport *viewport);                      /* 0x0040F940 */
