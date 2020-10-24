#include "font.h"
#include "types.h"

static void printOffsetTable(Font *font) {
	printf("%s\n", "Offset Table:");
	printf("%s%d\n", "SFNT Version: ", font->offsetTable->sfntVersion);
	printf("%s%d\n", "Num Tables: ", font->offsetTable->numTables);
	printf("%s%d\n", "Search Range: ", font->offsetTable->searchRange);
	printf("%s%d\n", "Entry Selector: ", font->offsetTable->entrySelector);
	printf("%s%d\n", "Range Shift: ", font->offsetTable->rangeShift);
}

static FontOffsetTable *parseOffsetTable(Font* font) {
	FontOffsetTable *offsetTable = (FontOffsetTable *) malloc(sizeof(FontOffsetTable));

	offsetTable->sfntVersion = READ_UINT32(font->fontBuffer);
	offsetTable->numTables = READ_UINT16(font->fontBuffer);
	offsetTable->searchRange = READ_UINT16(font->fontBuffer);
	offsetTable->entrySelector = READ_UINT16(font->fontBuffer);
	offsetTable->rangeShift = READ_UINT16(font->fontBuffer);

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

static TableDirectory *parseTableDirs(Font *font) {
	int numTables = font->offsetTable->numTables;
	TableDirectory *tableDirs = (TableDirectory *) malloc(sizeof(TableDirectory) * numTables);

	for (int i = 0; i < numTables; i++) {
		TableDirectory *dir = &tableDirs[i];
		dir->tag = READ_UINT32(font->fontBuffer);
		dir->checksum = READ_UINT32(font->fontBuffer);
		dir->offset = READ_UINT32(font->fontBuffer);
		dir->length = READ_UINT32(font->fontBuffer);
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

Font *fontParse(char *fontPath) {

	Font *font = (Font *) malloc(sizeof(Font));
	font->fontBuffer = readFontFile(fontPath);
	font->offsetTable = parseOffsetTable(font);
	font->tableDirs = parseTableDirs(font);

	font->head = parseHEAD(font);

	printOffsetTable(font);
	printTableDirs(font);
	printHEAD(font);

	return font;
}

void fontDestroy(Font *font) {

}

TableDirectory *getTableDirFromTag(Font *font, char *tag) {
	int numTables = font->offsetTable->numTables;
	for (int i = 0; i < numTables; i++) {
		if (font->tableDirs[i].tag == TAG_TO_UINT32(tag)) {
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