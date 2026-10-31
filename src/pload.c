/*
 *  Packet loader.
 *
 *  Address range 42b050-42b15f (see docs/ORDER.md).
 *  Boundary evidence: PROVEN: PacketLoad prints "Library\\Source\\Pload.c PacketLoad".
 */
#include "wc1.h"

/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned char *pTgtLockShapePtr; /* 0x005a6bf4 */
unsigned char *apTextFonts[4]; /* 0x005a6c00 */
FontWorkspace **apFontWorkspaces[4]; /* 0x005a6c10 */
FixedVector aDirectionViewRightVector[DIRECTION_VIEW_COUNT]; /* 0x005a6c20 */
FixedVector aDirectionViewUpVectorArray[DIRECTION_VIEW_COUNT]; /* 0x005a6f10 */
FixedVector aDirectionViewForwardVector[DIRECTION_VIEW_COUNT]; /* 0x005a7200 */

/* Function start: 0x42B050 */
void * __stdcall PacketLoad(const char *filename, short section,
                            void *destination, unsigned short flags,
                            void *decompressionWorkspace)
{
    unsigned char *packet;
    PacketSectionHandle handle;
    int compression;

    packet = 0;
    if (OpenPacketSection(filename, section, &handle) != 0) {
        compression = handle.compression;
        switch (compression) {
        case 1:
#ifdef SDL_PORT
        {
            unsigned char sizeBytes[4];
            unsigned char *compressedData;
            unsigned int compressedSize;
            unsigned int outputSize;
            size_t writtenSize;
            int allocatedPacket;

            if (handle.dataSize < 4 ||
                ReadDataFileAtOffset((unsigned short)handle.file,
                                     (int)handle.dataOffset, 4,
                                     sizeBytes) == 0) {
                nPacketError = 6;
                break;
            }
            outputSize = (unsigned int)sizeBytes[0] |
                ((unsigned int)sizeBytes[1] << 8) |
                ((unsigned int)sizeBytes[2] << 16) |
                ((unsigned int)sizeBytes[3] << 24);
            compressedSize = handle.dataSize - 4;
            compressedData = (unsigned char *)malloc(
                compressedSize != 0 ? compressedSize : 1);
            if (compressedData == 0) {
                nPacketError = 1;
                break;
            }
            handle.position = 4;
            if (ReadPacketSectionData(&handle, compressedData,
                                      compressedSize) == 0) {
                free(compressedData);
                packet = 0;
                break;
            }

            allocatedPacket = destination == 0;
            packet = destination;
            if (packet == 0)
                packet = AllocateTaggedMemory(outputSize, flags);
            pLastPacketAllocation = packet;
            if (packet == 0) {
                nPacketError = 4;
            } else if (!SdlDecompressOriginLzw(
                           compressedData, compressedSize, packet,
                           outputSize, &writtenSize)) {
                if (allocatedPacket != 0)
                    ReleasePacketHandle(packet);
                packet = 0;
                pLastPacketAllocation = 0;
                nPacketError = 6;
            }
            free(compressedData);
            break;
        }
#else
            SystemDebugPrintf(
                "[SYSTEM] : ERROR\n"
                "Library\\Source\\Pload.c PacketLoad  Compressed data in '%s'\n",
                filename);
            SystemDebugPrintf("[SYSTEM] : Exiting prematurely\n");
            ClearDebugPauseFlags();
            PumpMessagesDuringWait();
            exit(0);
            packet = DecompressPacketSection(
                &handle, destination, flags, decompressionWorkspace);
            break;
#endif
        default:
            if (handle.dataSize == 0) {
                nPacketError = 8;
            } else {
                packet = destination;
                if (packet == 0) {
                    packet = AllocateTaggedMemory(
                        handle.dataSize,
                        (unsigned short)(flags | 0x40));
                    pLastPacketAllocation = packet;
                    if (packet == 0)
                        nPacketError = 4;
                }
                if (packet != 0) {
                    if (IsPushedPacketHandle(packet) == 0)
                        exit_squadron(
                            "qq PacketLoad with non-pushed dest");
                    if (ReadPacketSectionData(
                            &handle, packet, handle.dataSize) == 0)
                        packet = 0;
                }
            }
            break;
        }
        CloseDataFileByHandle(&handle);
    }
    return packet;
}
