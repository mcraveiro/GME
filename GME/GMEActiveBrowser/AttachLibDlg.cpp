// AttachLibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "AttachLibDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <stdlib.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// CAttachLibDlg dialog


CAttachLibDlg::CAttachLibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAttachLibDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttachLibDlg)
	m_strConnString = _T("");
	m_bOptimized = FALSE;
	m_bRelativePath = TRUE;
	//}}AFX_DATA_INIT
}


void CAttachLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttachLibDlg)
	DDX_Text(pDX, IDC_CONN_STRING, m_strConnString);
	DDX_Check(pDX, IDC_CHECKOPTIMIZED, m_bOptimized);
	DDX_Check(pDX, IDC_CHECK_RELATIVE_PATH, m_bRelativePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttachLibDlg, CDialog)
	//{{AFX_MSG_MAP(CAttachLibDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttachLibDlg message handlers

void CAttachLibDlg::OnOK() 
{

	if(!UpdateData(TRUE)) return;
	CDialog::OnOK();
	
	if (m_strConnString.Right(3).CompareNoCase(_T("mga")) == 0) {
		CString mgaPath = m_strConnString;
		if (m_strConnString.Left(4).CompareNoCase(_T("MGA=")) == 0) {
			mgaPath = m_strConnString.Right(m_strConnString.GetLength() - 4);
		}
		if (m_bRelativePath) {
			if (PathRelativePathTo(relativePath.GetBuffer(MAX_PATH), currentMgaPath, FILE_ATTRIBUTE_DIRECTORY, 
				mgaPath, FILE_ATTRIBUTE_NORMAL)) {
				relativePath.ReleaseBuffer();
			} else {
				relativePath.ReleaseBufferSetLength(0);
			}
		} else {
			CString filename, dirname;
			GetFullPathName(mgaPath, filename, dirname);
			if (filename != "")
				m_strConnString = _T("MGA=") + dirname + _T("\\") + filename;
		}
	}
}

void CAttachLibDlg::OnBrowse() 
{
	UpdateData(TRUE);

	static TCHAR BASED_CODE szFilter[] = _T("Binary Project Files (*.mga)|*.mga|Multiuser Project Files (*.mgx)|*.mgx|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE,_T("mga"),NULL,NULL,szFilter, this);

	if (currentMgaPath != "")
		dlg.GetOFN().lpstrInitialDir = currentMgaPath;

	if(dlg.DoModal()!=IDOK) return;

	bool is_mga = dlg.GetFileExt().CompareNoCase( _T("mga")) == 0;
	if( is_mga)
		m_strConnString = _T("MGA=") + dlg.GetPathName();
	else
		m_strConnString = _T("MGX=\"") + dlg.GetPathName().Left( dlg.GetPathName().ReverseFind( '\\')) + _T("\"");

	UpdateData(FALSE);
	
}

BOOL CAttachLibDlg::OnInitDialog() 
{
	// convenience for beta-testers
	int pos = m_strConnString.Find( _T(" (optimized)"));
	if( pos != -1) // remove if found
	{
		m_bOptimized = TRUE;
		m_strConnString = m_strConnString.Left( pos);
	}
	// end of special section // REMOVE later

	// show the expanded path as a hint
	// when env variable found
	CString hint = m_strConnString;
	pos = hint.Find( _T("%"));
	if( pos != -1) // found
	{
		int npos = hint.Find( _T("%"), pos + 1); //next pos
		if( npos != -1 && npos > pos + 1)
		{
			// get the value of the environment variable between the two %'s
			TCHAR *value = _tgetenv( hint.Mid( pos + 1, npos - pos - 1));
			hint.Replace( _T("%") + hint.Mid( pos + 1, npos - pos - 1) + _T("%"), value);
		}
	}

	// show hint if needed: it differs from m_strConnString
	CWnd *ptr = GetDlgItem( IDC_CONNSTR);
	if( ptr && hint != m_strConnString)
	{
		// replace _T("Mga Connection String") with expanded path
		ptr->SetWindowText( (LPCTSTR) hint);
	}

	if (m_strParentConnection.GetLength() > 4 && m_strParentConnection.Left(4) == "MGA=") {
		const TCHAR* zsConn = m_strParentConnection;
		zsConn += 4; // skip MGA=
		TCHAR* filename;
		if (!GetFullPathName(zsConn, MAX_PATH, currentMgaPath.GetBuffer(MAX_PATH), &filename) || filename == 0) {
		} else {
			*filename = _T('\0');
		}
		currentMgaPath.ReleaseBuffer();
	}

	CDialog::OnInitDialog();
	
	SetWindowText(m_strCaption);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
