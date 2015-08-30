#ifndef __DATABASE_H__
#define __DATABASE_H__

#define DATABASE_FONTCACHENAME	"FEdit-FontCache"
#define DATABASE_CREATOR		'FEDt'
#define DATABASE_DATACACHE		'FEDc'
#define DATABASE_DATASLOT		'FED\0'

#include "grid.h"

typedef struct {
	int drawmode;
	int glyph;
} PrefsType;

int BaseGetChar ( GridType * grid, int index );
int BaseSetChar ( GridType * grid, int index );
int BaseGetPrefs ( PrefsType * grid );
int BaseSetPrefs ( PrefsType * grid );
int BaseDelAllChars ();

int BaseSaveFont ( const Char * fontname, UInt32 slotid );

#endif
