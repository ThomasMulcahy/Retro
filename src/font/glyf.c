#include "font.h"
#include "types.h"
#include <stdlib.h>

/*
 * Parsing of glyphs takes heavy inspiration from another open font parser.
 * This will be changed once font parsing becomes more mature.
 *
 * https://github.com/dluco/ttf/blob/master/parse/parse.c
 */

static SimpleGlyph *parseSimpleGlyph(Font *font, glyf *glyf, int *index) {
    SimpleGlyph *simpleGlyph = (SimpleGlyph *) mallocate(sizeof(SimpleGlyph), 1, "GLYF Simple Glyph struct");

    simpleGlyph->endPtsOfContours = (uint16 *) mallocate(sizeof(uint16), glyf->numberOfContours, "GLYF Simple Glyph: endPtsOfContours");
    for (int i = 0; i < glyf->numberOfContours; i++) {
        simpleGlyph->endPtsOfContours[i] = getUInt16(font->fontBuffer, index);
    } 

    glyf->instructionLength = getUInt16(font->fontBuffer, index);

    glyf->instructions = (uint8 *) mallocate(sizeof(uint8), glyf->instructionLength, "GLYF Instructions");
    for (int i = 0; i < glyf->instructionLength; i++) {
        glyf->instructions[i] = getUInt8(font->fontBuffer, index);
    }

    int lastEndPt = simpleGlyph->endPtsOfContours[glyf->numberOfContours - 1];
    int numPoints = lastEndPt + 1;

    //Read flags
    simpleGlyph->flags = (uint8 *) mallocate(sizeof(uint8), numPoints, "GLYF Simple Glyph: flags");
    for (int i = 0; i < numPoints; i++) {
        simpleGlyph->flags[i] = getUInt8(font->fontBuffer, index);

        if ((simpleGlyph->flags[i] & REPEAT_FLAG) != 0) {
            uint8 repeats = getUInt8(font->fontBuffer, index);
            for (int j = 0; j <= repeats; j++) {
                simpleGlyph->flags[i + j] = simpleGlyph->flags[i];
            }

            i += repeats;
        }
    }

    //Read coords
    simpleGlyph->xCoordinates = (int16 *) mallocate(sizeof(int16), numPoints, "GLYF Simple Glyph: xCoords");
    simpleGlyph->yCoordinates = (int16 *) mallocate(sizeof(int16), numPoints, "GLYF Simple Glyph: yCoords");

    int16 x = 0, y = 0;
    for (int i = 0; i < numPoints; i++) {
        if ((simpleGlyph->flags[i] & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) != 0) {
            if ((simpleGlyph->flags[i] & X_SHORT_VECTOR) != 0) {
                x += (int16_t) getInt8(font->fontBuffer, index);
            }
        } else {
            if ((simpleGlyph->flags[i] & X_SHORT_VECTOR) != 0) {
                x += -((int16_t) getInt8(font->fontBuffer, index));
            } else {
                x    += getInt16(font->fontBuffer, index);
            }
        }
        simpleGlyph->xCoordinates[i] = x;
    }

    for (int i = 0; i < numPoints; i++) {
        if ((simpleGlyph->flags[i] & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) != 0) {
            if ((simpleGlyph->flags[i] & Y_SHORT_VECTOR) != 0) {
                y += (int16_t) getInt8(font->fontBuffer, index);
            }
        } else {
            if ((simpleGlyph->flags[i] & Y_SHORT_VECTOR) != 0) {
                y += -((int16_t) getInt8(font->fontBuffer, index));
            } else {
                y    += getInt16(font->fontBuffer, index);
            }
        }
        simpleGlyph->yCoordinates[i] = y;
    }

    return simpleGlyph;
}

