#pragma once

#include <string>
#include <afxwin.h>


namespace GMEConsole
{
	const COLORREF CLOUDBLUE = RGB(128, 184, 223);
	const COLORREF WHITE = RGB(255, 255, 255);
	const COLORREF BLACK = RGB(1, 1, 1);
	const COLORREF DARKGRAY = RGB(128, 128, 128);
	const COLORREF LIGHTGRAY = RGB(210, 210, 210);
	const COLORREF GREEN = RGB(185, 245, 148);
	const COLORREF RED = RGB(186, 12, 72);


	class Formatter
	{
	public:
		static CString MakeBold(const CString & text){return CString("<b>") + text +"</b>";}
		static CString MakeItalic(const CString & text){return CString("<i>") + text +"</i>";}
		static CString MakeUnderline(const CString & text){return CString("<u>") + text +"</u>";}
		static CString MakeHyperlink(const CString & text, const CString& url){return CString("<a href=\"") + url + "\">" + text +"</a>";}
		static CString MakeObjectHyperlink(const CString & text, const CString& objectId);
		static CString MakeObjectHyperlink(const CString & text, long objectId);
		static CString MakeColored(const CString & text, COLORREF color);

	};
}
/*
"<A HREF=\"mga:" + id +"\">" + nm + "</A>";
*/
