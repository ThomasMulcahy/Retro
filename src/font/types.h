#ifndef TYPES_H
#define TYPES_H

#include <ctype.h>
#include <stdint.h>

#define int8 int8_t
#define uint8 uint8_t
#define int16 int16_t
#define uint16 uint16_t
#define int32 int32_t
#define uint32 uint32_t
#define int64 long long

typedef struct _EncodingRecord {
    uint16 platformID;
    uint16 encodingID;
    uint32 offset;

    uint16 format;

    union {
        uint16 length;
        uint16 language;
        uint8 glyphIdArray[256];
    } Format_0;

    // Currently we are only interested in Format 0, Format 4 and Format 12
    union {
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

    union {
        uint16 reserved;
        uint32 length;
        uint32 language;
        uint32 numGroups;

        struct SequentialMapGroup {
            uint32 startCharCode;
            uint32 endCharCode;
            uint32 startGlyphID;
        }* groups; //Size: numGroups
    } Format_12;
} EncodingRecord;

typedef struct _cmap {
    uint16 version;
    uint16 numTables;

    struct _EncodingRecord *encodingRecords;    
} cmap;

typedef struct _glyf {

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

typedef struct _name {

} name;

typedef struct _post {

} post;

#endif