#include "font.h"
#include "types.h"

hmtx *parseHMTX(Font *font, int *index, uint16 numberOfHMetrics, uint16 numGlyphs) {
      TableDirectory *dir = getTableDirFromTag(font, "hmtx");

    if (dir == NULL) {
        printf("%s\n", "Error reading hmtx table directory");
        exit(EXIT_FAILURE);
    }

    hmtx *hmtx = malloc(sizeof(hmtx));
    seek(index, dir->offset);

    hmtx->hMetrics = malloc(sizeof(hmtx->hMetrics) * numberOfHMetrics);
    for (int i = 0; i < numberOfHMetrics; i++) {
        hmtx->hMetrics[i].advanceWidth = getUInt16(font->fontBuffer, index);
        hmtx->hMetrics[i].lsb = getInt16(font->fontBuffer, index);
    }

    hmtx->leftSideBearings = malloc(sizeof(int16) * numGlyphs);
    for (int i = 0; i < numGlyphs; i++) {
        hmtx->leftSideBearings[i] = getInt16(font->fontBuffer, index);
    }

    return hmtx;
}
