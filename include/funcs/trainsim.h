/* trainsim.c */
unsigned short EraseTextContextBackground(TextContext *context);      /* 0x00425C30 */
int DisplayTrainSimHighScoreTable(short phase);                       /* 0x00425C60 */
int AnimateTrainSimTitle(void);                                       /* 0x00425D00 */
unsigned char *GetHighScoreEntry(short i);                           /* 0x00425DF0 */
unsigned int GetHighScoreValue(short i);                                      /* 0x00425E20 */
unsigned int SetHighScoreEntry(short i, unsigned char b,
                               unsigned int v);                       /* 0x00425E30 */
void SortTrainSimHighScores(void);                                   /* 0x00425E50 */
short FindTrainSimHighScore(short pilot);                             /* 0x00425ED0 */
short InsertTrainSimHighScore(short pilot, unsigned int score);       /* 0x00425EF0 */
void InitializeTrainSimHighScores(void);                              /* 0x00425F40 */
short IsHighScoreSlotUsed(short i);                                    /* 0x00425FE0 */
void AddRandomTrainSimHighScores(void);                               /* 0x00426000 */
void DrawTextInputCursor(char character);                             /* 0x004260E0 */
void ClearTextInputCharacter(short character);                        /* 0x00426140 */
void ClearNextTextInputCharacter(short character);                    /* 0x004261D0 */
short ReadTextInput(char *destination, short maximumLength,
                    short mode);                                     /* 0x00426200 */
void ReadRequiredPilotField(short x, short y, const char *label,
                            char *destination,
                            short maximumLength);                    /* 0x004265A0 */
void PromptForPilotField(short x, short y, const char *label,
                         char *destination, short maximumLength,
                         const char *defaultText);                    /* 0x00426600 */
void InitializeTrainSimTextPanel(void);                               /* 0x00426660 */
void ShowTrainSimTextMessage(const char *message);                    /* 0x00426700 */
void EnterPilotNameAndCallsign(void);                                 /* 0x00426750 */
void UpdateTrainSimHighScores(int score);                             /* 0x00426820 */
void ShowTrainSimHighScores(void);                                    /* 0x004268E0 */
struct Shape *LoadTrainSimOpponentShape(int opponent);               /* 0x00426C50 */
short SelectTrainSimMission(short *mission);                           /* 0x00426C70 */
void RunTrainSim(void);                                                /* 0x00427080 */
