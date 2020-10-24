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

Font *fontParse(char *fontPath) {

	Font *font = (Font *) malloc(sizeof(Font));
	font->fontBuffer = readFontFile(fontPath);
	font->offsetTable = parseOffsetTable(font);
	font->tableDirs = parseTableDirs(font);

	printOffsetTable(font);
	printTableDirs(font);

	return font;
}

void fontDestroy(Font *font) {

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