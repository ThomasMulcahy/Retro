#include "font.h"
#include "types.h"

static void parseFormat_0(Font *font, EncodingRecord *record, int *index) {
    record->format_0 = malloc(sizeof(record->format_0));

    record->format_0->length = getUInt16(font->fontBuffer, index);
    record->format_0->language = getUInt16(font->fontBuffer, index);

    for (int i = 0; i < 256; i++) {
        record->format_0->glyphIdArray[i] = getUInt8(font->fontBuffer, index);
    }
}

static void parseFormat_4(Font *font,EncodingRecord *record, int *index, int numGlyphs) {
    record->format_4 = malloc(sizeof(record->format_4));
    record->format_4->length = getUInt16(font->fontBuffer, index);
    record->format_4->language = getUInt16(font->fontBuffer, index);

    int segCount= getUInt16(font->fontBuffer, index) / 2;
    record->format_4->segCountX2 = segCount * 2;
    record->format_4->searchRange = getUInt16(font->fontBuffer, index);
    record->format_4->entrySelector = getUInt16(font->fontBuffer, index);
    record->format_4->rangeShift = getUInt16(font->fontBuffer, index);

    record->format_4->endCode = malloc(sizeof(uint16) * (segCount));
    for (int i = 0; i < segCount; i++) {
        record->format_4->endCode[i] = getUInt16(font->fontBuffer, index);
    }

    record->format_4->reservePad = getUInt16(font->fontBuffer, index);

    record->format_4->startCode = malloc(sizeof(uint16) * (segCount));
    for (int i = 0; i < segCount; i++) {
        record->format_4->startCode[i] = getUInt16(font->fontBuffer, index);
    }

    record->format_4->idDelta = malloc(sizeof(uint16) * (segCount));
    for (int i = 0; i < segCount; i++) {
        record->format_4->idDelta[i] = getUInt16(font->fontBuffer, index);
    }

    record->format_4->idRangeOffset = malloc(sizeof(uint16) * (segCount));
    for (int i = 0; i < segCount; i++) {
        record->format_4->idRangeOffset[i] = getUInt16(font->fontBuffer, index);
    }

    record->format_4->glyphIdArray = malloc(sizeof(uint16) * numGlyphs);
    int pos = *index;
    
    for (int i = 0; i < segCount; i++) {
        uint16 start = record->format_4->startCode[i];
        uint16 end = record->format_4->endCode[i];
        uint16 delta = record->format_4->idDelta[i];
        uint16 range = record->format_4->idRangeOffset[i];

        if (start != (65536-1) && end != (65536-1)) {
            int j;
            for (j = start; j <= end; j++) {
                if (range == 0) {
                    record->format_4->glyphIdArray[(j + delta) % 65536] = j;
                } else {
                    uint32_t glyphOffset = pos + ((range/2) + (j-start) + (i-segCount))*2;
                    seek(index, glyphOffset);
                    uint16 glyphIndex = getUInt16(font->fontBuffer, index);
                    if (glyphIndex != 0) {
                        glyphIndex = (glyphIndex + delta) % 65536;
                        if (record->format_4->glyphIdArray[glyphIndex] == 0) {
                            record->format_4->glyphIdArray[glyphIndex] = j;
                        }
                    }
                }
            }
        }
    }
}

static void parseFormat_12(Font *font, EncodingRecord *record, int *index) {
    record->format_12 = malloc(sizeof(record->format_12));
    record->format_12->reserved = getUInt16(font->fontBuffer, index);
    record->format_12->length = getUInt32(font->fontBuffer, index);
    record->format_12->language = getUInt32(font->fontBuffer, index);
    record->format_12->numGroups = getUInt32(font->fontBuffer, index);

    record->format_12->groups = malloc(sizeof(record->format_12->groups) * record->format_12->numGroups);
    for (int i = 0; i < record->format_12->numGroups; i++) {
        record->format_12->groups->startCharCode = getUInt32(font->fontBuffer, index);
        record->format_12->groups->endCharCode = getUInt32(font->fontBuffer, index);
        record->format_12->groups->startGlyphID = getUInt32(font->fontBuffer, index);
    }
}

cmap *parseCMAP(Font *font, int *index, int numGlyphs) {
      TableDirectory *dir = getTableDirFromTag(font, "cmap");

    if (dir == NULL) {
        printf("%s\n", "Error reading cmap table directory");
        exit(EXIT_FAILURE);
    }

    cmap *cmap = malloc(sizeof(cmap));
    seek(index, dir->offset);
    cmap->version = getUInt16(font->fontBuffer, index);
    cmap->numTables = getUInt16(font->fontBuffer, index);

    int pos;
    cmap->encodingRecords = malloc(sizeof(cmap->encodingRecords) * cmap->numTables);
    for (int i = 0; i < cmap->numTables; i++) {
        cmap->encodingRecords[i].platformID = getUInt16(font->fontBuffer, index);
        cmap->encodingRecords[i].encodingID = getUInt16(font->fontBuffer, index);
        cmap->encodingRecords[i].offset = getUInt32(font->fontBuffer, index);

        pos = *index;
        seek(index, dir->offset + cmap->encodingRecords[i].offset);

        cmap->encodingRecords[i].format = getUInt16(font->fontBuffer, index);
        switch (cmap->encodingRecords[i].format) {
            case 0:
                parseFormat_0(font, &cmap->encodingRecords[i], index);
                break;
            case 4:
                parseFormat_4(font, &cmap->encodingRecords[i], index, numGlyphs);
                break;
            case 12:
                parseFormat_12(font, &cmap->encodingRecords[i], index);
                break;
        }

        seek(index, pos);
    }

    return cmap;
}