#include "wc1.h"

typedef struct Wc1SdlRegistryKey {
    DWORD musicVolume;
    DWORD soundEffectsVolume;
    DWORD cheater;
    int hasMusicVolume;
    int hasSoundEffectsVolume;
    int hasCheater;
    char *path;
} Wc1SdlRegistryKey;

DWORD RegOpenKeyExA(HKEY root, const char *subkey, DWORD options,
                    DWORD access, HKEY *result)
{
    Wc1SdlRegistryKey *key;
    char *directory;
    unsigned char *contents;
    size_t contentsSize;

    key = (Wc1SdlRegistryKey *)SDL_calloc(1, sizeof(*key));
    if (key == 0)
        return ERROR_FILE_NOT_FOUND;
    directory = SDL_GetPrefPath("Origin Systems", "WC Kilrathi Saga");
    if (directory == 0) {
        SDL_free(key);
        return ERROR_FILE_NOT_FOUND;
    }
    key->path = (char *)SDL_malloc(strlen(directory) + 20);
    if (key->path == 0) {
        SDL_free(directory);
        SDL_free(key);
        return ERROR_FILE_NOT_FOUND;
    }
    sprintf(key->path, "%swc1-modern.cfg", directory);
    SDL_free(directory);
    contents = (unsigned char *)SDL_LoadFile(key->path, &contentsSize);
    if (contents != 0) {
        unsigned int musicVolume;
        unsigned int soundEffectsVolume;
        unsigned int cheater;

        if (sscanf((char *)contents,
                   "MusicVolume=%u\nSFXVolume=%u\nCheater=%u",
                   &musicVolume, &soundEffectsVolume, &cheater) == 3) {
            key->musicVolume = musicVolume;
            key->soundEffectsVolume = soundEffectsVolume;
            key->cheater = cheater;
            key->hasMusicVolume = 1;
            key->hasSoundEffectsVolume = 1;
            key->hasCheater = 1;
        }
        SDL_free(contents);
    }
    *result = (HKEY)key;
    return ERROR_SUCCESS;
}

DWORD RegQueryValueExA(HKEY handle, const char *name, DWORD *reserved,
                       DWORD *type, BYTE *data, DWORD *size)
{
    Wc1SdlRegistryKey *key;
    DWORD value;
    int present;

    key = (Wc1SdlRegistryKey *)handle;
    present = 0;
    value = 0;
    if (strcmp(name, "MusicVolume") == 0) {
        present = key->hasMusicVolume;
        value = key->musicVolume;
    } else if (strcmp(name, "SFXVolume") == 0) {
        present = key->hasSoundEffectsVolume;
        value = key->soundEffectsVolume;
    } else if (strcmp(name, "Cheater") == 0) {
        present = key->hasCheater;
        value = key->cheater;
    }
    if (!present || *size < sizeof(value))
        return ERROR_FILE_NOT_FOUND;
    memcpy(data, &value, sizeof(value));
    *size = sizeof(value);
    if (type != 0)
        *type = REG_DWORD;
    return ERROR_SUCCESS;
}

DWORD RegSetValueExA(HKEY handle, const char *name, DWORD reserved,
                     DWORD type, const BYTE *data, DWORD size)
{
    Wc1SdlRegistryKey *key;
    DWORD value;
    SDL_RWops *file;
    char contents[128];
    int length;

    if (type != REG_DWORD || size < sizeof(value))
        return ERROR_FILE_NOT_FOUND;
    key = (Wc1SdlRegistryKey *)handle;
    memcpy(&value, data, sizeof(value));
    if (strcmp(name, "MusicVolume") == 0) {
        key->musicVolume = value;
        key->hasMusicVolume = 1;
    } else if (strcmp(name, "SFXVolume") == 0) {
        key->soundEffectsVolume = value;
        key->hasSoundEffectsVolume = 1;
    } else if (strcmp(name, "Cheater") == 0) {
        key->cheater = value;
        key->hasCheater = 1;
    } else {
        return ERROR_FILE_NOT_FOUND;
    }
    length = snprintf(contents, sizeof(contents),
                      "MusicVolume=%u\nSFXVolume=%u\nCheater=%u\n",
                      key->musicVolume, key->soundEffectsVolume,
                      key->cheater);
    file = SDL_RWFromFile(key->path, "wb");
    if (file == 0)
        return ERROR_FILE_NOT_FOUND;
    SDL_RWwrite(file, contents, 1, (size_t)length);
    SDL_RWclose(file);
    return ERROR_SUCCESS;
}

DWORD RegCloseKey(HKEY handle)
{
    Wc1SdlRegistryKey *key;

    key = (Wc1SdlRegistryKey *)handle;
    SDL_free(key->path);
    SDL_free(key);
    return ERROR_SUCCESS;
}
