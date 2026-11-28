/* killbrd.c */
void CorrectPointers(void);                                            /* 0x0043F640 */
void ClearRoomMenuLabel(void);                                        /* 0x0043F690 */
char IsRoomMenuLabelEmpty(void);                                       /* 0x0043F6A0 */
void DrawRoomMenuLabel(TextContext *context,
                       const char *label);                            /* 0x0043F6B0 */
void RefreshRoomMenuLabel(void);                                      /* 0x0043F6F0 */
void ClearRoomMenuCursorFrame(void);                                  /* 0x0043F720 */
void SelectRoomMenuLabel(short i);                                    /* 0x0043F730 */
void InitializeRoomMenu(const TitleMenuRegion *regions, char **labels,
                        Viewport *viewport, char *text,
                        unsigned char alignment);                     /* 0x0043F750 */
int FindMenuRegionAtPoint(short x, short y,
                          const TitleMenuRegion *regions);             /* 0x0043F7C0 */
void InitializeRoomViewports(void);                                   /* 0x0043F810 */
short RecRoom(void);                                                  /* 0x0043F940 */
void ShowChalkBoard(void);                                            /* 0x00440510 */
void ResetCampaignData(void);                                         /* 0x00440800 */
short __stdcall ReadPacketSectionData(PacketSectionHandle *handle,
                                      void *destination,
                                      unsigned int length);            /* 0x00440840 */