static void parseCompGlpyh(Font *font, CompGlyph *glyph, int *index) {
    glyph->flags = getUInt16(font->fontBuffer, index);
    glyph->glyphIndex = getUInt16(font->fontBuffer, index);

    if (glyph->flags & ARG_1_AND_2_ARE_WORDS) {
		glyph->arg1 = getInt16(font->fontBuffer, index);
		glyph->arg2 = getInt16(font->fontBuffer, index);
	} else {
		glyph->arg1 = (int16_t) getInt8(font->fontBuffer, index);
		glyph->arg2 = (int16_t) getInt8(font->fontBuffer, index);
	}

	// Assign arguments depending on type
	if (glyph->flags & ARGS_ARE_XY_VALUES) {
		glyph->xtranslate = glyph->arg1;
		glyph->ytranslate = glyph->arg2;
	} else {
		glyph->point1 = (uint16_t) glyph->arg1;
		glyph->point2 = (uint16_t) glyph->arg2;
	}

	// Read scaling information
	if (glyph->flags & WE_HAVE_SCALE) {
		glyph->xscale = glyph->yscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
	} else if (glyph->flags & WE_HAVE_AN_X_AND_Y_SCALE) {
		glyph->xscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
		glyph->yscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
	} else if (glyph->flags & WE_HAVE_A_TWO_BY_TWO) {
		glyph->xscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
		glyph->scale01 = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
		glyph->scale10 = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
		glyph->yscale = (float) getInt16(font->fontBuffer, index) / (float) 0x4000;
	}
}

static CompGlyphArray *parseCompGlyphArray(Font *font, glyf *glyf, int *index) {
    CompGlyphArray *compGlyphArray = (CompGlyphArray *) mallocate(sizeof(CompGlyphArray), 1, "GLYF CompGlyphArray");
    compGlyphArray->numComps = 0;

    do {
        compGlyphArray->comps = realloc(compGlyphArray->comps, (compGlyphArray->numComps + 1) * sizeof(compGlyphArray->comps));
        parseCompGlpyh(font, &compGlyphArray->comps[compGlyphArray->numComps], index);

        compGlyphArray->numComps++;
    } while (glyf->compGlyphArray->comps && (glyf->compGlyphArray->comps[compGlyphArray->numComps - 1].flags & MORE_COMPONENTS));

    if (glyf->compGlyphArray->comps && (glyf->compGlyphArray->comps[compGlyphArray->numComps - 1].flags & WE_HAVE_INSTRUCTIONS)) {
        glyf->instructionLength = getUInt16(font->fontBuffer, index);

        glyf->instructions = (uint8 *) mallocate(sizeof(uint8), glyf->instructionLength, "GLYF Instructions");
        for (int i = 0; i < glyf->instructionLength; i++) {
            glyf->instructions[i] = getUInt8(font->fontBuffer, index);
        }
    }

    return compGlyphArray;
}

void loadGlyf(Font *font, glyf *glyf, int *index) {
    glyf->numberOfContours = getInt16(font->fontBuffer, index);
    glyf->xMin = getInt16(font->fontBuffer, index);
    glyf->yMin = getInt16(font->fontBuffer, index);
    glyf->xMax = getInt16(font->fontBuffer, index);
    glyf->yMax = getInt16(font->fontBuffer, index);

    if (glyf->numberOfContours == 0) {
        
    } else if (glyf->numberOfContours > 0) {
		glyf->simpleGlyph = parseSimpleGlyph(font, glyf, index);
	} else {
		glyf->compGlyphArray = parseCompGlyphArray(font, glyf, index);
	}
}

GlyphTable *parseGLYF(Font *font, int *index, maxp *maxp, loca *loca, head *head) {
    TableDirectory *dir = getTableDirFromTag(font, "glyf");
    if (dir == NULL) {
        printf("%s\n", "Error reading glyf table directory");
        exit(EXIT_FAILURE);
    }

    GlyphTable *result = (GlyphTable *) mallocate(sizeof(GlyphTable), 1, "GlyphTable struct");
    glyf *glyfs = (glyf *) mallocate(sizeof(glyf), maxp->numGlyphs, "GlyphTable GLYF array");

    for (int i = 0; i < maxp->numGlyphs; i++) {
        glyf *glyf = &glyfs[i];
        if (head->indexToLocFormat == 0) {
            if ((loca->offsets16[i + 1] - loca->offsets16[i]) == 0) {
                continue;
            }

            seek(index, dir->offset + loca->offsets16[i]);
            loadGlyf(font, glyf, index);
            
        } else if (head->indexToLocFormat == 1) {
            if ((loca->offsets32[i + 1] - loca->offsets32[i]) == 0) {
                continue;
            } 

            seek(index, dir->offset + loca->offsets32[i]);
            loadGlyf(font, glyf, index);

        } else {
            printf("%s\n", "Error reading indexToLocFormat");
        }
    }

    return result;
}