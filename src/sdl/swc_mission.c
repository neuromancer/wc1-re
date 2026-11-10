/* Mac CMF input adapter for WC1's existing mission state and gameplay.
 * SWC LoadMissionData: CODE_03 +0x16d2. Record consumers: CODE_04
 * set_sphere_point +0x000c and Set_up_ship_info +0x1340. */
#include "wc1.h"
#include "swc.h"

#include <limits.h>
#include <string.h>

int SdlFindSwcMissionData(char *path, unsigned long capacity)
{
    SDL_RWops *file;

    if (!SdlResolvePath("CMFs/Data.CMF", path, capacity))
        return 0;
    file = SDL_RWFromFile(path, "rb");
    if (file == NULL)
        return 0;
    SDL_RWclose(file);
    return 1;
}

/* The first application fork form whose clut/251 palette loads, then any
 * renamed copy of the application; directory NULL searches the current
 * (demo) directory. */
int SdlFindSwcResources(const char *directory, char *path, unsigned long capacity)
{
    SDL_Color colors[256];
    char relative[PATH_MAX];
    size_t index;

    for (index = 0; index < SWC_APPLICATION_FORKS; index++) {
        if (directory == NULL)
            SDL_strlcpy(relative, swcApplicationForks[index], sizeof(relative));
        else if (SDL_snprintf(relative, sizeof(relative), "%s/%s", directory,
                     swcApplicationForks[index]) >= (int)sizeof(relative))
            continue;
        if (SdlResolvePath(relative, path, capacity) && SwcReadPalette(path, 251, colors) == 0)
            return 1;
    }
    return SwcFindApplicationFork(directory != NULL ? directory : ".", path, capacity);
}

/* Retail CODE_09 StartNewCampaign/MoveNewCampaign load CMPn/2. Convert
 * its 128-byte, big-endian records to the 90-byte WC1 packet consumed by
 * FullMissionScore, PlayersMissionScore and UpdateSeries. The spare leading
 * record makes UpdateSeries's terminal-series (-1) look-ahead safe. */
int SdlLoadSwcCampaignData(const char *path, SwcBuffer *campaign)
{
    SwcCmf cmf = {0};
    SwcBuffer source = {0};
    unsigned char converted[15 * 90] = {0};
    unsigned char *record;
    SDL_RWops *reader = NULL;
    int32_t fields[8];
    int32_t value;
    short word;
    int series;
    int mission;
    int field;
    int result = -1;
    char type[5];

    if ((unsigned int)nCampaignDataSet > 2)
        return SDL_SetError("Invalid SWC campaign data set");
    SDL_snprintf(type, sizeof(type), "CMP%d", nCampaignDataSet);
    if (SwcCMOpen(path, &cmf) != 0 || SwcCMGetChunk(&cmf, type, 2, &source) != 0)
        goto done;
    if (source.size < 13 * 128) {
        SDL_SetError("Truncated SWC campaign table");
        goto done;
    }
    reader = SDL_RWFromConstMem(source.data, 13 * 128);
    if (reader == NULL)
        goto done;
    for (series = 0; series < 13; series++) {
        record = converted + (series + 1) * 90;
        for (field = 0; field < 8; field++)
            fields[field] = (int32_t)SDL_ReadBE32(reader);
        if (fields[0] < 0 || fields[0] > 15 || fields[1] < 0 || fields[1] > 4 ||
            fields[2] < INT16_MIN || fields[2] > INT16_MAX ||
            fields[3] < -1 || fields[3] > 65 ||
            fields[4] < -1 || fields[4] > 13 || fields[6] < -1 || fields[6] > 13 ||
            fields[5] < -1 || fields[5] > 3 || fields[7] < -1 || fields[7] > 3) {
            SDL_SetError("Invalid SWC campaign series %d", series + 1);
            goto done;
        }
        word = fields[0];
        memcpy(record, &word, sizeof(word));
        record[2] = fields[1];
        word = fields[2];
        memcpy(record + 3, &word, sizeof(word));
        /* Retail UpdateSeries 0x01091cbc maps -1 to the system warp,
         * and 5 to a campaign transition. The host restores this selection
         * after WC1's different repeated-sequence suppression. */
        record[5] = fields[3] == -1 ? 4 : fields[3] == 5 ? 64 : fields[3];
        for (field = 4; field < 8; field++)
            record[field + 2] = fields[field];
        for (mission = 0; mission < 4; mission++) {
            for (field = 0; field < 2; field++) {
                value = (int32_t)SDL_ReadBE32(reader);
                if (value < INT16_MIN || value > INT16_MAX ||
                    (field == 0 && (value < -1 || value > 4))) {
                    SDL_SetError("Invalid SWC campaign medal");
                    goto done;
                }
                word = value;
                memcpy(record + 10 + mission * 20 + field * 2, &word, sizeof(word));
            }
            SDL_RWread(reader, record + 14 + mission * 20, 1, 16);
        }
    }
    if (campaign->size != sizeof(converted)) {
        unsigned char *allocation = SDL_realloc(campaign->data, sizeof(converted));

        if (allocation == NULL) {
            SDL_OutOfMemory();
            goto done;
        }
        campaign->data = allocation;
        campaign->size = sizeof(converted);
    }
    memcpy(campaign->data, converted, sizeof(converted));
    pMissionCampaignInfo = campaign->data + 90;
    result = 0;
done:
    if (reader != NULL)
        SDL_RWclose(reader);
    SDL_free(source.data);
    SwcCMClose(&cmf);
    return result;
}

