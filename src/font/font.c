#include "font.h"
#include "types.h"

void seek(int *index, int n) {
	 *index = n;
}

void skip(int *index, int amt) {
	*index += amt;
}

uint8 getUInt8(char *buffer, int *index) {
	uint8 result = buffer[*index];
	++(*index);
	return result;
}

int8 getInt8(char *buffer, int *index) {
	int8 result = buffer[*index];
	++(*index);
	return result;
}

uint16 getUInt16(char *buffer, int *index) {
	uint16 result = 0;
	result |= getUInt8(buffer, index) << 8;
	result |= getUInt8(buffer, index) << 0;
	return result;
}

int16 getInt16(char *buffer, int *index) {
	int16 result = 0;
	result |= getInt8(buffer, index) << 8;
	result |= getInt8(buffer, index) << 0;
	return result;
}

uint32 getUInt32(char *buffer, int *index) {
	uint32 result = 0;
	result |= getUInt8(buffer, index) << 24;
	result |= getUInt8(buffer, index) << 16;
	result |= getUInt8(buffer, index) << 8;
	result |= getUInt8(buffer, index) << 0;
	return result;
}

int32 getInt32(char *buffer, int *index) {
	int32 result = 0;
	result |= getInt8(buffer, index) << 24;
	result |= getInt8(buffer, index) << 16;
	result |= getInt8(buffer, index) << 8;
	result |= getInt8(buffer, index) << 0;
	return result;
}

int64 getInt64(char *buffer, int *index) {
	int64 result = 0;
	result |= getInt8(buffer, index) << 56;
	result |= getInt8(buffer, index) << 48;
	result |= getInt8(buffer, index) << 40;
	result |= getInt8(buffer, index) << 32;
	result |= getInt8(buffer, index) << 24;
	result |= getInt8(buffer, index) << 16;
	result |= getInt8(buffer, index) << 8;
	result |= getInt8(buffer, index) << 0;
	return result;
}

uint32 tagToUInt32(char *tag) {
	return (uint32) ((tag[0] << 24) | (tag[1] << 16) | (tag[2] << 8) | (tag[3] << 0));
}

static void printOffsetTable(Font *font) {
	printf("%s\n", "Offset Table:");
	printf("%s%d\n", "SFNT Version: ", font->offsetTable->sfntVersion);
	printf("%s%d\n", "Num Tables: ", font->offsetTable->numTables);
	printf("%s%d\n", "Search Range: ", font->offsetTable->searchRange);
	printf("%s%d\n", "Entry Selector: ", font->offsetTable->entrySelector);
	printf("%s%d\n", "Range Shift: ", font->offsetTable->rangeShift);
}

static FontOffsetTable *parseOffsetTable(Font* font, int *index) {
	FontOffsetTable *offsetTable = (FontOffsetTable *) malloc(sizeof(FontOffsetTable));

	offsetTable->sfntVersion = getUInt32(font->fontBuffer, index);
	offsetTable->numTables = getUInt16(font->fontBuffer, index);
	offsetTable->searchRange = getUInt16(font->fontBuffer, index);
	offsetTable->entrySelector = getUInt16(font->fontBuffer, index);
	offsetTable->rangeShift = getUInt16(font->fontBuffer, index);

	return offsetTable;
}

static void printTableDirs(Font *font) {
	for (int i = 0; i < font->offsetTable->numTables; i++) {
		//The tag prints backwards but it seems to be working fine?
		printf("%s%.4s\n", "Tag: ", (char *) &font->tableDirs[i].tag);
		printf("%s%d\n", "Checksum: ", font->tableDirs[i].checksum);
		printf("%s%d\n", "Offset: ", font->tableDirs[i].offset);
		printf("%s%d\n", "Length: ", font->tableDirs[i].length);
	}
}

static TableDirectory *parseTableDirs(Font *font, int *index) {
	int numTables = font->offsetTable->numTables;
	TableDirectory *tableDirs = (TableDirectory *) malloc(sizeof(TableDirectory) * numTables);

	for (int i = 0; i < numTables; i++) {
		TableDirectory *dir = &tableDirs[i];
		dir->tag = getUInt32(font->fontBuffer, index);
		dir->checksum = getUInt32(font->fontBuffer, index);
		dir->offset = getUInt32(font->fontBuffer, index);
		dir->length = getUInt32(font->fontBuffer, index);
	}

	return tableDirs;
}

static void printHEAD(Font *font) {
    printf("%s\n", "HEAD Table");
    printf("%s%d\n", "Major Ver: ", font->head->majorVersion);
    printf("%s%d\n", "Minor Ver: ", font->head->minorVersion);
    printf("%s%d\n", "Font Rev: ", font->head->fontRevision);
    printf("%s%d\n", "Check Sum Adj: ", font->head->checkSumAdjustment);
    printf("%s%d\n", "Magic Number: ", font->head->magicNumber);
    printf("%s%d\n", "Flags: ", font->head->flags);
    printf("%s%d\n", "Units per Em: ", font->head->unitsPerEm);
    printf("%s%lld\n", "Created: ", font->head->created);
    printf("%s%lld\n", "Modified: ", font->head->modified);
    printf("%s%d\n", "xMin: ", font->head->xMin);
    printf("%s%d\n", "yMin: ", font->head->yMin);
    printf("%s%d\n", "xMax: ", font->head->xMax);
    printf("%s%d\n", "yMax: ", font->head->yMax);
    printf("%s%d\n", "Mac Style: ", font->head->macStyle);
    printf("%s%d\n", "Lowest Rec PPEM: ", font->head->lowestRecPPEM);
    printf("%s%d\n", "Font Dir Hint: ", font->head->fontDirectionHint);
    printf("%s%d\n", "Index to Loc Format: ", font->head->indexToLocFormat);
    printf("%s%d\n", "Glyph Format Data: ", font->head->glpyhDataFormat);
}

