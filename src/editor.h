#ifdef EDITOR_H
#define EDITOR_H

typedef struct _Cursor {
    int column;
    int row;
} Cursor;

typedef struct _Editor {
    char *pathToFile;
    char* buffer;

    Cursor *cursor;
} Editor;

#endif