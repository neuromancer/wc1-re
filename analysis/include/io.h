#ifndef TEACHER_ANALYSIS_IO_H
#define TEACHER_ANALYSIS_IO_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _finddata_t {
    unsigned attrib;
    long time_create;
    long time_access;
    long time_write;
    long size;
    char name[260];
};

int _access(const char*, int);
int _open(const char*, int, ...);
int _read(int, void*, unsigned int);
int _write(int, const void*, unsigned int);
int _close(int);
long _lseek(int, long, int);
long _filelength(int);
long _findfirst(const char*, struct _finddata_t*);
int _findnext(long, struct _finddata_t*);
int _findclose(long);
void _splitpath(const char*, char*, char*, char*, char*);

#ifdef __cplusplus
}
#endif

#endif
