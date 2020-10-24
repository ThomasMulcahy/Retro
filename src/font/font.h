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
    post *post;
} Font;

cmap *parseCMAP(Font *font);
glyf *parseGLYF(Font *font);
head *parseHEAD(Font *font);
hhea *parseHHEA(Font *font);
hmtx *parseHMTX(Font *font);
loca *parseLOCA(Font *font);
maxp *parseMAXP(Font *font);
name *parseNAME(Font *font);
post *parsePOST(Font *font);

Font *fontParse(char *fontPath);
void fontDestroy(Font *font);
TableDirectory *getTableDirFromTag(Font *font, char *tag);

/* 
 * TODO: This really needs to be in the platform layer as we interact with the
 * underlying OS. Although, this currently avoids a circular dependency.
 */
char *readFontFile(char *path);

#endif