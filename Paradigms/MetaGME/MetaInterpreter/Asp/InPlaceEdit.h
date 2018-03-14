//////////////////////////////////////////////////////////////////////
// InPlaceEdit.h : header file
//
// MFC Grid Control - inplace editing class
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.10+
//
//////////////////////////////////////////////////////////////////////

#define IDC_IPEDIT	1976

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit window

class CInPlaceEdit : public CEdit
{
// Construction
public:
	CInPlaceEdit(int iItem, int iSubItem, CString sInitText);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL    m_bESC;	 	// To indicate whether ESC key was pressed
};

/////////////////////////////////////////////////////////////////////////////