int SdlLoadSwcMissionData(const char *path, short series, short mission)
{
    static const size_t blockSizes[6] = {24, 16 * 88, 16 * 68, 32 * 76, 40, 40};
    static const unsigned char byteFields[] = {2, 3, 5, 12};
    static const unsigned char shortFields[] = {9, 10, 11, 13, 16};
    SwcCmf cmf = {0};
    SwcBuffer chunks[6] = {{0}};
    const unsigned char *blocks[6];
    SDL_RWops *reader = NULL;
    MissionNavPoint navPoints[ACTIVE_MISSION_NAV_POINT_COUNT] = {{0}};
    MissionObjectiveSource objectives[MISSION_OBJECTIVE_COUNT] = {{0}};
    MissionShipRecord ships[ACTIVE_MISSION_SHIP_COUNT] = {{0}};
    short header[12];
    int32_t values[18];
    int32_t value;
    size_t missionIndex;
    size_t blockIndex;
    size_t offset;
    size_t chunk;
    size_t field;
    int index;
    int item;
    int leader;
    int depth;
    int objectiveEnd;
    int result;
    char type[5];

    if (series < 0 || series >= 16 || mission < 0 || mission >= 4 ||
        nCampaignDataSet < 0 || nCampaignDataSet > 2)
        return SDL_SetError("SWC mission selection is outside MOD0..2, series 0..15, mission 0..3");
    missionIndex = (size_t)series * 4 + mission;
    SDL_snprintf(type, sizeof(type), "MOD%d", nCampaignDataSet);
    if (SwcCMOpen(path, &cmf) != 0)
        return -1;
    result = -1;
    for (chunk = 0; chunk < 6; chunk++) {
        if (SwcCMGetChunk(&cmf, type, (uint32_t)chunk + 1, &chunks[chunk]) != 0)
            goto done;
        blockIndex = chunk == 5 ? (size_t)series : missionIndex;
        offset = blockIndex * blockSizes[chunk];
        if (offset > chunks[chunk].size || blockSizes[chunk] > chunks[chunk].size - offset) {
            SDL_SetError("Truncated %s/%u for series %d mission %d",
                         type, (unsigned int)chunk + 1, series, mission);
            goto done;
        }
        blocks[chunk] = chunks[chunk].data + offset;
    }
    reader = SDL_RWFromConstMem(blocks[0], blockSizes[0]);
    if (reader == NULL)
        goto done;
    for (index = 0; index < 12; index++)
        header[index] = SDL_ReadBE16(reader);
    SDL_RWclose(reader);
    reader = NULL;
    if (header[0] < 0 || header[0] >= ACTIVE_MISSION_NAV_POINT_COUNT ||
        header[1] < -1 || header[1] >= ACTIVE_MISSION_SHIP_COUNT ||
        header[2] < 0 || header[2] >= ACTIVE_MISSION_SHIP_COUNT) {
        SDL_SetError("Empty or invalid SWC mission header");
        goto done;
    }
    for (index = 3; index < 11; index++) {
        if (header[index] < -1 || header[index] >= ACTIVE_MISSION_SHIP_COUNT) {
            SDL_SetError("Invalid SWC initial mission ship");
            goto done;
        }
    }

    reader = SDL_RWFromConstMem(blocks[1], blockSizes[1]);
    if (reader == NULL)
        goto done;
    for (index = 0; index < ACTIVE_MISSION_NAV_POINT_COUNT; index++) {
        /* Mac record: name area +0x00, type +0x20, vector +0x24,
         * radius +0x30, triggers +0x34, preload types +0x3c, ships +0x44. */
        if (memchr(blocks[1] + index * 88, 0, sizeof(navPoints[index].name)) == NULL) {
            SDL_SetError("SWC nav %d name exceeds the WC1 name buffer", index);
            goto done;
        }
        SDL_RWread(reader, navPoints[index].name, 1, sizeof(navPoints[index].name));
        SDL_RWseek(reader, (Sint64)index * 88 + 32, RW_SEEK_SET);
        value = SDL_ReadBE32(reader);
        if (value < SCHAR_MIN || value > SCHAR_MAX) {
            SDL_SetError("SWC nav %d type exceeds WC1 storage", index);
            goto done;
        }
        navPoints[index].type = value;
        navPoints[index].position.x = SDL_ReadBE32(reader);
        navPoints[index].position.y = SDL_ReadBE32(reader);
        navPoints[index].position.z = SDL_ReadBE32(reader);
        value = SDL_ReadBE32(reader);
        if (value < SHRT_MIN || value > SHRT_MAX) {
            SDL_SetError("SWC nav %d radius exceeds WC1 storage", index);
            goto done;
        }
        navPoints[index].proximityRadius = value;
        SDL_RWread(reader, navPoints[index].triggers, 1, sizeof(navPoints[index].triggers));
        for (item = 0; item < 4; item++) {
            if (navPoints[index].triggers[item][0] != -1 &&
                (navPoints[index].triggers[item][1] < 0 ||
                 navPoints[index].triggers[item][1] >= ACTIVE_MISSION_NAV_POINT_COUNT)) {
                SDL_SetError("SWC nav %d has an invalid trigger target", index);
                goto done;
            }
        }
        for (item = 0; item < 2; item++) {
            value = SDL_ReadBE32(reader);
            if (value < -1 || value >= OBJECT_TYPE_COUNT) {
                SDL_SetError("SWC nav %d uses an unsupported object type %d", index, value);
                goto done;
            }
            navPoints[index].preloadObjectTypes[item] = (enum ObjectType)value;
        }
        for (item = 0; item < 10; item++) {
            value = (int16_t)SDL_ReadBE16(reader);
            if (value < -1 || value >= ACTIVE_MISSION_SHIP_COUNT) {
                SDL_SetError("SWC nav %d references an invalid mission ship", index);
                goto done;
            }
            navPoints[index].missionShips[item] = value;
        }
    }
    SDL_RWclose(reader);
    reader = SDL_RWFromConstMem(blocks[2], blockSizes[2]);
    if (reader == NULL)
        goto done;
    objectiveEnd = 0;
    for (index = 0; index < MISSION_OBJECTIVE_COUNT; index++) {
        objectives[index].type = SDL_ReadBE32(reader);
        objectives[index].index = SDL_ReadBE16(reader);
        SDL_RWread(reader, objectives[index].description, 1, sizeof(objectives[index].description));
        SDL_RWseek(reader, 2, RW_SEEK_CUR);
        if (objectives[index].type == -1)
            objectiveEnd = 1;
        if (objectiveEnd)
            continue;
        if (objectives[index].type < 0 || objectives[index].type > 4 ||
            objectives[index].index < 0 ||
            objectives[index].index >= (objectives[index].type == 0 ?
                ACTIVE_MISSION_NAV_POINT_COUNT : ACTIVE_MISSION_SHIP_COUNT) ||
            memchr(objectives[index].description, 0, sizeof(objectives[index].description)) == NULL) {
            SDL_SetError("Invalid SWC objective %d", index);
            goto done;
        }
    }
    if (!objectiveEnd) {
        SDL_SetError("SWC objective list has no terminator for WC1's objective builder");
        goto done;
    }
    SDL_RWclose(reader);
    reader = SDL_RWFromConstMem(blocks[3], blockSizes[3]);
    if (reader == NULL)
        goto done;
    for (index = 0; index < ACTIVE_MISSION_SHIP_COUNT; index++) {
        for (field = 0; field < SDL_arraysize(values); field++)
            values[field] = SDL_ReadBE32(reader);
        for (field = 0; field < SDL_arraysize(byteFields); field++) {
            value = values[byteFields[field]];
            if (value < SCHAR_MIN || value > SCHAR_MAX) {
                SDL_SetError("SWC ship %d field +0x%x exceeds WC1 byte storage",
                             index, byteFields[field] * 4);
                goto done;
            }
        }
        for (field = 0; field < SDL_arraysize(shortFields); field++) {
            value = values[shortFields[field]];
            if (value < SHRT_MIN || value > SHRT_MAX) {
                SDL_SetError("SWC ship %d field +0x%x exceeds WC1 word storage",
                             index, shortFields[field] * 4);
                goto done;
            }
        }
        ships[index].type = (enum ObjectType)values[0];
        ships[index].side = (enum Side)values[1];
        ships[index].leader = values[2];
        ships[index].field_9 = values[3];
        ships[index].missionType = (enum ShipMissionType)values[4];
        ships[index].navPoint = values[5];
        ships[index].position.x = values[6];
        ships[index].position.y = values[7];
        ships[index].position.z = values[8];
        /* WC1 calls alter_yaw with its field named pitch, just like SWC's
         * +0x24 field; preserve consumer behavior rather than swapping names. */
        ships[index].pitch = values[9];
        ships[index].yaw = values[10];
        ships[index].roll = values[11];
        ships[index].formationSpot = values[12];
        ships[index].speed = values[13];
        ships[index].rating = values[14];
        ships[index].behaviour.pilot = values[15];
        ships[index].field_2c = values[16];
        ships[index].field_2e = values[17];
        SDL_RWread(reader, &ships[index].state, 1, 1);
        SDL_RWread(reader, &ships[index].leaderMissionIndex, 1, 1);
        SDL_RWread(reader, &ships[index].formationIndex, 1, 1);
        SDL_RWread(reader, &ships[index].targetMissionIndex, 1, 1);
        if (values[0] < -1 || values[0] >= OBJECT_TYPE_COUNT ||
            (values[0] != -1 &&
             (values[1] < SIDE_IMPERIAL || values[1] > SIDE_NEUTRAL ||
              values[5] < 0 || values[5] >= ACTIVE_MISSION_NAV_POINT_COUNT ||
              values[4] < MISSION_TYPE_NONE || values[4] > MISSION_TYPE_BOGUS_AVOID_CRASH ||
              values[4] == MISSION_TYPE_CANNED_SEQUENCE))) {
            SDL_SetError("SWC ship %d uses an unsupported type, nav point or mission mode", index);
            goto done;
        }
        if (values[0] != -1 &&
            (ships[index].speed < 0 || values[15] < 0 ||
             values[15] >= (int)SDL_arraysize(anPilotTurnInterval) ||
             ships[index].leaderMissionIndex < -1 ||
             ships[index].leaderMissionIndex >= ACTIVE_MISSION_SHIP_COUNT ||
             ships[index].targetMissionIndex < -1 ||
             ships[index].targetMissionIndex >= ACTIVE_MISSION_SHIP_COUNT ||
             ships[index].formationIndex < -1 ||
             ships[index].formationIndex >= (int)SDL_arraysize(aaFormationPositions) ||
             (ships[index].formationIndex != -1 &&
              (ships[index].formationSpot < 0 ||
               ships[index].formationSpot >= (int)SDL_arraysize(aaFormationPositions[0]))) ||
             (ships[index].missionType == MISSION_TYPE_GOTO_WARP &&
              (ships[index].targetMissionIndex < 0 ||
               ships[index].targetMissionIndex >= ACTIVE_MISSION_NAV_POINT_COUNT)))) {
            SDL_SetError("SWC ship %d exceeds WC1 pilot, formation or mission-target limits", index);
            goto done;
        }
    }
    if ((int)ships[header[2]].type < 0) {
        SDL_SetError("SWC mission has no player ship");
        goto done;
    }
    for (index = 0; index < MISSION_OBJECTIVE_COUNT && objectives[index].type != -1; index++) {
        if (objectives[index].type != 0 && (int)ships[objectives[index].index].type < 0) {
            SDL_SetError("SWC objective %d references an empty ship", index);
            goto done;
        }
    }
    for (index = 0; index < ACTIVE_MISSION_SHIP_COUNT; index++) {
        if ((int)ships[index].type < 0 || ships[index].formationIndex == -1)
            continue;
        leader = index;
        depth = 0;
        while (ships[leader].leaderMissionIndex != -1 && depth < ACTIVE_MISSION_SHIP_COUNT) {
            leader = ships[leader].leaderMissionIndex;
            if ((int)ships[leader].type < 0)
                break;
            depth++;
        }
        if (depth == ACTIVE_MISSION_SHIP_COUNT || (int)ships[leader].type < 0 ||
            ships[leader].formationIndex == -1) {
            SDL_SetError("SWC ship %d has an invalid or cyclic formation leader chain", index);
            goto done;
        }
    }

    /* Publish only after every block has decoded successfully. The unchanged
     * WC1 functions consume these same structures for DOS and Saga missions. */
    stCurrentMissionHeader.entryNavPoint = header[0];
    stCurrentMissionHeader.homeMissionShip = header[1];
    stCurrentMissionHeader.playerMissionShip = header[2];
    memcpy(stCurrentMissionHeader.initialMissionShips, header + 3, sizeof(stCurrentMissionHeader.initialMissionShips));
    stCurrentMissionHeader.field_16 = header[11];
    memcpy(aMissionNavPoints, navPoints, sizeof(navPoints));
    memcpy(aMissionObjectiveSources, objectives, sizeof(objectives));
    memcpy(aMissionShips, ships, sizeof(ships));
    memcpy(abMissionAuxData, blocks[4], sizeof(abMissionAuxData));
    memcpy(abSeriesAuxData, blocks[5], sizeof(abSeriesAuxData));
    result = 0;
done:
    if (reader != NULL)
        SDL_RWclose(reader);
    for (chunk = 0; chunk < 6; chunk++)
        SDL_free(chunks[chunk].data);
    SwcCMClose(&cmf);
    return result;
}
