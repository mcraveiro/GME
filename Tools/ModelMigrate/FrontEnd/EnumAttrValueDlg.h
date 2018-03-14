#pragma once


// EnumAttrValueDlg dialog

class EnumAttrValueDlg : public CDialog
{
	DECLARE_DYNAMIC(EnumAttrValueDlg)

public:
	EnumAttrValueDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~EnumAttrValueDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PROPENATTRCHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CString m_enumAttr;
	CString m_oldEnumV;
	CString m_newEnumV;
	CString m_global;
	CString m_owner;

public:
	void init( const CString& par1, const CString& par2, const CString& par3, const CString& par4, const CString& par5);

	const CString& getPar1() const;
	const CString& getPar2() const;
	const CString& getPar3() const;
	const CString& getPar4() const;
	const CString& getPar5() const;

	afx_msg void OnBnClickedGlobal();
};
