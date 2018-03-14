/////////////////////////////////////////////////////////////////////////////
//
// CrashTest.cpp : Nasty functions which cause various crashes
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrashTest.h"

static	CrashTest	g_CrashTester;	// Global instance of CrashTest

CrashTest::CrashTest()
{
}

CrashTest::~CrashTest()
{
}

void CrashTest::IllegalWrite(void)
{
	char *p = 0;	// Null pointer.
	char x = 0;

	*p = x;
}

void CrashTest::IllegalRead(void)
{
	char *p = 0;	// Null pointer.
	char x = 0;

	x = *p;
}

void CrashTest::IllegalReadInCRuntime(void)
{
	strcpy(0, 0);
}

typedef void (*tBogusFunction)();

void CrashTest::IllegalCodeRead(void)
{
	tBogusFunction BadFunc = (tBogusFunction) 0;
	BadFunc();
}

void CrashTest::DivideByZero(void)
{
	int y = 0;

	y = y / y;
}

void CrashTest::Abort(void)
{
	abort();
}

void CrashTest::Terminate(void)
{
	terminate();
}
