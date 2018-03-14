#if !defined(AFX_FILESINUSEDETAILSDLG_H__E9CCDF19_CC29_45BC_B4BA_46DBE96F620D__INCLUDED_)
#define AFX_FILESINUSEDETAILSDLG_H__E9CCDF19_CC29_45BC_B4BA_46DBE96F620D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilesInUseDetailsDlg.h : header file
//

#include <vector>
#include <string>

typedef std::vector<std::string> strVec;

/////////////////////////////////////////////////////////////////////////////
// CFilesInUseDetailsDlg dialog

class CFilesInUseDetailsDlg : public CDialog
{
// Construction
public:
	CFilesInUseDetailsDlg(CWnd* pParent = NULL, bool alternateText = false);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilesInUseDetailsDlg)
	enum { IDD = IDD_DIALOG_FILESUSEDETAILS };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilesInUseDetailsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilesInUseDetailsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool    m_alternateText;
public:
    strVec m_fileList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILESINUSEDETAILSDLG_H__E9CCDF19_CC29_45BC_B4BA_46DBE96F620D__INCLUDED_)
