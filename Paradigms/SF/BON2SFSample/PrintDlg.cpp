//###############################################################################################################################################
//
//	Meta and Builder Object Network V2.0 for GME
//	PrintDlg.cpp
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

#include "stdafx.h"
#include "PrintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//###############################################################################################################################################
//
// 	C L A S S : CPrintDlg
//
//###############################################################################################################################################

CPrintDlg::CPrintDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintDlg)
	m_strValue = _T("");
	//}}AFX_DATA_INIT
}


void CPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintDlg)
	DDX_Text(pDX, IDC_EDIT, m_strValue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrintDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPrintDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData( false );
	return TRUE;
}

void CPrintDlg::OnOK()
{
	CDialog::OnOK();
}
