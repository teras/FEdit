/* Main code for Dokimh */

#include <PalmOS.h>
#include "helper.h"

void SetFieldFromHandle ( UInt16 fieldID, MemHandle txtH )
{
	MemHandle oldTxtH;	/* Old text */
	FormType * frmP;	/* Form pointer */
	FieldType * fldP;	/* Field pointer */

	frmP = FrmGetActiveForm();	/* Find form pointer */
	fldP = FrmGetObjectPtr (frmP, FrmGetObjectIndex ( frmP, fieldID)); /* Find field pointer */

	ErrNonFatalDisplayIf ( !fldP, "missing field");	/* Check if the current object exists */
	
	oldTxtH = FldGetTextHandle(fldP);	/* Find text pointer of old text */
	
	FldSetTextHandle(fldP, txtH);	/* Set the new text */
	FldDrawField(fldP);	/* Redraw field */
	
	if (oldTxtH != NULL)	/* Free up old text */
		MemHandleFree(oldTxtH);

}


void SetFieldFromInt ( UInt16 fieldID, int data ) {
	
	/* Temporary place in heap in order to store the string data */
	MemHandle txtH;
	
	txtH = MemHandleNew ( 10 );
	if ( !txtH) return;

	StrPrintF ( MemHandleLock(txtH), "%i", data);
	SetFieldFromHandle ( fieldID, txtH);
	MemHandleUnlock (txtH);

}
	
void setFieldFromStr ( UInt16 fieldID, Char * strP ) 
{
	/* Temporary place in heap in order to store the string data */
	MemHandle txtH;

	/* Create new heap space */
	txtH = MemHandleNew ( StrLen (strP) + 1 ) ;
	if ( !txtH )
		return;

	/* Copy string to the heap, in order to be useful */
	StrCopy (MemHandleLock(txtH), strP);	/* Lock the current memory position, in order to accomply the specified task of setting text */
	
	/* Set text of field to the selected  heap data */
	SetFieldFromHandle (fieldID, txtH);
	
	/* Free lock for this (text) item */
	MemHandleUnlock(txtH);
	return;
}


Char * GetTextFromField ( UInt16 fieldID ) {
	FieldType *fldP;
	FormType * frmP;

	frmP = FrmGetActiveForm();
	fldP = FrmGetObjectPtr (frmP, FrmGetObjectIndex ( frmP, fieldID )); /* Find field pointer */
	return FldGetTextPtr (fldP);
}

