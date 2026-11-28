/* mathfp.c */
short RandomBelow(short n);                                           /* 0x00434CD0 */
void __stdcall SeedRandomFromClock(int seed);                          /* 0x00434CF0 */
unsigned short RandomWord(void);                                        /* 0x00434D10 */
short __stdcall RandomInRange(short lo, short hi);                      /* 0x00434D20 */
short RandomBelowOrEqual(short n);                                      /* 0x00434D50 */
long MultiplyFixed(int left, int right);                              /* 0x00434D80 */
long DivideFixed(int numerator, int denominator);                     /* 0x00434DB0 */
long SinFixed(short degrees);                                    /* 0x00434E00 */
long CosFixed(short degrees);                                    /* 0x00434E30 */
long ArcSin(int value);                                               /* 0x00434E60 */
long ArcCos(int value);                                               /* 0x00434E90 */
long Magnitude(int value);                                           /* 0x00434EC0 */
long PlanarMagnitude(int x, int y);                                  /* 0x00434EE0 */
long Vector_magnitude(const FixedVector *vector);                     /* 0x00434F20 */
void __stdcall SetTextCursor(unsigned short a, unsigned short b);   /* 0x00434F70 */
void __stdcall SetTextContext(TextContext *context);                  /* 0x00434FA0 */
void WaitForVerticalBlankThunk(void);                                  /* 0x00434FB0 */
void *__stdcall IdentityHandle(void *v);                               /* 0x00434FC0 */
void __stdcall SetWholePaletteFromTriplets(unsigned char *palette);               /* 0x00434FD0 */
unsigned short __stdcall FontHeight(const void *p);                    /* 0x00434FE0 */
unsigned short __stdcall GetFontCharWidth(char i);                                     /* 0x00434FF0 */
void ReleaseVideoResourcesHook(void);                                           /* 0x00435010 */
short __stdcall GetShapeFrameBounds(short *bounds, short x, short y,
                                    struct Shape *shape, short frame); /* 0x00435020 */
short __stdcall IsPointInRect(short x, short y, const short *rect);       /* 0x00435090 */
void __stdcall SplitPackedPoint(ShortPoint point, short *p);             /* 0x004350D0 */
void __stdcall DrawTextString(const char *text);                       /* 0x004350F0 */
void __stdcall DrawTextCharacter(char character);                    /* 0x00435290 */
void __stdcall AppendTextCharacter(char character);                  /* 0x00435310 */
int __stdcall MeasureShapeFrameStorage(struct Shape *shape,
                                       short frame);                   /* 0x00435340 */
void ResetTextCursor(void);                                           /* 0x004353F0 */
