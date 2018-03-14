
#include "stdafx.h"
#include "CommonStl.h"
#include <stdio.h>

// --------------------------- string

void vFormat(std::string &s, const char *format, va_list args)
{
	const int maxsize = 1024;

	s.resize(maxsize);
	int len = _vsnprintf(&s[0], maxsize-1, format, args);
	ASSERT( len <= maxsize-1 );

	if( len < 0 )
	{
		ASSERT(false);	// output was truncated
		len = maxsize-1;
	}

	s.resize(len);
}

void vFormat(std::wstring &s, const wchar_t *format, va_list args)
{
	const int maxsize = 1024;

	s.resize(maxsize);
	int len = _vsnwprintf(&s[0], maxsize-1, format, args);
	ASSERT( len <= maxsize-1 );

	if( len < 0 )
	{
		ASSERT(false);	// output was truncated
		len = maxsize-1;
	}

	s.resize(len);
}

