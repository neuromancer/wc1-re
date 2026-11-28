/* view.c */
unsigned int parse_view_script(void);                                  /* 0x0042CDB0 */
unsigned int update_scripted_view(void);                               /* 0x0042D1C0 */
void initialize_scripted_view(const short *script);                    /* 0x0042D230 */
unsigned int InitializeFireworks(void);                                /* 0x0042D270 */
short TheEndFireWorks(Viewport *viewport, short count);                /* 0x0042D2A0 */
unsigned int InitializeConstellationField(Viewport *viewport,
                                          short direction,
                                          short density);             /* 0x0042D390 */
unsigned int DrawConstellationField(void);                             /* 0x0042D500 */
short __stdcall OpenPacketSection(const char *filename, short section,
                                  PacketSectionHandle *handle);        /* 0x0042D730 */
void __stdcall CloseDataFileByHandle(PacketSectionHandle *p);                       /* 0x0042D870 */
void * __stdcall DecompressPacketSection(
    PacketSectionHandle *handle, void *destination, unsigned short flags,
    void *decompressionWorkspace);                                    /* 0x0042D880 */
short GetTargetColourIndex(void);                                                /* 0x0042DB70 */
void show_target_disp(void);                                         /* 0x0042DB90 */
void DrawTargetRangeReadout(void);                                   /* 0x0042DEA0 */
void LogDisplayMode(const char *mode);                                 /* 0x0042E020 */
short __stdcall CalcRectangleArea(const Viewport *viewport);           /* 0x0042E050 */
unsigned short __stdcall AllocateViewport(Viewport *viewport,
                                          short clearColour,
                                          short flags);                /* 0x0042E090 */
void __stdcall AlignSpriteFrameToRectCorner(
    const ShortRect *rectangle, ShortPoint *position, short corner,
    struct Shape *shape, short frame);                                /* 0x0042E1D0 */
