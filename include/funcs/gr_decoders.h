/* gr.c */
/* Graphics-private shape decoders (gr.c takes their addresses). Declared last:
 * their parameter names then precede only the units' own symbols, which every
 * unit but gr.c tolerates and gr.c balances. */
void GetShapeFrameExtents(struct Shape *shape, short frame,
                          short *width, short *height,
                          short *leftExtent, short *topExtent);       /* 0x004408F0 */
void DecodeShapeFrame(struct Shape *shape, short frame,
                      unsigned char *bitmap, int width, int height,
                      int leftExtent, int topExtent);                 /* 0x00440960 */
