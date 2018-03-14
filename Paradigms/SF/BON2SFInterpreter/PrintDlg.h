//###############################################################################################################################################
//
//	Meta and Builder Object Network V2.0 for GME
//	PrintDlg.h
//
//###############################################################################################################################################

/*
	Copyright (c) Vanderbilt University, 2000-2004
	ALL RIGHTS RESERVED

	Vanderbilt University disclaims all warranties with regard to this
	software, including all implied warranties of merchantability
	and fitness.  In no event shall Vanderbilt University be liable for
	any special, indirect or consequential damages or any damages
	whatsoever resulting from loss of use, data or profits, whether
	in an action of contract, negligence or other tortious action,
	arising out of or in connection with the use or performance of
	this software.
*/

#if !defined(AFX_PRINTDLG_H__E15D2E62_02CC_4A19_9822_3AF3B984F04A__INCLUDED_)
#define AFX_PRINTDLG_H__E15D2E62_02CC_4A19_9822_3AF3B984F04A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

//###############################################################################################################################################
//
// 	C L A S S : CPrintDlg
//
//###############################################################################################################################################

class CPrintDlg : public CDialog
{
// Construction
public:
	CPrintDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrintDlg)
	enum { IDD = IDD_DIALOG };
	CString	m_strValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrintDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTDLG_H__E15D2E62_02CC_4A19_9822_3AF3B984F04A__INCLUDED_)
