#include "font.h"
#include "types.h"

hmtx *parseHMTX(Font *font, int *index, uint16 numberOfHMetrics, uint16 numGlyphs) {
      TableDirectory *dir = getTableDirFromTag(font, "hmtx");

    if (dir == NULL) {
        printf("%s\n", "Error reading hmtx table directory");
        exit(EXIT_FAILURE);
    }

    hmtx *hmtx = mallocate(sizeof(head), 1, "HMTX struct");
    seek(index, dir->offset);

    hmtx->hMetrics = (LongHorMetric *) mallocate(sizeof(LongHorMetric), numberOfHMetrics, "HMTX hMetrics array");
    for (int i = 0; i < numberOfHMetrics; i++) {
        hmtx->hMetrics[i].advanceWidth = getUInt16(font->fontBuffer, index);
        hmtx->hMetrics[i].lsb = getInt16(font->fontBuffer, index);
    }

    hmtx->leftSideBearings = (int16 *) mallocate(sizeof(int16), numGlyphs, "HMTX lsb array");
    for (int i = 0; i < numGlyphs; i++) {
        hmtx->leftSideBearings[i] = getInt16(font->fontBuffer, index);
    }

    return hmtx;
}
