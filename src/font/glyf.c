#include "font.h"
#include "types.h"
#include <stdlib.h>

/*
 * Parsing of glyphs takes heavy inspiration from another open font parser.
 * This will be changed once font parsing becomes more mature.
 *
 * https://github.com/dluco/ttf/blob/master/parse/parse.c
 */

static void parseSimpleGlyph(Font *font, glyf *glyf, int *index) {
    glyf->simpleGlyphTable.endPtsOfContours = malloc(sizeof(uint16) * glyf->numberOfContours);;
    for (int i = 0; i < glyf->numberOfContours; i++) {
        glyf->simpleGlyphTable.endPtsOfContours[i] = getUInt16(font->fontBuffer, index);
    } 

    glyf->simpleGlyphTable.instructionLength = getUInt16(font->fontBuffer, index);

    glyf->simpleGlyphTable.instructions = malloc(sizeof(uint8) * glyf->simpleGlyphTable.instructionLength);
    for (int i = 0; i < glyf->simpleGlyphTable.instructionLength; i++) {
        glyf->simpleGlyphTable.instructions[i] = getUInt8(font->fontBuffer, index);
    }

    int lastEndPt = glyf->simpleGlyphTable.endPtsOfContours[glyf->numberOfContours - 1];
    int numPoints = lastEndPt + 1;

    //Read flags
    glyf->simpleGlyphTable.flags = malloc(sizeof(uint8) * numPoints);
    for (int i = 0; i < numPoints; i++) {
        glyf->simpleGlyphTable.flags[i] = getUInt8(font->fontBuffer, index);

        if ((glyf->simpleGlyphTable.flags[i] & REPEAT_FLAG) != 0) {
            uint8 repeats = getUInt8(font->fontBuffer, index);
            for (int j = 0; j <= repeats; j++) {
                glyf->simpleGlyphTable.flags[i + j] = glyf->simpleGlyphTable.flags[i];
            }

            i += repeats;
        }
    }

    //Read coords
    glyf->simpleGlyphTable.xCoordinates = malloc(sizeof(int16) * numPoints);
    glyf->simpleGlyphTable.yCoordinates = malloc(sizeof(int16) * numPoints);

    int16 x = 0, y = 0;
    for (int i = 0; i < numPoints; i++) {
        if ((glyf->simpleGlyphTable.flags[i] & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) != 0) {
            if ((glyf->simpleGlyphTable.flags[i] & X_SHORT_VECTOR) != 0) {
                x += (int16_t) getInt8(font->fontBuffer, index);
            }
        } else {
            if ((glyf->simpleGlyphTable.flags[i] & X_SHORT_VECTOR) != 0) {
                x += -((int16_t) getInt8(font->fontBuffer, index));
            } else {
                x    += getInt16(font->fontBuffer, index);
            }
        }
        glyf->simpleGlyphTable.xCoordinates[i] = x;
    }

    for (int i = 0; i < numPoints; i++) {
        if ((glyf->simpleGlyphTable.flags[i] & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) != 0) {
            if ((glyf->simpleGlyphTable.flags[i] & Y_SHORT_VECTOR) != 0) {
                y += (int16_t) getInt8(font->fontBuffer, index);
            }
        } else {
            if ((glyf->simpleGlyphTable.flags[i] & Y_SHORT_VECTOR) != 0) {
                y += -((int16_t) getInt8(font->fontBuffer, index));
            } else {
                y    += getInt16(font->fontBuffer, index);
            }
        }
        glyf->simpleGlyphTable.yCoordinates[i] = y;
    }
}

