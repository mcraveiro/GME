// StringArrayEx.h: interface for the CStringArrayEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGARRAYEX_H__E3BD7904_81CD_4022_9FA7_F1092FA4F199__INCLUDED_)
#define AFX_STRINGARRAYEX_H__E3BD7904_81CD_4022_9FA7_F1092FA4F199__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringArrayEx : public CStringArray  
{
public:
	int InsertAtOrder(CString &strElement);
	CStringArrayEx();
	virtual ~CStringArrayEx();

};

#endif // !defined(AFX_STRINGARRAYEX_H__E3BD7904_81CD_4022_9FA7_F1092FA4F199__INCLUDED_)
