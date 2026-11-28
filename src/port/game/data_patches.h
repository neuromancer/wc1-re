/* Corrections to the game's data, made as it loads (data_patches.c). */
#ifndef SDL_PORT_DATA_PATCHES_H
#define SDL_PORT_DATA_PATCHES_H

#include <stddef.h>

struct PacketSectionHandle;

#ifdef __cplusplus
extern "C" {
#endif

int SdlPatchPacketSection(const char *filename, short section,
                         struct PacketSectionHandle *handle);
void SdlPatchStreamData(const char *filename, unsigned int fileOffset,
                        void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_DATA_PATCHES_H */
