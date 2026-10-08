/* Exercise the Mac input adapter through WC1's real mission consumers. */
#include "wc1.h"
#include "swc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, \
                #expression, SDL_GetError()); \
        exit(1); \
    } \
} while (0)

static void WriteFixture(const char *path, const SwcBuffer *fixture)
{
    SDL_RWops *file;

    file = SDL_RWFromFile(path, "wb");
    CHECK(file != NULL);
    CHECK(SDL_RWwrite(file, fixture->data, 1, fixture->size) == fixture->size);
    CHECK(SDL_RWclose(file) == 0);
}

static SwcBuffer CreateFixture(size_t offsets[6])
{
    const size_t strides[] = {24, 1408, 1088, 2432, 40, 40};
    const char *const names[] = {"Carrier", "Nav test"};
    SwcBuffer fixture;
    SDL_RWops *writer;
    size_t offset;
    size_t toc;
    int chunk;
    int index;
    int field;

    offset = 28;
    for (chunk = 0; chunk < 6; chunk++) {
        offsets[chunk] = offset;
        offset += strides[chunk] * (chunk == 5 ? 2 : 5);
    }
    toc = offset;
    fixture.size = toc + 8 + 4 + 8 + 6 * 16 + 4;
    fixture.data = SDL_calloc(fixture.size, 1);
    CHECK(fixture.data != NULL);
    writer = SDL_RWFromMem(fixture.data, (int)fixture.size);
    CHECK(writer != NULL);
    SDL_RWwrite(writer, "CMF1", 1, 4);
    SDL_WriteLE32(writer, 0x10000);
    SDL_WriteLE32(writer, 28);
    SDL_WriteLE32(writer, (Uint32)toc);
    SDL_WriteLE32(writer, (Uint32)(fixture.size - toc));
    SDL_RWseek(writer, (Sint64)toc, RW_SEEK_SET);
    SDL_WriteLE32(writer, 8);
    SDL_WriteLE32(writer, (Uint32)(fixture.size - toc - 4));
    SDL_WriteLE32(writer, 1);
    SDL_RWwrite(writer, "MOD0", 1, 4);
    SDL_WriteLE32(writer, 6);
    for (chunk = 0; chunk < 6; chunk++) {
        SDL_WriteLE32(writer, chunk + 1);
        SDL_WriteLE32(writer, (Uint32)offsets[chunk]);
        SDL_WriteLE32(writer, (Uint32)(strides[chunk] * (chunk == 5 ? 2 : 5)));
        SDL_WriteLE32(writer, 0);
        offsets[chunk] += strides[chunk] * (chunk == 5 ? 1 : 4);
    }
    SDL_WriteLE32(writer, 0);
    SDL_RWseek(writer, (Sint64)offsets[0], RW_SEEK_SET);
    SDL_WriteBE16(writer, 0);
    SDL_WriteBE16(writer, 0);
    SDL_WriteBE16(writer, 1);
    SDL_WriteBE16(writer, 2);
    for (index = 0; index < 7; index++)
        SDL_WriteBE16(writer, 0xffff);
    SDL_WriteBE16(writer, 17);
    for (index = 0; index < 16; index++) {
        SDL_RWseek(writer, (Sint64)(offsets[1] + index * 88), RW_SEEK_SET);
        if (index < 2)
            SDL_RWwrite(writer, names[index], 1, strlen(names[index]));
        SDL_RWseek(writer, (Sint64)(offsets[1] + index * 88 + 32), RW_SEEK_SET);
        SDL_WriteBE32(writer, index < 2 ? 1 : UINT32_MAX);
        SDL_WriteBE32(writer, index == 1 ? 30000 * 256 : 0);
        SDL_WriteBE32(writer, index == 1 ? (Uint32)(-5000 * 256) : 0);
        SDL_WriteBE32(writer, index == 1 ? 15000 * 256 : 0);
        SDL_WriteBE32(writer, 15000);
        for (field = 0; field < 4; field++)
            SDL_WriteBE16(writer, 0xff00);
        SDL_WriteBE32(writer, UINT32_MAX);
        SDL_WriteBE32(writer, UINT32_MAX);
        for (field = 0; field < 10; field++)
            SDL_WriteBE16(writer, 0xffff);
    }
    for (index = 0; index < 16; index++) {
        SDL_RWseek(writer, (Sint64)(offsets[2] + index * 68), RW_SEEK_SET);
        SDL_WriteBE32(writer, index < 2 ? (Uint32)index : UINT32_MAX);
        SDL_WriteBE16(writer, index == 0 ? 1 : 0);
        SDL_RWwrite(writer, "Objective", 1, 9);
        SDL_RWseek(writer, (Sint64)(offsets[2] + index * 68 + 66), RW_SEEK_SET);
        SDL_WriteBE16(writer, 0xaabb);
    }
    for (index = 0; index < 32; index++) {
        SDL_RWseek(writer, (Sint64)(offsets[3] + index * 76), RW_SEEK_SET);
        SDL_WriteBE32(writer, index == 0 ? OBJECT_TYPE_TIGERS_CLAW :
                               index < 3 ? OBJECT_TYPE_HORNET : UINT32_MAX);
        SDL_WriteBE32(writer, SIDE_IMPERIAL);
        SDL_WriteBE32(writer, UINT32_MAX);
        SDL_WriteBE32(writer, UINT32_MAX);
        SDL_WriteBE32(writer, MISSION_TYPE_PATROL);
        SDL_WriteBE32(writer, index == 2 ? 1 : 0);
        SDL_WriteBE32(writer, 256);
        SDL_WriteBE32(writer, (Uint32)-512);
        SDL_WriteBE32(writer, 768);
        SDL_WriteBE32(writer, (Uint32)-300);
        SDL_WriteBE32(writer, 301);
        SDL_WriteBE32(writer, 255);
        SDL_WriteBE32(writer, 2);
        SDL_WriteBE32(writer, 20);
        SDL_WriteBE32(writer, 0x12345);
        SDL_WriteBE32(writer, 13);
        SDL_WriteBE32(writer, (Uint32)-123);
        SDL_WriteBE32(writer, 0x12345678);
        SDL_WriteBE32(writer, 0x00ffff00);
    }
    SDL_RWseek(writer, (Sint64)offsets[4], RW_SEEK_SET);
    SDL_RWwrite(writer, "Alpha Wing", 1, 10);
    SDL_RWseek(writer, (Sint64)offsets[5], RW_SEEK_SET);
    SDL_RWwrite(writer, "Enyo", 1, 4);
    SDL_RWclose(writer);
    return fixture;
}

