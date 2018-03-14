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
#if !defined(AFX_INPLACEEDIT_H__0C0027AD_6641_498A_BE01_8947A0F64FFD__INCLUDED_)
#define AFX_INPLACEEDIT_H__0C0027AD_6641_498A_BE01_8947A0F64FFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit window

class CInPlaceEdit : public CEdit
{
// Construction
public:
	CInPlaceEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACEEDIT_H__0C0027AD_6641_498A_BE01_8947A0F64FFD__INCLUDED_)
