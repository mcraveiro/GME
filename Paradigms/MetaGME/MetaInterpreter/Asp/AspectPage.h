#if !defined(AFX_ASPECTPAGE_H__CD948654_3126_480E_B332_504C347524BF__INCLUDED_)
#define AFX_ASPECTPAGE_H__CD948654_3126_480E_B332_504C347524BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <AFXDLGS.H>
#include "AspectSpecDlg.h"
#include "AspectSpecTbl.h"

class CAspectSpecTbl;
class CAspectSpecDlg;

typedef struct {
	int	rowID;
	CString roleName;
	CString kindAspect;
	CString isPrimary;
	const void * ptr;
} entry;

typedef CTypedPtrList<CPtrList, entry*> CEntryList;
// AspectPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAspectPage dialog

class CAspectPage : public CPropertyPage
{
	//DECLARE_DYNCREATE(CAspectPage)

// Construction
public:
	void AddEntry(int rowID, CString roleName, CString kindAspect, CString isPrimary, const void * ptr);
	bool GetEntry(int rowID, CString &roleName, CString& kindAspect, CString& isPrimary, const void * &ptr);
	void SetAspectName(CString name);
	CString GetAspectName();
	CAspectPage() ;
	~CAspectPage();
	static int m_actHeight; // the height of the user area (since stacking tabs may occupy a huge amount of place)
	static int m_actWidth;	// do not forget to reset to 0 upon destruction
	const int m_deflateVal;
	void resizeTableToFitIn();
	void calcPlace( CRect& loc);

// Dialog Data
	//{{AFX_DATA(CAspectPage)
	enum { IDD = IDD_ASPECT_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAspectPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CEntryList entries;
	CAspectSpecTbl aspectTable;
	CString	aspectName;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	// Generated message map functions
	//{{AFX_MSG(CAspectPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASPECTPAGE_H__CD948654_3126_480E_B332_504C347524BF__INCLUDED_)