static void CheckFailurePreservesMission(const char *path, const SwcBuffer *fixture)
{
    unsigned char nav[sizeof(aMissionNavPoints)];
    unsigned char ships[sizeof(aMissionShips)];
    unsigned char objectives[sizeof(aMissionObjectiveSources)];
    unsigned char missionAux[sizeof(abMissionAuxData)];
    unsigned char seriesAux[sizeof(abSeriesAuxData)];
    short initial[8];
    short entry;
    short home;
    short player;
    short extra;

    memcpy(nav, aMissionNavPoints, sizeof(nav));
    memcpy(ships, aMissionShips, sizeof(ships));
    memcpy(objectives, aMissionObjectiveSources, sizeof(objectives));
    memcpy(missionAux, abMissionAuxData, sizeof(missionAux));
    memcpy(seriesAux, abSeriesAuxData, sizeof(seriesAux));
    memcpy(initial, nInitialMissionShipIndices, sizeof(initial));
    entry = nMissionEntryNavPoint;
    home = nHomeMissionShipIndex;
    player = nPlayerMissionShipIndex;
    extra = DAT_005a86a6;
    WriteFixture(path, fixture);
    CHECK(SdlLoadSwcMissionData(path, 1, 0) != 0);
    CHECK(memcmp(nav, aMissionNavPoints, sizeof(nav)) == 0);
    CHECK(memcmp(ships, aMissionShips, sizeof(ships)) == 0);
    CHECK(memcmp(objectives, aMissionObjectiveSources, sizeof(objectives)) == 0);
    CHECK(memcmp(missionAux, abMissionAuxData, sizeof(missionAux)) == 0);
    CHECK(memcmp(seriesAux, abSeriesAuxData, sizeof(seriesAux)) == 0);
    CHECK(memcmp(initial, nInitialMissionShipIndices, sizeof(initial)) == 0);
    CHECK(nMissionEntryNavPoint == entry && nHomeMissionShipIndex == home);
    CHECK(nPlayerMissionShipIndex == player && DAT_005a86a6 == extra);
}