static void parseCompGlyph(Font *font, glyf *glyf, int *index) {
    int numComps = 0;

    do {
        glyf->compGlyph.compGlyphs = realloc(glyf->compGlyph.compGlyphs, (numComps + 1) * sizeof(glyf->compGlyph.compGlyphs));

        glyf->compGlyph.compGlyphs[numComps].flags = getInt16(font->fontBuffer, index);
	    glyf->compGlyph.compGlyphs[numComps].glyphIndex = getUInt16(font->fontBuffer, index);

	// Read arguments as words or bytes
	if (glyf->compGlyph.compGlyphs[numComps].flags & ARG_1_AND_2_ARE_WORDS) {
		glyf->compGlyph.compGlyphs[numComps].arg1 = getInt16(font->fontBuffer, index);
		glyf->compGlyph.compGlyphs[numComps].arg2 = getInt16(font->fontBuffer, index);
	} else {
		glyf->compGlyph.compGlyphs[numComps].arg1 = (int16_t) getInt8(font->fontBuffer, index);
		glyf->compGlyph.compGlyphs[numComps].arg2 = (int16_t) getInt8(font->fontBuffer, index);
	}

	// Assign arguments depending on type
	if (glyf->compGlyph.compGlyphs[numComps].flags & ARGS_ARE_XY_VALUES) {
		glyf->compGlyph.compGlyphs[numComps].xtranslate = glyf->compGlyph.compGlyphs[numComps].arg1;
		glyf->compGlyph.compGlyphs[numComps].ytranslate = glyf->compGlyph.compGlyphs[numComps].arg2;
	} else {
		glyf->compGlyph.compGlyphs[numComps].point1 = (uint16_t) glyf->compGlyph.compGlyphs[numComps].arg1;
		glyf->compGlyph.compGlyphs[numComps].point2 = (uint16_t) glyf->compGlyph.compGlyphs[numComps].arg2;
	}

	// Read scaling information
	if (glyf->compGlyph.compGlyphs[numComps].flags & WE_HAVE_SCALE) {
		glyf->compGlyph.compGlyphs[numComps].xscale = glyf->compGlyph.compGlyphs[numComps].yscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
	} else if (glyf->compGlyph.compGlyphs[numComps].flags & WE_HAVE_AN_X_AND_Y_SCALE) {
		glyf->compGlyph.compGlyphs[numComps].xscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
		glyf->compGlyph.compGlyphs[numComps].yscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
	} else if (glyf->compGlyph.compGlyphs[numComps].flags & WE_HAVE_A_TWO_BY_TWO) {
		glyf->compGlyph.compGlyphs[numComps].xscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
		glyf->compGlyph.compGlyphs[numComps].scale01 = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
		glyf->compGlyph.compGlyphs[numComps].scale10 = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
		glyf->compGlyph.compGlyphs[numComps].yscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
	}
        
        numComps++;
    } while (glyf->compGlyph.compGlyphs && (glyf->compGlyph.compGlyphs[numComps - 1].flags & MORE_COMPONENTS));

    if (glyf->compGlyph.compGlyphs && (glyf->compGlyph.compGlyphs[numComps - 1].flags & WE_HAVE_INSTRUCTIONS)) {
        glyf->simpleGlyphTable.instructionLength = getUInt16(font->fontBuffer, index);

        glyf->simpleGlyphTable.instructions = malloc(sizeof(uint8) * glyf->simpleGlyphTable.instructionLength);
        for (int i = 0; i < glyf->simpleGlyphTable.instructionLength; i++) {
            glyf->simpleGlyphTable.instructions[i] = getUInt8(font->fontBuffer, index);
        }
    }
}

glyf *parseGLYF(Font *font, int *index) {
    TableDirectory *dir = getTableDirFromTag(font, "glyf");

    if (dir == NULL) {
        printf("%s\n", "Error reading glyf table directory");
        exit(EXIT_FAILURE);
    }

    glyf *glyf = malloc(sizeof(glyf));
    seek(index, dir->offset);
    glyf->numberOfContours = getInt16(font->fontBuffer, index);
    glyf->xMin = getInt16(font->fontBuffer, index);
    glyf->yMin = getInt16(font->fontBuffer, index);
    glyf->xMax = getInt16(font->fontBuffer, index);
    glyf->yMax = getInt16(font->fontBuffer, index);

    //Simple glyf
    if (glyf->numberOfContours > 0) {
        parseSimpleGlyph(font, glyf, index);
    } else if (glyf->numberOfContours < 0) { //Comp glyf
        parseCompGlyph(font, glyf, index);
    }
    return glyf;
}