#if !defined(AFX_PARADIGMPROPERTIESDLG_H__12D6E035_2A61_47DF_AC05_8BE55A9282FD__INCLUDED_)
#define AFX_PARADIGMPROPERTIESDLG_H__12D6E035_2A61_47DF_AC05_8BE55A9282FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParadigmPropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParadigmPropertiesDlg dialog

class CParadigmPropertiesDlg : public CDialog
{
// Construction
public:
	CParadigmPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParadigmPropertiesDlg)
	enum { IDD = IDD_PARADIGMPROPERTIES_DIALOG };
	CEdit	version;
	CEdit	name;
	CEdit	guid;
	CEdit	modified;
	CEdit	created;
	CEdit	comment;
	CEdit	author;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParadigmPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParadigmPropertiesDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARADIGMPROPERTIESDLG_H__12D6E035_2A61_47DF_AC05_8BE55A9282FD__INCLUDED_)
