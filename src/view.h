#ifndef VIEW_H
#define VIEW_H

#include "document.h"

#define KEY_D_CODE 0x64
#define KEY_F_CODE 0x66
#define KEY_J_CODE 0x6a
#define KEY_K_CODE 0x6b

typedef struct _FontMetrics {
    double lineHeight;
    double advancement;
} FontMetrics;

typedef struct _Cursor {
    int line;
    int col;
} Cursor;

typedef enum _ViewMode {
    INSERT,
    COMMAND
} ViewMode;

typedef enum _ViewEventKind {
    KEY_DOWN_EVENT
} ViewEventKind;

typedef struct _ViewEvent {
    ViewEventKind eventKind;

    //Keydown Event
    char code;
} ViewEvent;

//TODO: Allow Views to be split
typedef struct _View {
    FontMetrics *fontMetrics;

    Cursor *cursor;
    Document *document;
} View;

void onViewEvent(View *view, ViewEvent event);

void viewDestroy(View *view);

#endif
