/* Main code for Dokimh */

#include <PalmOS.h>
#include "callback.h"

#include "feditRsc.h"
#include "helper.h"
#include "grid.h"
#include "database.h"


static GridType grid;
static GridType edit;	/* memory to save grid for copy & paste functions */

static PrefsType prefs;

static int drawcolor = DRAWMODE_DRAW;
static int oldX = -1;
static int oldY = -1;


static void PrefsInit () {
	prefs.glyph = DEFAULT_GLYPH;
	prefs.drawmode = DRAWMODE_DRAW;
}

static void ResetDrawMode ( FormType * frm ) {
	CtlSetGraphics ( FrmGetObjectPtr ( frm, FrmGetObjectIndex(frm, ButtonDrawMode)), BitmapDrawInit + prefs.drawmode, NULL);
}

static void DrawPoint (int x, int y) {
	if ( GridIncludesPoint ( &grid, x, y) ) {	/* Inside boundaries */
		GridDrawCell ( &grid, x, y, drawcolor);
//		GridRedraw ( &grid );
	}
}


static Boolean FormSaveHandleEvent (EventType * e) {
    Boolean handled = false;
    FormType * frm;
	
#ifdef __GNUC__
    CALLBACK_PROLOGUE
#endif

	frm = FrmGetActiveForm();
    switch (e->eType) {
	case frmOpenEvent:
		FrmDrawForm(frm);
		break;

	case frmCloseEvent:
		break;

	case ctlSelectEvent:
		switch(e->data.ctlSelect.controlID) {
			Char * fontname;
			Char * slot;

		case ButtonSaveSave:
			fontname = GetTextFromField ( FieldSaveName );
			slot = GetTextFromField (FieldSaveSlot);
			if ( fontname != NULL && slot != NULL ) {
				UInt32 slotID = DATABASE_DATASLOT + (*slot);
				BaseSaveFont (fontname, slotID);
				FrmReturnToForm (FormMain);
			}
			break;
		case ButtonSaveCancel:
			FrmReturnToForm (FormMain);
			break;
		}

    	handled = true;
		break;


    default:
        break;
    }

#ifdef __GNUC__
    CALLBACK_EPILOGUE
#endif

    return handled;
}


