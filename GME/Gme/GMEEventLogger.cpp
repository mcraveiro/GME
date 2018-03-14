// GMEEventLogger.cpp: implementation of the CGMEEventLogger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMEEventLogger.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGMEEventLogger::CGMEEventLogger()
{
}

CGMEEventLogger::~CGMEEventLogger()
{
}

BOOL CGMEEventLogger::initialized = FALSE;
FILE *CGMEEventLogger::EventLog = NULL;
BOOL CGMEEventLogger::newLine = TRUE;
CComPtr<IMgaEventLogger> CGMEEventLogger::comLogger = NULL;