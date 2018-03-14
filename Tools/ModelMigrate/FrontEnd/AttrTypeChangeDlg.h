#pragma once
#include "afxwin.h"


// AttrTypeChangeDlg dialog

class AttrTypeChangeDlg : public CDialog
{
	DECLARE_DYNAMIC(AttrTypeChangeDlg)

public:
	AttrTypeChangeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AttrTypeChangeDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PROPATTRTYPECHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CString m_attrName;
	BOOL m_bTerm;
	CString m_global;
	CString m_owner;

protected:
	static void genSeparateScript( CString pAttrName, CString pFileName, bool pGlobal, CString pOwner);


public:
	void init( const CString& par1, const CString& par2, const CString& par3, const CString& par4);
	const CString& getPar1() const;
	      CString  getPar2() const;
	const CString& getPar3() const;
	const CString& getPar4() const;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedGlobal();
};
