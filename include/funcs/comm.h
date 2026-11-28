/* comm.c */
short get_face(short rating, unsigned int side);                      /* 0x00430BC0 */
void LoadCommPortraitShape(short face, signed char alternate);        /* 0x00430BF0 */
void ResetCommMenuChoices(short reuse);                               /* 0x00430C50 */
short IsCommMenuIdle(void);                                            /* 0x00430CA0 */
void AppendCommMenuChoice(const char *text, short command);            /* 0x00430CB0 */
void SendCommMenuChoice(short i);                                          /* 0x00430D30 */
void OpenCommMenuForTarget(const char *heading,
                           const char *message);                       /* 0x00430D50 */
short IsCommChoiceMenuOpen(void);                                     /* 0x00430D80 */
short GetPendingMenuAction(void);                                            /* 0x00430DA0 */
void SetPendingMenuAction(unsigned char v);                                 /* 0x00430DB0 */
void OpenCommRecipientMenu(void);                                     /* 0x00430DC0 */
void CloseCommChoiceMenu(void);                                       /* 0x00430DE0 */
short wingman_dead(void);                                             /* 0x00430E10 */
short have_target(void);                                              /* 0x00430E30 */
unsigned short CanOpenCommMenu(void);                                 /* 0x00430E50 */
void SelectCommRecipient(short recipient);                            /* 0x00430E70 */
void BuildCommunicationRecipientMenu(void);                           /* 0x00430E90 */
void BuildCommunicationCommandMenu(void);                             /* 0x00430FC0 */
void RefreshCommunicationMenu(void);                                 /* 0x00431200 */
void HandleCommunicationMenuRequest(void);                            /* 0x00431240 */
void show_communications_disp(void);                                  /* 0x00431290 */
unsigned int Chosen_communicate_option(short choice);                 /* 0x00431350 */
void talk_equiv(void);                                                /* 0x00431400 */
void FreeCommDisplayResources(void);                                  /* 0x00431410 */
void EndCommSessionWithWingman(void);                                  /* 0x00431470 */
void EndCommMenu(void);                                              /* 0x004314C0 */
void ShowCentredPrompt(char *text, unsigned short arg);                       /* 0x004314F0 */
short LoadCommDisplayResources(short rating, enum Side side);         /* 0x00431520 */
char *ExpandCommMessageTokens(const char *text);                      /* 0x004315C0 */
void real_vid_transmit(short obj, short message);                     /* 0x004316E0 */