static void printMAXP(Font *font) {
	printf("%s\n", "MAXP");
	printf("%s%d\n", "Ver: ", font->maxp->version);
	printf("%s%d\n", "Num Glyphs: ", font->maxp->numGlyphs);
	printf("%s%d\n", "Max Points: ", font->maxp->maxPoints);
	printf("%s%d\n", "Max Contours: ", font->maxp->maxContours);
	printf("%s%d\n", "Max Composite Points: ", font->maxp->maxCompositePoints);
	printf("%s%d\n", "Max Composite Contours: ", font->maxp->maxCompositeContours);
	printf("%s%d\n", "Max Zones: ", font->maxp->maxZones);
	printf("%s%d\n", "Max Twilight Zones: ", font->maxp->maxTwilightPoints);
	printf("%s%d\n", "Max Storage: ", font->maxp->maxStorage);
	printf("%s%d\n", "Max Function Defs: ", font->maxp->maxFunctionDefs);
	printf("%s%d\n", "Max Instruction Defs: ", font->maxp->maxInstructionDefs);
	printf("%s%d\n", "Max Stack Elements: ", font->maxp->maxStackElements);
	printf("%s%d\n", "Max Size Of Instructions: ", font->maxp->maxSizeOfInstructions);
	printf("%s%d\n", "Max Component Elements: ", font->maxp->maxComponentElements);
	printf("%s%d\n", "Max Component Depth: ", font->maxp->maxComponentDepth);
}

void printHHEA(Font *font) {
	printf("%s\n", "HHEA");
	printf("%s%d\n", "Major Version: ", font->hhea->majorVersion);
	printf("%s%d\n", "Minor Version: ", font->hhea->minorVersion);
	printf("%s%d\n", "Ascender: ", font->hhea->ascender);
	printf("%s%d\n", "Descender: ", font->hhea->descender);
	printf("%s%d\n", "Line Gap: ", font->hhea->lineGap);
	printf("%s%d\n", "Advanced Width Max: ", font->hhea->advanceWidthMax);
	printf("%s%d\n", "Min Left Side Bearing: ", font->hhea->minLeftSideBearing);
	printf("%s%d\n", "Min Right Side Bearing: ", font->hhea->minRightSideBearing);
	printf("%s%d\n", "X Max Extent: ", font->hhea->xMaxExtent);
	printf("%s%d\n", "Caret Slope Rise: ", font->hhea->caretSlopeRise);
	printf("%s%d\n", "Caret Slope Run: ", font->hhea->caretSlopeRun);
	printf("%s%d\n", "Caret Offset: ", font->hhea->caretOffset);
	printf("%s%d\n", "Res 1: ", font->hhea->res1);
	printf("%s%d\n", "Res 2: ", font->hhea->res2);
	printf("%s%d\n", "Res 3: ", font->hhea->res3);
	printf("%s%d\n", "Res 4: ", font->hhea->res4);
	printf("%s%d\n", "Metric Data Offset: ", font->hhea->metricDataOffset);
	printf("%s%d\n", "Number of H Metrics: ", font->hhea->numberOfHMetrics);
}

Font *fontParse(char *fontPath) {

	Font *font = (Font *) malloc(sizeof(Font));
	int index = 0;
	font->fontBuffer = readFontFile(fontPath);
	font->offsetTable = parseOffsetTable(font, &index);
	font->tableDirs = parseTableDirs(font, &index);

	printOffsetTable(font);
	printTableDirs(font);

	font->head = parseHEAD(font, &index);
	font->maxp = parseMAXP(font, &index);
	font->hhea = parseHHEA(font, &index);

	printHEAD(font);
	printMAXP(font);
	printHHEA(font);

	return font;
}

void fontDestroy(Font *font) {
	/*
	 * TODO: Assess what is needed for rendering, everything else can be 
	 * released back to the system.
	 */
}

TableDirectory *getTableDirFromTag(Font *font, char *tag) {
	int numTables = font->offsetTable->numTables;
	for (int i = 0; i < numTables; i++) {
		if (font->tableDirs[i].tag == tagToUInt32(tag)) {
			return &font->tableDirs[i];
		}
	}

	return NULL;
}

char *readFontFile(char *path) {
	FILE *file = fopen(path, "rb");
    if (file == NULL) {
        printf("%s%s\n", "Error reading file: ", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *result = (char *)malloc(sizeof(char) * size);
    int bytesRead = fread(result, sizeof(char), size, file);
    result[bytesRead] = '\0';

    return result;
}