#if !defined(AFX_COMBOEDIT_H__6B331639_23DB_4D4F_BE28_06F069185526__INCLUDED_)
#define AFX_COMBOEDIT_H__6B331639_23DB_4D4F_BE28_06F069185526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboEdit.h : header file
//

#include "AutoComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CComboEdit window

class CComboEdit : public CEdit
{
// Construction
public:
	CComboEdit(CAutoComboBox* );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboEdit)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CAutoComboBox* m_pAutoComboCtrl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOEDIT_H__6B331639_23DB_4D4F_BE28_06F069185526__INCLUDED_)
