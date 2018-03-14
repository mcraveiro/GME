#pragma once


// AttrNameDlg dialog

class AttrNameDlg : public CDialog
{
	DECLARE_DYNAMIC(AttrNameDlg)

public:
	AttrNameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AttrNameDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PROPATTRCHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CString m_oldV;
	CString m_newV;
	CString m_global;
	CString m_owner;

	DECLARE_MESSAGE_MAP()
public:
	void init( const CString& par1, const CString& par2, const CString& par3, const CString& par4);
	const CString& getPar1() const;
	const CString& getPar2() const;
	const CString& getPar3() const;
	const CString& getPar4() const;

	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedGlobal();
	virtual BOOL OnInitDialog();
};
