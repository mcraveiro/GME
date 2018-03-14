#pragma once


// AttrGlobalDlg dialog

class AttrGlobalDlg : public CDialog
{
	DECLARE_DYNAMIC(AttrGlobalDlg)

public:
	AttrGlobalDlg( const CString& pGlobal, const CString& pOwner, CWnd* pParent = NULL);   // standard constructor
	virtual ~AttrGlobalDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PROPATTRGLOBALLOCAL };

	CString getGlobal() const;
	const CString& getOwner() const;

protected:
	int m_iGlobalLocal;
	CString m_ownerKind;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	virtual BOOL OnInitDialog();

};
