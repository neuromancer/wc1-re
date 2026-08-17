/*
 *  Packet loader.
 *
 *  Address range 0x42b000-0x42b3ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN: PacketLoad prints "Library\\Source\\Pload.c PacketLoad".
 */
#include "wc1.h"

#pragma function(strcmp)

/* Function start: 0x446560 */
short LoadGraphicsDriver(short rewritePacketExtensions)
{
    (void)rewritePacketExtensions;
    return 1;
}

/* Function start: 0x4465A0 */
void * __stdcall PacketLoad(const char *filename, short section,
                            void *destination, unsigned short flags,
                            void *decompressionWorkspace,
                            short registerHandle)
{
    unsigned char *packet;
    PacketSectionHandle handle;
    int compression;

    packet = 0;
    if (OpenPacketSection(filename, section, &handle) != 0) {
        compression = handle.compression;
        switch (compression) {
        default:
            if (handle.dataSize == 0) {
                g_nPacketError_0049ca90 = 8;
            } else {
                packet = destination;
                if (packet == 0) {
                    packet = AllocateTaggedMemory(
                        handle.dataSize,
                        (unsigned short)(flags | 0x40));
                    g_pLastPacketAllocation_005a68f0 = packet;
                    if (packet == 0)
                        g_nPacketError_0049ca90 = 4;
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
        case 1:
#ifdef WC1_SDL
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
                g_nPacketError_0049ca90 = 6;
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
                g_nPacketError_0049ca90 = 1;
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
            g_pLastPacketAllocation_005a68f0 = packet;
            if (packet == 0) {
                g_nPacketError_0049ca90 = 4;
            } else if (!Wc1SdlDecompressOriginLzw(
                           compressedData, compressedSize, packet,
                           outputSize, &writtenSize)) {
                if (allocatedPacket != 0)
                    ReleasePacketHandle(packet);
                packet = 0;
                g_pLastPacketAllocation_005a68f0 = 0;
                g_nPacketError_0049ca90 = 6;
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
            _exit(0);
            packet = DecompressPacketSection(
                &handle, destination, flags, decompressionWorkspace);
            break;
#endif
        }
        CloseDataFileByHandle((unsigned short *)&handle);
    }
    return packet;
}

/* Function start: 0x423CA0 */
void InitializeAudioSystem(HWND window)
{
    if (g_nAudioEnabled_0049c244 != 0 && DAT_0046a440 == 0) {
        ix_system_configure(3, (void *)1);
        ix_system_configure(0, window);
        ix_system_init();
        ix_system_set_voice_count(0x10);
        DAT_0046a440 = 1;
    }
}

/* Function start: 0x423D02 */
void ServiceAudioStream(void)
{
    if (g_nAudioEnabled_0049c244 != 0) {
        if (DAT_0046a440 != 0) {
            ix_system_delete_all_sounds();
            ix_system_delete_all_samples();
            ix_system_shutdown();
            FreeWaveTable();
            DAT_0046a440 = 0;
        }
        return;
    }
}

/* Function start: 0x423D4F */
WaveTableEntry *AllocateWaveTableEntry(void)
{
    if (g_pWaveTableHead_004961b4 == 0) {
        g_pWaveTableHead_004961b4 =
            malloc(sizeof(WaveTableEntry));
        g_pWaveTableTail_004961b8 = g_pWaveTableHead_004961b4;
    } else {
        g_pWaveTableTail_004961b8->next =
            malloc(sizeof(WaveTableEntry));
        g_pWaveTableTail_004961b8 = g_pWaveTableTail_004961b8->next;
    }
    g_pWaveTableTail_004961b8->next = 0;
    return g_pWaveTableTail_004961b8;
}

/* Function start: 0x423DBB */
WaveTableEntry *FindWaveTableEntryByName(const char *name)
{
    WaveTableEntry *entry = g_pWaveTableHead_004961b4;

    while (entry != 0) {
        if (strcmp(entry->name, name) == 0)
            return entry;
        entry = entry->next;
    }
    return 0;
}

/* Function start: 0x423E12 */
void RemoveWaveTableEntry(WaveTableEntry *target)
{
    WaveTableEntry *previous = 0;
    WaveTableEntry *entry = g_pWaveTableHead_004961b4;

    while (entry != 0 && target != entry) {
        previous = entry;
        entry = entry->next;
    }
    if (entry != 0) {
        if (previous != 0)
            previous->next = entry->next;
        if (g_pWaveTableTail_004961b8 == entry && previous != 0)
            g_pWaveTableTail_004961b8 = previous;
        if (g_pWaveTableHead_004961b4 == entry)
            g_pWaveTableHead_004961b4 = entry->next;
        free(entry->name);
        free(entry);
    }
}

/* Function start: 0x423ED1 */
void FreeWaveTable(void)
{
    WaveTableEntry *entry = g_pWaveTableHead_004961b4;

    while (entry != 0) {
        WaveTableEntry *next = entry->next;

        free(entry->name);
        free(entry);
        entry = next;
    }
    g_pWaveTableTail_004961b8 = 0;
    g_pWaveTableHead_004961b4 = 0;
}

/* Function start: 0x423F3F */
ActiveSoundEntry *AllocateActiveSoundEntry(void)
{
    if (g_pActiveSoundHead_0046a438 == 0) {
        g_pActiveSoundHead_0046a438 =
            malloc(sizeof(ActiveSoundEntry));
        g_pActiveSoundTail_0046a43c = g_pActiveSoundHead_0046a438;
    } else {
        g_pActiveSoundTail_0046a43c->next =
            malloc(sizeof(ActiveSoundEntry));
        g_pActiveSoundTail_0046a43c = g_pActiveSoundTail_0046a43c->next;
    }
    g_pActiveSoundTail_0046a43c->next = 0;
    return g_pActiveSoundTail_0046a43c;
}

/* Function start: 0x423FAB */
void RemoveActiveSoundEntry(ActiveSoundEntry *target)
{
    ActiveSoundEntry *entry = g_pActiveSoundHead_0046a438;
    ActiveSoundEntry *previous = 0;

    while (entry != 0 && entry != target) {
        previous = entry;
        entry = entry->next;
    }
    if (entry != 0) {
        if (previous != 0)
            previous->next = entry->next;
        if (g_pActiveSoundTail_0046a43c == entry && previous != 0)
            g_pActiveSoundTail_0046a43c = previous;
        if (g_pActiveSoundHead_0046a438 == entry)
            g_pActiveSoundHead_0046a438 = entry->next;
        free(entry);
    }
}

/* Function start: WC2_UNMAPPED */
ActiveSoundEntry *FindActiveSoundEntryBySample(IxSample *sample)
{
    ActiveSoundEntry *entry = g_pActiveSoundHead_0046a438;

    for (;;) {
        if (entry == 0)
            return 0;
        if (entry->sound->sample == sample)
            return entry;
        entry = entry->next;
    }
}
