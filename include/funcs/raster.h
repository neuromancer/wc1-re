/* raster.asm */
char *CopyRasterDriverName(void *const *callbacks);                   /* 0x00439C40 */
void InstallRasterDriverCallbacks(void *const *callbacks);            /* 0x00439C69 */
unsigned int SetRasterClipPixel(RasterClip *clip, int x, int y,
                                int colour);                           /* 0x00439C88 */
int ReadRasterClipPixel(RasterClip *clip, int x, int y);                /* 0x00439D63 */
unsigned int DrawClippedLine(RasterClip *clip, int x1, int y1, int x2, int y2,
                             int mode, int colour);                    /* 0x00439E39 */
int FillRasterClipCheckerboard(RasterClip *clip, int left, int top,
                               int right, int bottom,
                               unsigned char colour);                 /* 0x0043A83B */
int DrawRLEImage(RasterClip *clip, struct RLEShape *shape, int frame,
                 int x, int y);                                      /* 0x0043A974 */
int DrawRLEImageUnclipped(RasterClip *clip, RLEFrameHeader *frameHeader,
                          int x, int y,
                          int strideScratch);                         /* 0x0043AD78 */
void SetPaletteTranslationTable(const unsigned char *translation);    /* 0x0043AE3F */
int DrawRLEImageColor(RasterClip *clip, struct RLEShape *shape, int frame,
                      int x, int y);                                 /* 0x0043AE5E */
int DrawRLEImageColorUnclipped(RasterClip *clip,
                               RLEFrameHeader *frameHeader,
                               int x, int y,
                               int strideScratch);                    /* 0x0043B336 */
int RotateRLEImage(RasterClip *clip, struct RLEShape *shape, int frame,
                   int x, int y, unsigned char *scratch,
                   unsigned int angleTenths, int scaleX, int scaleY,
                   unsigned int flags);                              /* 0x0043B469 */
unsigned int GetRLEFrameBounds(struct RLEShape *shape, int frame,
                               int x, int y, unsigned int flags,
                               int *bounds);                          /* 0x0043C015 */
int EncodeRasterClipToRLEFrame(RasterClip *clip,
                               unsigned char transparentColour,
                               int originX, int originY,
                               unsigned char *output);                /* 0x0043C18D */
unsigned int TranslateRLEFramePalette(struct RLEShape *shape,
                                      int frame);                     /* 0x0043C410 */
void EncodeRLEScanline(int pixelCount, unsigned char transparentColour,
                       int sourceX);                                  /* 0x0043C4A2 */
void EmitRLEScanlineRun(int runType, int trailingCount,
                        int sourceX);                                 /* 0x0043C62B */
int FillRasterClip(RasterClip *clip, int colour);                      /* 0x0043C808 */
int BlitRasterClip(RasterClip *source, int sourceX, int sourceY,
                   RasterClip *destination, int destinationX,
                   int destinationY, unsigned int colour);            /* 0x0043C8E7 */
int ScrollRasterClipWrapped(RasterClip *clip, int deltaX, int deltaY,
                            int mode,
                            unsigned int fillOrScratch);              /* 0x0043CC83 */
unsigned int DrawRasterEllipse(RasterClip *clip, int x, int y,
                               int horizontalRadius, int verticalRadius,
                               int colour);                            /* 0x0043CE80 */
unsigned int FillRasterEllipse(RasterClip *clip, int x, int y,
                               int horizontalRadius, int verticalRadius,
                               int colour);                            /* 0x0043D1C1 */
void GetRLETransformTrig(int angleTenths, int *cosine, int *sine);     /* 0x0043E2D3 */
void CalculateRoundedRLEFixedProduct(int left, int right, int *result); /* 0x0043E38B */
void TransformRLEPoint(int *point, int *result, int *origin,
                       unsigned int angleTenths, int scaleX,
                       int scaleY);                                   /* 0x0043E3B1 */
