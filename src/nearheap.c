/*
 * DOS near-heap descriptor allocation and compaction.
 * Placement and boundary evidence are recorded in docs/ORDER.md.
 */
#ifndef SDL_PORT
#include <process.h>
#endif
#define WC1_NEARHEAP_SOURCE
#include "wc1.h"

static short nNearHeapActive = 0; /* 0x004688c0 */
short nNearHeapMaxDescriptors = 0x80; /* 0x004688c4 */
static unsigned int nNearHeapRelocationBytes = 0; /* 0x004688c8 */

/* Linker commons defined here; LINK orders them by identifier hash. */
int nNearHeapSize; /* 0x005a811c */
int nNearHeapBase; /* 0x005a8120 */
int nCurrentNearHeapFirstDescriptor; /* 0x005a8124 */
void *pCurrentNearHeapAlloc; /* 0x005a8128 */
short nNavMapMaximumPositionX; /* 0x005a812c */
short nNavigationMapMinimumCoordX; /* 0x005a812e */
unsigned short awMapNavObjectiveLabelIndex[16]; /* 0x005a8130 */
short nNavMapMaximumPosY; /* 0x005a8150 */
short nNavMapCentreX; /* 0x005a8152 */
short nNavigationMapMinimumPositionY; /* 0x005a8154 */
TextContext stCurrentNavMapTxtCtx; /* 0x005a8160 */

/* Function start: 0x40E890 */
unsigned short MergeAdjacentNearHeapBlocks(int descriptorAddress)
{
    NearHeapBlock *block;

    block = DosNearPtrToFar(descriptorAddress);
    if ((block->sizeAndFlags & 0x80000000) == 0 &&
        (block[1].sizeAndFlags & 0x80000000) == 0 &&
        block->address + (block->sizeAndFlags & 0xfffff) ==
            block[1].address) {
        block[1].address = block->address;
        block[1].sizeAndFlags += block->sizeAndFlags & 0xfffff;
        while (descriptorAddress > nCurrentNearHeapFirstDescriptor) {
            descriptorAddress -= 8;
            block = DosNearPtrToFar(descriptorAddress);
            block[1].address = block->address;
            block[1].sizeAndFlags = block->sizeAndFlags;
        }
        nCurrentNearHeapFirstDescriptor += 8;
        return 1;
    }
    return 0;
}

/* Function start: 0x40E900 */
int ReleaseNearHeapBlock(int descriptorAddress)
{
    NearHeapBlock *block;
    int nextDescriptorAddress;

    block = DosNearPtrToFar(descriptorAddress);
    block->sizeAndFlags &= 0x7fffffff;
    nextDescriptorAddress = descriptorAddress + 8;
    if (nextDescriptorAddress <
            nNearHeapBase + nNearHeapSize &&
        MergeAdjacentNearHeapBlocks(descriptorAddress) != 0)
        descriptorAddress = nextDescriptorAddress;
    if (descriptorAddress > nCurrentNearHeapFirstDescriptor)
        MergeAdjacentNearHeapBlocks(descriptorAddress - 8);
    return descriptorAddress;
}

/* Function start: 0x40E950 */
void PurgeNearHeapBlocks(unsigned short flags)
{
    NearHeapBlock *block;
    int descriptorAddress;

    if (nNearHeapActive != 0) {
        if ((flags & 0x10) != 0) {
            nCurrentNearHeapFirstDescriptor = nNearHeapBase + nNearHeapSize - 8;
            block = DosNearPtrToFar(nCurrentNearHeapFirstDescriptor);
            block->address = nNearHeapBase;
            block->sizeAndFlags =
                nNearHeapSize - nNearHeapMaxDescriptors * 8;
            return;
        }
        descriptorAddress = nNearHeapBase +
                            nNearHeapSize - 8;
        for (; descriptorAddress >= nCurrentNearHeapFirstDescriptor;
             descriptorAddress -= 8) {
            block = DosNearPtrToFar(descriptorAddress);
            if ((block->sizeAndFlags & 0x40000000) == 0)
                descriptorAddress = ReleaseNearHeapBlock(descriptorAddress);
        }
    }
}

/* Function start: 0x40E9E0 */
unsigned short InitializeNearHeap(void)
{
    if (nNearHeapActive == 0) {
        nNearHeapSize = (short)GetNavRangeSentinel();
        pCurrentNearHeapAlloc = 0;
        if (nNearHeapMaxDescriptors * 8 < nNearHeapSize)
            pCurrentNearHeapAlloc =
                AllocateTaggedMemory(nNearHeapSize, 0);
        if (pCurrentNearHeapAlloc != 0) {
            nNearHeapActive++;
            nNearHeapBase =
                DosFarPtrToNear(pCurrentNearHeapAlloc);
            if (*(unsigned short *)0x00400013 == 0x270) {
                nNearHeapRelocationBytes =
                    0x9c000 - nNearHeapSize - nNearHeapBase;
                nNearHeapSize =
                    0x98000 - nNearHeapRelocationBytes - nNearHeapBase;
                if (nNearHeapMaxDescriptors * 8 <
                    nNearHeapSize) {
                    DosMemcpy(
                        DosNearPtrToFar(
                            nNearHeapBase +
                            nNearHeapSize),
                        DosNearPtrToFar(
                            0x9c000 -
                            nNearHeapRelocationBytes),
                        nNearHeapRelocationBytes);
                } else {
                    FreeIfNotNull(pCurrentNearHeapAlloc);
                    nNearHeapActive = 0;
                }
            }
            if (nNearHeapActive != 0)
                PurgeNearHeapBlocks(0x10);
        }
    }
    return nNearHeapActive;
}

