// MetaConnectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "MetaConnectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMetaConnectionDlg dialog


CMetaConnectionDlg::CMetaConnectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMetaConnectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMetaConnectionDlg)
	m_strConnectionProperties = _T("");
	//}}AFX_DATA_INIT
}


void CMetaConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMetaConnectionDlg)
	DDX_Text(pDX, IDC_CONNECTION_PROP, m_strConnectionProperties);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMetaConnectionDlg, CDialog)
	//{{AFX_MSG_MAP(CMetaConnectionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMetaConnectionDlg message handlers
