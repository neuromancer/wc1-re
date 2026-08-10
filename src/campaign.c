/*
 *  Campaign mission packet decoding.
 *
 *  Address range 0x404610-0x40609f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: auto.c ends before 0x00404610 and shipai.c begins at
 *  0x004060A0; LoadMissionDefinition is the recovered function at 0x004059B0.
 */
#include "wc1.h"

#pragma pack(push, 1)
typedef struct MissionHeaderDisk {
    short entryNavPoint;
    short homeMissionShip;
    short playerMissionShip;
    short initialMissionShips[8];
    short field_16;
} MissionHeaderDisk;

typedef struct MissionNavPointDisk {
    char name[30];
    signed char type;
    FixedVector position;
    unsigned short proximityRadius;
    signed char triggers[4][2];
    short preloadObjectTypes[2];
    short missionShips[10];
} MissionNavPointDisk;

typedef struct MissionObjectiveDisk {
    short type;
    short index;
    char description[60];
} MissionObjectiveDisk;

typedef struct MissionShipDisk {
    short type;
    short side;
    short leader;
    short missionType;
    signed char navPoint;
    FixedVector position;
    short pitch;
    short yaw;
    short roll;
    signed char formationSpot;
    short speed;
    short rating;
    short pilot;
    short field_2c;
    short field_2e;
    signed char state;
    signed char leaderMissionIndex;
    signed char formationIndex;
    signed char targetMissionIndex;
} MissionShipDisk;
#pragma pack(pop)

typedef char MissionHeaderDisk_size_must_be_0x18[
    sizeof(MissionHeaderDisk) == 0x18 ? 1 : -1];
typedef char MissionNavPointDisk_size_must_be_0x4d[
    sizeof(MissionNavPointDisk) == 0x4d ? 1 : -1];
typedef char MissionObjectiveDisk_size_must_be_0x40[
    sizeof(MissionObjectiveDisk) == 0x40 ? 1 : -1];
typedef char MissionShipDisk_size_must_be_0x2a[
    sizeof(MissionShipDisk) == 0x2a ? 1 : -1];

/* Function start: 0x4059B0 */
unsigned int LoadMissionDefinition(short series, short mission)
{
    short logicalFile;
    int missionIndex;
    unsigned char *packet;
    MissionHeaderDisk *header;
    MissionNavPointDisk *diskNav;
    MissionObjectiveDisk *diskObjective;
    MissionShipDisk *diskShip;
    MissionNavPoint *nav;
    MissionObjectiveSource *objective;
    MissionShipRecord *ship;
    short index;
    short item;

    logicalFile = g_asMissionDataFiles_00469460[g_nCampaignDataSet_005a8118];
    missionIndex = (int)mission + (int)series * 4;

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 0, 0);
    if (packet == 0)
        return 0;
    header = (MissionHeaderDisk *)(packet + missionIndex * 0x18);
    g_nMissionEntryNavPoint_005a8690 = header->entryNavPoint;
    DAT_005a8692 = header->homeMissionShip;
    g_nPlayerMissionShipIndex_005a8694 = header->playerMissionShip;
    index = 0;
    do {
        g_nInitialMissionShipIndices_005a8696[index] =
            header->initialMissionShips[index];
        index++;
    } while (index < 8);
    DAT_005a86a6 = header->field_16;
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 1, 0);
    if (packet == 0)
        return 0;
    diskNav = (MissionNavPointDisk *)(packet + missionIndex * 0x4d0);
    nav = g_aMissionNavPoints_0046c2f0;
    index = 0;
    do {
        memcpy(nav->name, diskNav->name, sizeof(nav->name));
        nav->type = diskNav->type;
        nav->position = diskNav->position;
        nav->proximityRadius = diskNav->proximityRadius;
        memcpy(nav->triggers, diskNav->triggers, sizeof(nav->triggers));
        item = 0;
        do {
            nav->preloadObjectTypes[item] =
                (enum ObjectType)diskNav->preloadObjectTypes[item];
            item++;
        } while (item < 2);
        memcpy(nav->missionShips, diskNav->missionShips,
               sizeof(nav->missionShips));
        diskNav++;
        nav++;
        index++;
    } while (index < WC1_ACTIVE_MISSION_NAV_POINT_COUNT);
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 2, 0);
    if (packet == 0)
        return 0;
    diskObjective =
        (MissionObjectiveDisk *)(packet + missionIndex * 0x400);
    objective = g_aMissionObjectiveSources_005a8270;
    index = 0;
    do {
        objective->type = diskObjective->type;
        objective->index = diskObjective->index;
        memcpy(objective->description, diskObjective->description,
               sizeof(objective->description));
        diskObjective++;
        objective++;
        index++;
    } while (index < WC1_MISSION_OBJECTIVE_COUNT);
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 3, 0);
    if (packet == 0)
        return 0;
    diskShip = (MissionShipDisk *)(packet + missionIndex * 0x540);
    ship = g_aMissionShips_0046c948;
    index = 0;
    do {
        ship->type = (enum ObjectType)diskShip->type;
        ship->side = (enum Side)diskShip->side;
        ship->leader = diskShip->leader;
        ship->missionType = (enum ShipMissionType)diskShip->missionType;
        ship->navPoint = diskShip->navPoint;
        ship->position = diskShip->position;
        ship->pitch = diskShip->pitch;
        ship->yaw = diskShip->yaw;
        ship->roll = diskShip->roll;
        ship->formationSpot = diskShip->formationSpot;
        ship->speed = diskShip->speed;
        ship->rating = diskShip->rating;
        ship->behaviour.pilot = diskShip->pilot;
        ship->field_2c = diskShip->field_2c;
        ship->field_2e = diskShip->field_2e;
        ship->state = diskShip->state;
        ship->leaderMissionIndex = diskShip->leaderMissionIndex;
        ship->formationIndex = diskShip->formationIndex;
        ship->targetMissionIndex = diskShip->targetMissionIndex;
        diskShip++;
        ship++;
        index++;
    } while (index < 32);
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 4, 0);
    if (packet == 0)
        return 0;
    memcpy(g_abMissionAuxData_005a8218,
           packet + missionIndex * sizeof(g_abMissionAuxData_005a8218),
           sizeof(g_abMissionAuxData_005a8218));
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 5, 0);
    if (packet == 0)
        return 0;
    memcpy(g_abSeriesAuxData_005a8240,
           packet + series * sizeof(g_abSeriesAuxData_005a8240),
           sizeof(g_abSeriesAuxData_005a8240));
    ReleasePacketHandle((int)packet);
    return 0;
}
