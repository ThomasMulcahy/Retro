#include "document.h"
#include <string.h>

//https://stackoverflow.com/questions/7180293/how-to-extract-filename-from-path
static char *getFileNameFromPath(char *path) {
    if( path == NULL )
        return NULL;

    char *fileName = path;
    for(char *cur = path; *cur != '\0'; cur++) {
        if( *cur == '/' || *cur == '\\' )
            fileName = cur + 1;
    }
    
    return fileName;
}

int isEscapeChar(char c) {
    return c == '\a' || c == '\b' || c == '\e' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ||
            c == '\\';
}

char *documentGetLine(Document *doc, int lineNum) {
    return doc->lines[lineNum];
}

//TODO: Implement different line reading to handle escape chars
Document *documentLoad(char *path) {
    FILE *fp;
    char *line = NULL;
    size_t totalLen = 0;
    ssize_t lineLen;

    fp = fopen(path, "r");
        if (fp == NULL)
            exit(EXIT_FAILURE);
    
    char **lines = calloc(256, sizeof(char *));
    for (int i = 0; i < 256; i++) {
        lines[i] = calloc(256, sizeof(char));
    }

    int index = 0;
    int *lineLengths = malloc(sizeof(int) * 256);
    char *buffer = malloc(sizeof(char) * 256);
    while ((lineLen = getline(&line, &totalLen, fp)) != -1) {
        strcpy(buffer, line);
        strcpy(lines[index], line);

        if (lineLen == 1 && isEscapeChar(buffer[0]))
            lineLen = 0;

        lineLengths[index] = lineLen;
        index++;
    }

    Document *result = (Document *) malloc(sizeof(Document));
    result->filename = getFileNameFromPath(path);
    result->buffer = buffer;
    result->lines = lines;
    result->lineLengths = lineLengths;
    result->lineCount = index;
    return result;
}

void documentDestroy(Document *doc) {
    free(doc->filename);

    //TODO: Double free?
    free(doc->lines);
    free(doc->lineLengths);
    free(doc);
}

