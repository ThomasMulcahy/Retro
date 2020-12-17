#include "document.h"

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

char *documentGetLine(Document *doc, int lineNum) {
    int lineStart = doc->lineLengths[lineNum];
    int nextLineStart = doc->lineLengths[lineNum + 1];

    if (nextLineStart > (int) strlen(doc->buffer))
        nextLineStart = (int) strlen(doc->buffer);

    char *buffer = doc->buffer + lineStart;
    char *result = (char *) malloc(sizeof(char *) * (nextLineStart * lineStart));
    strncpy(result, buffer, nextLineStart - lineStart);

    return result;
}

Document *documentLoad(char *path) {
    //We want to open the file for reading and appending.
    FILE *file = fopen(path, "a+");
    if (file == NULL) {
        printf("%s%s\n", "Error reading file: ", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *buffer = (char *) calloc(size, sizeof(char));
    int newSize = fread(buffer, sizeof(char), size, file);
    buffer[newSize] = '\0';

    /*
     * TODO: We may want to keep this open but we will close it for now.
     * 
     * NOTE: Should we keep the file open? Or should we close and open on event (i.e Save)? 
     * This would allow other programs to edit the file.
     */
    fclose(file);

    int *lineLengths = (int *) malloc(sizeof(int) * size);
    int index = 0;
    int lineCount = 1;

    while (index < newSize) {
        char c = buffer[index++];
        if (c == '\r') {
            if (buffer[index] == '\n')
                index++;

            lineLengths[lineCount++] = index;
        } else if (c == '\n') {
            lineLengths[lineCount++] = index;
        }
    }
    lineLengths[lineCount] = newSize;

    Document *result = (Document *) malloc(sizeof(Document));
    result->filename = getFileNameFromPath(path);
    result->buffer = buffer;
    result->lineLengths = lineLengths;
    result->lineCount = lineCount;
    return result;
}

void documentClean(Document *doc) {
    free(doc->filename);
    free(doc->buffer);
    free(doc->lineLengths);
    free(doc);
}

