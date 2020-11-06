#ifndef FONT_H
#define FONT_H

#include <ctype.h>
#include <stdint.h>
#include <string.h>

//Font types
#define int8 int8_t
#define uint8 uint8_t
#define int16 int16_t
#define uint16 uint16_t
#define int32 int32_t
#define uint32 uint32_t
#define int64 int64_t

//Simple glyph
#define ON_CURVE_POINT 0x01
#define X_SHORT_VECTOR 0x02
#define Y_SHORT_VECTOR 0x04
#define REPEAT_FLAG 0x08
#define X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR 0x10
#define Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR 0x20
#define OVERLAP_SIMPLE 0x40
#define RESERVED_SIMPLE 0x80

//Comp glyph
#define ARG_1_AND_2_ARE_WORDS 0x0001
#define ARGS_ARE_XY_VALUES 0x0002
#define ROUND_XY_TO_GRID 0x0004
#define WE_HAVE_SCALE 0x0008
#define MORE_COMPONENTS 0x0020
#define WE_HAVE_AN_X_AND_Y_SCALE 0x0040
#define WE_HAVE_A_TWO_BY_TWO 0x0080
#define WE_HAVE_INSTRUCTIONS 0x0100
#define USE_MY_METRICS 0x0200
#define OVERLAP_COMPOUND 0x0400
#define SCALED_COMPONENT_OFFSET 0x0800
#define UNSCALED_COMPONENT_OFFSET 0x1000
#define RESERVED_COMP 0xE010

typedef struct  _TableDirectory {
    uint32 tag;
    uint32 checksum;
    uint32 offset;
    uint32 length;
} TableDirectory;

typedef struct _SimpleGlyph {
    uint16 *endPtsOfContours; //Size: numberOfContours
    uint8 *flags; //Size: numPoints + 1
    int16 *xCoordinates; //Size: numPoints + 1
    int16 *yCoordinates; //Size; numPoints + 1
} SimpleGlyph;

typedef struct _CompoundComponent {
    uint16 flags;
    uint16 glyphIndex;
    int16 arg1;
    int16 arg2;
    float xscale;
    float yscale;
    float scale01;
    float scale10;
    int16 xtranslate;
    int16 ytranslate;
    uint16 point1;
    uint16 point2;
} CompoundComponent;

typedef struct _CompoundGlyph {
    int numComps;
    CompoundComponent *components;
} CompoundGlyph; 

typedef struct _Glyph {
    int16 numberOfContours;
    int16 xMin;
    int16 yMin;
    int16 xMax;
    int16 yMax;

    //TODO: Unsure if we actually need glyph instructions?

    SimpleGlyph *simpleGlyph;
    CompoundGlyph *compoundGlyph;
} Glyph;

typedef struct _LongHorMetric {
        uint16 advanceWidth;
        int16 lsb;
} LongHorMetric;

typedef struct _FontData {
    //Font header
    uint32 sfntVersion;
    uint16 numTables;
    uint16 searchRange;
    uint16 entrySelector;
    uint16 rangeShift;

    TableDirectory *tableDirs; //Size: numTables

    //head
    uint16 unitsPerEm;
    int16 indexToLocFormat;

    //hhea
    int16 ascender;
    int16 descender;
    int16 lineGap;
    uint16 numberOfHMetrics;

    //maxp
    uint16	numGlyphs;

    //loca
    uint32 *offsets;

    //hmtx
    LongHorMetric *hMetrics;//Size: noOfHMetrics

    //glyf
    Glyph *glyphs; //Size: numGlyph

    //cmap

} FontData;

typedef struct _Font {
    char *path;

    FontData *fontData;
} Font;

Font *fontParse(char *path);

#endif