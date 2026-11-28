/* init.c */
unsigned int LoadGamePaletteFile(void);                                /* 0x004219C0 */
unsigned int EMShutDown(void);                                         /* 0x00421A40 */
unsigned short InitializeEventManagerResources(void);                  /* 0x00421A60 */
unsigned int EMStartUp(void);                                          /* 0x00421AB0 */
unsigned int LoadOriginFxDrivers(void);                                 /* 0x00421B10 */
unsigned int InitializeGameTextContexts(void);                          /* 0x00421D80 */
unsigned int initialize_direction_view_frame(short yaw, short pitch,
                                             signed char frame);       /* 0x00421E20 */
unsigned int initialize_direction_view_frames(void);                    /* 0x00421EF0 */
unsigned int LoadSpaceflightResources(void);                           /* 0x00421F50 */
unsigned int GetFxDriverInitResult(void);                                      /* 0x00421FE0 */
unsigned int GetMessagePumpResult(void);                                      /* 0x00421FF0 */
unsigned int GetFxDriverStatus(void);                                      /* 0x00422000 */
