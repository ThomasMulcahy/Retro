#ifndef PLATFORM_UTIL_H
#define PLATFORM_UTIL_H

#include <stdio.h>
#include <stdlib.h>

char *platformReadFileToBuffer(char *path);
char *platformReadFileBytesToBuffer(char *path);

//Memory allocator that provides info
void *mallocate(size_t size, size_t count, char *label);

#endif