// FilesInUseDetailsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "FilesInUseDetailsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilesInUseDetailsDlg dialog


CFilesInUseDetailsDlg::CFilesInUseDetailsDlg(CWnd* pParent /*=NULL*/, bool p_alternateText /*= false*/)
	: CDialog(CFilesInUseDetailsDlg::IDD, pParent)
	, m_alternateText( p_alternateText)
{
	//{{AFX_DATA_INIT(CFilesInUseDetailsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFilesInUseDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilesInUseDetailsDlg)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilesInUseDetailsDlg, CDialog)
	//{{AFX_MSG_MAP(CFilesInUseDetailsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilesInUseDetailsDlg message handlers

BOOL CFilesInUseDetailsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    if( m_alternateText)
	{
		SetWindowText( "Files changed and saved by other users");
		CStatic * p = (CStatic* ) GetDlgItem( IDC_STATIC);
		if( p)
			p->SetWindowText( "Latent changed files: user, model/folder name, type (model/folder)");
	}

	for( unsigned int i=0; i<m_fileList.size(); ++i )
        m_list.AddString( m_fileList[i].c_str() );
 
	
	return TRUE;
}
