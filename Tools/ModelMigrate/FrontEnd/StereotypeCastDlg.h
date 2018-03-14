#pragma once


// StereotypeCastDlg dialog

class StereotypeCastDlg : public CDialog
{
	DECLARE_DYNAMIC(StereotypeCastDlg)

public:
	StereotypeCastDlg( bool pCastAtom2Model, CWnd* pParent = NULL);   // standard constructor
	virtual ~StereotypeCastDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PROPSTEREOTYPECHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CString m_kind;
	CString m_title;

	DECLARE_MESSAGE_MAP()
public:
	void init( const CString& par1);
	const CString& getPar1() const;
	virtual BOOL OnInitDialog();
};