/* Function start: 0x40EB10 */
/* Nothing in the shipped executable calls this. It undoes
   InitializeNearHeap: copies the relocated bytes back below 0x9C000 and
   frees the heap's allocation. */
void ShutdownNearHeap(void)
{
    int relocationBytes;

    relocationBytes = nNearHeapRelocationBytes;
    if (relocationBytes != 0)
        DosMemcpy(DosNearPtrToFar(0x9c000 - relocationBytes),
                  DosNearPtrToFar(nNearHeapBase + nNearHeapSize),
                  relocationBytes);
    if (nNearHeapActive != 0)
        FreeIfNotNull(pCurrentNearHeapAlloc);
    nNearHeapActive = 0;
}

/* Function start: 0x40EB70 */
void *AllocateNearHeapBlockFromEnd(int size, unsigned short flags)
{
    int descriptorAddress;
    int blockSize;
    NearHeapBlock *block;
    NearHeapBlock *lastBlock;
    int shiftAddress;
    int allocationAddress;
    unsigned int allocationFlags;
    unsigned short alignment;

    if (size <= 0)
        return 0;
    allocationFlags = 0x80000000;
    alignment = flags & 3;
    if (alignment == 1) {
        allocationFlags = 0x90000000;
        size++;
    } else if (alignment == 2) {
        allocationFlags = 0xa0000000;
        size += 0xf;
    }
    if ((flags & 0x10) != 0)
        allocationFlags |= 0x40000000;

    allocationAddress = 0;
    descriptorAddress =
        nNearHeapBase + nNearHeapSize - 8;
    for (; descriptorAddress >= nCurrentNearHeapFirstDescriptor;
         descriptorAddress -= 8) {
        block = DosNearPtrToFar(descriptorAddress);
        blockSize = block->sizeAndFlags & 0xfffff;
        if ((block->sizeAndFlags & 0x80000000) != 0)
            continue;
        if ((int)blockSize < size)
            continue;
        if ((int)blockSize > size) {
            if (nNearHeapBase -
                    nNearHeapMaxDescriptors * 8 +
                    nNearHeapSize >=
                nCurrentNearHeapFirstDescriptor) {
                lastBlock = DosNearPtrToFar(
                    nNearHeapBase +
                    nNearHeapSize - 8);
                if ((lastBlock->sizeAndFlags & 0x80000000) == 0) {
                    blockSize = lastBlock->sizeAndFlags & 0xfffff;
                    if (lastBlock->address + blockSize ==
                            nCurrentNearHeapFirstDescriptor &&
                        (int)blockSize > 8) {
                        lastBlock->sizeAndFlags -= 8;
                        nNearHeapMaxDescriptors++;
                    }
                }
            }

            if (nNearHeapBase -
                    nNearHeapMaxDescriptors * 8 +
                    nNearHeapSize >=
                nCurrentNearHeapFirstDescriptor)
                continue;

            nCurrentNearHeapFirstDescriptor -= 8;
            shiftAddress = nCurrentNearHeapFirstDescriptor;
            for (; descriptorAddress > shiftAddress; shiftAddress += 8) {
                block = DosNearPtrToFar(shiftAddress);
                block->address = block[1].address;
                block->sizeAndFlags = block[1].sizeAndFlags;
            }
            descriptorAddress -= 8;
            block = DosNearPtrToFar(descriptorAddress);
            blockSize = block->sizeAndFlags - size;
            block->sizeAndFlags = blockSize;
            block[1].address += blockSize & 0xffffff;
            blockSize = allocationFlags + size;
            block[1].sizeAndFlags = blockSize;
            allocationAddress = block[1].address;
            break;
        }
        block->sizeAndFlags |= allocationFlags;
        allocationAddress = block->address;
        break;
    }

    if (allocationAddress != 0) {
        if (alignment == 1)
            allocationAddress = (allocationAddress + 1) & 0xfffffffe;
        else if (alignment == 2)
            allocationAddress = (allocationAddress + 0xf) & 0xfffffff0;
    }
    return DosNearPtrToFar(allocationAddress);
}

