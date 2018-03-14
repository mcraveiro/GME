// ActiveBrowsePropertySheet.h : header file
//
// CActiveBrowserPropertySheet is a modeless property sheet that is 
// created once and not destroyed until the application
// closes.  It is initialized and controlled from
// CActiveBrowserPropertyFrame.
 
#ifndef __ACTIVEBROWSEPROPERTYSHEET_H__
#define __ACTIVEBROWSEPROPERTYSHEET_H__

#include "ActiveBrowserPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// CActiveBrowserPropertySheet

class CActiveBrowserPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CActiveBrowserPropertySheet)

// Construction
public:
	CActiveBrowserPropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CAggregatePropertyPage m_PageAggregate;
	CInheritancePropertyPage m_PageInheritance;
	CMetaPropertyPage m_PageMeta;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActiveBrowserPropertySheet)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CActiveBrowserPropertySheet();
	 virtual void PostNcDestroy();
	void nextTab( bool);

// Generated message map functions
protected:
	//{{AFX_MSG(CActiveBrowserPropertySheet)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __ACTIVEBROWSEPROPERTYSHEET_H__
