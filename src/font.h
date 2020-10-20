#ifndef FONT_H
#define FONT_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define int8 int8_t
#define uint8 uint8_t
#define int16 int16_t
#define uint16 uint16_t
#define int32 int32_t
#define uint32 uint32_t

typedef struct _TTFOffsetTable {
    uint32 sfntVersion;
    uint16 numTables;
    uint16 searchRange;
    uint16 entrySelector;
    uint16 rangeShift;
} TTFOffsetTable;  

typedef struct  _TableDir {
    uint32 tag;
    uint32 checksum;
    uint32 offset;
    uint32 length;
} TableDir;

typedef struct _FontOptions {
    char *fontBinary;
} FontOptions;

// Required tables
typedef struct _cmap {

    
} cmap;

typedef struct _glyf {

} glyf;

typedef struct _head {

} head;

typedef struct _hhea {

} hhea;

typedef struct _hmtx {

} hmtx;

typedef struct _loca {

} loca;

typedef struct _maxp {

} maxp;

typedef struct _name {

} name;

typedef struct _post {

} post;

// Main font structure, this struct holds all the required data for font rendering.
typedef struct _Font {
    FontOptions *fontOpts;

    TTFOffsetTable *offsetTable;
    TableDir *tableDirs;

    // Required tables
    cmap cmap;
    glyf glyf;
    head head;
    hhea hhea;
    hmtx hmtx;
    loca loca;
    maxp maxp;
    name name;
    post post;
} Font;

Font *fontParse(FontOptions *opts);

void fontDestroy(Font *font);

#endif