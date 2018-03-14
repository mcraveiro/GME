#pragma once
#include "afxwin.h"


// SimpleReplace dialog

class SimpleReplace : public CDialog
{
	DECLARE_DYNAMIC(SimpleReplace)

public:
	SimpleReplace(CWnd* pParent = NULL);   // standard constructor
	virtual ~SimpleReplace();

// Dialog Data
	enum { IDD = IDD_DLG_PROPNAMECHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void init( const CString& par1, const CString& par2);
	const CString& getPar1() const;
	const CString& getPar2() const;

protected:
	CString m_oldV;
	CString m_newV;
};
