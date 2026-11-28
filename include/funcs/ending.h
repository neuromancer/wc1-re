/* ending.c */
void *AllocateTaggedMemory(int size, unsigned short flags);            /* 0x0042FA90 */
void ReleasePacketHandle(void *handle);                               /* 0x0042FAE0 */
unsigned int GetFixedOneMillionThunk(short memoryType);                /* 0x0042FB20 */
unsigned int GetFixedOneMillionThunkAlt(short memoryType);             /* 0x0042FB30 */
void CreateCannedSceneObject(short *object, short yaw, short unusedPitch,
                             short distance, struct Shape *shape,
                             short frame, short type,
                             short scale);                         /* 0x0042FB40 */
unsigned int ShowCampaignVictorySequence(void);                    /* 0x0042FC00 */
unsigned int ShowTigerClawEscapeScene(void);                          /* 0x00430150 */
unsigned int ShowTheEndScreen(short enableFireworks);                 /* 0x004304F0 */
