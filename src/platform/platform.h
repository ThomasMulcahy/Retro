#ifndef PLATFORM_H
#define PLATFORM_H

#include "../ui.h"
#include "../font.h"

typedef struct _WindowOpt {
    char *title;
    int width;
    int height;
} WindowOpt;

int platformRun(WindowOpt *winOptions, UIElement *headElement);

#endif