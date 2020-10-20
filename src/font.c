#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void seek(int *index, int loc) {
    (*index) = loc;
}

static void skip(int *index, int amt) {
    (*index) += amt;
}

static uint8 readInt8(char *buffer, int *index) {
    return (int8) buffer[(*index)++];
}

static uint8 readUInt8(char *buffer, int *index) {
    return (uint8) buffer[(*index)++];
}

static int16 readInt16(char *buffer, int *index) {
    int16 result = 0;
    result |= readInt8(buffer, index) << 8;
    result |= readInt8(buffer, index);
    return result;
}

static uint16 readUInt16(char *buffer, int *index) {
    uint16 result = 0;
    result |= readUInt8(buffer, index) << 8;
    result |= readUInt8(buffer, index);
    return result;
}

static int32 readInt32(char *buffer, int *index) {
    int32 result = 0;
    result |= readInt8(buffer, index) << 24;
    result |= readInt8(buffer, index) << 16;
    result |= readInt8(buffer, index) << 8;
    result |= readInt8(buffer, index);
    return result;
}

static uint32 readUInt32(char *buffer, int *index) {
    uint32 result = 0;
    result |= readUInt8(buffer, index) << 24;
    result |= readUInt8(buffer, index) << 16;
    result |= readUInt8(buffer, index) << 8;
    result |= readUInt8(buffer, index);
    return result;
}

// Reference: https://docs.microsoft.com/en-us/typography/opentype/spec/otff
static uint32 calcTableChecksum(uint32 *table, uint32 length) {
	uint32 Sum = 0L;
    uint32 *Endptr = table+((length+3) & ~3) / sizeof(uint32);
    while (table < Endptr)
        Sum += *table++;
    return Sum;
}

static void parseOffsetTable(Font *font, int *index) {
    font->offsetTable = (TTFOffsetTable *) malloc(sizeof(TTFOffsetTable));
    font->offsetTable->sfntVersion = readUInt32(font->fontOpts->fontBinary, index);
    font->offsetTable->numTables = readUInt16(font->fontOpts->fontBinary, index);
    font->offsetTable->searchRange = readUInt16(font->fontOpts->fontBinary, index);
    font->offsetTable->entrySelector = readUInt16(font->fontOpts->fontBinary, index);
    font->offsetTable->rangeShift = readUInt16(font->fontOpts->fontBinary, index);
}

static void uint32ToTag(char *buffer, uint32 tag) {
    buffer[3] = tag >> 24;
    buffer[2] = tag >> 16;
    buffer[1] = tag >> 8;
    buffer[0] = tag;
}

static TableDir *getTableFromTag(Font *font, char *tag) {
    
    for (int i = 0; i < font->offsetTable->numTables; i++) {
        char *tableTag = (char *)&font->tableDirs[i].tag;

        if (strcmp(tableTag, tag) == 0)
            return &font->tableDirs[i];
    }

    return NULL;
}

static void printOffsetTable(Font *font) {
    printf("%s\n", "-Offet Table-");
    printf("%s%d\n", "Scalar type: ", font->offsetTable->sfntVersion);
    printf("%s%d\n", "Num tables: ", font->offsetTable->numTables);
    printf("%s%d\n", "Search range: ", font->offsetTable->searchRange);
    printf("%s%d\n", "Entry selector: ", font->offsetTable->entrySelector);
    printf("%s%d\n", "Range shift: ", font->offsetTable->rangeShift);

}

static void validateChecksums(Font *font) {

    for (int i = 0; i < font->offsetTable->numTables; i++) {
        TableDir *table = &font->tableDirs[i];
        
        uint32 *data = (uint32 *) malloc(sizeof(uint32) * table->length);
        data[0] = table->tag;
        data[1] = table->checksum;
        data[2] = table->offset;
        data[3] = table->length;

        uint32 calcChecksum = calcTableChecksum(data, table->length);
        if (calcChecksum != table->checksum) {
            printf("%.*s%s\n", 4, (char *)&table->tag,": Warning table checksum could not be validated!");
            printf("%s%d\n", "Calculated checksum: ", calcChecksum);
            printf("%s%d\n", "Checksum: ", table->checksum);
        }
    }
}

static void parseTableDirectories(Font *font, int *index) {

    font->tableDirs = (TableDir *)malloc(sizeof(TableDir) * font->offsetTable->numTables);
    for (int i = 0; i < font->offsetTable->numTables; i++) {
        TableDir *table = &font->tableDirs[i];

        table->tag = readUInt32(font->fontOpts->fontBinary, index);
        table->checksum = readUInt32(font->fontOpts->fontBinary, index);
        table->offset = readUInt32(font->fontOpts->fontBinary, index);
        table->length = readUInt32(font->fontOpts->fontBinary, index);

        printf("%s%.*s\n", "Tag: ", 4, (char *)&table->tag);
        printf("%s%d\n", "Checksum: ", table->checksum);
        printf("%s%d\n", "Offset: ", table->offset);
        printf("%s%d\n", "Length: ", table->length);
    }
}

Font *fontParse(FontOptions *opts) {

    Font *font = (Font *) malloc(sizeof(Font));
    font->fontOpts = opts;

    int index = 0;
    parseOffsetTable(font, &index);
    parseTableDirectories(font, &index);

    // TODO: It seems that validation is not currently working?? Fix this.
    // Other more established projects seem to skip this step entirel???
    // validateChecksums(font);

    printOffsetTable(font);

    return font;
}

// TODO: Determine when we can let go of memory as we don't need to hold on to
// all of this for rendering.
void fontDestroy(Font *font) {

    free(font->fontOpts);
    free(font->offsetTable);
    free(font->tableDirs);

    free(font);
}