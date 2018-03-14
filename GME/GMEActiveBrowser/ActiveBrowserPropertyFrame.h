// ActiveBrowserPropertyFrame.h : header file
//
// This file contains the mini-frame that controls modeless 
// property sheet CActiveBrowserPropertySheet.

#ifndef __ACTIVEBROWSERPROPERTYFRAME_H__
#define __ACTIVEBROWSERPROPERTYFRAME_H__

#include "ActiveBrowsePropertySheet.h"

/////////////////////////////////////////////////////////////////////////////
// CActiveBrowserPropertyFrame frame

class CActiveBrowserPropertyFrame : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CActiveBrowserPropertyFrame)
//Construction
public:
	CActiveBrowserPropertyFrame();

// Attributes
public:
	CActiveBrowserPropertySheet* m_pModelessPropSheet;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActiveBrowserPropertyFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnCloseProject();

	void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);

	void OnOpenProject();
	virtual ~CActiveBrowserPropertyFrame();

	// Generated message map functions
	//{{AFX_MSG(CActiveBrowserPropertyFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif	// __ACTIVEBROWSERPROPERTYFRAME_H__
