#ifndef PLATFORM_UTIL_H
#define PLATFORM_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _Profiler {
    char *id;
    time_t start;
    time_t end;
    double diff;
} Profiler;

char *platformReadFileToBuffer(char *path);
char *platformReadFileBytesToBuffer(char *path);

//Memory allocator that provides info
void *mallocate(size_t size, size_t count, char *label);

//Profiling - Profiler is automatically freed on profileEnd
Profiler *profileStart(char *id);
void profileEnd(Profiler *profiler);

#endif