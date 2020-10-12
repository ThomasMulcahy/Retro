#include "ui.h"

typedef struct _Cursor {
    int column;
    int row;
} Cursor;

typedef struct _Editor {
    Cursor *cursor;
} Editor;