/* gr.c */
void CheckHeapBlockSignature(struct Shape *shape);                  /* 0x004408A0 */
struct RLEShape *GetPreparedShapeData(struct Shape *shape);           /* 0x004408C0 */
short __stdcall GetShapeFrameCount(struct Shape *shape);            /* 0x004408D0 */
int SignExtendClipCoord(volatile short v);                             /* 0x00440BE0 */
void ValidateViewportBounds(Viewport *viewport, RasterSurface *surface,
                            RasterClip *clip);                         /* 0x00440C00 */
void ClipViewportToScreen(Viewport *viewport);                         /* 0x00440CF0 */
void SetSolidColourTranslation(unsigned char colour);                 /* 0x00440D10 */
void PrepareShapeRLEData(struct Shape *shape);                        /* 0x00440D50 */
void DrawSpriteTransformed(Viewport *viewport, int x, int y,
                           struct Shape *shape, int frame,
                           int angle, int scaleX, int scaleY,
                           int flip, int blendMode);                /* 0x00440FE0 */
void RasterLineHook(const void *marker);                              /* 0x00441140 */
void DrawFontGlyph(char character, TextContext *context, int height,
                   int width, int y);                                 /* 0x00441150 */
void __copyline(void);                                                 /* 0x00441350 */
void __wipeline(void);                                                 /* 0x00441360 */
void __stdcall MarkActivePaletteEntries(Viewport *viewport,
                                         unsigned char *active);       /* 0x00441370 */
void __stdcall GetPaletteEntry(short index, short *rgb);                  /* 0x004413C0 */
void __stdcall SetPaletteEntry(short index, short *rgb);                  /* 0x004413E0 */
void DrawSpriteDefault(Viewport *viewport, short x, short y,
                       struct Shape *shape, short frame);          /* 0x00441400 */
void CaptureSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, struct Shape *shape,
                             short frame);                          /* 0x00441450 */
void RestoreSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, struct Shape *shape,
                             short frame);                          /* 0x00441740 */
void DrawSolidColourSprite(Viewport *viewport, short x, short y,
                           struct Shape *shape, short frame,
                           short colour);                            /* 0x00441A40 */
void CopyViewportContents(Viewport *source, Viewport *destination);    /* 0x00441A90 */
void ClearViewport(Viewport *viewport, short colour);                  /* 0x00441AE0 */
void DrawViewportPixel(Viewport *viewport, short x, short y,
                       short colour);                                 /* 0x00441B20 */
int GetViewportPixel(Viewport *viewport, short x, short y);            /* 0x00441B60 */
void DrawViewportLine(Viewport *viewport, short x1, short y1,
                      short x2, short y2, short colour);             /* 0x00441BA0 */
void DrawViewportPolygonHook(Viewport *viewport, const ShortPoint *points,
                             int count, int colour);                 /* 0x00441BF0 */
void triangle(Viewport *viewport, const ShortPoint *points, int colour); /* 0x00441C00 */
void shaded_triangle(Viewport *viewport);                             /* 0x00441C40 */
void DrawFilledViewportRect(Viewport *viewport, short left, short top,
                            short right, short bottom,
                            short colour);                           /* 0x00441C70 */
void DrawViewportBorder(Viewport *viewport, short left, short top,
                        short right, short bottom,
                        short colour);                                /* 0x00441CF0 */
void DrawViewportEllipse(Viewport *viewport, short x, short y,
                         short verticalRadius, short horizontalRadius,
                         short colour);                               /* 0x00441DD0 */
void FillViewportEllipse(Viewport *viewport, short x, short y,
                         short verticalRadius, short horizontalRadius,
                         short colour);                               /* 0x00441E20 */
void DrawViewportEllipseShadow(Viewport *viewport, short x, short y,
                               short verticalRadius,
                               short horizontalRadius,
                               short colour);                         /* 0x00441E70 */
void shadow_draw(Viewport *viewport);                                 /* 0x00441EC0 */
void DrawShapeFrameScaled(Viewport *viewport, struct Shape *shape,
                          int frame, int x, int y, int angle,
                          int scale);                                /* 0x00441EF0 */
void GetShapeFrameCorners(unsigned char *shape, int frame, int x, int y,
                          ShortPoint *corners, int *cornerCount);     /* 0x00441F30 */
void DrawSpriteScaled(Viewport *viewport, short x, short y,
                      struct Shape *shape, short frame, short angle,
                      short scale, short flip);                       /* 0x00441FC0 */
void DrawSolidColourSpriteScaled(Viewport *viewport, short x, short y,
                                 struct Shape *shape, short frame,
                                 short angle, short scale, short flip,
                                 short colour);                        /* 0x00442000 */
short GetTransformedShapeBounds(Viewport *viewport, short x, short y,
                              struct Shape *shape, short frame,
                              short angle, short scale, short flip,
                              ShortRect *bounds);                    /* 0x00442050 */
void BlitViewportScanlineRuns(Viewport *source, Viewport *destination,
                              const ScreenViewportGeometry *geometry); /* 0x00442200 */
void fizzle_fade(Viewport *source, Viewport *destination);             /* 0x004422D0 */
void snow_viewport(Viewport *viewport, int effect,
                   unsigned short colour);                            /* 0x00442300 */
void UpdateStreamerStoppedFlag(void);                                    /* 0x00442330 */
void SignalAudioMixerWakeEvent(void);                                 /* 0x00442350 */
void InitializeAudioStreamer(HWND window);                            /* 0x00442370 */
void Streamer_open(const char *streamName);                           /* 0x004423B0 */
void Streamer_play(void);                                             /* 0x00442430 */
void Streamer_stop(void);                                                /* 0x00442460 */
int GetStreamerTrigger(void);                                         /* 0x00442490 */
void ClearStreamerTrigger(void);                                      /* 0x004424B0 */
void Streamer_trigger(int trigger);                                   /* 0x004424D0 */
void SetStreamerIntensity(unsigned char intensity);                   /* 0x00442520 */
void ForceStreamerTrigger(int trigger);                               /* 0x00442550 */
void SetMusicStreamVolume(unsigned short volume);                     /* 0x00442590 */
unsigned int GetMusicStreamVolume(void);                              /* 0x004425B0 */
void Streamer_close(void);                                            /* 0x004425D0 */
void ShutdownAudioStreamer(void);                                     /* 0x004425F0 */
int ReadCheaterFlagFromRegistry(void);                                /* 0x00442600 */
