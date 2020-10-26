#ifndef FONT_H
#define FONT_H

#include "types.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct _FontOffsetTable {
    uint32 sfntVersion;
    uint16 numTables;
    uint16 searchRange;
    uint16 entrySelector;
    uint16 rangeShift;
} FontOffsetTable;  

typedef struct  _TableDirectory {
    uint32 tag;
    uint32 checksum;
    uint32 offset;
    uint32 length;
} TableDirectory;

// Required tables

// Main font structure, this struct holds all the required data for font rendering.
typedef struct _Font {
    char *fontBuffer;
    FontOffsetTable *offsetTable;
    TableDirectory *tableDirs;

    // Required tables
    cmap *cmap;
    glyf *glyf;
    head *head;
    hhea *hhea;
    hmtx *hmtx;
    loca *loca;
    maxp *maxp;
    name *name;
} Font;

void seek(int *index, int n);
void skip(int *index, int amt);
uint8 getUInt8(char *buffer, int *index);
int8 getInt8(char *buffer, int *index);
uint16 getUInt16(char *buffer, int *index);
int16 getInt16(char *buffer, int *index);
uint32 getUInt32(char *buffer, int *index);
int32 getInt32(char *buffer, int *index);
int64 getInt64(char *buffer, int *index);
uint32 tagToUInt32(char *tag);

cmap *parseCMAP(Font *font, int *index);
glyf *parseGLYF(Font *font, int *index);
head *parseHEAD(Font *font, int *index);
hhea *parseHHEA(Font *font, int *index);
hmtx *parseHMTX(Font *font, int *index, uint16 numberOfHMetrics, uint16 numGlyphs);
loca *parseLOCA(Font *font, int *index, int16 indexToLocFormat, uint16 numGlyphs);
maxp *parseMAXP(Font *font, int *index);
name *parseNAME(Font *font, int *index);

Font *fontParse(char *fontPath);
void fontDestroy(Font *font);
TableDirectory *getTableDirFromTag(Font *font, char *tag);

/* 
 * TODO: This really needs to be in the platform layer as we interact with the
 * underlying OS. Although, this currently avoids a circular dependency.
 */
char *readFontFile(char *path);

#endif