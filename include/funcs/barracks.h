/* barracks.c */
short InitializeModalTextPanel(ModalTextPanel *panel, short fontIndex,
                               ShortRect bounds,
                               short clearColour,
                               short backgroundColour,
                               short borderColour);                    /* 0x0041A9D0 */
void DrawModalTextPanel(ModalTextPanel *panel, short x, short y,
                        unsigned char alignment,
                        const char *format, ...);                      /* 0x0041AAE0 */
void RestoreModalTextPanel(ModalTextPanel *panel);                     /* 0x0041AB60 */
short ShowModalTextPanel(short fontIndex, const char *format, ...);    /* 0x0041AB90 */
void ReleaseModalTextPanel(void);                                      /* 0x0041AD10 */
short AnySavedGames(void);                                             /* 0x0041AD50 */
void CreateEmptySaveGameFile(void);                                    /* 0x0041ADA0 */
void EnsureSaveGameFile(void);                                         /* 0x0041B020 */
void InitializeBarracksAnimation(BarracksAnimationState *state);       /* 0x0041B070 */
void FreeBarracksMenuLabel(char **label);                              /* 0x0041B0E0 */
void SetAwakenBarracksMenuLabel(char **label, short series, short mission,
                                char *description);                    /* 0x0041B110 */
void FreeBarracksMenuLabels(void);                                     /* 0x0041B180 */
short SaveGame(short slot, SaveGameRecord *gameRecord);                /* 0x0041B1E0 */
short PromptForTextInput(short x, short y, const char *prompt,
                         char *destination, short maximumLength,
                         short inputMode);                             /* 0x0041B420 */
int WarnLoadGameFirst(void);                                           /* 0x0041B550 */
void SaveGameWithNamePrompt(short slot, CampaignState *campaign,
                            PilotRecord *pilots,
                            MissionObjective *objectives);             /* 0x0041B5C0 */
int LoadGame(short slot, SaveGameRecord *gameRecord);                  /* 0x0041B710 */
void LoadGameFromSlot(short slot, CampaignState *campaign,
                      PilotRecord *pilots,
                      MissionObjective *objectives);                   /* 0x0041B980 */
void SetBunkMenuLabel(short occupied, short side, char **label,
                      short series, short mission, char *description);     /* 0x0041BAD0 */
void GetBunkInfo(BarracksAnimationState *state);                       /* 0x0041BB20 */
void DrawBarracksBunks(Viewport *viewport, struct Shape *shape,
                       BarracksAnimationState *state);                 /* 0x0041BBD0 */
void DrawBarracksStaticDetails(Viewport *viewport,
                               struct Shape *shape);                  /* 0x0041BC90 */
void AnimateBarracks(Viewport *viewport, struct Shape *shape,
                     BarracksAnimationState *state);                   /* 0x0041BCE0 */
int ConfirmQuitWingCommander(void);                                   /* 0x0041BF10 */
int ConfirmAwakenAfterBadData(short slot);                             /* 0x0041BF60 */
int ConfirmReplaceFaultyData(short slot);                              /* 0x0041BFE0 */
void HandleBarracksBunkSelection(Viewport *viewport,
                                 struct Shape *shape,
                                 BarracksAnimationState *state,
                                 short region);                        /* 0x0041C090 */
void UpdateBarracksScreen(Viewport *viewport, struct Shape *shape,
                          BarracksAnimationState *state);              /* 0x0041C140 */
short BarracksScreen(void);                                            /* 0x0041C170 */
