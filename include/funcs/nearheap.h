/* nearheap.c */
unsigned short MergeAdjacentNearHeapBlocks(int descriptorAddress);   /* 0x0040E890 */
int ReleaseNearHeapBlock(int descriptorAddress);                     /* 0x0040E900 */
void PurgeNearHeapBlocks(unsigned short flags);                       /* 0x0040E950 */
unsigned short InitializeNearHeap(void);                              /* 0x0040E9E0 */
void *AllocateNearHeapBlockFromEnd(int size, unsigned short flags);   /* 0x0040EB70 */
void *AllocateNearHeapBlockByFlags(int size, unsigned short flags);   /* 0x0040ED30 */
