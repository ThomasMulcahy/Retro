#include "font.h"
#include "types.h"

head *parseHEAD(Font *font, int *index) {
    TableDirectory *dir = getTableDirFromTag(font, "head");

    if (dir == NULL) {
        printf("%s\n", "Error reading head table directory");
        exit(EXIT_FAILURE);
    }

    head *head = malloc(sizeof(head));
    seek(index, dir->offset);
    head->majorVersion = getUInt16(font->fontBuffer, index);
    head->minorVersion = getUInt16(font->fontBuffer, index);
    head->fontRevision = getUInt32(font->fontBuffer, index);
    head->checkSumAdjustment = getUInt32(font->fontBuffer, index);
    head->magicNumber = getUInt32(font->fontBuffer, index);
    head->flags = getUInt16(font->fontBuffer, index);
    head->unitsPerEm = getUInt16(font->fontBuffer, index);
    head->created = getInt64(font->fontBuffer, index);
    head->modified = getInt64(font->fontBuffer, index);
    head->xMin = getUInt16(font->fontBuffer, index);
    head->yMin = getUInt16(font->fontBuffer, index);
    head->xMax = getUInt16(font->fontBuffer, index);
    head->yMax = getUInt16(font->fontBuffer, index);
    head->macStyle = getUInt16(font->fontBuffer, index);
    head->lowestRecPPEM = getUInt16(font->fontBuffer, index);
    head->fontDirectionHint = getInt16(font->fontBuffer, index);
    head->indexToLocFormat = getInt16(font->fontBuffer, index);
    head->glpyhDataFormat = getInt16(font->fontBuffer, index);

    return head;
}