#include "font.h"
#include "types.h"
#include <stdlib.h>

head *parseHEAD(Font *font) {
    TableDirectory *dir = getTableDirFromTag(font, "head");

    if (dir == NULL) {
        printf("%s\n", "Error reading HEAD table directory");
        exit(EXIT_FAILURE);
    }

    head *head = malloc(sizeof(head));
    SEEK(font->fontBuffer, dir->offset);
    head->majorVersion = READ_UINT16(font->fontBuffer);
    head->minorVersion = READ_UINT16(font->fontBuffer);
    head->fontRevision = READ_UINT32(font->fontBuffer);
    head->checkSumAdjustment = READ_UINT32(font->fontBuffer);
    head->magicNumber = READ_UINT32(font->fontBuffer);
    head->flags = READ_UINT16(font->fontBuffer);
    head->unitsPerEm = READ_UINT16(font->fontBuffer);
    head->created = READ_INT64(font->fontBuffer);
    head->modified = READ_INT64(font->fontBuffer);
    head->xMin = READ_UINT16(font->fontBuffer);
    head->yMin = READ_UINT16(font->fontBuffer);
    head->xMax = READ_UINT16(font->fontBuffer);
    head->yMax = READ_UINT16(font->fontBuffer);
    head->macStyle = READ_UINT16(font->fontBuffer);
    head->lowestRecPPEM = READ_UINT16(font->fontBuffer);
    head->fontDirectionHint = READ_INT16(font->fontBuffer);
    head->indexToLocFormat = READ_INT16(font->fontBuffer);
    head->glpyhDataFormat = READ_INT16(font->fontBuffer);

    return head;
}