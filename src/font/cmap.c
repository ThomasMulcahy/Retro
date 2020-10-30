#include "font.h"
#include "types.h"

static Format_0 *parseFormat_0(Font *font, EncodingRecord *record, int *index) {
    Format_0 *format = (Format_0 *) mallocate(sizeof(Format_0), 1, "CMAP Format 0 Struct");
    format->length = getUInt16(font->fontBuffer, index);
    format->language = getUInt16(font->fontBuffer, index);

    for (int i = 0; i < 256; i++) {
        format->glyphIdArray[i] = getUInt8(font->fontBuffer, index);
    }
    
    return format;
}

static Format_4 *parseFormat_4(Font *font,EncodingRecord *record, int *index, int numGlyphs) {
    Format_4 *format = (Format_4 *) mallocate(sizeof(Format_4), 1, "CMAP Format 4 Struct");

    format->length = getUInt16(font->fontBuffer, index);
    format->language = getUInt16(font->fontBuffer, index);

    int segCount= getUInt16(font->fontBuffer, index) / 2;
    format->segCountX2 = segCount * 2;
    format->searchRange = getUInt16(font->fontBuffer, index);
    format->entrySelector = getUInt16(font->fontBuffer, index);
    format->rangeShift = getUInt16(font->fontBuffer, index);

    format->endCode = (uint16 *) mallocate(sizeof(uint16), segCount, "CMAP Format 4 endCode");
    for (int i = 0; i < segCount; i++) {
        format->endCode[i] = getUInt16(font->fontBuffer, index);
    }

    format->reservePad = getUInt16(font->fontBuffer, index);

    format->startCode = (uint16 *) mallocate(sizeof(uint16), segCount, "CMAP Format 4 startCode");
    for (int i = 0; i < segCount; i++) {
        format->startCode[i] = getUInt16(font->fontBuffer, index);
    }

    format->idDelta = (int16 *) mallocate(sizeof(int16), segCount, "CMAP Format 4 idDelta");
    for (int i = 0; i < segCount; i++) {
        format->idDelta[i] = getUInt16(font->fontBuffer, index);
    }

    format->idRangeOffset = (uint16 *) mallocate(sizeof(uint16), segCount, "CMAP Format 4 idRangeOffset");
    for (int i = 0; i < segCount; i++) {
        format->idRangeOffset[i] = getUInt16(font->fontBuffer, index);
    }

    format->glyphIdArray = (uint16 *) mallocate(sizeof(uint16), segCount, "CMAP Format 4 glyphIdArray");

    int pos = *index;
    for (int i = 0; i < segCount; i++) {
        uint16 start = format->startCode[i];
        uint16 end = format->endCode[i];
        uint16 delta = format->idDelta[i];
        uint16 range = format->idRangeOffset[i];

        if (start != (65536-1) && end != (65536-1)) {
            int j;
            for (j = start; j <= end; j++) {
                if (range == 0) {
                    format->glyphIdArray[(j + delta) % 65536] = j;
                } else {
                    uint32_t glyphOffset = pos + ((range/2) + (j-start) + (i-segCount))*2;
                    seek(index, glyphOffset);
                    uint16 glyphIndex = getUInt16(font->fontBuffer, index);
                    if (glyphIndex != 0) {
                        glyphIndex = (glyphIndex + delta) % 65536;
                        if (format->glyphIdArray[glyphIndex] == 0) {
                            format->glyphIdArray[glyphIndex] = j;
                        }
                    }
                }
            }
        }
    }

    return format;
}

static Format_12 *parseFormat_12(Font *font, EncodingRecord *record, int *index) {
    Format_12 *format = (Format_12 *) mallocate(sizeof(Format_12), 1, "CMAP Format 12 struct");
    format->reserved = getUInt16(font->fontBuffer, index);
    format->length = getUInt32(font->fontBuffer, index);
    format->language = getUInt32(font->fontBuffer, index);
    format->numGroups = getUInt32(font->fontBuffer, index);

    SequentialMapGroup *groups = (SequentialMapGroup *) mallocate(sizeof(SequentialMapGroup), format->numGroups, "CMAP Format 12 SeqMapGroup array");
    for (int i = 0; i < format->numGroups; i++) {
        format->groups->startCharCode = getUInt32(font->fontBuffer, index);
        format->groups->endCharCode = getUInt32(font->fontBuffer, index);
        format->groups->startGlyphID = getUInt32(font->fontBuffer, index);
    }

    format->groups = groups;
    return format;
}

cmap *parseCMAP(Font *font, int *index, int numGlyphs) {
      TableDirectory *dir = getTableDirFromTag(font, "cmap");

    if (dir == NULL) {
        printf("%s\n", "Error reading cmap table directory");
        exit(EXIT_FAILURE);
    }

    cmap *cmap = mallocate(sizeof(cmap), 1, "CMAP Struct");
    seek(index, dir->offset);
    cmap->version = getUInt16(font->fontBuffer, index);
    cmap->numTables = getUInt16(font->fontBuffer, index);

    int pos;
    cmap->encodingRecords = (EncodingRecord *) mallocate(sizeof(EncodingRecord), cmap->numTables, "CMAP Encoding Record array");
    for (int i = 0; i < cmap->numTables; i++) {
        cmap->encodingRecords[i].platformID = getUInt16(font->fontBuffer, index);
        cmap->encodingRecords[i].encodingID = getUInt16(font->fontBuffer, index);
        cmap->encodingRecords[i].offset = getUInt32(font->fontBuffer, index);

        pos = *index;
        seek(index, dir->offset + cmap->encodingRecords[i].offset);

        cmap->encodingRecords[i].format = getUInt16(font->fontBuffer, index);
        switch (cmap->encodingRecords[i].format) {
            case 0:
                cmap->encodingRecords[i].format_0 = parseFormat_0(font, &cmap->encodingRecords[i], index);
                break;
            case 4:
                cmap->encodingRecords[i].format_4 = parseFormat_4(font, &cmap->encodingRecords[i], index, numGlyphs);
                break;
            case 12:
                cmap->encodingRecords[i].format_12 = parseFormat_12(font, &cmap->encodingRecords[i], index);
                break;
        }

        seek(index, pos);
    }

    return cmap;
}