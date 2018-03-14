#pragma once
#include "afxwin.h"


// RemoveDlg dialog

class RemoveDlg : public CDialog
{
	DECLARE_DYNAMIC(RemoveDlg)

public:
	RemoveDlg( bool changeTitle, CWnd* pParent = NULL);   // standard constructor
	virtual ~RemoveDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PROPREMOVEELEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void init( const CString& par1);
	const CString& getPar1() const;

protected:
	CString m_name;

public:
	virtual BOOL OnInitDialog(); // will set just the title of the window
protected:
	CString m_title; // stores the title of the window
};
