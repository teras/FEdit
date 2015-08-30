/* Main code for Dokimh */

#ifndef __HELPER_H__
#define __HELPER_H__

#include <PalmOS.h>
#include "feditRsc.h"

void SetFieldFromStr ( UInt16 fieldID, Char * strP ) ;

void SetFieldFromInt ( UInt16 fieldID, int val );

void SetFieldFromHandle ( UInt16 fieldID, MemHandle txtH );

Char * GetTextFromField ( UInt16 fieldID );

#define MARK FrmCustomAlert ( AlertMessage, "Come inside and have a cup of coffee!", "", "")
#define INFO(x) FrmCustomAlert ( AlertMessage, x, "", "")

#endif
