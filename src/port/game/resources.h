/* Game data: installations, packets and decompression (resources.c). */
#ifndef SDL_PORT_RESOURCES_H
#define SDL_PORT_RESOURCES_H

#include <stddef.h>

struct DiskFileRecord;

#ifdef __cplusplus
extern "C" {
#endif

int SdlUsingDosData(void);
int SdlHasSecretMissions(short campaign);
int SdlDecompressOriginLzw(const unsigned char *source,
                              size_t sourceSize,
                              unsigned char *destination,
                              size_t destinationSize,
                              size_t *writtenSize);
int SdlExtractOriginPacketSection(const unsigned char *archive,
                                     size_t archiveSize,
                                     unsigned int sectionIndex,
                                     unsigned char **section,
                                     size_t *sectionSize);
void SdlCompleteDosInstallTable(struct DiskFileRecord *records);
void SdlSelectSm2Files(struct DiskFileRecord *records);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_RESOURCES_H */
