/////////////////////////////////////////////////////////////////////////////
//
// CrashTest.h : Nasty functions which cause various crashes
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(CRASH_TEST_INCLUDED)
#define CRASH_TEST_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CrashTest
{
public:
	CrashTest();
	~CrashTest();

	static void IllegalWrite(void);
	static void IllegalRead(void);
	static void IllegalReadInCRuntime(void);
	static void IllegalCodeRead(void);
	static void DivideByZero(void);
	static void Abort(void);
	static void Terminate(void);
};

#endif // !defined(CRASH_TEST_INCLUDED)