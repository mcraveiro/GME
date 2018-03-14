// ComboEdit.cpp : implementation file
//

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "ComboEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboEdit

CComboEdit::CComboEdit(CAutoComboBox* pAutoComboBox)
{
	m_pAutoComboCtrl=pAutoComboBox;
}

CComboEdit::~CComboEdit()
{
}


BEGIN_MESSAGE_MAP(CComboEdit, CEdit)
	//{{AFX_MSG_MAP(CComboEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboEdit message handlers

LRESULT CComboEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if (message == WM_KEYDOWN)
	{		

		int nVirtKey = (int) wParam;
		m_pAutoComboCtrl->OnEditKeyDown(nVirtKey);
			
	}

	return CEdit::WindowProc(message, wParam, lParam);
}