/* Function start: 0x40ED30 */
void *AllocateNearHeapBlockByFlags(int size, unsigned short flags)
{
    int blockSize;
    int descriptorAddress;
    int shiftAddress;
    NearHeapBlock *block;
    int allocationAddress;
    unsigned int sizeAndFlags;
    unsigned int allocationFlags;
    unsigned short alignment;

    if ((flags & 0x20) != 0)
        return AllocateNearHeapBlockFromEnd(size, flags);
    if (size <= 0)
        return 0;
    allocationFlags = 0x80000000;
    alignment = flags & 3;
    if (alignment == 1) {
        allocationFlags = 0x90000000;
        size++;
    } else if (alignment == 2) {
        allocationFlags = 0xa0000000;
        size += 0xf;
    }
    if ((flags & 0x10) != 0)
        allocationFlags |= 0x40000000;

    allocationAddress = 0;
    descriptorAddress = nCurrentNearHeapFirstDescriptor;
    for (; descriptorAddress <
               nNearHeapBase + nNearHeapSize;
         descriptorAddress += 8) {
        block = DosNearPtrToFar(descriptorAddress);
        sizeAndFlags = block->sizeAndFlags;
        blockSize = sizeAndFlags & 0xfffff;
        if ((sizeAndFlags & 0x80000000) != 0)
            continue;
        if (blockSize < size)
            continue;
        if (blockSize > size) {
            if (nNearHeapBase -
                    nNearHeapMaxDescriptors * 8 +
                    nNearHeapSize >=
                nCurrentNearHeapFirstDescriptor) {
                block = DosNearPtrToFar(
                    nNearHeapBase +
                    nNearHeapSize - 8);
                if ((block->sizeAndFlags & 0x80000000) == 0) {
                    blockSize = block->sizeAndFlags & 0xfffff;
                    if (block->address + blockSize ==
                            nCurrentNearHeapFirstDescriptor &&
                        blockSize > 8) {
                        block->sizeAndFlags -= 8;
                        nNearHeapMaxDescriptors++;
                    }
                }
            }

            if (nNearHeapBase -
                    nNearHeapMaxDescriptors * 8 +
                    nNearHeapSize >=
                nCurrentNearHeapFirstDescriptor)
                continue;

            nCurrentNearHeapFirstDescriptor -= 8;
            shiftAddress = nCurrentNearHeapFirstDescriptor;
            for (; descriptorAddress > shiftAddress; shiftAddress += 8) {
                block = DosNearPtrToFar(shiftAddress);
                block->address = block[1].address;
                block->sizeAndFlags = block[1].sizeAndFlags;
            }
            descriptorAddress -= 8;
            block = DosNearPtrToFar(descriptorAddress);
            block->sizeAndFlags = allocationFlags + size;
            block[1].address += size;
            block[1].sizeAndFlags -= size;
            allocationAddress = block->address;
            break;
        }
        block->sizeAndFlags |= allocationFlags;
        allocationAddress = block->address;
        break;
    }

    if (allocationAddress != 0) {
        if (alignment == 1)
            allocationAddress = (allocationAddress + 1) & 0xfffffffe;
        else if (alignment == 2)
            allocationAddress = (allocationAddress + 0xf) & 0xfffffff0;
    }
    return DosNearPtrToFar(allocationAddress);
}

/* Function start: 0x40EF00 */
/* Nothing in the shipped executable calls this. Releases the near heap
   block that contains the given pointer. */
void FreeNearHeapBlock(void *pointer)
{
    int descriptorAddress;
    NearHeapBlock *block;
    int blockAddress;
    int address;

    address = DosFarPtrToNear(pointer);
    descriptorAddress =
        nNearHeapBase + nNearHeapSize - 8;
    for (; descriptorAddress >= nCurrentNearHeapFirstDescriptor;
         descriptorAddress -= 8) {
        block = DosNearPtrToFar(descriptorAddress);
        blockAddress = block->address;
        if (blockAddress <= address &&
            blockAddress + (int)(block->sizeAndFlags & 0xfffff) >
                address) {
            ReleaseNearHeapBlock(descriptorAddress);
            break;
        }
    }
}

/* Function start: 0x40EF60 */
/* Nothing in the shipped executable calls this. Returns the usable size of
   the near heap block that contains the given pointer, or 0. */
int GetNearHeapBlockSize(void *pointer)
{
    NearHeapBlock *block;
    int descriptorAddress;
    int size;
    int address;

    size = 0;
    address = DosFarPtrToNear(pointer);
    descriptorAddress =
        nNearHeapBase + nNearHeapSize - 8;
    for (; descriptorAddress >= nCurrentNearHeapFirstDescriptor;
         descriptorAddress -= 8) {
        block = DosNearPtrToFar(descriptorAddress);
        if (address >= block->address &&
            block->address + (int)(block->sizeAndFlags & 0xfffff) >
                address) {
            size = block->sizeAndFlags & 0xfffff;
            if ((block->sizeAndFlags & 0x10000000) != 0)
                size--;
            else if ((block->sizeAndFlags & 0x20000000) != 0)
                size -= 0xf;
            break;
        }
    }
    return size;
}