static Boolean FormMainHandleEvent (EventType * e) {
    Boolean handled = false;
    FormType * frm;
	
#ifdef __GNUC__
    CALLBACK_PROLOGUE
#endif
			
	frm = FrmGetActiveForm();
    switch (e->eType) {

	case penDownEvent:
	{
		int x, y;
		x = GridCalcX(&grid, e->screenX);
		y = GridCalcY(&grid, e->screenY);
		if ( prefs.drawmode == DRAWMODE_SMART ) {
			if ( GridCheckPoint (&grid, x, y) ) {
				drawcolor = DRAWMODE_ERASE;
			}
			else {
				drawcolor = DRAWMODE_DRAW;
			}
		}
		else {
			drawcolor = prefs.drawmode;
		}
		DrawPoint (x, y);
	}
		break;

	case penMoveEvent:
	{
		int x, y;
		x = GridCalcX(&grid, e->screenX);
		y = GridCalcY(&grid, e->screenY);

		if ( oldX != x || oldY != y ) {
			oldX = x;
			oldY = y;
			DrawPoint(x, y);
		}
	}
		break;
		
	case frmOpenEvent:
		FrmDrawForm(frm);
		if ( !BaseGetPrefs ( &prefs) ) {
			PrefsInit ( &prefs );
		}
		if ( !BaseGetChar ( &grid, prefs.glyph ) ) {	/* glyph not found */
			GridInit(frm, GadgetPixels, &grid);
		}
		GridRedraw( &grid );
		SetFieldFromInt ( FieldGlyphNum, prefs.glyph);
		ResetDrawMode ( frm );
		break;

	case frmCloseEvent:
		BaseSetChar ( &grid, prefs.glyph );
		BaseSetPrefs ( &prefs );
		break;

	case frmUpdateEvent:
		GridRedraw(&grid);
		break;

    case menuEvent:
		MenuEraseStatus(NULL);

		switch(e->data.menu.itemID) {
		case MenuNew:
			if ( FrmCustomAlert ( AlertConfirm, "Please confirm you would like to delete current font.", "", "") == 0 ) {
				BaseDelAllChars ();
				GridReset ( &grid );
				GridRedraw(&grid);
			}
			break;
		case MenuLoad:
			break;
		case MenuSave:
			BaseSetChar ( &grid, prefs.glyph);
			FrmPopupForm(FormSave);
			break;
		case MenuInfo:
			break;
		case MenuTest:
			break;
		case MenuAbout:
			FrmAlert ( AlertAbout );
			break;
		case MenuCopy:
			GridCopy ( &grid, &edit);
			break;
		case MenuPaste:
			GridCopy ( &edit, &grid);
			GridRedraw (&grid);
			break;
		case MenuDelete:
			GridReset ( &grid );
			GridRedraw (&grid );
			break;
		case MenuReverse:
			GridReverse ( &grid );
			GridRedraw (&grid );
			break;
		}

	case ctlSelectEvent:
		switch(e->data.ctlSelect.controlID) {

		case ButtonIncX:
			GridModifySize ( &grid, 1 , 0);
			GridRedraw (&grid);
			break;
		case ButtonDecX:
			GridModifySize ( &grid, -1, 0);
			GridRedraw (&grid);
			break;
		case ButtonIncGlyph:
			BaseSetChar ( &grid, prefs.glyph);
			if ( prefs.glyph < GLYPH_MAX ) prefs.glyph++;
			if ( !BaseGetChar ( &grid, prefs.glyph ) ) {	/* glyph not found */
				GridReset( &grid );
			}
			GridRedraw (&grid);
			SetFieldFromInt ( FieldGlyphNum, prefs.glyph );
			break;
		case ButtonDecGlyph:
			BaseSetChar ( &grid, prefs.glyph);
			if ( prefs.glyph > GLYPH_MIN ) prefs.glyph--;
			if ( !BaseGetChar ( &grid, prefs.glyph ) ) {	/* glyph not found */
				GridReset( &grid );
			}
			GridRedraw (&grid);
			SetFieldFromInt ( FieldGlyphNum, prefs.glyph );
			break;
		case ButtonDrawMode:
			prefs.drawmode++;
			if ( prefs.drawmode > DRAWMODE_MAX ) 
				prefs.drawmode = DRAWMODE_MIN;
			ResetDrawMode ( frm );
			break;
		}

    	handled = true;
		break;


    default:
        break;
    }

#ifdef __GNUC__
    CALLBACK_EPILOGUE
#endif

    return handled;
}

static Boolean ApplicationHandleEvent(EventType * e)
{
    FormType * frm;
    UInt16    formId;
    Boolean handled = false;

    if (e->eType == frmLoadEvent) {
	formId = e->data.frmLoad.formID;
	frm = FrmInitForm(formId);
	FrmSetActiveForm(frm);

	switch(formId) {
	case FormMain:
	    FrmSetEventHandler(frm, FormMainHandleEvent);
	    break;
	case FormSave:
		FrmSetEventHandler(frm, FormSaveHandleEvent);
		break;
	}
	handled = true;
    }

    return handled;
}

/* Get preferences, open (or create) app database */
static UInt16 StartApplication(void)
{
    FrmGotoForm(FormMain);
    return 0;
}

/* Save preferences, close forms, close app database */
static void StopApplication(void)
{
    FrmSaveAllForms();
    FrmCloseAllForms();
}

/* The main event loop */
static void EventLoop(void)
{
    UInt16 err;
    EventType e;

    do {
	EvtGetEvent(&e, evtWaitForever);
	if (! SysHandleEvent (&e))
	    if (! MenuHandleEvent (NULL, &e, &err))
		if (! ApplicationHandleEvent (&e))
		    FrmDispatchEvent (&e);
    } while (e.eType != appStopEvent);
}

/* Main entry point; it is unlikely you will need to change this except to
   handle other launch command codes */
UInt32 PilotMain(UInt16 cmd, void * cmdPBP, UInt16 launchFlags)
{
    UInt16 err;

    if (cmd == sysAppLaunchCmdNormalLaunch) {

		err = StartApplication();
		if (err) return err;

		EventLoop();
		StopApplication();

    }
	else {
		return sysErrParamErr;
    }

    return 0;
}
