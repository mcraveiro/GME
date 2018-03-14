#if !defined(AFX_MGAOPENDLG_H__13874436_F1E6_11D3_B38F_005004D38590__INCLUDED_)
#define AFX_MGAOPENDLG_H__13874436_F1E6_11D3_B38F_005004D38590__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MgaOpenDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CMgaOpenDlg dialog

class CMgaOpenDlg : public CDialog
{
public:
	enum DialogTypes {
		OpenDialog,
		SaveDialog,
		SaveAsDialog,
		NewDialog,
		ImportDialog,
		ClearLocksDialog
	};
// Construction
public:
	CMgaOpenDlg(DialogTypes dType = OpenDialog, CWnd* pParent = NULL);   // standard constructor

	void SetFileNameHint(const CString& hint);
	void SetFolderPathHint(const CString& hint);

	bool pressed_back;

	CString AskConnectionString(bool allowXme, bool openFileDialog);
	CString AskMGAConnectionString();
private:
	CString PruneConnectionString(const CString& conn);
	CString FilterInvalidCharacters(const CString& path, bool isPath = false);

public:
// Dialog Data
	//{{AFX_DATA(CMgaOpenDlg)
	enum { IDD = IDD_MGAOPEN };
	int		m_radio;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMgaOpenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	CString fileNameHint;
	CString folderPathHint;
	BOOL flag_isopen;
	bool flag_back;
	bool flag_create;
	CString title;
	CString filemsg;
    CString xmlfilemsg;

public:
	// Generated message map functions
	//{{AFX_MSG(CMgaOpenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBack();
	afx_msg void OnNext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MGAOPENDLG_H__13874436_F1E6_11D3_B38F_005004D38590__INCLUDED_)
