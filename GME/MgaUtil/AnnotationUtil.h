// AnnotationUtil.h: interface for the CAnnotationUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANNOTATIONUTIL_H__C3F11A89_9701_438e_887E_C724A14A45DF__INCLUDED_)
#define AFX_ANNOTATIONUTIL_H__C3F11A89_9701_438e_887E_C724A14A45DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAnnotationUtil
{
public:
	static void		LogfontEncode(CString &str, const LOGFONT* lfp);
	static bool		LogfontDecode(const CString &str, LOGFONT* lfp);
	static void		FillLogFontWithDefault(LOGFONT* lfp);
	static CString	ResolveNewLinesToCRLF(const CString& str);
	static CString	ResolveNewLinesToLF(const CString& str);
};

#endif // !defined(AFX_ANNOTATIONUTIL_H__C3F11A89_9701_438e_887E_C724A14A45DF__INCLUDED_)