#include <PalmOS.h>

#include "database.h"
#include "grid.h"
#include "font.h"

/*    Warning Info:
 *
 * The only function which creates the caching database is ....
 */

int BaseSaveFont ( const Char * fontname, UInt32 slotid) {
	DmOpenRef dbase;
	Err error;
	LocalID dbID;
	UInt16 card;
	MemHandle recordh;
	char * recdata;
	UInt16 recindex;
	char * fontdata;
	UInt32 fontsize;
	
	/* Delete this database, if already exists */
	dbase = DmOpenDatabaseByTypeCreator ( 'Font', slotid, dmModeReadOnly );
	if ( dbase ) {
		DmOpenDatabaseInfo ( dbase, &dbID, NULL, NULL, &card, NULL );
		DmCloseDatabase ( dbase );
		DmDeleteDatabase ( card, dbID );
	}

	error = DmCreateDatabase (0, fontname, slotid, 'Font', 0);
	if (error) return false;
	dbase = DmOpenDatabaseByTypeCreator ( 'Font', slotid, dmModeWrite );
	if ( !dbase ) return false;

	fontdata = DumpFont ();
	if ( fontdata == NULL ) {
		return false;
	}
	fontsize = MemPtrSize (fontdata);
	
	/* Get a handler for this record, create it if it does not exists */
	recordh = DmNewRecord ( dbase, &recindex, fontsize);
	recdata = MemHandleLock(recordh);
	
	DmWrite ( recdata, 0, fontdata, fontsize );

	MemHandleUnlock (recordh);
	DmReleaseRecord (dbase, recindex, true);
	MemPtrFree (fontdata);
	DmCloseDatabase (dbase);
	return true;
}


int BaseGetPrefs ( PrefsType * prefs){
	DmOpenRef dbase;
	LocalID dbID;
	UInt16 card;
	LocalID dbdataID;
	MemHandle h;

	PrefsType * lp;

	/* Open DB */
	dbase = DmOpenDatabaseByTypeCreator ( DATABASE_DATACACHE, DATABASE_CREATOR, dmModeReadOnly );
	if ( !dbase ) {
		return false; 	/* Should immediately return, since nothing was found */
	}

	/* Get DB ID */
	if ( DmOpenDatabaseInfo ( dbase, &dbID, NULL, NULL, &card, NULL ) ) {
		DmCloseDatabase (dbase);
		return false; 	/* Should immediately return, since nothing was found */
	}

	/* Get local data ID */
	if ( DmDatabaseInfo (card, dbID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dbdataID, NULL, NULL, NULL) ) {
		DmCloseDatabase (dbase);
		return false;
	}

	/* Get Global reference */
	h =  ((MemHandle) MemLocalIDToGlobal (dbdataID, card));
	/* Retrieve stored preferences pointer */
	lp = MemHandleLock (h);

	/* Retrieve actual preferences */
	*prefs = *lp;

	/* Close database */
	MemPtrUnlock(lp);
	DmCloseDatabase (dbase);
	return true;
}

int BaseSetPrefs ( PrefsType * prefs){
	DmOpenRef dbase;
	UInt16 card;
	MemHandle h;
	LocalID dbID;
	LocalID dbdataID;
	PrefsType * lp;

	/* Open DB */
	dbase = DmOpenDatabaseByTypeCreator ( DATABASE_DATACACHE, DATABASE_CREATOR, dmModeWrite );
	if ( !dbase ) {
		return false; 	/* Should immediately return, since nothing was found */
	}

	/* Get DB ID */
	if ( DmOpenDatabaseInfo ( dbase, &dbID, NULL, NULL, &card, NULL ) ) {
		DmCloseDatabase (dbase);
		return false; 	/* Should immediately return, since nothing was found */
	}

	/* Create new memory space for preferences */
	h = DmNewHandle ( dbase, sizeof(PrefsType));
	if ( !h ) {
		DmCloseDatabase (dbase);
		return false;
	}

	/* Find local pointer */
	dbdataID = MemHandleToLocalID(h);

	/* Set prefs pointer to the database */
	DmSetDatabaseInfo ( card, dbID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dbdataID, NULL, NULL, NULL);

	/* Set database preferences */
	lp = (PrefsType *)MemHandleLock(h);
	DmSet(lp, 0, sizeof(PrefsType), 0);
	DmWrite ( lp, 0, prefs, (sizeof(PrefsType)) );

	/* CLose database */
	MemPtrUnlock(lp);
	DmCloseDatabase (dbase);

	return true;
}


int BaseGetChar ( GridType * grid, int index) {
	DmOpenRef dbase;
	Err error;
	MemHandle recordh;
	GridType * recdata;
	UInt16 recindex;
	
	dbase = DmOpenDatabaseByTypeCreator ( DATABASE_DATACACHE, DATABASE_CREATOR, dmModeReadOnly );
	if ( !dbase ) {
		return false; 	/* Should immediately return, since nothing was found */
	}

	/* Get a handler for this record, create it if it does not exists */
	error = DmFindRecordByID ( dbase, index, &recindex);
	if ( error ) {
		return false;
	}
	recordh = DmQueryRecord ( dbase, recindex);
	if ( !recordh ) {	/* Record could not initialize */ 
		DmCloseDatabase (dbase);
		return false;
	}

	recdata = MemHandleLock(recordh);
	*grid = *recdata;
			
	MemHandleUnlock (recordh);
	DmReleaseRecord (dbase, recindex, true);
	DmCloseDatabase (dbase);
	return true;
}

int BaseSetChar ( GridType * grid, int index ) {
	DmOpenRef dbase;
	Err error;
	MemHandle recordh;
	GridType * recdata;
	UInt16 recindex;
	
	/* Open database and create it if not exists */
	dbase = DmOpenDatabaseByTypeCreator ( DATABASE_DATACACHE, DATABASE_CREATOR, dmModeWrite );
	if ( !dbase ) {
		error = DmCreateDatabase (0, DATABASE_FONTCACHENAME, DATABASE_CREATOR, DATABASE_DATACACHE, 0);
		if (error) return false;
		dbase = DmOpenDatabaseByTypeCreator ( DATABASE_DATACACHE, DATABASE_CREATOR, dmModeWrite );
		if ( !dbase ) return false;
	}

	/* Get a handler for this record, create it if it does not exists */
	error = DmFindRecordByID ( dbase, index, &recindex);
	if ( error ) {
		UInt32 newindex = index;
		recordh = DmNewRecord ( dbase, &recindex, sizeof(GridType));
		DmSetRecordInfo ( dbase, recindex, NULL, &newindex);
	}
	else {
		recordh = DmGetRecord ( dbase, recindex);
	}
	if ( !recordh ) {	/* Record could not initialize */ 
		DmCloseDatabase (dbase);
		return false;
	}

	recdata = MemHandleLock(recordh);
	DmWrite ( recdata, 0, grid, (sizeof(GridType)) );

	MemHandleUnlock (recordh);
	DmReleaseRecord (dbase, recindex, true);
	DmCloseDatabase (dbase);
	return true;
}


int BaseDelAllChars () {
	DmOpenRef dbase;
	LocalID dbID;
	UInt16 card;

	/* Delete this database, if already exists */
	dbase = DmOpenDatabaseByTypeCreator ( DATABASE_DATACACHE, DATABASE_CREATOR, dmModeWrite );
	if ( dbase ) {
		DmOpenDatabaseInfo ( dbase, &dbID, NULL, NULL, &card, NULL );
		DmCloseDatabase ( dbase );
		DmDeleteDatabase ( card, dbID );
		return true;
	}
	return false;
}
