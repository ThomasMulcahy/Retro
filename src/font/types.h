#ifndef TYPES_H
#define TYPES_H

#include <ctype.h>
#include <stdint.h>
#include <string.h>

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

typedef struct _Format_0 {
    uint16 length;
    uint16 language;
    uint8 glyphIdArray[256];
} Format_0;

typedef struct _Format_4 {
    uint16 length;
    uint16 language;
    uint16 segCountX2;
    uint16 searchRange;
    uint16 entrySelector;
    uint16 rangeShift;
    uint16 *endCode; // Size: segCount
    uint16 reservePad;
    uint16 *startCode; // Size: segCount
    int16 *idDelta; //Size: segCount
    uint16 *idRangeOffset; // Size: segCount
    uint16 *glyphIdArray; // Arbitrary size
} Format_4;

typedef struct _Format_12 {
    uint16 reserved;
    uint32 length;
    uint32 language;
    uint32 numGroups;

    struct SequentialMapGroup {
        uint32 startCharCode;
        uint32 endCharCode;
        uint32 startGlyphID;
    } *groups; //Size: numGroups
} Format_12;

typedef struct _EncodingRecord {
    uint16 platformID;
    uint16 encodingID;
    uint32 offset;

    uint16 format;

    Format_0 *format_0;
    Format_4 *format_4;
    Format_12 *format_12;
} EncodingRecord;

typedef struct _cmap {
    uint16 version;
    uint16 numTables;

    struct _EncodingRecord *encodingRecords;    
} cmap;

typedef struct _glyf {
    int16 numberOfContours;
    int16 xMin;
    int16 yMin;
    int16 xMax;
    int16 yMax;

    struct {
        uint16 *endPtsOfContours; //Size: numberOfContours
        uint16 instructionLength;
        uint8 *instructions; //Size: instructionLength
        uint8 *flags; //Size: numPoints + 1
        int16 *xCoordinates; //Size: numPoints + 1
        int16 *yCoordinates; //Size; numPoints + 1
    } simpleGlyphTable;

    struct {
        uint16 numComps;

        struct {
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
        } *compGlyphs;
    } compGlyph;
} glyf;

typedef struct _head {
    uint16 majorVersion;
    uint16 minorVersion;
    uint32 fontRevision;
    uint32 checkSumAdjustment;
    uint32 magicNumber;
    uint16 flags;
    uint16 unitsPerEm;
    long long created;
    long long modified;
    int16 xMin;
    int16 yMin;
    int16 xMax;
    int16 yMax;
    uint16 macStyle;
    uint16 lowestRecPPEM;
    int16 fontDirectionHint;
    int16 indexToLocFormat;
    int16 glpyhDataFormat;
} head;

typedef struct _hhea {
    uint16 majorVersion;
    uint16 minorVersion;
    int16 ascender;
    int16 descender;
    int16 lineGap;
    uint16 advanceWidthMax;
    int16 minLeftSideBearing;
    int16 minRightSideBearing;
    int16 xMaxExtent;
    int16 caretSlopeRise;
    int16 caretSlopeRun;
    int16 caretOffset;
    int16 res1;
    int16 res2;
    int16 res3;
    int16 res4;
    int16 metricDataOffset;
    uint16 numberOfHMetrics;
} hhea;

typedef struct _hmtx {
    struct LongHorMetrix {
        uint16 advanceWidth;
        int16 lsb;
    } *hMetrics; //Size: noOfHMetrics
    int16 *leftSideBearings; //Size: numGlyphs * nnoOfHMetrics
} hmtx;

typedef struct _loca {
    //Version = 0
    int16 *offsets16; //Size: numGlyphs + 1

    //Version = 1
    int32 *offsets32; //Size: numGlyphs + 1
} loca;

typedef struct _maxp {
    uint32	version;
    uint16	numGlyphs;
    uint16	maxPoints;
    uint16	maxContours;
    uint16	maxCompositePoints;
    uint16	maxCompositeContours;
    uint16	maxZones;
    uint16	maxTwilightPoints;
    uint16	maxStorage;
    uint16	maxFunctionDefs;
    uint16	maxInstructionDefs;
    uint16	maxStackElements;
    uint16	maxSizeOfInstructions;
    uint16	maxComponentElements;
    uint16	maxComponentDepth;
} maxp;

typedef struct _OS2 {
    uint16 version;
    int16 xAvgCharWidth;
    uint16 usWeightClass;
    uint16 usWidthClass;
    uint16 fsType;
    int16 ySubscriptXSize;
    int16 ySubscriptYSize;
    int16 ySubscriptXOffset;
    int16 ySubscriptYOffset;
    int16 ySuperscriptXSize;
    int16 ySuperscriptYSize;
    int16 ySuperscriptXOffset;
    int16 ySuperscriptYOffset;
    int16 yStrikeoutSize;
    int16 yStrikeoutPosition;
    int16 sFamilyClass;
    uint8 panose[10];
    uint32 ulUnicodeRange1;
    uint32 ulUnicodeRange2;
    uint32 ulUnicodeRange3;
    uint32 ulUnicodeRange4;
    uint32 achVendID;
    uint16 fsSelection;
    uint16 usFirstCharIndex;
    uint16 usLastCharIndex;
    int16 sTypoAscender;
    int16 sTypoDescender;
    int16 sTypoLineGap;
    uint16 usWinAscent;
    uint16 usWinDescent;

    //Version 1
    uint32 ulCodePageRange1;
    uint32 ulCodePageRange2;

    //Version 2/3/4
    int16 sxHeight;
    int16 sCapHeight;
    uint16 usDefaultChar;
    uint16 usBreakChar;
    uint16 usMaxContext;

    //Version 5
    uint16 usLowerOpticalPointSize;
    uint16 usUpperOpticalPointSize;
} OS2;

#endif