#include "font.h"
#include "platform/platform_util.h"

static inline void SKIP(int *index, int n) {
    *(index) += n;
}

static inline void SEEK(int *index, int n) {
    *(index) = n;
}

static inline uint8 READ_UINT8(char *buffer, int *index) {
    uint8 result = buffer[*index];
    (*index)++;
    return result;
}

static inline uint8 READ_INT8(char *buffer, int *index) {
    int8 result = buffer[*index];
    (*index)++;
    return result;
}

static inline uint16 READ_UINT16(char *buffer, int *index) {
    return (uint16) (READ_UINT8(buffer, index) << 8 | READ_UINT8(buffer, index) << 0);
}

static inline int16 READ_INT16(char *buffer, int *index) {
    return (int16) (READ_INT8(buffer, index) << 8 | READ_INT8(buffer, index) << 0);
}

static inline uint32 READ_UINT32(char *buffer, int *index) {
    return (uint32) (READ_UINT8(buffer, index) << 24 | 
                     READ_UINT8(buffer, index) << 16 |
                     READ_UINT8(buffer, index) << 8 | 
                     READ_UINT8(buffer, index) << 0);
}

static inline int32 READ_INT32(char *buffer, int *index) {
    return (int32) (READ_UINT8(buffer, index) << 24 | 
                     READ_UINT8(buffer, index) << 16 |
                     READ_UINT8(buffer, index) << 8 | 
                     READ_UINT8(buffer, index) << 0);
}

static uint32 getTableOffsetFromTag(FontData* data, char *tag) {
        for(int i = 0; i < data->numTables; i++) {
            TableDirectory *dir = &data->tableDirs[i];
            uint32 convertedTag = 0;
            convertedTag |= (tag[3] << 0);
            convertedTag |= (tag[2] << 8);
            convertedTag |= (tag[1] << 16);
            convertedTag |= (tag[0] << 24);

            if (convertedTag == dir->tag) {
                return dir->offset;
            }
        }

    printf("%s%s\n", "Failed to find offset for table: ", tag);
    return 0;
}

Font *fontParse(char *path) {
    Font *font = mallocate(sizeof(Font), 1, "Font struct");
    FontData *data = mallocate(sizeof(FontData), 1, "FontData struct");
    char *buffer = platformReadFileBytesToBuffer(path);

    //Parse Font header
    int index = 0;
    data->sfntVersion = READ_UINT32(buffer, &index);
    data->numTables = READ_UINT16(buffer, &index);
    data->searchRange = READ_UINT16(buffer, &index);
    data->entrySelector = READ_UINT16(buffer, &index);
    data->rangeShift = READ_UINT16(buffer, &index);

#if DEBUG
    printf("%s%d\n", "SFNT Version:", data->sfntVersion);
    printf("%s%d\n", "Number of Tables:", data->numTables);
    printf("%s%d\n", "Search Range:", data->searchRange);
    printf("%s%d\n", "Entry Selector:", data->entrySelector);
    printf("%s%d\n", "Range Shift:", data->rangeShift);
#endif

    TableDirectory *dirs = mallocate(sizeof(TableDirectory), data->numTables, "TableDirectories array");
    for (int i = 0; i < data->numTables; i++) {
        dirs[i].tag = READ_UINT32(buffer, &index);
        dirs[i].checksum = READ_UINT32(buffer, &index);
        dirs[i].offset = READ_UINT32(buffer, &index);
        dirs[i].length = READ_UINT32(buffer, &index);
    }
    data->tableDirs = dirs;

    //head
    SEEK(&index, getTableOffsetFromTag(data, "head"));
    SKIP(&index, 18);
    data->unitsPerEm = READ_UINT16(buffer, &index);
    SKIP(&index, 32);
    data->indexToLocFormat = READ_INT16(buffer, &index);

#if DEBUG
    printf("%s%d\n", "Units per EM: ", data->unitsPerEm);
    printf("%s%d\n", "indexToLocFormat: ", data->indexToLocFormat);
#endif

    //hhea
    SEEK(&index, getTableOffsetFromTag(data, "hhea"));
    SKIP(&index, 4);
    data->ascender = READ_INT16(buffer, &index);
    data->descender = READ_INT16(buffer, &index);
    data->lineGap = READ_INT16(buffer, &index);
    SKIP(&index, 24);
    data->numberOfHMetrics = READ_UINT16(buffer, &index);

#if DEBUG
    printf("%s%d\n", "Ascender: ", data->ascender);
    printf("%s%d\n", "Descender: ", data->descender);
    printf("%s%d\n", "Line gap: ", data->lineGap);
    printf("%s%d\n", "Number of HMetrics: ", data->numberOfHMetrics);
#endif

    //maxp
    SEEK(&index, getTableOffsetFromTag(data, "maxp"));
    SKIP(&index, 4);
    data->numGlyphs = READ_UINT16(buffer, &index);

#if DEBUG
    printf("%s%d\n", "Number of Glyphs: ",  data->numGlyphs);
#endif

    //loca
    SEEK(&index, getTableOffsetFromTag(data, "loca"));
    data->offsets = mallocate(sizeof(uint32), data->numGlyphs + 1, "Offsets array");
    for (int i = 0; i < data->numGlyphs + 1; i++) {
        if (data->indexToLocFormat == 0) {
            data->offsets[i] = READ_UINT16(buffer, &index);
        } else if (data->indexToLocFormat == 1) {
            data->offsets[i] = READ_UINT32(buffer, &index);
        } else {
            printf("%s\n", "Error: indexToLocFormat is incorrect");
            exit(EXIT_FAILURE);
        }
    }

    //hmtx
    SEEK(&index, getTableOffsetFromTag(data, "hmtx"));
    data->hMetrics = mallocate(sizeof(LongHorMetric), data->numberOfHMetrics, "hMetrics array");
    for (int i = 0; i < data->numberOfHMetrics; i++) {
        data->hMetrics[i].advanceWidth = READ_UINT16(buffer, &index);
        data->hMetrics[i].lsb = READ_INT16(buffer, &index);
    }

    font->fontData = data;
    return font;
}