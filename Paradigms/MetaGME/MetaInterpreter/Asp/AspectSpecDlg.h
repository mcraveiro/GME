#if !defined(AFX_ASPECTSPECDLG_H__A771B477_7ECF_41F9_8FCD_1557C770B87C__INCLUDED_)
#define AFX_ASPECTSPECDLG_H__A771B477_7ECF_41F9_8FCD_1557C770B87C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <AFXDLGS.H>

#include "AspectPage.h"

class CAspectPage;
class CAspectSpecDlg;
extern CAspectSpecDlg	*theAspectDlg;

typedef CTypedPtrList<CPtrList, CAspectPage*> CAspectPageList;

// AspectSpecDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAspectSpecDlg

class CAspectSpecDlg : public CPropertySheet
{

// Construction
public:
	CAspectSpecDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CAspectPageList	aspectPages;
	CStringList	aspectNames;
	int	lastID;
	int	countPages;

// For resizability, see: MS KB 300606: http://support.microsoft.com/kb/300606
// "How to implement a resizable property sheet class that contains a menu bar in Visual C++ 6.0"
protected:
	BOOL	m_bNeedInit;
	CRect	m_rCrt;
	int		m_nMinCX;
	int		m_nMinCY;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAspectSpecDlg)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetEntry(int entryNum, CString &roleName, CString &kindAspect, CString &isPrimary, const void * &ptr);
	int AddEntry(CString aspectName, CString roleName, CString  kindAspect, CString primaryAspect, const void * ptr);
	void GetAspects(CStringList& aspects);
	void GetAspects(int rowID, CStringList& aspects);
	virtual ~CAspectSpecDlg();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAspectSpecDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* MinMaxInfo);
	afx_msg LRESULT OnTabPageSelectionChange(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASPECTSPECDLG_H__A771B477_7ECF_41F9_8FCD_1557C770B87C__INCLUDED_)
