#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct _Document {
    char *filename;
    char *buffer;
    char **lines;
    int *lineLengths;
    int lineCount;
} Document;

char *documentGetLine(Document *doc, int lineNum);

Document *documentLoad(char *path);

void documentDestroy(Document *doc);

#endif