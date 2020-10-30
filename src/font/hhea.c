#include "font.h"
#include "types.h"

hhea *parseHHEA(Font *font, int *index) {
    TableDirectory *dir = getTableDirFromTag(font, "hhea");

    if (dir == NULL) {
        printf("%s\n", "Error reading hhea table directory");
        exit(EXIT_FAILURE);
    }

    hhea *hhea = mallocate(sizeof(head), 1, "HHEA struct");
    seek(index, dir->offset);
    hhea->majorVersion = getUInt16(font->fontBuffer, index);
    hhea->minorVersion = getUInt16(font->fontBuffer, index);
    hhea->ascender = getInt16(font->fontBuffer, index);
    hhea->descender = getInt16(font->fontBuffer, index);
    hhea->lineGap = getInt16(font->fontBuffer, index);
    hhea->advanceWidthMax = getUInt16(font->fontBuffer, index);
    hhea->minLeftSideBearing = getInt16(font->fontBuffer, index);
    hhea->minRightSideBearing = getInt16(font->fontBuffer, index);
    hhea->xMaxExtent = getInt16(font->fontBuffer, index);
    hhea->caretSlopeRise = getInt16(font->fontBuffer, index);
    hhea->caretSlopeRun = getInt16(font->fontBuffer, index);
    hhea->caretOffset = getInt16(font->fontBuffer, index);
    hhea->res1 = getInt16(font->fontBuffer, index);
    hhea->res2 = getInt16(font->fontBuffer, index);
    hhea->res3 = getInt16(font->fontBuffer, index);
    hhea->res4 = getInt16(font->fontBuffer, index);
    hhea->metricDataOffset = getInt16(font->fontBuffer, index);
    hhea->numberOfHMetrics = getUInt16(font->fontBuffer, index);
    
    return hhea;
}