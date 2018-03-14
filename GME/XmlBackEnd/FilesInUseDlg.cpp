// FilesInUseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "FilesInUseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilesInUseDlg dialog


CFilesInUseDlg::CFilesInUseDlg(CWnd* pParent /*=NULL*/, bool changeText /* = false */)
	: CDialog(CFilesInUseDlg::IDD, pParent)
	, m_alternateText( changeText)
{
	//{{AFX_DATA_INIT(CFilesInUseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFilesInUseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilesInUseDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilesInUseDlg, CDialog)
	//{{AFX_MSG_MAP(CFilesInUseDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilesInUseDlg message handlers

BOOL CFilesInUseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if( m_alternateText)
	{
		SetWindowText( "Files have been changed");
		CStatic * p = (CStatic* ) GetDlgItem( IDC_STATIC);
		if( p)
			p->SetWindowText( "Cannot perform this operation because certain parts of the model have been changed and saved by other users.");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
