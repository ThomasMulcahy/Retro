#include "font.h"
#include "types.h"

loca *parseLOCA(Font *font, int *index, int16 indexToLocFormat, uint16 numGlyphs) {
    TableDirectory *dir = getTableDirFromTag(font, "loca");

    if (dir == NULL) {
        printf("%s\n", "Error reading loca table directory");
        exit(EXIT_FAILURE);
    }

    loca *loca = malloc(sizeof(loca));
    seek(index, dir->offset);

    if (indexToLocFormat == 0) {
        loca->offsets16 = malloc(sizeof(int16) * (numGlyphs + 1));
        for (int i = 0; i < (numGlyphs + 1); i++) {
            loca->offsets16[i] = getInt16(font->fontBuffer, index);
        }
    } else if (indexToLocFormat == 1) {
        loca->offsets32 = malloc(sizeof(int32) * (numGlyphs + 1));
        for (int i = 0; i < (numGlyphs + 1); i++) {
            loca->offsets32[i] = getInt32(font->fontBuffer, index);
        }
    } else {
        printf("%s\n", "Error loca format is not correct!");
        exit(EXIT_FAILURE);
    }

    return loca;
}