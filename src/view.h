#ifndef VIEW_H
#define VIEW_H

#include "document.h"

typedef struct _FontMetrics {
    double lineHeight;
} FontMetrics;

typedef struct _Cursor {
    int line;
    int col;
} Cursor;

typedef enum _ViewEvent {
    KEY_EVENT
} ViewEvent;

typedef enum _ViewMode {
    INSERT,
    COMMAND
} ViewMode;

//TODO: Allow Views to be split
typedef struct _View {
    FontMetrics *fontMetrics;

    Document *document;
} View;

void onViewEvent(View *view, ViewEvent event);

#endif
