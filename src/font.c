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

//See here for glyph parsing inspiration: https://github.com/dluco/ttf/blob/master/parse/parse.c
static void parseSimpleGlyph(Glyph *glyph, char *buffer, int *index) {
    SimpleGlyph *simpleGlyph = mallocate(sizeof(SimpleGlyph), 1, "");
    
    simpleGlyph->endPtsOfContours = mallocate(sizeof(uint16_t), glyph->numberOfContours, "");
    for (int i = 0; i < glyph->numberOfContours; i++) {
        simpleGlyph->endPtsOfContours[i] = READ_UINT16(buffer, index);
    }

    int numPoints = simpleGlyph->endPtsOfContours[glyph->numberOfContours - 1] + 1;

    simpleGlyph->flags = mallocate(sizeof(uint8), numPoints, "");
    for (int i = 0; i < numPoints; i++) {
        simpleGlyph->flags[i] = READ_UINT8(buffer, index);

        if ((simpleGlyph->flags[i] & REPEAT_FLAG) != 0) {
            uint8 repeats = READ_UINT8(buffer, index);
            for (int j = 0; j < repeats; j++) {
                simpleGlyph->flags[i + j] = simpleGlyph->flags[i];
            }
            i += repeats;
        }
    }

    simpleGlyph->xCoordinates = mallocate(sizeof(int16), numPoints, "");
    int16 x = 0;
    for (int i = 0; i < numPoints; i++) {
        if ((simpleGlyph->flags[i] & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) != 0) {
            if ((simpleGlyph->flags[i] & X_SHORT_VECTOR) != 0) {
                x += (int16) READ_INT8(buffer, index);
            }
        } else {
            if ((simpleGlyph->flags[i] & X_SHORT_VECTOR) != 0) {
                x += -(int16) READ_INT8(buffer, index);
            } else {
                x += READ_INT16(buffer, index);
            }
        }
        simpleGlyph->xCoordinates[i] = x;
    }

    simpleGlyph->yCoordinates = mallocate(sizeof(int16), numPoints, "");
    int16 y = 0;
    for (int i = 0; i < numPoints; i++) {
        if ((simpleGlyph->flags[i] & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) != 0) {
            if ((simpleGlyph->flags[i] & Y_SHORT_VECTOR) != 0) {
                y += (int16) READ_INT8(buffer, index);
            }
        } else {
            if ((simpleGlyph->flags[i] & Y_SHORT_VECTOR) != 0) {
                y += -(int16) READ_INT8(buffer, index);
            } else {
                y += READ_INT16(buffer, index);
            }
        }
        simpleGlyph->yCoordinates[i] = y;
    }

    glyph->simpleGlyph = simpleGlyph;
}

static void parseCompoundGlyphComponents(CompoundComponent *component, char *buffer, int *index) {
    component->flags = READ_UINT16(buffer, index);
    component->glyphIndex = READ_UINT16(buffer, index);

    if (component->flags & ARG_1_AND_2_ARE_WORDS) {
        component->arg1 = READ_INT16(buffer, index);
        component->arg2 = READ_INT16(buffer, index);
    } else {
        component->arg1 = (int16) READ_INT8(buffer, index);
        component->arg2 = (int16) READ_INT8(buffer, index);
    }

    if (component->flags & ARGS_ARE_XY_VALUES) {
        component->xtranslate = component->arg1;
        component->ytranslate = component->arg2;
    } else {
        component->point1 = (uint16) component->arg1;
        component->point2 = (uint16) component->arg2;
    }

    if (component->flags & WE_HAVE_SCALE) {
        component->xscale = component-> yscale = (float) READ_INT16(buffer, index) / (float) 0x4000;
    } else if (component->flags & WE_HAVE_AN_X_AND_Y_SCALE) {
        component->xscale =  (float) READ_INT16(buffer, index) / (float) 0x4000;
        component->yscale =  (float) READ_INT16(buffer, index) / (float) 0x4000;
    } else if (component->flags & WE_HAVE_A_TWO_BY_TWO) {
        component->xscale =  (float) READ_INT16(buffer, index) / (float) 0x4000;
        component->scale01 =  (float) READ_INT16(buffer, index) / (float) 0x4000;
        component->scale10 =  (float) READ_INT16(buffer, index) / (float) 0x4000;
        component->yscale =  (float) READ_INT16(buffer, index) / (float) 0x4000;
    }
}
 
static void parseCompoundGlyph(Glyph *glyph, char *buffer, int *index) {

    CompoundGlyph *compoundGlyph = mallocate(sizeof(CompoundGlyph), 1, "");

    compoundGlyph->components = malloc(sizeof(CompoundComponent) * 1);
    do {
        CompoundComponent *reallocComp = (CompoundComponent *) realloc(compoundGlyph->components, sizeof(*compoundGlyph->components) * (compoundGlyph->numComps + 1));
        if (!compoundGlyph->components) {
            printf("%s\n", "Error reallocating components array");
            exit(EXIT_FAILURE);
        } else {
            compoundGlyph->components = reallocComp;
        }

        parseCompoundGlyphComponents(&compoundGlyph->components[compoundGlyph->numComps], buffer, index);
        compoundGlyph->numComps++;
    } while(compoundGlyph->components && (compoundGlyph->components[compoundGlyph->numComps + 1].flags & MORE_COMPONENTS));

    glyph->compoundGlyph = compoundGlyph;
}

Font *fontParse(char *path) {
    Profiler *profiler = profileStart("Font parsing");

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

    //glyf
    Glyph *glyphs = mallocate(sizeof(Glyph), data->numGlyphs, "Glyph array");
    for (int i = 0; i < data->numGlyphs; i++) {
        Glyph *glyph = &glyphs[i];

        if (data->offsets[i + 1] - data->offsets[i] == 0) {
            continue;
        }

        SEEK(&index, getTableOffsetFromTag(data, "glyf") + data->offsets[i]);
        glyph->numberOfContours = READ_INT16(buffer, &index);
        glyph->xMin = READ_INT16(buffer, &index);
        glyph->yMin = READ_INT16(buffer, &index);
        glyph->xMax = READ_INT16(buffer, &index);
        glyph->yMax = READ_INT16(buffer, &index);

        if (glyph->numberOfContours == 0) {
        } else if(glyph[i].numberOfContours > 0) {
            parseSimpleGlyph(glyph, buffer, &index);
        } else {
            parseCompoundGlyph(glyph, buffer, &index);
        }
    }
    data->glyphs = glyphs;
    //cmap

    font->fontData = data;
    profileEnd(profiler);
    free(buffer);
    return font;
}