static void TestMissionAdapter(const char *path)
{
    SwcBuffer fixture;
    size_t offsets[6];
    uint8_t *saved;
    SDL_RWops *writer;
    FixedVector point;
    int index;

    fixture = CreateFixture(offsets);
    saved = SDL_malloc(fixture.size);
    CHECK(saved != NULL);
    memcpy(saved, fixture.data, fixture.size);
    WriteFixture(path, &fixture);
    nCampaignDataSet = 0;
    CHECK(SdlLoadSwcMissionData(path, 1, 0) == 0);
    CHECK(nMissionEntryNavPoint == 0 && nHomeMissionShipIndex == 0 && nPlayerMissionShipIndex == 1);
    CHECK(nInitialMissionShipIndices[0] == 2 && nInitialMissionShipIndices[7] == -1);
    CHECK(DAT_005a86a6 == 17);
    CHECK(strcmp(aMissionNavPoints[1].name, "Nav test") == 0);
    CHECK(aMissionNavPoints[1].position.x == 30000 * 256);
    CHECK(aMissionNavPoints[1].position.y == -5000 * 256);
    CHECK(aMissionNavPoints[1].proximityRadius == 15000);
    CHECK(aMissionNavPoints[1].triggers[0][0] == -1);
    CHECK(aMissionNavPoints[1].missionShips[0] == -1);
    CHECK(aMissionObjectiveSources[1].type == 1 && aMissionObjectiveSources[1].index == 0);
    CHECK(aMissionObjectiveSources[2].type == -1);
    CHECK(aMissionShips[1].leader == -1 && aMissionShips[1].field_9 == -1);
    CHECK(aMissionShips[1].pitch == -300 && aMissionShips[1].yaw == 301 && aMissionShips[1].roll == 255);
    CHECK(aMissionShips[1].speed == 20 && aMissionShips[1].formationSpot == 2);
    CHECK(aMissionShips[1].rating == 0x12345 && aMissionShips[1].behaviour.pilot == 13);
    CHECK(aMissionShips[1].field_2c == -123 && aMissionShips[1].field_2e == 0x12345678);
    CHECK(aMissionShips[1].leaderMissionIndex == -1 && aMissionShips[1].formationIndex == -1);
    CHECK(strcmp((char *)abMissionAuxData, "Alpha Wing") == 0);
    CHECK(strcmp((char *)abSeriesAuxData, "Enyo") == 0);
    set_sphere_point(&aMissionShips[2], &point);
    CHECK(point.x == 30001 * 256 && point.y == -5002 * 256 && point.z == 15003 * 256);
    CHECK(is_team_member(1) && is_team_member(2) && !is_team_member(3));
    Build_objective_list();
    CHECK(cMissionObjectiveCount == 2 && aMissionObjectives[2].type == -1);
    CHECK(aMissionObjectives[0].mapX == 300 && aMissionObjectives[0].mapY == 150);
    CHECK(aMissionObjectives[1].mapX == 0 && aMissionObjectives[1].mapY == 0);
    CHECK(strcmp(aMissionObjectives[1].displayName, "Tiger's Claw") == 0);
    CHECK(cycle_next_objective() && cCurrentObjective == 1);
    CHECK(cycle_next_objective() && cCurrentObjective == 0);
    SetScale();
    CHECK(nNavMapScale > 0);
    nav_getxy(&aMissionObjectives[1].mapX, &aMissionObjectives[1].mapY, 0, 0);
    CHECK(aMissionObjectives[1].mapX == 25 && aMissionObjectives[1].mapY == 92);
    CHECK(SdlLoadSwcMissionData(path, -1, 0) != 0);
    CHECK(SdlLoadSwcMissionData(path, 16, 0) != 0);
    CHECK(SdlLoadSwcMissionData(path, 0, 4) != 0);
    nCampaignDataSet = 3;
    CHECK(SdlLoadSwcMissionData(path, 1, 0) != 0);
    nCampaignDataSet = 0;

    writer = SDL_RWFromMem(fixture.data, (int)fixture.size);
    CHECK(writer != NULL);
    SDL_RWseek(writer, (Sint64)(offsets[0] + 4), RW_SEEK_SET);
    SDL_WriteBE16(writer, 0xffff);
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    SDL_RWseek(writer, (Sint64)(offsets[1] + 48), RW_SEEK_SET);
    SDL_WriteBE32(writer, 40000);
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    SDL_RWseek(writer, (Sint64)(offsets[3] + 76 + 52), RW_SEEK_SET);
    SDL_WriteBE32(writer, 40000);
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    SDL_RWseek(writer, (Sint64)(offsets[3] + 76), RW_SEEK_SET);
    SDL_WriteBE32(writer, 82);
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    SDL_RWseek(writer, (Sint64)(offsets[3] + 76 + 60), RW_SEEK_SET);
    SDL_WriteBE32(writer, 18);
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    SDL_RWseek(writer, (Sint64)(offsets[3] + 76 + 73), RW_SEEK_SET);
    SDL_WriteBE16(writer, 0x0100);
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    SDL_RWseek(writer, (Sint64)(offsets[1] + 52), RW_SEEK_SET);
    SDL_WriteBE16(writer, 0x0110);
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    for (index = 0; index < 16; index++) {
        SDL_RWseek(writer, (Sint64)(offsets[2] + index * 68), RW_SEEK_SET);
        SDL_WriteBE32(writer, 0);
    }
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    SDL_RWseek(writer, (Sint64)(offsets[2] + 4), RW_SEEK_SET);
    SDL_WriteBE16(writer, 16);
    CheckFailurePreservesMission(path, &fixture);
    memcpy(fixture.data, saved, fixture.size);
    SDL_RWseek(writer, (Sint64)(fixture.size - 12), RW_SEEK_SET);
    SDL_WriteLE32(writer, 39);
    CheckFailurePreservesMission(path, &fixture);
    SDL_RWclose(writer);
    SDL_free(saved);
    SDL_free(fixture.data);
    CHECK(remove(path) == 0);
    puts("SWC mission adapter: endian/layout, WC1 field widths, references and atomic failure checks passed.");
}

