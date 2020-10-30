#include "font.h"
#include "types.h"

OS2 *parseOS2(Font *font, int *index) {
    TableDirectory *dir = getTableDirFromTag(font, "OS/2");

    if (dir == NULL) {
        printf("%s\n", "Error reading OS2 table directory");
        exit(EXIT_FAILURE);
    }

    OS2 *OS2 = mallocate(sizeof(OS2), 1, "OS/2 struct");
    seek(index, dir->offset);

    OS2->version = getUInt16(font->fontBuffer, index);
    OS2->xAvgCharWidth = getInt16(font->fontBuffer, index);
    OS2->usWeightClass = getUInt16(font->fontBuffer, index);
    OS2->usWidthClass = getUInt16(font->fontBuffer, index);
    OS2->fsType = getUInt16(font->fontBuffer, index);
    OS2->ySubscriptXSize = getInt16(font->fontBuffer, index);
    OS2->ySubscriptYSize = getInt16(font->fontBuffer, index);
    OS2->ySubscriptXOffset = getInt16(font->fontBuffer, index);
    OS2->ySubscriptYOffset = getInt16(font->fontBuffer, index);
    OS2->ySuperscriptXSize = getInt16(font->fontBuffer, index);
    OS2->ySuperscriptYSize = getInt16(font->fontBuffer, index);
    OS2->ySuperscriptXOffset = getInt16(font->fontBuffer, index);
    OS2->ySuperscriptYOffset = getInt16(font->fontBuffer, index);
    OS2->yStrikeoutSize = getInt16(font->fontBuffer, index);
    OS2->yStrikeoutPosition = getInt16(font->fontBuffer, index);
    OS2->sFamilyClass = getInt16(font->fontBuffer, index);

    for (int i = 0; i < 10; i++)
        OS2->panose[i] = getUInt8(font->fontBuffer, index);

    OS2->ulUnicodeRange1 = getUInt32(font->fontBuffer, index);
    OS2->ulUnicodeRange2 = getUInt32(font->fontBuffer, index);
    OS2->ulUnicodeRange3 = getUInt32(font->fontBuffer, index);
    OS2->ulUnicodeRange4 = getUInt32(font->fontBuffer, index);
    OS2->achVendID = getUInt32(font->fontBuffer, index);
    OS2->fsSelection = getUInt16(font->fontBuffer, index);
    OS2->usFirstCharIndex = getUInt16(font->fontBuffer, index);
    OS2->usLastCharIndex = getUInt16(font->fontBuffer, index);
    OS2->sTypoAscender = getInt16(font->fontBuffer, index);
    OS2->sTypoDescender = getInt16(font->fontBuffer, index);
    OS2->sTypoLineGap = getInt16(font->fontBuffer, index);
    OS2->usWinAscent = getUInt16(font->fontBuffer, index);
    OS2->usWinDescent = getUInt16(font->fontBuffer, index);

    //TODO: Implement this better, this is just lazy
    switch (OS2->version) {
        case 0x0001:
            OS2->ulCodePageRange1 = getUInt32(font->fontBuffer, index);
            OS2->ulCodePageRange2 = getUInt32(font->fontBuffer, index);
            break;
        case 0x0002:
        case 0x0003:
        case 0x0004:
            OS2->ulCodePageRange1 = getUInt32(font->fontBuffer, index);
            OS2->ulCodePageRange2 = getUInt32(font->fontBuffer, index);

            OS2->sxHeight = getInt16(font->fontBuffer, index);
            OS2->sCapHeight = getInt16(font->fontBuffer, index);
            OS2->usDefaultChar = getUInt16(font->fontBuffer, index);
            OS2->usBreakChar = getUInt16(font->fontBuffer, index);
            OS2->usMaxContext = getUInt16(font->fontBuffer, index);
            break;
        case 0x0005:
            OS2->ulCodePageRange1 = getUInt32(font->fontBuffer, index);
            OS2->ulCodePageRange2 = getUInt32(font->fontBuffer, index);

            OS2->sxHeight = getInt16(font->fontBuffer, index);
            OS2->sCapHeight = getInt16(font->fontBuffer, index);
            OS2->usDefaultChar = getUInt16(font->fontBuffer, index);
            OS2->usBreakChar = getUInt16(font->fontBuffer, index);
            OS2->usMaxContext = getUInt16(font->fontBuffer, index);

            OS2->usLowerOpticalPointSize = getUInt16(font->fontBuffer, index);
            OS2->usUpperOpticalPointSize = getUInt16(font->fontBuffer, index);
            break;
    }

    return OS2;
}