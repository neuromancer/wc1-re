#ifndef TEACHER_ANALYSIS_DIRECT_H
#define TEACHER_ANALYSIS_DIRECT_H

#ifdef __cplusplus
extern "C" {
#endif

int _chdir(const char*);
int _mkdir(const char*);
int _rmdir(const char*);
char* _getcwd(char*, int);
void _splitpath(const char*, char*, char*, char*, char*);

#ifdef __cplusplus
}
#endif

#endif
