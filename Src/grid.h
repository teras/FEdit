#ifndef __GRID_H__
#define __GRID_H__

#include <PalmOS.h>

#define DRAWMODE_DRAW	0
#define DRAWMODE_ERASE	1
#define DRAWMODE_SMART	2
#define DRAWMODE_MIN	DRAWMODE_DRAW
#define DRAWMODE_MAX	DRAWMODE_SMART

#define MAXGRID 24
#define MAXGRIDY MAXGRID
#define MAXGRIDX ((MAXGRID-1)/8 + 1)

//#define DEFAULT_X       1
//#define DEFAULT_Y       1
#define DEFAULT_X       8
#define DEFAULT_Y       8
#define DEFAULT_GLYPH   48

#define GLYPH_MAX       255
#define GLYPH_MIN       0


typedef struct {
	int hc;	/* How many horizontal cells */
	int vc;	/* How many vertical cells */

	RectangleType bounds;	/* Bounds of the original widget */

	int x1;	/* X upper corner of the real viewable area */
	int y1;	/* Y upper corner of the real viewable area */
	int x2;	/* X lower corner of the real viewable area */
	int y2;	/* Y lower corner of the real viewable area */
	int cw;	/* Width of every cell */

	char points[MAXGRIDX][MAXGRIDY];
} GridType;


void GridInit ( FormType * frm, UInt16 gridID, GridType * grid);

void GridRedraw ( GridType *grid );
void GridErase ( GridType *grid) ;
void GridReset ( GridType *grid) ;
void GridReverse ( GridType *grid) ;
void GridCopy ( GridType *sourcegrid, GridType *destgrid);

void GridSetSize ( GridType *grid, int x, int y);
void GridModifySize ( GridType *grid, int dx, int dy);
int GridIncludesPoint ( GridType *grid, int screenx, int screeny);
void GridDrawCell ( GridType *grid, int x, int y, int col);
int GridCheckPoint ( GridType *grid, int x, int y);

int GridCalcX ( GridType *grid, int x);
int GridCalcY ( GridType *grid, int y);

#endif