static void TestDemoMission(const char *directory)
{
    const short mapPoints[4][2] = {{300, 150}, {450, -200}, {50, -450}, {0, 0}};
    FixedVector player;
    int index;

    CHECK(SdlChangeDirectory(directory) == 0);
    nCampaignDataSet = 0;
    CHECK(LoadMissionData(1, 0) == 0);
    CHECK(nMissionEntryNavPoint == 0 && nPlayerMissionShipIndex == 1);
    CHECK(aMissionShips[0].type == OBJECT_TYPE_TIGERS_CLAW);
    CHECK(aMissionShips[1].type == OBJECT_TYPE_HORNET);
    CHECK(strcmp(aMissionNavPoints[0].name, "Tiger's Claw") == 0);
    CHECK(strcmp((char *)abMissionAuxData, "Alpha Wing") == 0);
    CHECK(strcmp((char *)abSeriesAuxData, "Enyo") == 0);
    set_sphere_point(&aMissionShips[1], &player);
    CHECK(player.x == 0 && player.y == 0 && player.z == -1500 * 256);
    nCurrentNavPoint = nMissionEntryNavPoint;
    init_ijk(0);
    Set_up_ship_info(0, nPlayerMissionShipIndex, -1);
    CHECK(memcmp(&aShipPosition[0], &player, sizeof(player)) == 0);
    CHECK(anShipSpeed[0] == 20 * 256);
    CHECK(aiPilotLevel[0] == 13 && nShipMissionIndices[0] == 1);
    Build_objective_list();
    CHECK(cMissionObjectiveCount == 4);
    CHECK(aMissionObjectives[4].type == -1 && abFlightPath[4] == -1);
    for (index = 0; index < 4; index++) {
        CHECK(aMissionObjectives[index].mapX == mapPoints[index][0]);
        CHECK(aMissionObjectives[index].mapY == mapPoints[index][1]);
        CHECK(cCurrentObjective == index);
        printf("  %d: %s (%s), WC1 map=(%d,%d)\n", index,
               aMissionObjectives[index].name, aMissionObjectives[index].displayName,
               aMissionObjectives[index].mapX, aMissionObjectives[index].mapY);
        CHECK(cycle_next_objective());
    }
    CHECK(cCurrentObjective == 0);
    SetScale();
    CHECK(nNavMapScale > 0);
    puts("SWC Enyo 1: existing WC1 mission loader, ship setup, objective builder and navigation checks passed.");
}

int main(int argc, char **argv)
{
    CHECK(argc == 2 || argc == 3);
    TestMissionAdapter(argv[1]);
    if (argc == 3)
        TestDemoMission(argv[2]);
    return 0;
}
