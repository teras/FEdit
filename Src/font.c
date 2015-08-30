/*
 * I would like to thank:
 *      Wes Cherry   (mailto:wesc@technosis.com)
 *      and
 *      Aaron Ardiri (mailto:aaron@ardiri.com)
 *
 * for providing the source code and the valuable information about how is 
 * a font stored into palm memory.
 *
 * GPL rules!
 */

#include <PalmOS.h>
#include "font.h"
#include "database.h"

#include "helper.h"

int CalcAttribs ( FontType *header ) {
	GridType grid;
	int last, rowbits, width;
	int notfound;
	int i;
	
	rowbits = 0;
	notfound = true;
	width = 0;
	last = 0;

	for ( i = 0 ; i < 255 ; i++ ) {
		if ( BaseGetChar ( &grid, i)) {		/* char found! */
			if ( notfound ) {
				notfound = false;
				header->firstChar = i;
				header->fRectHeight = grid.vc;
			}
			last = i;
			rowbits += grid.hc;
			if ( width < grid.hc ) width = grid.hc;
		}
	}
	if ( notfound ) return false;

	if ( last == header->firstChar ) return false;
	
	header->fontType = 36864;
//	header->lastChar = last-1;
	header->lastChar = last;
	header->maxWidth = width;
	header->kernMax = 0;
	header->nDescent = 0;
	header->fRectWidth = width;
	header->ascent = 6;
	header->descent = 1;
	header->leading = 0;
	header->rowWords = (rowbits + 15) / 16;
	header->owTLoc = header->rowWords * header->fRectHeight + 8 + header->lastChar - header->firstChar;
	return true;
}


char * DumpFont() {
	GridType grid;

	FontType pre;

	FontType *header;
	char * bitmap;
	UInt16 * coltable;
	FontCharInfoType * fntOW;
	UInt16 * footer;

	int charCount = 0;
	int charRelative;
	int x;
	int row = 0;
	int col = 0;
	
	char * data;	/* Pointer for current data output */

	int rowoffset, memsize;

	/* Calculate font attributes. Return null if font empty. */
	if ( !CalcAttribs ( &pre ) ) {
		return NULL;
	}
	
	/* Reserve memory for output */
	charCount = pre.lastChar - pre.firstChar + 2;

	memsize  = sizeof(FontType) + 2 * pre.rowWords * pre.fRectHeight + 4 + (sizeof(UInt16) + sizeof(FontCharInfoType) ) * charCount ;
	data = MemPtrNew(memsize);	/* Freeing of this memory is done elsewhere, since we still need this data */
	MemSet (data, memsize, 0);
	
	header = (FontType *) data;
	*header = pre;
	bitmap = (char *)((void *)header + sizeof(FontType));
	coltable = (UInt16 *)((void *)bitmap + 2 * header->rowWords * header->fRectHeight) ;
	fntOW = (FontCharInfoType *) ((void *)coltable + ( sizeof(UInt16)* charCount) );
	footer = (UInt16 *) ((void *)fntOW + (sizeof(FontCharInfoType) * charCount) + 2);

	/* Debug 
	SetFieldFromInt ( FieldXSize, memsize);
	SetFieldFromInt ( FieldGlyph, ((void *)footer - (void *)header + 2));
	*/
	
	MemSet(fntOW, sizeof(fntOW), -1);	/* Make negative glyph info */
//	*(fntOW-1) = 0;
	
	col  = 0;
	/* Store every character */
	for ( charRelative = 0 ; charRelative <= charCount ; charRelative++ ) {

		coltable[charRelative] = col;
		if ( !BaseGetChar ( &grid, charRelative + header->firstChar )) {	/* char not found! */
			fntOW[charRelative].offset = 0;
		}
		else {
			fntOW[charRelative].offset = 0;
			fntOW[charRelative].width = grid.hc;
			for (row = 0 ; row < grid.vc ; row++ ) {
				rowoffset = row * header->rowWords * 2;
				for (x = 0; x < grid.hc; x++) {
					if ( grid.points[x/8][row] & (128 >> (x%8)) ) {
						bitmap[rowoffset + ((col + x) / 8)] |= 128 >> ((col + x)%8) ;
					}
				}
			}
			col += grid.hc;
		}
	}
//	*(footer - 2) = col;
	*(footer) = 0xffff;
	
	return (char *)header;
}

