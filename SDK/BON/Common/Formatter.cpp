#include "stdafx.h"
#include "Formatter.h"

namespace GMEConsole
{
	CString Formatter::MakeObjectHyperlink(const CString & text, const CString& objectId)
	{
	
		CString result("<a href=\"mga:");		
		result+= objectId;
		result+= "\">";
		result+= text ;
		result+= "</a>";
		
		return result;
	}

	CString Formatter::MakeObjectHyperlink(const CString & text, long objectId)
	{
		// Inverse of CBuilderObject::GetObjId()
		unsigned long id = objectId, c, p;
		p=id%100000000;
		c=id/100000000;
		c+=100;

		CString result;
		result.Format(_T("<a href=\"mga:id-%04X-%08X\">"), c, p);
		result+= text;
		result+=_T("</a>");
		return result;
	}

	CString Formatter::MakeColored(const CString & text, COLORREF color)
	{
		CString result;
		result.Format(_T("<font color=\"#%02X%02X%02X\"> %s </font>"), (int)GetRValue(color), (int)GetGValue(color),(int)GetBValue(color), text);
		return result;
	}
}

/*
<font color="#ffffff">
My very first html page RGB
</font>
*/