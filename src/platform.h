#ifndef PLATFORM_H
#define PLATFORM_H

#include "ui.h"

typedef struct _WindowOpt {
    char *title;
    int width;
    int height;
} WindowOpt;

int platformRun(WindowOpt *winOptions, UIElement *headElement);

char *platformReadFile(char *path);

#endif