#if !defined(AFX_FILESINUSEDLG_H__25B5D7A4_0605_4898_8C3D_C81FA8855472__INCLUDED_)
#define AFX_FILESINUSEDLG_H__25B5D7A4_0605_4898_8C3D_C81FA8855472__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilesInUseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilesInUseDlg dialog

class CFilesInUseDlg : public CDialog
{
// Construction
public:
	CFilesInUseDlg(CWnd* pParent = NULL, bool changeTxt = false);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilesInUseDlg)
	enum { IDD = IDD_DIALOG_FILESAREUSED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilesInUseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilesInUseDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool         m_alternateText;
public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILESINUSEDLG_H__25B5D7A4_0605_4898_8C3D_C81FA8855472__INCLUDED_)
