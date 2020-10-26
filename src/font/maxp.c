#include "font.h"
#include "types.h"

maxp *parseMAXP(Font *font, int *index) {
    TableDirectory *dir = getTableDirFromTag(font, "maxp");

    if (dir == NULL) {
        printf("%s\n", "Error reading maxp table directory");
        exit(EXIT_FAILURE);
    }

    maxp *maxp = malloc(sizeof(maxp));
    seek(index, dir->offset);
    maxp->version = getUInt32(font->fontBuffer, index);
    maxp->numGlyphs = getUInt16(font->fontBuffer, index);
    maxp->maxPoints = getUInt16(font->fontBuffer, index);
    maxp->maxContours = getUInt16(font->fontBuffer, index);
    maxp->maxCompositePoints = getUInt16(font->fontBuffer, index);
    maxp->maxCompositeContours = getUInt16(font->fontBuffer, index);
    maxp->maxZones = getUInt16(font->fontBuffer, index);
    maxp->maxTwilightPoints = getUInt16(font->fontBuffer, index);
    maxp->maxStorage = getUInt16(font->fontBuffer, index);
    maxp->maxFunctionDefs = getUInt16(font->fontBuffer, index);
    maxp->maxInstructionDefs = getUInt16(font->fontBuffer, index);
    maxp->maxStackElements = getUInt16(font->fontBuffer, index);
    maxp->maxSizeOfInstructions = getUInt16(font->fontBuffer, index);
    maxp->maxComponentElements = getUInt16(font->fontBuffer, index);
    maxp->maxComponentDepth = getUInt16(font->fontBuffer, index);

    return maxp;
}