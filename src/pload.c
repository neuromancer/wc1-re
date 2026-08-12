/*
 *  Packet loader.
 *
 *  Address range 0x42b000-0x42b3ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN: PacketLoad prints "Library\\Source\\Pload.c PacketLoad".
 */
#include "wc1.h"

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
        default:
            if (handle.dataSize == 0) {
                g_nPacketError_00465460 = 8;
            } else {
                packet = (unsigned char *)destination;
                if (packet == 0) {
                    packet = (unsigned char *)AllocateTaggedMemory(
                        handle.dataSize,
                        (unsigned short)(flags | 0x40));
                    g_pLastPacketAllocation_005a68f0 = packet;
                    if (packet == 0)
                        g_nPacketError_00465460 = 4;
                }
                if (packet != 0) {
                    if (IsPushedPacketHandle((int)packet) == 0)
                        exit_squadron(
                            "qq PacketLoad with non-pushed dest");
                    if (ReadPacketSectionData(
                            &handle, packet, handle.dataSize) == 0)
                        packet = 0;
                }
            }
            break;
        case 1:
            SystemDebugPrintf(
                "[SYSTEM] : ERROR\n"
                "Library\\Source\\Pload.c PacketLoad  Compressed data in '%s'\n",
                filename);
            SystemDebugPrintf("[SYSTEM] : Exiting prematurely\n");
            ClearDebugPauseFlags();
            PumpMessagesDuringWait();
            _exit(0);
            packet = (unsigned char *)DecompressPacketSection(
                &handle, destination, flags, decompressionWorkspace);
            break;
        }
        CloseDataFileByHandle((unsigned short *)&handle);
    }
    return packet;
}

/* Function start: 0x42B160 */
void InitializeAudioSystem(HWND window)
{
    if (DAT_00465058 != 0 && DAT_0046a440 == 0) {
        ix_system_configure(3, (void *)1);
        ix_system_configure(0, window);
        ix_system_init();
        ix_system_set_voice_count(0x10);
        DAT_0046a440 = 1;
    }
}

/* Function start: 0x42B1B0 */
void ServiceAudioStream(void)
{
    if (DAT_00465058 != 0 && DAT_0046a440 != 0) {
        ix_system_delete_all_sounds();
        ix_system_delete_all_samples();
        ix_system_shutdown();
        FreeWaveTable();
        DAT_0046a440 = 0;
    }
}

/* Function start: 0x42B1F0 */
WaveTableEntry *AllocateWaveTableEntry(void)
{
    if (g_pWaveTableHead_0046a444 == 0) {
        g_pWaveTableHead_0046a444 =
            (WaveTableEntry *)malloc(sizeof(WaveTableEntry));
        g_pWaveTableTail_0046a448 = g_pWaveTableHead_0046a444;
    } else {
        g_pWaveTableTail_0046a448->next =
            (WaveTableEntry *)malloc(sizeof(WaveTableEntry));
        g_pWaveTableTail_0046a448 = g_pWaveTableTail_0046a448->next;
    }
    g_pWaveTableTail_0046a448->next = 0;
    return g_pWaveTableTail_0046a448;
}

/* Function start: 0x42B240 */
WaveTableEntry *FindWaveTableEntryByName(const char *name)
{
    WaveTableEntry *entry = g_pWaveTableHead_0046a444;

    while (entry != 0) {
        if (strcmp(entry->name, name) == 0)
            return entry;
        entry = entry->next;
    }
    return 0;
}

/* Function start: 0x42B290 */
void RemoveWaveTableEntry(WaveTableEntry *target)
{
    WaveTableEntry *previous = 0;
    WaveTableEntry *entry = g_pWaveTableHead_0046a444;

    while (entry != 0 && target != entry) {
        previous = entry;
        entry = entry->next;
    }
    if (entry != 0) {
        if (previous != 0)
            previous->next = entry->next;
        if (g_pWaveTableTail_0046a448 == entry && previous != 0)
            g_pWaveTableTail_0046a448 = previous;
        if (g_pWaveTableHead_0046a444 == entry)
            g_pWaveTableHead_0046a444 = entry->next;
        free(entry->name);
        free(entry);
    }
}

/* Function start: 0x42B300 */
void FreeWaveTable(void)
{
    WaveTableEntry *entry = g_pWaveTableHead_0046a444;

    while (entry != 0) {
        WaveTableEntry *next = entry->next;

        free(entry->name);
        free(entry);
        entry = next;
    }
    g_pWaveTableTail_0046a448 = 0;
    g_pWaveTableHead_0046a444 = 0;
}

/* Function start: 0x42B340 */
ActiveSoundEntry *AllocateActiveSoundEntry(void)
{
    if (g_pActiveSoundHead_0046a438 == 0) {
        g_pActiveSoundHead_0046a438 =
            (ActiveSoundEntry *)malloc(sizeof(ActiveSoundEntry));
        g_pActiveSoundTail_0046a43c = g_pActiveSoundHead_0046a438;
    } else {
        g_pActiveSoundTail_0046a43c->next =
            (ActiveSoundEntry *)malloc(sizeof(ActiveSoundEntry));
        g_pActiveSoundTail_0046a43c = g_pActiveSoundTail_0046a43c->next;
    }
    g_pActiveSoundTail_0046a43c->next = 0;
    return g_pActiveSoundTail_0046a43c;
}

/* Function start: 0x42B390 */
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

/* Function start: 0x42B3F0 */
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