unsigned int GetRawImageHeight(unsigned char *shape);                 /* 0x0043E478 */
unsigned int GetRawFrameWidth(unsigned char *shape, int frame);       /* 0x0043E48B */
int BlitRawFrame(RasterClip *clip, int x, int y, unsigned char *shape,
                 int frame, unsigned char *translation);              /* 0x0043E4AB */
void BlitSelectedRawFrames(RasterClip *clip, int x, int y,
                           unsigned char *shape,
                           const unsigned char *frames,
                           unsigned char *translation);               /* 0x0043E63E */
int BlitRawScanline(RasterClip *clip, int y,
                    const unsigned char *pixels,
                    int width);                                      /* 0x0043E675 */
unsigned char *FindIFFChunkData(const char *chunkId,
                                const unsigned char *iffData);        /* 0x0043E784 */
unsigned int DecodeIFFImage(RasterClip *clip,
                            const unsigned char *iffData);            /* 0x0043E7C6 */
void CopyILBMPalette(const unsigned char *iffData,
                     unsigned char *palette);                         /* 0x0043E98D */
unsigned int GetILBMImageSize(const unsigned char *iffData);          /* 0x0043E9BE */
unsigned int DecodePCXImage(RasterClip *clip,
                            const unsigned char *pcxData);            /* 0x0043E9EB */
void CopyPCXPaletteFromFileTail(const unsigned char *fileData,
                                unsigned int fileSize,
                                unsigned char *palette);              /* 0x0043EA6D */
unsigned int GetPCXImageSize(const unsigned char *header);            /* 0x0043EA98 */
void ResetGIFLZWDictionary(void);                                     /* 0x0043EABF */
unsigned int ReadGIFDataSubBlockByte(void);                           /* 0x0043EB07 */
unsigned int ReadGIFLZWCode(void);                                    /* 0x0043EB20 */
void AppendGIFLZWDictionaryEntry(void);                               /* 0x0043EB66 */
void EmitGIFDecodedPixel(void);                                       /* 0x0043EBAC */
unsigned int ExpandGIFLZWImage(RasterClip *clip,
                               const unsigned char *gifData,
                               void *workspace);                      /* 0x0043EC29 */
void CopyGIFPalette(const unsigned char *gifData,
                    unsigned char *palette);                          /* 0x0043EE42 */
unsigned int GetGIFImageSize(const unsigned char *gifData);           /* 0x0043EEA3 */
unsigned int GetRLEFrameDimensions(struct RLEShape *shape,
                                    int frame);                        /* 0x0043EEDB */
unsigned int GetRLEFrameExtents(struct RLEShape *shape,
                                int frame);                           /* 0x0043EEFD */
unsigned int GetRLEImageSize(struct RLEShape *shape, int frame);         /* 0x0043EF20 */
unsigned int GetRLEImageOrigin(struct RLEShape *shape, int frame);       /* 0x0043EF54 */
void ApplyRLEFramePalette(struct RLEShape *shape, int frame,
                          unsigned char *palette);                    /* 0x0043EF7E */
unsigned int CopyRLEFramePalette(struct RLEShape *shape, int frame,
                                 unsigned char *entries);             /* 0x0043EFC9 */
unsigned int SetRLEFramePalette(struct RLEShape *shape, int frame,
                                const unsigned char *entries);        /* 0x0043F011 */
unsigned int GetRLEFrameCount(const struct RLEShape *shape);            /* 0x0043F05B */
int CollectUniqueRLEImageFrames(const struct RLEShape *shape,
                                unsigned int *frames);                /* 0x0043F06E */
int CollectUniqueRLEPaletteFrames(const struct RLEShape *shape,
                                  unsigned int *frames);              /* 0x0043F0D0 */
void FadeRasterPaletteToPalette(RasterSurface *surface,
                                const unsigned char *targetPalette,
                                unsigned int duration);               /* 0x0043F425 */
int CollectRasterClipColours(RasterClip *clip,
                             unsigned int *colours);                  /* 0x0043F5A9 */
