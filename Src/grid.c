/* Main code for Dokimh */

#include "grid.h"
#include "helper.h"
#include "feditRsc.h"

void GridRedraw ( GridType *grid) {
	int p, i, j;
	RectangleType fatpixel;
	int cw;
	
	cw = grid->cw;
	WinEraseRectangle ( &(grid->bounds), 0);
	for ( i = 0 ; i <= grid->hc ; i++ ) {
		p = grid->x1 + cw * i ;
		WinDrawGrayLine ( p, grid->y1, p, grid->y2);
	}
	for ( i = 0 ; i <= grid->vc ; i++ ) {
		p = grid->y1 + cw * i;
		WinDrawGrayLine ( grid->x1, p, grid->x2, p);
	}

	fatpixel.extent.x = cw-1;
	fatpixel.extent.y = cw-1;
	for ( j = 0 ; j < grid->vc ; j++ ) {
		for ( i = 0 ; i < grid->hc ; i++ ) {
			int i1x, i2x;
			i1x = i/8;
			i2x = 128 >> (i%8);
			if ( grid->points[i1x][j] & i2x ) {
				fatpixel.topLeft.x = grid->x1 + cw * i + 1 ;
				fatpixel.topLeft.y = grid->y1 + cw * j + 1 ;
				WinPaintRectangle ( &fatpixel, 2);
			}
		}
	}

	/* Display proper grid parameters */
	SetFieldFromInt ( FieldXSize, grid->hc );
}

void GridCalculateSizes ( GridType *grid, int x, int y ) {

	int bx, by, cx, cy, cw;
	
	if ( x < 1 || y < 1 || x > MAXGRID || y > MAXGRID)
		return;
	bx = grid->bounds.extent.x;
	by = grid->bounds.extent.y;
	cx = (bx-1)/x;
	cy = (by-1)/y;
	cw = ( cx < cy ) ? cx : cy;
	
	grid->hc = x;
	grid->vc = y;
	grid->cw = cw;
	grid->x1 = grid->bounds.topLeft.x + ( bx - cw * x )/2;
 	grid->x2 = grid->x1 + cw * x;
	grid->y1 = grid->bounds.topLeft.y + ( by - cw * y )/2;
	grid->y2 = grid->y1 + cw * y;
}



void GridModifySize ( GridType *grid, int dx, int dy) {
	GridCalculateSizes ( grid, grid->hc + dx, grid->vc + dy );
}


void GridSetSize ( GridType *grid, int x, int y) {
	GridCalculateSizes (grid, x, y );
}

int GridIncludesPoint ( GridType *grid, int x, int y) {
	if ( x >= 0 && x < grid->hc && y >= 0 && y < grid->vc ) {
		return true;
	}
	return false;
}

int GridCalcX ( GridType *grid, int screenx ) {
	int dx = screenx - grid->x1;
	if ( dx < 0 ) return -1;
	return dx / grid->cw;
}

int GridCalcY ( GridType *grid, int screeny) {
	int dy = screeny - grid->y1;
	if ( dy < 0 ) return -1;
	return dy / grid->cw;
}

int GridCheckPoint ( GridType *grid, int x, int y) {
	return ( grid->points[x/8][y] & (128 >> (x%8)) );
}



void GridDrawCell ( GridType *grid, int cx, int cy, int col) {
	RectangleType drawbox;
	int block, bit;
	int cw = grid->cw;

	drawbox.topLeft.x = grid->x1 + 1 + cx * cw;
	drawbox.topLeft.y = grid->y1 + 1 + cy * cw;
	drawbox.extent.x  = grid->cw - 1;
	drawbox.extent.y  = grid->cw - 1;
	

	bit = 128 >> (cx%8);
	block = cx / 8;
	if ( col == DRAWMODE_DRAW ) {
		grid->points[block][cy] |= bit;	
		WinPaintRectangle (&drawbox, 0);
	}
	else if ( col == DRAWMODE_ERASE ) {
		grid->points[block][cy] &= ~bit;	
		WinEraseRectangle (&drawbox, 0);
	}
}

void GridErase ( GridType * grid) {
	int i, j;

	for ( j = 0 ; j < MAXGRIDY ; j++ ) {
		for ( i = 0 ; i < MAXGRIDX ; i++ ) {
			grid->points[i][j] = 0;
		}
	}
}

void GridReset ( GridType * grid ) {
	GridErase ( grid );
	GridSetSize (grid, DEFAULT_X, DEFAULT_Y);
}

void GridInit ( FormType * frm, UInt16 gridID, GridType * grid) {
	FrmGetObjectBounds ( frm, FrmGetObjectIndex ( frm, gridID ), &(grid->bounds) );
	GridReset ( grid );
}


void GridReverse ( GridType * grid ) {
	int xp, yp;
	for ( xp = 0 ; xp < MAXGRIDX; xp++ ) {
		for ( yp = 0 ; yp < MAXGRIDY; yp++ ) {
			grid->points[xp][yp] ^= 255;
		}
	}
}

void GridCopy ( GridType * s, GridType * d ) {	/* s=source, d=destination */
	int i,j;
	
	d->hc = s->hc;
	d->vc = s->vc;
	d->x1 = s->x1;
	d->y1 = s->y1;
	d->x2 = s->x2;
	d->y2 = s->y2;
	d->cw = s->cw;

	for ( i = 0 ; i < MAXGRIDX; i++ ) {
		for ( j = 0 ; j < MAXGRIDY ; j++ ) {
			d->points[i][j] = s->points[i][j];
		}
	}
}

