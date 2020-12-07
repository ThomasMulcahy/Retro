#ifndef PLATFORM_H
#define PLATFORM_H


typedef struct _WindowOpt {
    char *title;
    int width;
    int height;
} WindowOpt;

int platformRun(WindowOpt *winOptions);

#endif