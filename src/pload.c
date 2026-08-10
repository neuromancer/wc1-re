/*
 *  Packet loader.
 *
 *  Address range 0x42b000-0x42b3ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN: PacketLoad prints "Library\\Source\\Pload.c PacketLoad".
 */
#include "wc1.h"

/* Function start: 0x42B050 */
void *PacketLoad(const char *filename, short section, void *destination,
                 unsigned short flags)
{
    FILE *file;
    unsigned char *packet;
    unsigned int declaredSize;
    unsigned int firstEntry;
    unsigned int startEntry;
    unsigned int endEntry;
    unsigned int start;
    unsigned int end;
    unsigned int size;
    short sectionCount;
    int allocated;

    DAT_00465460 = 0;
    file = fopen(filename, "rb");
    if (file == 0) {
        DAT_00465460 = 1;
        return 0;
    }
    if (fread(&declaredSize, sizeof(declaredSize), 1, file) != 1 ||
        fread(&firstEntry, sizeof(firstEntry), 1, file) != 1) {
        fclose(file);
        DAT_00465460 = 2;
        return 0;
    }
    start = firstEntry & 0x00ffffff;
    if (start < 8 || (start & 3) != 0) {
        fclose(file);
        DAT_00465460 = 3;
        return 0;
    }
    sectionCount = (short)(start / 4 - 1);
    if (section < 0 || section >= sectionCount) {
        fclose(file);
        DAT_00465460 = 3;
        return 0;
    }
    if (fseek(file, 4 + (long)section * 4, SEEK_SET) != 0 ||
        fread(&startEntry, sizeof(startEntry), 1, file) != 1) {
        fclose(file);
        DAT_00465460 = 2;
        return 0;
    }
    if (section + 1 < sectionCount) {
        if (fread(&endEntry, sizeof(endEntry), 1, file) != 1) {
            fclose(file);
            DAT_00465460 = 2;
            return 0;
        }
    } else {
        endEntry = declaredSize;
    }
    start = startEntry & 0x00ffffff;
    end = endEntry & 0x00ffffff;
    if (start > end || end > declaredSize) {
        fclose(file);
        DAT_00465460 = 3;
        return 0;
    }
    size = end - start;
    if (size == 0) {
        fclose(file);
        DAT_00465460 = 8;
        return 0;
    }

    allocated = destination == 0;
    packet = (unsigned char *)destination;
    if (allocated)
        packet = (unsigned char *)AllocateTaggedMemory(
            size, (unsigned short)(flags | 0x40));
    if (packet == 0) {
        fclose(file);
        DAT_00465460 = 4;
        return 0;
    }
    if ((startEntry >> 24) == 1 ||
        fseek(file, start, SEEK_SET) != 0 ||
        fread(packet, 1, size, file) != size) {
        fclose(file);
        if (allocated)
            ReleasePacketHandle((int)packet);
        DAT_00465460 = 2;
        return 0;
    }
    fclose(file);
    DAT_00465460 = 0;
    return packet;
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
    ActiveSoundEntry *previous = 0;
    ActiveSoundEntry *entry = g_pActiveSoundHead_0046a438;

    while (entry != 0 && target != entry) {
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
